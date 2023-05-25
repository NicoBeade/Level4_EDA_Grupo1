/**
 * @file EDAoogleHttpRequestHandler.h
 * @author Marc S. Ressl
 * @authors Nicolás Beade - Franco Dorfman - Vito Pensa Piccolo - Federico Gentile
 * @brief EDAoggle search engine
 * @version 0.2
 *
 * @copyright Copyright (c) 2022-2023
 * 
 * @cite https://www.tutorialspoint.com/sqlite/sqlite_c_cpp.htm was consulted
 *
 */

#include "EDAoogleHttpRequestHandler.h"
#include <locale>
#include <codecvt>

static int callback(void *NotUsed, int argc, char **argv, char **azColName);
string addCharacterNextTo(const string &input, char targetChar, char charToAdd);
wstring stringToWstring(const string &str);

/**
 *@brief class constructor
 *
 *@param homePath path to the folder with the html files
 **/
EDAoogleHttpRequestHandler::EDAoogleHttpRequestHandler(string homePath) : ServeHttpRequestHandler(homePath)
{
    if (filesystem::exists(PATH_CORRECTION DB_NAME)) 
    {
        return;
    }

    /* Create SQL statement and requirements */
    char *zErrMsg = 0;
    int rc;
    char *sql;

    sqlite3 *db;
    sqlite3_open(PATH_CORRECTION DB_NAME, &db);

    sql = "CREATE TABLE ARTICLES("
          "TITLE           TEXT    NOT NULL,"
          "BODY            TEXT     NOT NULL,"
          "PATH        CHAR(50));";

    /* Execute SQL statement */
    rc = sqlite3_exec(db, sql, callback, 0, &zErrMsg);

    if (rc != SQLITE_OK)
    {
        fprintf(stderr, "SQL error: %s\n", zErrMsg);
        sqlite3_free(zErrMsg);
    }
    else
    {
        fprintf(stdout, "Table created successfully\n");
    }

    /* Iterate through all files in /wiki folder */
    filesystem::path folderPath = homePath + "/wiki";
    filesystem::directory_iterator fileIterator(folderPath);
    for (const auto &file : fileIterator)
    {
        if (file.is_regular_file())
        {
            wstring wfilePath = stringToWstring(file.path().u8string()); // Convert to wstring if needed
            string htmlContent = readHTMLFile(wfilePath);
            pair<string, string> filteredContent = filterHTMLContent(htmlContent);
            string correctedPath; // character ' in file names was conflictive with SQL

            string sqlstring = "INSERT INTO ARTICLES (TITLE, BODY, PATH)";
            sqlstring += " VALUES ('";
            sqlstring += filteredContent.first;
            sqlstring += "', '";
            sqlstring += filteredContent.second;
            sqlstring += "', '";
            correctedPath = addCharacterNextTo(file.path().u8string(), '\'', '\'');
            sqlstring += correctedPath;
            sqlstring += "');";

            sql = sqlstring.data();
            rc = sqlite3_exec(db, sql, callback, 0, &zErrMsg);

            if (rc != SQLITE_OK)
            {
                fprintf(stderr, "SQL error: %s\n", zErrMsg);
                sqlite3_free(zErrMsg);
            }
        }
    }

    sqlite3_close(db);
}

/**
 *@brief class constructor
 *
 *@param homePath path to the folder with the html files
 **/
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

        // Se inicia el cronometro
        std::chrono::time_point<std::chrono::system_clock> start, end; // time point y system clocks
        start = std::chrono::system_clock::now();

        // YOUR JOB: fill in results
        vector<string> results;

        // results returned from the search handler needs to be in vector<string> format
        // not the name of the file or the title, just the raw url

        results.push_back("/wiki/Yen.html");
        // path is relative to "folder/www" so the url works like this
        results.push_back("/wiki/Xi_Jinping.html");
        // works like a queue and not like a stack: Yen will show up first, then Xi Jiping
        // the function that returns the search results needs to return them in order of priority

        /* Create SQL statement and requirements */
        char *zErrMsg = 0;
        int rc;
        char *sql;
        sqlite3 *db;
        const char* data = "Callback function called";
        
        /* Open database */
        rc = sqlite3_open(PATH_CORRECTION "wiki.db", &db);
        
        if( rc ) {
            fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
        } else {
            fprintf(stderr, "Opened database successfully\n");
        }

        /* Create SQL statement */
        sql = "SELECT * from ARTICLES";

        /* Execute SQL statement */
        rc = sqlite3_exec(db, sql, callback, (void*)data, &zErrMsg);
        
        if( rc != SQLITE_OK ) {
            fprintf(stderr, "SQL error: %s\n", zErrMsg);
            sqlite3_free(zErrMsg);
        } else {
            fprintf(stdout, "Operation done successfully\n");
        }

        sqlite3_close(db);

        // Finaliza el cronometro
        end = std::chrono::system_clock::now();

        std::chrono::duration<double> duration = end - start;
        searchTime = (float)duration.count();

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

