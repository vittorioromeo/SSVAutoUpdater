// Copyright (c) 2013 Vittorio Romeo
// License: Academic Free License ("AFL") v. 3.0
// AFL License page: http://opensource.org/licenses/AFL-3.0

#ifndef AUTOUPDATER_UTILS
#define AUTOUPDATER_UTILS

#include <string>
#include <sstream>
#include <unordered_set>
#include <json/json.h>
#include <json/reader.h>
#include <SSVStart.h>

namespace ssvau
{
	namespace Utils
	{
		std::string getFileContents(const std::string& mPath);
		template<typename T> T getValue(const Json::Value& mRoot, const std::string& mValue);
		template<typename T> T getValueOrDefault(const Json::Value& mRoot, const std::string& mValue, T mDefault) { return mRoot.isMember(mValue) ? getValue<T>(mRoot, mValue) : mDefault; }
		Json::Value getRootFromFile(const std::string& mPath);
		Json::Value getRootFromString(const std::string& mString);
		std::vector<std::string> getStringArray(const Json::Value& mRoot, const std::string& mValue);
	}
}

#endif
