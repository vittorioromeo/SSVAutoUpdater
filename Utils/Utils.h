// Copyright (c) 2013 Vittorio Romeo
// License: Academic Free License ("AFL") v. 3.0
// AFL License page: http://opensource.org/licenses/AFL-3.0

#ifndef AUTOUPDATER_UTILS
#define AUTOUPDATER_UTILS

#include <string>
#include <sstream>

#include <SSVStart.h>
#include <SFML/Network.hpp>

namespace ssvau
{
	namespace Utils
	{
		std::string getMD5Hash(const std::string& mString);
		std::vector<std::string> getFolderNames(const std::string& mPath);
	}
}

#endif
