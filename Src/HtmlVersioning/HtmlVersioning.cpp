//
// 2022-08-23, jjuiddong
//
// HTML Source Code Version Program
// css, js link file versioning
// ex) ./index.css -> ./index.css?ver=1.0.1
//     ./header.js -> ./header.js?ver=1.0.2
//
// process sequence
// 
// 1. read version.txt file
//    - version.txt
//      - ./index.css   1.0.0
//      - ./header.js   1.0.2
// 2. read *.html file in current directory
// 3. parse all html file
// 4. find filename contain version.txt files
//  - <link ... href=* >
//  - <script ... src=* >
// 5. change css, js filename to <filename>+<?ver=version>
//

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <list>
#include "utility.h"

using namespace std;
typedef unsigned int uint;

uint CheckAndReplaceFileName(string &src, const uint cursor
    , const string &pred, const string &srcStr, const string &replaceStr);


int main()
{
    struct sVerFile
    {
        string type; // 'css' or 'js'
        string filename;
        string ver;        
    };

    // read version.txt
    vector<sVerFile> verFiles;
    {
        ifstream ifs("version.txt");
        if (!ifs.is_open())
        {
            cout << "not found version file" << endl;
            return 0;
        }

        string line;
        while (getline(ifs, line))
        {
            trim(line);
            if (line.empty())
                continue;

            vector<string> toks;
            tokenizer_space(line.c_str(), toks);
            if (toks.size() < 2)
                continue;

            sVerFile ver;
            ver.filename = toks[0];
            ver.ver = toks[1];

            string name = toks[0];
            lowerCase(name);
            if ((name.size() > 4)
                && (name[name.size() - 3] == 'c')
                && (name[name.size() - 2] == 's')
                && (name[name.size() - 1] == 's')
                )
            {
                ver.type = "css";
            }
            else if ((name.size() > 3)
                && (name[name.size() - 2] == 'j')
                && (name[name.size() - 1] == 's')
                )
            {
                ver.type = "js";
            }
            else
            {
                continue; // error, no *.css, *.js file
            }

            verFiles.push_back(ver);
        }
    }

    if (verFiles.empty())
    {
        cout << "no has version file" << endl;
        return 0;
    }

    // find *.html file in current directory
    list<string> exts;
    exts.push_back(".html");
    list<string> htmlFiles;
    CollectFiles(exts, "./", htmlFiles);

    if (htmlFiles.empty())
    {
        cout << "not found *.html files" << endl;
        return 0;
    }

    // parse html file
    for (auto& htmlFileName: htmlFiles)
    {
        ifstream ifs(htmlFileName);
        if (!ifs.is_open())
            continue;

        stringstream ss;
        ss << ifs.rdbuf();
        ifs.close();
        string str = ss.str();

        // replace *.css, *.js filename
        bool isChange = false;
        for (auto& v : verFiles)
        {
            const string replaceFileName = v.filename + "?ver=" + v.ver;
            uint cursor = 0;
            while (1)
            {
                // search *.css, *.js file
                //  - <link ... href=* >
                //  - <script ... src=* >
                uint c = str.find(v.filename, cursor);
                if (string::npos == c)
                    break;

                if (v.type == "css")
                    cursor = CheckAndReplaceFileName(str, c, "href", v.filename, replaceFileName);
                else
                    cursor = CheckAndReplaceFileName(str, c, "src", v.filename, replaceFileName);
                isChange = true;
            }
        }

        if (isChange)
        {
            // update *.html file
            ofstream ofs(htmlFileName);
            if (ofs.is_open())
               ofs << str;
            cout << "update " << htmlFileName << endl;
        }
    }

    cout << "finish" << endl;
    return 1;
}


// replace filename
// src: html code
// return: next cursor pos
uint CheckAndReplaceFileName(string& src, const uint cursor
    , const string& pred, const string &srcStr, const string& replaceStr)
{
    uint srcStrLen = srcStr.size();
    // check double quote
    if (cursor == 0)
        return cursor + 1; // error occurred!
    if ('"' != src[cursor - 1])
        return cursor + 1; // error occurred!
    const uint next = cursor + srcStr.size();
    if (next >= src.size())
        return cursor + 1; // error occurred!
    if ('?' == src[next])
    {
        //srcStrLen
        const uint nextQuote = src.find("\"", cursor);
        if (string::npos == nextQuote)
            return cursor + 1; // error occurred!
        if (nextQuote - (cursor + srcStr.size()) > 20)
            return cursor + 1; // error occurred!
        srcStrLen = nextQuote - cursor;
    }
    //~

    // search pred = "~~
    int state = 0;
    uint cnt = 0; // exception process
    int c = (int)cursor - 2;
    string tok;
    while ((c >= 0) && (cnt < 100))
    {
        // find '='
        const char t = src[c];
        if (0 == state)
        {
            switch (t)
            {
            case '=':
                state = 1;
                break;
            case '\t':
            case '\r':
            case '\n':
            case ' ':
                break;

            default:
                return cursor + 1; // error
            }
        }
        else
        {
            switch (t)
            {
            case '\t':
            case '\r':
            case '\n':
            case ' ':
                if (1 == state)
                {
                    state = 2;
                }
                else
                {
                    std::reverse(tok.begin(), tok.end());
                    if (pred == tok)
                    {
                        // replace
                        const string s1 = src.substr(0, cursor);
                        const string s2 = src.substr(cursor + srcStrLen);
                        src = s1 + replaceStr + s2;
                        return cursor + replaceStr.size(); // finish
                    }
                    else
                    {
                        return cursor + 1; // not match pred token
                    }
                }
                break;

            default:
                if (1 == state)
                    state = 2;
                tok += t;
                break;
            }
        }
        --c;
        ++cnt;
    }

    return cursor + 1;
}
