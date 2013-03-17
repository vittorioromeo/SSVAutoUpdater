// Copyright (c) 2013 Vittorio Romeo
// License: Academic Free License ("AFL") v. 3.0
// AFL License page: http://opensource.org/licenses/AFL-3.0

#include "AutoUpdater.h"

int main()
{
	ssvau::AutoUpdater autoUpdater{"http://vittorioromeo.info", "Misc/testUpdater/", "LocalData/"};
	autoUpdater.run();
}
