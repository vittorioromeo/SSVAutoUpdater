// Copyright (c) 2013 Vittorio Romeo
// License: Academic Free License ("AFL") v. 3.0
// AFL License page: http://opensource.org/licenses/AFL-3.0

#include <json/json.h>
#include <json/reader.h>
#include <dirent.h>
#include <sys/stat.h>
#include <fstream>
#include "Utils/Utils.h"

using namespace std;
using namespace sf;
using namespace ssvs;
using namespace ssvs::Utils;

namespace ssvau
{
	namespace Utils
	{
		using namespace Json;

		string getFileContents(const string& mPath)
		{
			FILE* fptr{fopen(mPath.c_str(), "rb")};
			fseek(fptr, 0, SEEK_END);
			size_t fsize(ftell(fptr));
			fseek(fptr, 0, SEEK_SET);
			string content; content.resize(fsize);
			if(fread(const_cast<char*>(content.c_str()), 1, fsize, fptr) != fsize) log("Error: " + mPath, "File loading");
			fclose(fptr); return content;
		}

		template<> int getValue(const Value& mRoot, const string& mValue) 			{ return mRoot[mValue].asInt(); }
		template<> float getValue(const Value& mRoot, const string& mValue) 		{ return mRoot[mValue].asFloat(); }
		template<> bool getValue(const Value& mRoot, const string& mValue) 			{ return mRoot[mValue].asBool(); }
		template<> string getValue(const Value& mRoot, const string& mValue) 		{ return mRoot[mValue].asString(); }
		template<> char const* getValue(const Value& mRoot, const string& mValue)	{ return mRoot[mValue].asCString(); }

		Value getRootFromFile(const string& mPath) { return getRootFromString(getFileContents(mPath)); }
		Value getRootFromString(const string& mString)
		{
			Value result; Reader reader;
			if(!reader.parse(mString, result, false)) log(reader.getFormatedErrorMessages() + "\n" + "From: [" + mString + "]", "JSON Error");
			return result;
		}

		vector<string> getStringArray(const Value& mRoot, const string& mValue)
		{
			vector<string> result;
			for(auto& item : mRoot[mValue]) result.push_back(item.asString());
			return result;
		}
	}
}
