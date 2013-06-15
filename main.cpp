// Copyright (c) 2013 Vittorio Romeo
// License: Academic Free License ("AFL") v. 3.0
// AFL License page: http://opensource.org/licenses/AFL-3.0

#include <string>
#include <vector>
#include <fstream>
#include <SFML/Network.hpp>
#include <SSVUtils/SSVUtils.h>
#include <SSVUtilsJson/SSVUtilsJson.h>

using namespace std;
using namespace sf;
using namespace ssvu;
using namespace ssvu::Encryption;
using namespace ssvu::FileSystem;
using namespace ssvuj;

struct Target { string remoteFolder, localFolder; };
struct DownloadData { bool localExists; string remotePath, remoteMD5, localPath, localMD5; };

string backupFolder{"_BACKUP/"}, host, hostMainFolder, hostMainConfig, hostMainScript, remoteDataFolder;
vector<string> remoteExcludedFiles, remoteExcludedFolders, remoteOnlyNewFiles;
vector<Target> targets;
vector<DownloadData> downloadDatas;

vector<string> getFolderNames(const string& mPath)
{
	vector<string> splitted{getSplit<char, SplitMode::KeepSeparator>(mPath, '/')}, result;
	string& lastSplitted(splitted[splitted.size() - 1]);
	auto sizeToSearch(splitted.size());
	if(lastSplitted[lastSplitted.length() - 1] != '/') --sizeToSearch;

	for(unsigned int i{0}; i < sizeToSearch; ++i)
	{
		if(splitted[i] == "/") continue;
		string toPush{""};
		for(unsigned int j{0}; j < i; ++j) toPush.append(splitted[j]);
		toPush.append(splitted[i] + "/");
		result.push_back(getNormalizedPath(toPush));
	}

	return result;
}

string downloadFileContents(const string& mRemotePath)
{
	log("Getting <" + mRemotePath + "> from server...", "downloadFileContents");
	Http::Response response{Http(host).sendRequest(mRemotePath)};
	if(response.getStatus() == Http::Response::Ok) { log("<" + mRemotePath + "> got successfully", "downloadFileContents"); return response.getBody(); }
	log("Get <" + mRemotePath + "> error", "downloadFileContents"); return "";
}

void download(const DownloadData& mDownloadData)
{
	log("Processing <" + mDownloadData.localPath + ">", "download");

	if(mDownloadData.localExists)
	{
		log("Backing up <" + mDownloadData.localPath + "> to <" + backupFolder + mDownloadData.localPath + ">", "download");
		if(!exists(backupFolder)) createFolder(backupFolder);
		for(auto& f : getFolderNames(mDownloadData.localPath)) if(!exists(backupFolder + f)) createFolder(backupFolder + f);
		ofstream ofs{backupFolder + mDownloadData.localPath, ofstream::binary};
		ofs << getFileContents(mDownloadData.localPath); ofs.flush(); ofs.close();
	}

	for(auto& f : getFolderNames(mDownloadData.localPath)) if(!exists(f)) { log("Creating folder <" + f + ">", "download"); createFolder(f); }

	ofstream ofs{mDownloadData.localPath, ofstream::binary};
	ofs << downloadFileContents(hostMainFolder + mDownloadData.remotePath); ofs.flush(); ofs.close();
	log("Finished processing <" + mDownloadData.localPath + ">", "download");
}

void loadLocalConfig()
{
	log(""); log("loading local config...", "loadLocalConfig"); log("");

	const Json::Value localConfig{getRootFromFile("updaterConfig.json")};
	host			= as<string>(localConfig, "host"); log("host: <" + host + ">", "loadLocalConfig");
	hostMainFolder	= as<string>(localConfig, "hostMainFolder"); log("hostMainFolder: <" + hostMainFolder + ">", "loadLocalConfig");
	hostMainConfig	= as<string>(localConfig, "hostMainConfig"); log("hostMainConfig: <" + hostMainConfig + ">", "loadLocalConfig");
	hostMainScript	= as<string>(localConfig, "hostMainScript"); log("hostMainScript: <" + hostMainScript + ">", "loadLocalConfig");

	for(const auto& t : localConfig["targets"])
	{
		auto remoteFolder(as<string>(t, "remoteFolder")), localFolder(as<string>(t, "localFolder"));
		targets.push_back({getNormalizedPath(remoteFolder), getNormalizedPath(localFolder)});
		log("target: <" + remoteFolder + "> -> <" + localFolder + ">", "loadLocalConfig");
	}
}

void loadRemoteConfig()
{
	log(""); log("loading remote config...", "loadRemoteConfig"); log("");

	const Json::Value remoteConfig{getRootFromString(downloadFileContents(hostMainFolder + hostMainConfig))};
	remoteDataFolder = as<string>(remoteConfig, "dataFolder"); log("remoteDataFolder: <" + remoteDataFolder + ">", "loadRemoteConfig");
	for(const auto& f : as<vector<string>>(remoteConfig, "excludedFiles"))		{ remoteExcludedFiles.push_back(f);		log("remoteExcludedFile: <" + f + ">", "loadRemoteConfig"); }
	for(const auto& f : as<vector<string>>(remoteConfig, "excludedFolders"))	{ remoteExcludedFolders.push_back(f);	log("remoteExcludedFolder: <" + f + ">", "loadRemoteConfig"); }
	for(const auto& f : as<vector<string>>(remoteConfig, "onlyNewFiles"))		{ remoteOnlyNewFiles.push_back(f);		log("remoteOnlyNewFile: <" + f + ">", "loadRemoteConfig"); }
}

void loadRemoteScript()
{
	log(""); log("loading remote script...", "loadRemoteScript"); log("");

	const Json::Value remoteScriptResult{getRootFromString(downloadFileContents(hostMainFolder + hostMainScript))};

	for(auto& f : remoteScriptResult)
	{
		auto remotePath(as<string>(f, "path")), remoteMD5(as<string>(f, "md5"));
		log("remoteFiles: <" + remotePath + "> <" + remoteMD5 + ">", "loadRemoteScript");

		string localPath{getReplaced(remotePath, remoteDataFolder, "")}, localMD5{""};
		for(const auto& t : targets) replace(localPath, t.remoteFolder, t.localFolder);
		bool localExists{exists(localPath)};

		if(isFolder(localPath)) continue;
		if(localExists) { localMD5 = encrypt<Encryption::Type::MD5>(getFileContents(localPath)); log("localFiles: <" + localPath + "> <" + localMD5 + ">", "loadRemoteScript"); }
		else log("localFiles: <" + localPath + "> <does not exist>", "loadRemoteScript");

		downloadDatas.push_back({localExists, remotePath, remoteMD5, localPath, localMD5});
	}
}

void processDownloads()
{
	log(""); log("processing downloads...", "processDownloads"); log("");

	for(const auto& d : downloadDatas)
	{
		if(d.localExists && d.localMD5 == d.remoteMD5) log("no need to update: <" + d.localPath + ">", "processDownloads");
		else if(d.localExists && contains(remoteOnlyNewFiles, d.remotePath)) log("<" + d.localPath + "> doesn't match, but won't be downloaded because it exists", "processDownloads");
		else download(d);
	}
}

int main() { loadLocalConfig(); loadRemoteConfig(); loadRemoteScript(); processDownloads(); saveLogToFile("updaterLog.txt"); return 0; }
