//
// utility library
//
#pragma once

#ifndef OUT
	#define OUT
#endif

using namespace std;


string& trim(string& str);
string& lowerCase(string& str);

void tokenizer(const string& str, const string& delimeter, const string& ignoreStr
	, OUT vector<string>& out);

void tokenizer_space(const char* str, OUT vector<string>& out);

bool CollectFiles(const list<string>& findExt, const string& searchPath, OUT list<string>& out);
