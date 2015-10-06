// Copyright (c) 2013-2015 Vittorio Romeo
// License: Academic Free License ("AFL") v. 3.0
// AFL License page: http://opensource.org/licenses/AFL-3.0

#include <SFML/Network.hpp>
#include <SSVUtils/Core/Core.hpp>
#include <SSVUtils/Encryption/Encryption.hpp>
#include <SSVUtils/Json/Json.hpp>

using namespace std;
using namespace sf;
using namespace ssvu;
using namespace ssvu::Encryption;
using namespace ssvu::FileSystem;
using namespace ssvj;

struct Target
{
    ssvu::FileSystem::Path remoteFolder, localFolder;
    inline Target(const ssvu::FileSystem::Path& mRemoteFolder,
        const ssvu::FileSystem::Path& mLocalFolder)
        : remoteFolder{mRemoteFolder}, localFolder{mLocalFolder}
    {
    }
};
struct DownloadData
{
    bool localExists;
    ssvu::FileSystem::Path remotePath, localPath;
    std::string remoteMD5, localMD5;
};

std::string backupFolder{"_BACKUP/"}, host, hostMainFolder, hostMainConfig,
    hostMainScript, remoteDataFolder;
std::vector<ssvu::FileSystem::Path> remoteExcludedFiles, remoteExcludedFolders,
    remoteOnlyNewFiles;
std::vector<Target> targets;
std::vector<DownloadData> downloadDatas;

std::vector<ssvu::FileSystem::Path> getFolderNames(
    const ssvu::FileSystem::Path& mPath)
{
    return ssvu::FileSystem::getScan<ssvu::FileSystem::Mode::Recurse,
        ssvu::FileSystem::Type::Folder>(mPath);
}

std::string downloadFileContents(const ssvu::FileSystem::Path& mRemotePath)
{
    ssvu::lo("downloadFileContents")
        << "Getting <" + mRemotePath.getStr() + "> from server...\n";
    sf::Http::Response response{
        sf::Http(host).sendRequest(mRemotePath.getStr())};
    if(response.getStatus() == sf::Http::Response::Ok)
    {
        ssvu::lo("downloadFileContents")
            << "<" + mRemotePath.getStr() + "> got successfully\n";
        return response.getBody();
    }
    ssvu::lo("downloadFileContents")
        << "Get <" + mRemotePath.getStr() + "> error\n";
    return "";
}

void download(const DownloadData& mDownloadData)
{
    ssvu::lo("download") << "Processing <" + mDownloadData.localPath + ">";

    if(mDownloadData.localExists)
    {
        ssvu::lo("download") << "Backing up <" + mDownloadData.localPath +
                                    "> to <" + backupFolder +
                                    mDownloadData.localPath + ">\n";
        if(!ssvufs::Path{backupFolder}.exists<ssvufs::Type::Folder>())
            ssvu::FileSystem::createFolder(backupFolder);
        for(const auto& f : getFolderNames(mDownloadData.localPath))
            if(!ssvufs::Path{backupFolder + f}.exists<ssvufs::Type::Folder>())
                ssvu::FileSystem::createFolder(backupFolder + f);
        std::ofstream ofs{
            backupFolder + mDownloadData.localPath, std::ofstream::binary};
        ofs << mDownloadData.localPath.getContentsAsStr();
        ofs.flush();
        ofs.close();
    }

    for(auto& f : getFolderNames(mDownloadData.localPath))
        if(!f.exists<ssvufs::Type::Folder>())
        {
            ssvu::lo("download") << "Creating folder <" + f + ">";
            ssvu::FileSystem::createFolder(f);
        }

    std::ofstream ofs{mDownloadData.localPath, std::ofstream::binary};
    ofs << downloadFileContents(hostMainFolder + mDownloadData.remotePath);
    ofs.flush();
    ofs.close();
    ssvu::lo("download") << "Finished processing <" + mDownloadData.localPath +
                                ">\n";
}

