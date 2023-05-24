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

#include <microhttpd.h>

#include "HttpServer.h"
#include "EDAoogleHttpRequestHandler.h"

using namespace std;

#define PATH_CORRECTION_HTML "..\\..\\www\\wiki\\"
#define HEADER 0
#define NON_HEADER 1

class EDAoogleHttpRequestHandler : public ServeHttpRequestHandler
{
public:
    EDAoogleHttpRequestHandler(std::string homePath);

    bool handleRequest(std::string url, HttpArguments arguments, std::vector<char> &response);

private:
    pair<string, string> EDAoogleHttpRequestHandler::filterHTMLContent(const string &htmlContent);
    string EDAoogleHttpRequestHandler::readHTMLFile(const std::string &filePath);
};


#endif
