/**
 * @file ServeHttpRequestHandler.h
 * @author Marc S. Ressl
 * @authors Nicolás Beade - Franco Dorfman - Vito Pensa Piccolo - Federico Gentile
 * @brief Simple HTTP request handler that serves files
 * @version 0.2
 *
 * @copyright Copyright (c) 2022-2023
 *
 */

#ifndef SERVEHTTPREQUESTHANDLER_H
#define SERVEHTTPREQUESTHANDLER_H

#include "HttpServer.h"

class ServeHttpRequestHandler : public HttpRequestHandler
{
public:
    ServeHttpRequestHandler(std::string homePath);

    bool handleRequest(std::string url, HttpArguments arguments, std::vector<char> &response);

protected:
    bool serve(std::string path, std::vector<char> &response);

private:
    std::string homePath;
};

#endif
