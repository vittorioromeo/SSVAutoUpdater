// Copyright (c) 2013 Vittorio Romeo
// License: Academic Free License ("AFL") v. 3.0
// AFL License page: http://opensource.org/licenses/AFL-3.0

#include "AutoUpdater.h"
#include "Utils/MD5.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

using namespace std;
using namespace sf;
using namespace ssvs;
using namespace ssvs::Utils;
using namespace ssvs::FileSystem;
using namespace ssvau::Utils;

using Request = Http::Request;
using Response = Http::Response;
using Status = Http::Response::Status;

namespace ssvau
{
	AutoUpdater::AutoUpdater(const string& mHost, const string& mHostFolder, const string& mLocalFolder) : host{mHost}, hostFolder{mHostFolder}, localFolder{mLocalFolder} { }
	AutoUpdater::~AutoUpdater() { terminateAll(); }

	void AutoUpdater::run()
	{
		Json::Value updaterConfigRoot, serverFilesRoot;
		vector<FileData> serverFiles, localFiles;
		vector<DownloadData> toDownload;
		vector<string> serverExcludedFiles, serverExcludedFolders;

		// Concurrently starts the threads that get data from the server
		auto& configRootThread(startGetJsonRoot(updaterConfigRoot, hostConfigFile));
		auto& serverFilesRootThread(startGetJsonRoot(serverFilesRoot, hostScript));

		// Check if the target local folder exists, otherwise create it
		if(!exists(localFolder))
		{
			log("Local folder does not exist, creating");
			mkdir(localFolder.c_str());
		}

		// Wait until server config file has been downloaded, then set values
		waitFor(configRootThread);
		serverFolder = getValue<string>(updaterConfigRoot, "dataFolder");
		serverExcludedFiles = getStringArray(updaterConfigRoot, "excludedFiles");
		serverExcludedFolders = getStringArray(updaterConfigRoot, "excludedFolders");

		// Display what files the server excluded from sending you
		for(auto& f : serverExcludedFiles) log(f, "ServerExcludedFile");
		for(auto& f : serverExcludedFolders) log(f, "ServerExcludedFolder");

		// Wait until server PHP script finished returning file data, then fill data vectors
		waitFor(serverFilesRootThread);
		for(auto& f : serverFilesRoot) serverFiles.push_back({getValue<string>(f, "md5"), getChild(serverFolder, getValue<string>(f, "path")), getValue<string>(f, "path")});
		for(auto& f : getRecursiveFiles(localFolder)) localFiles.push_back({getMD5Hash(getFileContents(f)), getChild(localFolder, f), f});
		
		// Display server file data
		for(auto& f : serverFiles) log(f.path + " " + f.md5, "ServerFile");
		log("");

		// Display local file data
		for(auto& f : localFiles) log(f.path + " " + f.md5, "LocalFile");
		log("");

		// For each file data got from the server
		for(auto& serverFile : serverFiles)
		{
			// Find a file with the same name in the local file data
			auto localItr(find(begin(localFiles), end(localFiles), serverFile));
			bool mustContinue{false};

			// Check if the server excludes this particular file
			if(contains(serverExcludedFiles, serverFile.path))
			{
				log("<" + serverFile.path + "> excluded");
				mustContinue = true;
			}

			// Check if the server excludes the folder the file is in
			for(auto& excludedFolder : serverExcludedFolders)
				if(startsWith(serverFile.path, excludedFolder))
				{
					log("Folder of <" + serverFile.path + "> excluded");
					mustContinue = true;
				}

			// If any of the previous checks were true, continue with the next file
			if(mustContinue) { log(""); continue; }

			// Check if a file with the same name exists locally, otherwise force download
			if(localItr != localFiles.end())
			{
				FileData localFile = *localItr;
				log("<" + serverFile.path + "> exists locally - comparing...");

				// If the file exists locally and has same MD5 skip - otherwise force download
				if(localFile.md5 == serverFile.md5) log("<" + serverFile.path + "> matches");
				else
				{
					log("<" + serverFile.path + "> doesn't match, must download");
					toDownload.push_back({serverFile.path, true});
				}
			}
			else
			{
				log("<" + serverFile.path + "> doesn't exist locally - must download");
				toDownload.push_back({serverFile.path, false});
			}

			log("");
		}

		log("");

		// Download files that need to be created/updated
		if(!toDownload.empty()) runDownload(toDownload);

		log("Finished");
	}
	void AutoUpdater::runDownload(const vector<DownloadData>& mToDownload)
	{
		log("Starting...", "Download");
		ThreadWrapper& startDownloadThread(startDownload(serverFolder, localFolder, mToDownload));
		waitFor(startDownloadThread);
	}

