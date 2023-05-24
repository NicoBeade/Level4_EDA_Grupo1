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

#include <microhttpd.h>
#include <sqlite3.h>

#include "HttpServer.h"
#include "EDAoogleHttpRequestHandler.h"

using namespace std;

#define PATH_CORRECTION_HTML "..\\..\\www\\wiki\\"
#define HEADER 0
#define NON_HEADER 1

#ifdef WIN32
#define PATH_CORRECTION "..\\..\\"

#else
#define PATH_CORRECTION "../"
#endif

class EDAoogleHttpRequestHandler : public ServeHttpRequestHandler
{
public:
    EDAoogleHttpRequestHandler(string homePath);

    bool handleRequest(std::string url, HttpArguments arguments, vector<char> &response);

private:
    pair<string, string> EDAoogleHttpRequestHandler::filterHTMLContent(const string &htmlContent);
    string EDAoogleHttpRequestHandler::readHTMLFile(const wstring &filePath);
};


#endif
