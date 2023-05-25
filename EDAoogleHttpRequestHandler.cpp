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

/*Callback Prototypes*/
int termFreqCallback(void *data, int argc, char **argv, char **columnNames);
bool compareByTermFrequency(const pair<string, float> &a, const pair<string, float> &b);
int countSpaceCharacters(const std::string& input);

/**
 *@brief class constructor
 *
 *@param homePath path to the folder with the html files
 **/
EDAoogleHttpRequestHandler::EDAoogleHttpRequestHandler(string homePath) : 
ServeHttpRequestHandler(homePath)
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
          "PATH        CHAR(50),"
          "WORDC          INT);";

    /* Execute SQL statement */
    rc = sqlite3_exec(db, sql, nullptr, 0, &zErrMsg);

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
            wstring wfilePath = stringToWstring(file.path().u8string()); // Convert to wstring
            string htmlContent = readHTMLFile(wfilePath);
            pair<string, string> filteredContent = filterHTMLContent(htmlContent);
            string correctedPath; // character ' in file names was conflictive with SQL

            string sqlstring = "INSERT INTO ARTICLES (TITLE, BODY, PATH, WORDC)";
            sqlstring += " VALUES ('";
            sqlstring += filteredContent.first;
            sqlstring += "', '";
            sqlstring += filteredContent.second;
            sqlstring += "', '";
            correctedPath = addCharacterNextTo(file.path().u8string(), '\'', '\'');
            sqlstring += correctedPath;
            sqlstring += "', '";
            sqlstring += to_string(countSpaceCharacters(filteredContent.second));
            sqlstring += "');";

            sql = sqlstring.data();
            rc = sqlite3_exec(db, sql, nullptr, 0, &zErrMsg);

            if (rc != SQLITE_OK)
            {
                fprintf(stderr, "SQL error: %s\n", zErrMsg);
                sqlite3_free(zErrMsg);
            }
        }
    }
}

/**
 *@brief Request Handler. Processes input and calls the methods to perform searches
 *
 *@param url            Cleaned url
 *@param arguments      Needed to perform searches
 *@param response       Needed to display the results of the search
 *
 *@return
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

        chrono::time_point<chrono::system_clock> start, end; // time point y system clocks
        start = chrono::system_clock::now();

        float searchTime;
        vector<string> separatedStringSearch = splitStringBySpace(searchString);
        vector<pair<string, float>> termFrequencies;
        vector<string> results;

        for (const auto &word : separatedStringSearch)
        {
            string rectifiedWord = addCharacterNextTo(word, '\'', '\'');
            calculateTermFrequency(rectifiedWord, termFrequencies);
        }

        sort(termFrequencies.begin(), termFrequencies.end(), compareByTermFrequency);

        for (const auto &pair : termFrequencies)
        {
            string path = pair.first.substr(EXTRA_CHARACTERS_IN_PATH); // Corrected path
            results.push_back(path);
        }

        end = chrono::system_clock::now();

        chrono::duration<double> duration = end - start;
        searchTime = (float)duration.count();

        // Print search results
        responseString += "<div class=\"results\">" + to_string(results.size()) +
                          " results (" + to_string(searchTime) + " seconds):</div>";
        for (auto &result : results)
        {
            string cleanedString = result.substr(5, result.length() - 10);
            responseString += "<div class=\"result\"><a href=\"" +
                              result + "\">" + cleanedString + "</a></div>";
        }


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



/*________________________________________________________________________________________________

                                AUXILIAR FUNCTIONS AND METHODS
__________________________________________________________________________________________________*/


/* FREQUENCY CALCULATOR */

/**
 *@brief Calculate the term frequency of a given word
 *
 *@param word                   searched word
 *@param termFrequencies        vector of pairs: path vs term frequency for that register
 *
 **/
void EDAoogleHttpRequestHandler::calculateTermFrequency(const string &word, vector<pair<string, 
                                                        float>> &termFrequencies)
{
    sqlite3 *database;
    int result = sqlite3_open(PATH_CORRECTION DB_NAME, &database);

    if (result != SQLITE_OK)
    {
        cerr << "Failed to open database: " << sqlite3_errmsg(database) << endl;
        return;
    }

    string query = "SELECT PATH, (LENGTH(BODY) - LENGTH(REPLACE(LOWER(BODY), \
                    LOWER('" + word + "'), '')))\
                     / CAST(LENGTH('" +
                   word + "') AS FLOAT) / WORDC;";

    result = sqlite3_exec(database, query.c_str(), termFreqCallback, &termFrequencies, nullptr);

    if (result != SQLITE_OK)
    {
        cerr << "Failed to execute query: " << sqlite3_errmsg(database) << endl;
    }

    sqlite3_close(database);
}