	void AutoUpdater::cleanUp() { for(auto& t : memoryManager.getItems()) if(t->getFinished()) memoryManager.del(t); memoryManager.cleanUp(); }
	void AutoUpdater::terminateAll() { for(auto& t : memoryManager.getItems()) t->terminate(); memoryManager.cleanUp(); }
	string AutoUpdater::getMD5Hash(const string& mString) { MD5 key{mString}; return key.GetHash(); }
	Response AutoUpdater::getGetResponse(const string& mRequestFile) { return Http(host).sendRequest({hostFolder + mRequestFile}); }
	Response AutoUpdater::getPostResponse(const string& mRequestFile, const string& mBody) { return Http(host).sendRequest({hostFolder + mRequestFile, Request::Post, mBody}); }
	void AutoUpdater::waitFor(ThreadWrapper& mThreadWrapper) { while(!mThreadWrapper.getFinished()) sleep(seconds(1)); }
	string AutoUpdater::getChild(const string& mFolderName, const string& mPath) { return replace(mPath, mFolderName, ""); }
	vector<string> AutoUpdater::getFolderNames(const string& mPath)
	{
		vector<string> splitted{split(mPath, '/', true)}, result;
		string& lastSplitted(splitted[splitted.size() - 1]);
		unsigned int sizeToSearch{splitted.size()};
		if(lastSplitted[lastSplitted.length() - 1] != '/') --sizeToSearch;

		for(unsigned int i{0}; i < sizeToSearch; ++i)
		{
			if(splitted[i] == "/") continue;
			string toPush{""};
			for(unsigned int j{0}; j < i; ++j) toPush.append(splitted[j]);
			toPush.append(splitted[i] + "/");
			result.push_back(toPush);
		}

		return result;
	}

	ThreadWrapper& AutoUpdater::startGetJsonRoot(Json::Value& mTargetRoot, const string& mServerFileName)
	{
		ThreadWrapper& thread = memoryManager.create([=, &mTargetRoot]
		{
			log("Getting <" + mServerFileName + "> from server...", "Online");

			Response response{getGetResponse(mServerFileName)};
			Status status{response.getStatus()};

			if(status == Response::Ok)
			{
				log("<" + mServerFileName + "> got successfully", "Online");
				mTargetRoot = getRootFromString(response.getBody());
			}
			else log("Get <" + mServerFileName + "> error", "Online");

			log("Finished getting <" + mServerFileName + ">", "Online");
		});

		thread.launch(); return thread;
	}

	ThreadWrapper& AutoUpdater::startGetFileContents(string& mTargetString, const string& mServerFileName)
	{
		ThreadWrapper& thread = memoryManager.create([=, &mTargetString]
		{
			log("Getting <" + mServerFileName + "> from server...", "Online");

			Response response{getGetResponse(mServerFileName)};
			Status status{response.getStatus()};

			if(status == Response::Ok)
			{
				log("<" + mServerFileName + "> got successfully", "Online");
				mTargetString = response.getBody();
			}
			else log("Get <" + mServerFileName + "> error", "Online");

			log("Finished getting <" + mServerFileName + ">", "Online");
		});

		thread.launch(); return thread;
	}

	ThreadWrapper& AutoUpdater::startGetFile(const string& mServerFolder, const string& mLocalFolder, const DownloadData& mDownloadData)
	{
		ThreadWrapper& thread = memoryManager.create([=]
		{
			log("Processing <" + mDownloadData.path + ">", "Online");

			if(mDownloadData.existsLocally)
			{
				log("Backing up <" + mDownloadData.path + ">");
				ofstream ofs{mLocalFolder + mDownloadData.path + ".bak", ofstream::binary};
				string backupContents{getFileContents(mLocalFolder + mDownloadData.path)};
				ofs << backupContents;
				ofs.flush(); ofs.close();
			}

			for(auto& folderName : getFolderNames(mDownloadData.path)) if(!exists(mLocalFolder + folderName)) mkdir((mLocalFolder + folderName).c_str());

			string serverContents{""};
			ThreadWrapper& getFileContentsThread(startGetFileContents(serverContents, mServerFolder + mDownloadData.path));
			waitFor(getFileContentsThread);
			ofstream ofs{mLocalFolder + mDownloadData.path, ofstream::binary};
			ofs << serverContents;
			ofs.flush(); ofs.close();

			log("Finished processing <" + mDownloadData.path + ">", "Online");
		});

		thread.launch(); return thread;
	}

	ThreadWrapper& AutoUpdater::startDownload(const string& mServerFolder, const string& mLocalFolder, const vector<DownloadData>& mToDownload)
	{
		ThreadWrapper& thread = memoryManager.create([=]
		{
			for(auto& td : mToDownload)
			{
				log("Downloading <" + td.path + ">...", "Download");
				waitFor(startGetFile(mServerFolder, mLocalFolder, td));
				log("<" + td.path + "> downloaded", "Download");
				log("");
			}
		});

		thread.launch(); return thread;
	}
}

