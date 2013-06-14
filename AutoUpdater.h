// Copyright (c) 2013 Vittorio Romeo
// License: Academic Free License ("AFL") v. 3.0
// AFL License page: http://opensource.org/licenses/AFL-3.0

#ifndef SSVAU_AUTOUPDATER
#define SSVAU_AUTOUPDATER

#include <string>
#include <SFML/Network.hpp>
#include <SSVStart/SSVStart.h>
#include <jsoncpp/json.h>

namespace ssvau
{
	struct FileData { std::string md5, path; bool operator==(const FileData& mRhs) { return path == mRhs.path; } };
	struct DownloadData { std::string path; bool existsLocally; };

	class AutoUpdater
	{
		private:
			ssvu::MemoryManager<ssvs::Utils::ThreadWrapper> memoryManager;
			std::string localFolder, host, hostFolder, hostConfigFile, hostScript, backupFolder, serverFolder;
			Json::Value updaterConfigRoot, serverFilesRoot;
			std::vector<FileData> serverFiles, localFiles;
			std::vector<DownloadData> toDownload;
			std::vector<std::string> serverExcludedFiles, serverExcludedFolders, serverOnlyNewFiles;

			void runGetServerData();
			void runDisplayData();
			void runDownload();
			void terminateAll();
			
			ssvs::Utils::ThreadWrapper& startGetJsonRoot(Json::Value& mTargetRoot, const std::string& mServerFileName);
			ssvs::Utils::ThreadWrapper& startGetFileContents(std::string& mTargetString, const std::string& mServerFileName);
			ssvs::Utils::ThreadWrapper& startGetFile(const DownloadData& mDownloadData);
			ssvs::Utils::ThreadWrapper& startDownload(const std::vector<DownloadData>& mToDownload);

		public:
			AutoUpdater(const std::string& mLocalFolder, const std::string& mHost, const std::string& mHostFolder, const std::string& mHostConfig, const std::string& mHostScript);
			~AutoUpdater();

			void run();
	};
}

#endif
