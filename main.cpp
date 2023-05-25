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
 *  to how representative of a file is a given word. Note that the algorithm designed is BASED on
 *  TF-IDF, some modifications were made either in order to make the program more efficient or to
 *  decrease the complexity of developing the algorithm. We decided to skip the IDF part of the 
 *  algorithm due to the fact that it involved a lot more of additional computational time which
 *  made response times way higher.
 * @cite https://www.youtube.com/watch?v=6HuKFh0BatQ
 * -There was another layer of complexity that could have been added to the program. 
 *  an innitial design of ours involved detecting whether a word was in the header or in the body
 *  of an html so that we could reward the presence of the searched word in the headers of the html.
 *  This involved a more complex parser that was initially written, but we opted out parsing every-
 *  thing into a single string instead of separating headers from body. This parser can still be 
 *  found however in the class EDAoogleHttpRequestHandler in case future designs wanted to incorpo-
 *  rate this functionality. The method's name is filterHTMLContent.
 *
 * 
 * A problem we encountered but could not solve:
 * There is a problem when searching with unicode characters, which arises from html encoding in
 * html entities. We tried installing a library to encode the search string but we got nowhere and
 * could not solve this issue after several attempts. This problem could have been solved as well 
 * by doing the parsing of the html files with an external library, but we decided instead to 
 * write the parser ourselves. 
 * 
 * 
 * USED LIBRARIES
 * 
 * sqlite3 - Install by writting vcpkg install sqlite3 in a console
 * find_package(unofficial-sqlite3 CONFIG REQUIRED)
 * target_link_libraries(edahttpd PRIVATE unofficial::sqlite3::sqlite3)
 * THIS LIBRARY WAS USED TO BE ABLE TO PERFORM SQL SEARCHES
 * 
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