/* HTML CONDITIONING */

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
        if ((tag.substr(0, 3) == "<h1" || tag.substr(0, 3) == "<h2" || tag.substr(0, 3) == "<h3" 
             || tag.substr(0, 6) == "<title") && tag.back() == '>')
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
            if (auxiliarTag.substr(0, 3) == "<h1" || auxiliarTag.substr(0, 3) == "<h2" || 
                auxiliarTag.substr(0, 3) == "<h3")
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
        textToAdd = addCharacterNextTo(textToAdd, '\'', '\''); // char ' was problematic with SQL

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
 *@param filePath       path to file

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

/* STRING MANAGEMENT */

/**
 *@brief auxiliar function to work with files with unicode characters in the file names
 *
 *@param homePath               path to the folder with the html files
 *
 *@returns wstring              transformed string to wstring
 **/
wstring EDAoogleHttpRequestHandler::stringToWstring(const string &str)
{
    wstring wideStr;
    wideStr.reserve(str.length());

    for (char ch : str)
    {
        wideStr.push_back(static_cast<wchar_t>(ch));
    }

    return wideStr;
}

/**
 *@brief separates a given string into substrings for every space
 *
 *
 *
 *@param input            string to divide into substrings
 *
 *@return vector of separated strings
 **/
vector<string> EDAoogleHttpRequestHandler::splitStringBySpace(const string &input)
{
    vector<string> separatedWords;
    stringstream ss(input);
    string word;

    while (getline(ss, word, ' '))
    {
        if (!word.empty())
        {
            separatedWords.push_back(word);
        }
    }

    return separatedWords;
}

/**
 *@brief The character ' was problematic when working with SQL. Our solution was to add another ' next
 *       to the ' in the html text to "escape" in SQL. We could also have eliminated de ' character but
 *       this conflicted with finding path names when answering the user's search
 *
 *@param input                  string to rectify
 *@param targetChar             character to detect
 *@param charToAdd              character to add

 *@return string                rectified string
 **/
string EDAoogleHttpRequestHandler::addCharacterNextTo(const string &input, char targetChar, 
                                                      char charToAdd)
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
 *@brief Counts space characters in a string to approximate number of words in a file
 *
 *@param input                  string to count spaces in
 *
 *@return int                   count
 **/
int countSpaceCharacters(const std::string& input) 
{
    int count = 0;

    for (char c : input) {
        if (c == ' ') {
            count++;
        }
    }

    return count;
}

/* CALLBACKS */

/**
 *@brief Callback necessary to calculate the frequency of a word in a register. If the register
 *       was already added then it adds up the frequencies of the terms involved
 *
 *@param data            vector of pairs string vs float
 *@param argv            array containing the path in the 1st column and term frequency in the 2nd
 *
 *@return 0 (successfull)
 **/
int termFreqCallback(void *data, int argc, char **argv, char **columnNames)
{
    vector<pair<string, float>> &termFrequencies = *static_cast<vector<pair<string, float>> *>(data);

    auto it = find_if(termFrequencies.begin(), termFrequencies.end(),
    [&](const pair<string, float>& pair) {
        return pair.first == argv[0];
    });

    if (it == termFrequencies.end())
    {
        string path = argv[0] ? argv[0] : "NULL";
        float termFrequency = stof(argv[1] ? argv[1] : "0");
        termFrequencies.emplace_back(path, termFrequency);
    }
    else 
    {
        (*it).second += stof(argv[1]);
    }

    return 0;
}

/**
 *@brief Compare callback to sort the results with "sort"
 *
 *@param a, b           two elements
 *
 *@returns bool indicating which of the parameters has a higher priority
 **/
bool compareByTermFrequency(const pair<string, float> &a, const pair<string, float> &b)
{
    // Sort in descending order based on the second element (term frequency)
    return a.second > b.second;
}
