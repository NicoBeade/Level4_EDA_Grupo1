/**
 * @file EDAoogleHttpRequestHandler.h
 * @author Marc S. Ressl
 * @authors Nicolás Beade - Franco Dorfman - Vito Pensa Piccolo - Federico Gentile
 * @brief EDAoggle search engine
 * @version 0.2
 *
 * @copyright Copyright (c) 2022-2023
 *
 */

#ifndef EDAOOGLEHTTPREQUESTHANDLER_H
#define EDAOOGLEHTTPREQUESTHANDLER_H

#include "ServeHttpRequestHandler.h"

#include <iostream>
#include <string>
#include <fstream>
#include <chrono>
#include <filesystem>
#include <unordered_map>
#include <algorithm>
#include <sstream>
#include <codecvt>

#include <microhttpd.h>
#include <sqlite3.h>

#include "HttpServer.h"
#include "EDAoogleHttpRequestHandler.h"

using namespace std;

#define HEADER 1
#define NON_HEADER 2

#ifdef WIN32
#define PATH_CORRECTION "..\\..\\"
#define PATH_CORRECTION_HTML "..\\..\\www\\wiki\\"
#define EXTRA_CHARACTERS_IN_PATH 10
#define DB_NAME "wiki.db"

#else
#define PATH_CORRECTION "../"
#define PATH_CORRECTION_HTML "../www/wiki/"
#define EXTRA_CHARACTERS_IN_PATH 7
#endif

class EDAoogleHttpRequestHandler : public ServeHttpRequestHandler
{
public:
    EDAoogleHttpRequestHandler(string homePath);
    bool handleRequest(string url, HttpArguments arguments, vector<char> &response);

private:
    /*String Management*/
    wstring EDAoogleHttpRequestHandler::stringToWstring(const string &str);
    vector<string> EDAoogleHttpRequestHandler::splitStringByAddSymbol(const string &input);
    string EDAoogleHttpRequestHandler::addCharacterNextTo(const string &input, char targetChar, 
                                                          char charToAdd);
    int EDAoogleHttpRequestHandler::countSpaceCharacters(const std::string& input);

    /*Frequency calculations*/
    void EDAoogleHttpRequestHandler::calculateTermFrequency(const string& word, vector<pair<string,
                                                            float>>& termFrequencies);
    
    /*HTML processing*/
    pair<string, string> EDAoogleHttpRequestHandler::filterHTMLContent(const string &htmlContent);
    string EDAoogleHttpRequestHandler::parseHTMLContent(const string &htmlContent);
    string EDAoogleHttpRequestHandler::readHTMLFile(const wstring &filePath);

    

};



#endif
