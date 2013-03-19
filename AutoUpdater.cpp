// Copyright (c) 2013 Vittorio Romeo
// License: Academic Free License ("AFL") v. 3.0
// AFL License page: http://opensource.org/licenses/AFL-3.0

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <SSVUtilsJson.h>
#include "AutoUpdater.h"
#include "Utils/MD5.h"
#include "Utils/Utils.h"

using namespace std;
using namespace sf;
using namespace ssvs;
using namespace ssvs::Utils;
using namespace ssvu;
using namespace ssvu::Utils;
using namespace ssvu::UtilsJson;
using namespace ssvu::FileSystem;
using namespace ssvau::Utils;

using Request = Http::Request;
using Response = Http::Response;
using Status = Http::Response::Status;

namespace ssvau
{
	AutoUpdater::AutoUpdater(const string& mLocalFolder, const string& mHost, const string& mHostFolder, const string& mHostConfig, const string& mHostScript)
		: localFolder{mLocalFolder}, host{mHost}, hostFolder{mHostFolder}, hostConfigFile{mHostConfig}, hostScript{mHostScript}, backupFolder{localFolder + "_BACKUP/"} { }
	AutoUpdater::~AutoUpdater() { terminateAll(); }

	void AutoUpdater::runGetServerData()
	{
		// Concurrently starts the threads that get data from the server
		auto& configRootThread(startGetJsonRoot(updaterConfigRoot, hostConfigFile));
		auto& serverFilesRootThread(startGetJsonRoot(serverFilesRoot, hostScript));

		// Wait until server config file has been downloaded, then set values
		waitFor(configRootThread);
		serverFolder = 			getValue<string>(updaterConfigRoot, "dataFolder");
		serverExcludedFiles = 	getArray<string>(updaterConfigRoot, "excludedFiles");
		serverExcludedFolders = getArray<string>(updaterConfigRoot, "excludedFolders");
		serverOnlyNewFiles = 	getArray<string>(updaterConfigRoot, "onlyNewFiles");

		// Wait until server PHP script finished returning file data, then fill data vectors
		waitFor(serverFilesRootThread);
		for(auto& f : serverFilesRoot)
		{
			string childPath{getReplaced(getValue<string>(f, "path"), serverFolder, "")};
			serverFiles.push_back({getValue<string>(f, "md5"), childPath});
		}
		for(auto& f : getRecursiveAll(localFolder))
		{
			string childPath{getReplaced(f, localFolder, "")};
			if(isFolder(f)) localFiles.push_back({"FOLDER", childPath + "/"});
			else localFiles.push_back({getMD5Hash(getFileContents(f)), childPath});
		}
	}
	void AutoUpdater::runDisplayData()
	{
		// Display what files the server excluded from sending you
		for(auto& f : serverExcludedFiles) log(f, "ServerExcludedFile");
		for(auto& f : serverExcludedFolders) log(f, "ServerExcludedFolder");

		// Display server file data
		for(auto& f : serverFiles) log(f.path + " " + f.md5, "ServerFile");
		log("");

		// Display local file data
		for(auto& f : localFiles) log(f.path + " " + f.md5, "LocalFile");
		log("");
	}
	void AutoUpdater::runDownload()
	{
		log("Starting...", "Download");
		waitFor(startDownload(toDownload));
	}

	void AutoUpdater::run()
	{
		runGetServerData();
		runDisplayData();

		// Check if the target local folder exists, otherwise create it
		if(!exists(localFolder))
		{
			log("Local folder does not exist, creating");
			mkdir(localFolder.c_str());
		}

		if(!exists(backupFolder))
		{
			log("Backup folder does not exist, creating");
			mkdir(backupFolder.c_str());
		}

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
				if(localFile.md5 == "FOLDER" || localFile.md5 == serverFile.md5) log("<" + serverFile.path + "> matches");
				else
				{
					if(contains(serverOnlyNewFiles, serverFile.path)) log("<" + serverFile.path + "> doesn't match, but won't be downloaded because it exists");
					else
					{
						log("<" + serverFile.path + "> doesn't match, must download");
						toDownload.push_back({serverFile.path, true});
					}
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
		if(!toDownload.empty()) runDownload();

		log("Finished");
	}

	void AutoUpdater::terminateAll() { for(auto& t : memoryManager.getItems()) t->terminate(); memoryManager.cleanUp(); }

	ThreadWrapper& AutoUpdater::startGetJsonRoot(Json::Value& mTargetRoot, const string& mServerFileName)
	{
		auto& thread = memoryManager.create([=, &mTargetRoot]
		{
			log("Getting <" + mServerFileName + "> from server...", "Online");

			Response response{getGetResponse(host, hostFolder, mServerFileName)};
			
			if(response.getStatus() == Response::Ok)
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
		auto& thread = memoryManager.create([=, &mTargetString]
		{
			log("Getting <" + mServerFileName + "> from server...");

			Response response{getGetResponse(host, hostFolder, mServerFileName)};

			if(response.getStatus() == Response::Ok)
			{
				log("<" + mServerFileName + "> got successfully");
				mTargetString = response.getBody();
			}
			else log("Get <" + mServerFileName + "> error");

			log("Finished getting <" + mServerFileName + ">");
		});

		thread.launch(); return thread;
	}

	ThreadWrapper& AutoUpdater::startGetFile(const DownloadData& mDownloadData)
	{
		auto& thread = memoryManager.create([=]
		{
			log("Processing <" + mDownloadData.path + ">");

			if(mDownloadData.existsLocally)
			{
				log("Backing up <" + mDownloadData.path + ">");
				for(auto& folderName : getFolderNames(mDownloadData.path)) if(!exists(backupFolder + folderName)) mkdir((backupFolder + folderName).c_str());
				ofstream ofs{backupFolder + mDownloadData.path, ofstream::binary};
				string backupContents{getFileContents(localFolder + mDownloadData.path)};
				ofs << backupContents;
				ofs.flush(); ofs.close();
			}

			for(auto& folderName : getFolderNames(mDownloadData.path)) if(!exists(localFolder + folderName)) mkdir((localFolder + folderName).c_str());

			string serverContents{""};
			ThreadWrapper& getFileContentsThread(startGetFileContents(serverContents, serverFolder + mDownloadData.path));
			waitFor(getFileContentsThread);
			ofstream ofs{localFolder + mDownloadData.path, ofstream::binary};
			ofs << serverContents;
			ofs.flush(); ofs.close();

			log("Finished processing <" + mDownloadData.path + ">");
		});

		thread.launch(); return thread;
	}

	ThreadWrapper& AutoUpdater::startDownload(const vector<DownloadData>& mToDownload)
	{
		auto& thread = memoryManager.create([=]
		{
			for(auto& td : mToDownload)
			{
				log("Downloading <" + td.path + ">...", "Download");
				waitFor(startGetFile(td));
				log("<" + td.path + "> downloaded", "Download");
				log("");
			}
		});

		thread.launch(); return thread;
	}
}

