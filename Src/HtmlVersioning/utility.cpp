
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <algorithm>
#include <assert.h>
#include <list>
#include <shlwapi.h> // filesystem library
#pragma comment(lib, "shlwapi")
#include "utility.h"

using namespace std;


template <class Seq>
void removevector2(Seq& seq, const unsigned int idx)
{
	if (seq.size() > idx)
		seq.erase(seq.begin() + idx);
}


// "  skdfjskdjf  "
// "$$$skdfjskdjf$$$"
// "skdfjskdjf"
string& trim(string& str)
{
	// search front
	for (int i = 0; i < (int)str.length(); ++i)
	{
		if ((str[i] == '\n') || (str[i] == '\t') || (str[i] == '\r') || (str[i] == ' '))
		{
			removevector2(str, i);
			--i;
		}
		else
			break;
	}

	// search back
	for (int i = (int)str.length() - 1; i >= 0; --i)
	{
		if ((str[i] == '\n') || (str[i] == '\t') || (str[i] == '\r') || (str[i] == ' '))
		{
			removevector2(str, i);
		}
		else
			break;
	}

	return str;
}
string trim2(const string& str)
{
	string val = str;
	trim(val);
	return val;
}


string& lowerCase(string& str)
{
	std::transform(str.begin(), str.end(), str.begin(), tolower);
	return str;
}


void tokenizer(const string& str, const string& delimeter
	, const string& ignoreStr, OUT vector<string>& out)
{
	string tmp = str;
	int offset = 0;
	int first = 0;

	while (!tmp.empty())
	{
		const int pos = (int)tmp.find(delimeter, offset);
		if (string::npos == pos)
		{
			string tok = trim2(tmp.substr(first));
			if (!tok.empty())
				out.push_back(tok);
			break;
		}
		else
		{
			const string tok = tmp.substr(offset, pos - offset);
			offset += (int)tok.length() + (int)delimeter.length();
			if (tok != ignoreStr)
			{
				out.push_back(tmp.substr(first, pos - first));
				first = offset;
			}
		}
	}
}


void tokenizer_space(const char* str, OUT vector<string>& out)
{
	int state = 0;
	string tok;
	const char* c = str;
	while (*c)
	{
		if (0 == state)
		{
			switch (*c)
			{
			case '\n':
			case ' ':
			case '\t':
			case '\r':
			case ',':
				if (!tok.empty())
				{
					out.push_back(tok);
					tok.clear();
				}
				break;

			case '\"':
				state = 1;
				break;

			default:
				tok += *c;
				break;
			}
		}
		else if (1 == state)
		{
			if (*c == '\"')
			{
				out.push_back(tok);
				tok.clear();
				state = 0;
			}
			else
			{
				tok += *c;
			}
		}
		else
		{
			assert(0);
		}

		++c;
	}

	if (!tok.empty())
		out.push_back(tok);
}


bool CompareExtendName(const char* srcFileName, const int srcStringMaxLength
	, const char* compareExtendName)
{
	const int len = (int)strnlen_s(srcFileName, srcStringMaxLength);
	if (len <= 0)
		return FALSE;

	const int TEMPSIZE = 16;
	int count = 0;
	char temp[TEMPSIZE];
	for (int i = 0; i < len && i < (ARRAYSIZE(temp) - 1); ++i)
	{
		const char c = srcFileName[len - i - 1];
		temp[count++] = c;
		if ('.' == c)
			break;
	}
	temp[count] = NULL;

	char extendName[TEMPSIZE];
	for (int i = 0; i < count; ++i)
		extendName[i] = temp[count - i - 1];
	extendName[count] = NULL;

	return !strcmp(extendName, compareExtendName);
}


bool CollectFiles(const list<string>& findExt, const string& searchPath
	, OUT list<string>& out)
{
	string modifySearchPath;
	if (!searchPath.empty() &&
		(searchPath[searchPath.size() - 1] == '/') || (searchPath[searchPath.size() - 1] == '\\'))
	{
		modifySearchPath = searchPath;
	}
	else
	{
		modifySearchPath = searchPath.empty() ? "" : searchPath + "\\";
	}

	WIN32_FIND_DATAA fd;
	const string searchDir = modifySearchPath + "*.*";
	HANDLE hFind = FindFirstFileA(searchDir.c_str(), &fd);

	while (1)
	{
		if (fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
		{
			if ((string(".") != fd.cFileName) && (string("..") != fd.cFileName))
			{
				CollectFiles(findExt, modifySearchPath + string(fd.cFileName) + "\\", out);
			}
		}
		else if (fd.dwFileAttributes & FILE_ATTRIBUTE_ARCHIVE)
		{
			const string fileName = fd.cFileName;

			if (findExt.empty())
			{
				out.push_back(modifySearchPath + fileName);
			}
			else
			{
				auto it = findExt.begin();
				while (findExt.end() != it)
				{
					if (CompareExtendName(fileName.c_str(), (int)fileName.length(), it->c_str()))
					{
						out.push_back(modifySearchPath + fileName);
						break;
					}
					++it;
				}
			}
		}

		if (!FindNextFileA(hFind, &fd))
			break;
	}

	FindClose(hFind);

	return true;
}
