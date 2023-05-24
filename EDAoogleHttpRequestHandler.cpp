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

#include <iostream>
#include <fstream>
#include <sqlite3.h>
#include <chrono>   
#include <string.h> 

#include "EDAoogleHttpRequestHandler.h"

using namespace std;

static int callback(void *NotUsed, int argc, char **argv, char **azColName);

EDAoogleHttpRequestHandler::EDAoogleHttpRequestHandler(string homePath) : 
ServeHttpRequestHandler(homePath)
{
    char *zErrMsg = 0;
    int rc;
    char* sql;

    sqlite3 *db;
    sqlite3_open("test.db", &db);

    /* Create SQL statement */
    sql = "CREATE TABLE ARTICLES("  \
        "ID INT PRIMARY  KEY     NOT NULL," \
        "TITLE           TEXT    NOT NULL," \
        "BODY            TEXT     NOT NULL," \
        "PATH        CHAR(50));";
        
    cout << sql << '\n';
    /* Execute SQL statement */
    rc = sqlite3_exec(db, sql, callback, 0, &zErrMsg);

    if( rc != SQLITE_OK )
    {
        fprintf(stderr, "SQL error: %s\n", zErrMsg);
        sqlite3_free(zErrMsg);
    } 
    else 
    {
        fprintf(stdout, "Table created successfully\n");
    }

    int i = 1;
    string title = "pito";
    string body  = "obama";
    string path = "/wiki/Xi_Jinping.html";
    
    string sqlstring  = "INSERT INTO ARTICLES (ID, TITLE, BODY, PATH)";
    sqlstring += " VALUES (";
    sqlstring += to_string(i);
    sqlstring += ", '";
    sqlstring += title;
    sqlstring += "', '";
    sqlstring += body;
    sqlstring += "', '";
    sqlstring += path;
    sqlstring += "');";          

    sql = sqlstring.data();
    rc = sqlite3_exec(db, sql, callback, 0, &zErrMsg);

    if( rc != SQLITE_OK )
    {
        fprintf(stderr, "SQL error: %s\n", zErrMsg);
        sqlite3_free(zErrMsg);
    } 
    else 
    {
        fprintf(stdout, "item %d created successfully\n", i);
    }

    sqlite3_close(db);
}

bool EDAoogleHttpRequestHandler::handleRequest(string url,
                                               HttpArguments arguments,
                                               vector<char> &response)
{
    string searchPage = "/search";
    if (url.substr(0, searchPage.size()) == searchPage)
    {
        string searchString;
        if (arguments.find("q") != arguments.end())
            searchString = arguments["q"];

        // Header
        string responseString = string("<!DOCTYPE html>\
<html>\
\
<head>\
    <meta charset=\"utf-8\" />\
    <title>EDAoogle</title>\
    <link rel=\"preload\" href=\"https://fonts.googleapis.com\" />\
    <link rel=\"preload\" href=\"https://fonts.gstatic.com\" crossorigin />\
    <link href=\"https://fonts.googleapis.com/css2?family=Inter:wght@400;800&display=swap\"\
    rel=\"stylesheet\" />\
    <link rel=\"preload\" href=\"../css/style.css\" />\
    <link rel=\"stylesheet\" href=\"../css/style.css\" />\
</head>\
\
<body>\
    <article class=\"edaoogle\">\
        <div class=\"title\"><a href=\"/\">EDAoogle</a></div>\
        <div class=\"search\">\
            <form action=\"/search\" method=\"get\">\
                <input type=\"text\" name=\"q\" value=\"" +
                                       searchString + "\" autofocus>\
            </form>\
        </div>\
        ");

        float searchTime;

        //Se inicia el cronometro
        std::chrono::time_point<std::chrono::system_clock> start, end;      //time point y system clocks
        start = std::chrono::system_clock::now();

        // YOUR JOB: fill in results
        vector<string> results;
        
        
        

        //results returned from the search handler needs to be in vector<string> format
        //not the name of the file or the title, just the raw url

        results.push_back("/wiki/Yen.html");
        //path is relative to "folder/www" so the url works like this
        results.push_back("/wiki/Xi_Jinping.html");
        //works like a queue and not like a stack: Yen will show up first, then Xi Jiping
        //the function that returns the search results needs to return them in order of priority

        //Finaliza el cronometro
        end = std::chrono::system_clock::now();

        std::chrono::duration<double> duration = end - start;
        searchTime = (float) duration.count();

        // Print search results
        responseString += "<div class=\"results\">" + to_string(results.size()) +
                          " results (" + to_string(searchTime) + " seconds):</div>";
        for (auto &result : results)
            responseString += "<div class=\"result\"><a href=\"" +
                              result + "\">" + result + "</a></div>";

        // Trailer
        responseString += "    </article>\
</body>\
</html>";

        response.assign(responseString.begin(), responseString.end());

        return true;
    }
    else
        return serve(url, response);

    return false;
}

static int callback(void *NotUsed, int argc, char **argv, char **azColName) {
   int i;
   for(i = 0; i<argc; i++) {
      printf("%s = %s\n", azColName[i], argv[i] ? argv[i] : "NULL");
   }
   printf("\n");
   return 0;
}

