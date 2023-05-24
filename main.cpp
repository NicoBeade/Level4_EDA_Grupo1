/**
 * @file main.cpp
 * @author Marc S. Ressl
 * @authors Nicolás Beade - Franco Dorfman - Vito Pensa Piccolo - Federico Gentile
 * @brief
 * @version 0.2
 *
 * @copyright Copyright (c) 2022-2023
 * 
 * Implementation of a search engine for html pages
 * -The output of the program is based on the TF-IDF algorithm, which sorts the pages according 
 * to how representative of a file is a given word.
 * @cite https://www.youtube.com/watch?v=6HuKFh0BatQ
 * -There was another layer of complexity added to the program. If the word being searched is in 
 * a header or title then it weighs more than if it were in the body or in table.
 *
 */

#include <iostream>
#include <microhttpd.h>

#include "CommandLineParser.h"
#include "HttpServer.h"
#include "EDAoogleHttpRequestHandler.h"

using namespace std;

int main(int argc, const char *argv[])
{
    CommandLineParser parser(argc, argv);

    // Configuration
    int port = 8000;
    string homePath = PATH_CORRECTION "www";

    // Parse command line
    if (parser.hasOption("--help"))
    {
        cout << "edahttpd 0.1" << endl
             << endl;
        cout << "Usage: edahttpd [-p PORT] [-h HOME_PATH]" << endl;

        return 0;
    }

    if (parser.hasOption("-p"))
        port = stoi(parser.getOption("-p"));

    if (parser.hasOption("-h"))
        homePath = parser.getOption("-h");

    // Start server
    HttpServer server(port);

    EDAoogleHttpRequestHandler edaOogleHttpRequestHandler(homePath);
    server.setHttpRequestHandler(&edaOogleHttpRequestHandler);

    if (server.isRunning())
    {
        cout << "Running server..." << endl;

        // Wait for keyboard entry
        char value;
        cin >> value;

        cout << "Stopping server..." << endl;
    }
}
