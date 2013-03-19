// Copyright (c) 2013 Vittorio Romeo
// License: Academic Free License ("AFL") v. 3.0
// AFL License page: http://opensource.org/licenses/AFL-3.0

#ifndef SSVAU_AUTOUPDATER
#define SSVAU_AUTOUPDATER

#include <string>
#include <SFML/Network.hpp>
#include <SSVStart.h>
#include <json/json.h>

namespace ssvau
{
	struct FileData { std::string md5, path; bool operator==(const FileData& mRhs) { return path == mRhs.path; } };
	struct DownloadData { std::string path; bool existsLocally; };

	class AutoUpdater
	{
		private:
			ssvu::MemoryManager<ssvs::Utils::ThreadWrapper> memoryManager;
			std::string host, hostFolder, localFolder, serverFolder, hostConfigFile{"updaterConfig.json"}, hostScript{"updaterGetFiles.php"}, backupFolder;
			Json::Value updaterConfigRoot, serverFilesRoot;
			std::vector<FileData> serverFiles, localFiles;
			std::vector<DownloadData> toDownload;
			std::vector<std::string> serverExcludedFiles, serverExcludedFolders;

			void runGetServerData();
			void runDisplayData();
			void runDownload();
			void terminateAll();
			
			ssvs::Utils::ThreadWrapper& startGetJsonRoot(Json::Value& mTargetRoot, const std::string& mServerFileName);
			ssvs::Utils::ThreadWrapper& startGetFileContents(std::string& mTargetString, const std::string& mServerFileName);
			ssvs::Utils::ThreadWrapper& startGetFile(const DownloadData& mDownloadData);
			ssvs::Utils::ThreadWrapper& startDownload(const std::vector<DownloadData>& mToDownload);

		public:
			AutoUpdater(const std::string& mHost, const std::string& mHostFolder, const std::string& mLocalFolder);
			~AutoUpdater();

			void run();
	};
}

#endif
