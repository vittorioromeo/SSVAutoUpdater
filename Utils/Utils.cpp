// Copyright (c) 2013 Vittorio Romeo
// License: Academic Free License ("AFL") v. 3.0
// AFL License page: http://opensource.org/licenses/AFL-3.0

#include <SSVUtils.h>
#include "Utils/Utils.h"
#include "Utils/MD5.h"

using namespace std;
using namespace ssvu::Utils;

namespace ssvau
{
	namespace Utils
	{
		string getMD5Hash(const string& mString) { return MD5{mString}.GetHash(); }
		
		vector<string> getFolderNames(const string& mPath)
		{
			vector<string> splitted{split(mPath, '/', true)}, result;
			string& lastSplitted(splitted[splitted.size() - 1]);
			unsigned int sizeToSearch{splitted.size()};
			if(lastSplitted[lastSplitted.length() - 1] != '/') --sizeToSearch;

			for(unsigned int i{0}; i < sizeToSearch; ++i)
			{
				if(splitted[i] == "/") continue;
				string toPush{""};
				for(unsigned int j{0}; j < i; ++j) toPush.append(splitted[j]);
				toPush.append(splitted[i] + "/");
				result.push_back(toPush);
			}

			return result;
		}
	}
}