void loadLocalConfig()
{
    ssvu::lo("loadLocalConfig") << "loading local config...\n\n";

    const auto localConfig(fromFile("updaterConfig.json"));
    host = localConfig["host"].as<Str>();
    ssvu::lo("loadLocalConfig") << "host: <" + host + ">\n";
    hostMainFolder = localConfig["hostMainFolder"].as<Str>();
    ssvu::lo("loadLocalConfig") << "hostMainFolder: <" + hostMainFolder + ">\n";
    hostMainConfig = localConfig["hostMainConfig"].as<Str>();
    ssvu::lo("loadLocalConfig") << "hostMainConfig: <" + hostMainConfig + ">\n";
    hostMainScript = localConfig["hostMainScript"].as<Str>();
    ssvu::lo("loadLocalConfig") << "hostMainScript: <" + hostMainScript + ">\n";

    for(const auto& t : localConfig["targets"].forArr())
    {
        auto remoteFolder(t["remoteFolder"].as<Str>()),
            localFolder(t["localFolder"].as<Str>());
        targets.emplace_back(remoteFolder, localFolder);
        ssvu::lo("loadLocalConfig")
            << "target: <" + remoteFolder + "> -> <" + localFolder + ">\n";
    }
}

void loadRemoteConfig()
{
    ssvu::lo("loadRemoteConfig") << "loading remote config...\n\n";

    const auto remoteConfig(
        fromStr(downloadFileContents(hostMainFolder + hostMainConfig)));
    remoteDataFolder = remoteConfig["dataFolder"].as<Str>();
    ssvu::lo("loadRemoteConfig") << "remoteDataFolder" + remoteDataFolder
                                 << "\n";
    for(const auto& f : remoteConfig["excludedFiles"].forArrAs<Str>())
    {
        remoteExcludedFiles.emplace_back(f);
        ssvu::lo("loadRemoteConfig") << "remoteExcludedFile: <" + f + ">\n";
    }
    for(const auto& f : remoteConfig["excludedFolders"].forArrAs<Str>())
    {
        remoteExcludedFolders.emplace_back(f);
        ssvu::lo("loadRemoteConfig") << "remoteExcludedFolder: <" + f + ">\n";
    }
    for(const auto& f : remoteConfig["onlyNewFiles"].forArrAs<Str>())
    {
        remoteOnlyNewFiles.emplace_back(f);
        ssvu::lo("loadRemoteConfig") << "remoteOnlyNewFile: <" + f + ">\n";
    }
}

void loadRemoteScript()
{
    ssvu::lo("loadRemoteScript") << "loading remote script...\n";

    const auto remoteScriptResult(
        fromStr(downloadFileContents(hostMainFolder + hostMainScript)));

    for(auto& f : remoteScriptResult.forArr())
    {
        auto remotePath(f["path"].as<Str>()), remoteMD5(f["md5"].as<Str>());
        ssvu::lo("loadRemoteScript")
            << "remoteFiles: <" + remotePath + "> <" + remoteMD5 + ">\n";

        ssvu::FileSystem::Path localPath{
            ssvu::getReplaced(remotePath, remoteDataFolder, "")};
        std::string localMD5;

        for(const auto& t : targets)
            localPath = Path(ssvu::getReplaced(localPath.getStr(),
                t.remoteFolder.getStr(), t.localFolder.getStr()));
        bool localExists{localPath.exists<ssvufs::Type::All>()};

        if(localPath.exists<ssvufs::Type::Folder>()) continue;
        if(localExists)
        {
            localMD5 = ssvu::Encryption::encrypt<ssvu::Encryption::Type::MD5>(
                localPath.getContentsAsStr());
            ssvu::lo("loadRemoteScript")
                << "localFiles: <" + localPath + "> <" + localMD5 + ">\n";
        }
        else
            ssvu::lo("loadRemoteScript")
                << "localFiles: <" + localPath + "> <does not exist>\n";

        downloadDatas.push_back(
            {localExists, remotePath, remoteMD5, localPath, localMD5});
    }
}

void processDownloads()
{
    ssvu::lo("processDownloads") << "processing downloads...\n";

    for(const auto& d : downloadDatas)
    {
        if(d.localExists && d.localMD5 == d.remoteMD5)
            ssvu::lo("processDownloads")
                << "no need to update: <" + d.localPath + ">\n";
        else if(d.localExists &&
                ssvu::contains(remoteOnlyNewFiles, d.remotePath))
            ssvu::lo("processDownloads") << "<" + d.localPath +
                                                "> doesn't match, but won't be "
                                                "downloaded because it "
                                                "exists\n";
        else
            download(d);
    }
}

int main()
{
    loadLocalConfig();
    loadRemoteConfig();
    loadRemoteScript();
    processDownloads();
    saveLogToFile("updaterLog.txt");
    ssvu::lo().flush();
    return 0;
}
