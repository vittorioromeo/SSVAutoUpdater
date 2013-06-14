// Copyright (c) 2013 Vittorio Romeo
// License: Academic Free License ("AFL") v. 3.0
// AFL License page: http://opensource.org/licenses/AFL-3.0

#include <string>
#include <SSVUtilsJson/SSVUtilsJson.h>
#include "AutoUpdater.h"

using namespace std;
using namespace ssvau;
using namespace ssvu;
using namespace ssvuj;

int main()
{
	// Get config values from local JSON file
	const Json::Value localConfig	{getRootFromFile("updaterConfig.json")};
	const string localFolder		{as<string>(localConfig, "localFolder")};
	const string host				{as<string>(localConfig, "host")};
	const string hostFolder			{as<string>(localConfig, "hostFolder")};
	const string hostConfig			{as<string>(localConfig, "hostConfig")};
	const string hostScript			{as<string>(localConfig, "hostScript")};

	// Create and run the autoupdater
	AutoUpdater autoUpdater{localFolder, host, hostFolder, hostConfig, hostScript};
	autoUpdater.run();

	// Save log to file
	saveLogToFile("updaterLog.txt");

	return 0;
}