static int callback(void *NotUsed, int argc, char **argv, char **azColName)
{
    int i;
    for (i = 0; i < argc; i++)
    {
        //printf("%s = %s\n", azColName[i], argv[i] ? argv[i] : "NULL");
    }
    printf("\n");
    return 0;
}

/**
 *@brief The character ' was problematic when working with SQL. Our solution was to add another ' next
 *       to the ' in the html text to "escape" in SQL. We could also have eliminated de ' character but
 *       this conflicted with finding path names when answering the user's search
 *@param input                      string to rectify
 *@param targetChar
 *@param charToAdd
 *@return string                    rectified string
 **/
string addCharacterNextTo(const string &input, char targetChar, char charToAdd)
{
    string result;
    for (size_t i = 0; i < input.length(); i++)
    {
        result += input[i];
        if (input[i] == targetChar)
        {
            result += charToAdd;
        }
    }
    return result;
}

/**
 *@brief Separates header text from all other text
 *
 *@param htmlContent            raw html string
 *@return pair<string, string>  separated strings of headers and non-headers
 *@cite https://www.geeksforgeeks.org/html-parser-in-c-cpp/
 **/
pair<string, string> EDAoogleHttpRequestHandler::filterHTMLContent(const string &htmlContent)
{
    string headersString = "";
    string bodyString = "";

    size_t pos = 0;

    int condition = 0;

    while (pos != string::npos && pos < htmlContent.length())
    {
        // Find the start of the next HTML tag
        size_t startTagPos = htmlContent.find('<', pos);
        if (startTagPos == string::npos)
            break;

        // Find the end of the current HTML tag
        size_t endTagPos = htmlContent.find('>', startTagPos);
        if (endTagPos == string::npos)
            break;

        // Extract the current HTML tag
        string tag = htmlContent.substr(startTagPos, endTagPos - startTagPos + 1);

        // Check if it is a header tag
        if ((tag.substr(0, 3) == "<h1" || tag.substr(0, 3) == "<h2" || tag.substr(0, 3) == "<h3" || tag.substr(0, 6) == "<title") && tag.back() == '>')
        {
            condition = HEADER;
        }
        else
        {
            condition = NON_HEADER;
        }

        startTagPos = endTagPos + 1;
        endTagPos = htmlContent.find('<', startTagPos);

        while (htmlContent[startTagPos] == '<')
        {
            string auxiliarTag = htmlContent.substr(startTagPos, 5);
            // Check for "hidden" header tags
            if (auxiliarTag.substr(0, 3) == "<h1" || auxiliarTag.substr(0, 3) == "<h2" || auxiliarTag.substr(0, 3) == "<h3")
            {
                condition = HEADER;
            }
            startTagPos = htmlContent.find('>', startTagPos) + 1;
            endTagPos = htmlContent.find('<', startTagPos);
        }

        if (endTagPos == string::npos)
        {
            endTagPos = htmlContent.length();
        }
        string textToAdd = htmlContent.substr(startTagPos, endTagPos - startTagPos);
        textToAdd = addCharacterNextTo(textToAdd, '\'', '\''); // Character ' was problematic with SQL

        switch (condition)
        {
        case HEADER:
            headersString += textToAdd + ' ';
            break;
        case NON_HEADER:
            bodyString += textToAdd + ' ';
            break;
        default:
            break;
        }

        // Move the position to the last processed character
        pos = endTagPos;
    }

    return make_pair(headersString, bodyString);
}

/**
 *@brief Puts html file into a single string
 *
 *@param filePath
 *@return string        raw html text

 **/
string EDAoogleHttpRequestHandler::readHTMLFile(const wstring &filePath)
{
    wifstream file(filePath);
    if (!file.is_open())
    {
        wcerr << L"Failed to open file: " << filePath << endl;
        return "";
    }

    string htmlContent((istreambuf_iterator<wchar_t>(file)), istreambuf_iterator<wchar_t>());
    file.close();

    return htmlContent;
}

/**
 *@brief auxiliar function to work with files with unicode characters in the file names
 *
 *@param homePath path to the folder with the html files
 **/
wstring stringToWstring(const string &str)
{
    wstring_convert<codecvt_utf8<wchar_t>> converter;
    return converter.from_bytes(str);
}