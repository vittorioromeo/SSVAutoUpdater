// Copyright (c) 2013 Vittorio Romeo
// License: Academic Free License ("AFL") v. 3.0
// AFL License page: http://opensource.org/licenses/AFL-3.0

#ifndef SSVAU_AUTOUPDATER
#define SSVAU_AUTOUPDATER

#include <string>
#include <SFML/Network.hpp>
#include <SSVStart.h>
#include <json/json.h>
#include "Utils/ThreadWrapper.h"

namespace ssvau
{
	struct FileData { std::string md5, path, fullPath; bool operator==(const FileData& mRhs) { return path == mRhs.path; } };
	struct DownloadData { std::string path; bool existsLocally; };

	class AutoUpdater
	{
		private:
			ssvs::Utils::MemoryManager<ssvs::Utils::ThreadWrapper> memoryManager;
			std::string host, hostFolder, localFolder, serverFolder;
			std::string hostConfigFile{"updaterConfig.json"};
			std::string hostScript{"getFiles.php"};

			void runDownload(const std::vector<DownloadData>& mToDownload);

			void cleanUp();
			void terminateAll();
			std::string getMD5Hash(const std::string& mString);
			sf::Http::Response getGetResponse(const std::string& mRequestFile);
			sf::Http::Response getPostResponse(const std::string& mRequestFile, const std::string& mBody);
			void waitFor(ssvs::Utils::ThreadWrapper& mThreadWrapper);
			std::string getChild(const std::string& mFolderName, const std::string& mPath);
			std::vector<std::string> getFolderNames(const std::string& mPath);
			
			ssvs::Utils::ThreadWrapper& startGetJsonRoot(Json::Value& mTargetRoot, const std::string& mServerFileName);
			ssvs::Utils::ThreadWrapper& startGetFileContents(std::string& mTargetString, const std::string& mServerFileName);
			ssvs::Utils::ThreadWrapper& startGetFile(const std::string& mServerFolder, const std::string& mLocalFolder, const DownloadData& mDownloadData);
			ssvs::Utils::ThreadWrapper& startDownload(const std::string& mServerFolder, const std::string& mLocalFolder, const std::vector<DownloadData>& mToDownload);

		public:
			AutoUpdater(const std::string& mHost, const std::string& mHostFolder, const std::string& mLocalFolder);
			~AutoUpdater();

			void run();
	};
}

#endif
