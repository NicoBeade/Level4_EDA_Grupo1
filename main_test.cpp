/**
 * @file main_test.cpp
 * @authors Nicolás Beade - Franco Dorfman - Vito Pensa Piccolo - Federico Gentile
 * @brief EDAoogle - test module
 * @version 0.2
 *
 * @copyright Copyright (c) 2022-2023
 *
 */

#include <iostream>
#include <string>
#include <stack>
#include <fstream>

#include <microhttpd.h>

#include "CommandLineParser.h"
#include "HttpServer.h"
#include "EDAoogleHttpRequestHandler.h"

using namespace std;

#define PATH_CORRECTION_HTML "..\\..\\www\\wiki\\"
#define HEADER 0
#define NON_HEADER 1

void print(string s);
int fail();
int pass();

using namespace std;

/**
 *@brief Separates header text from all other text
 *
 *@param htmlContent            raw html string          
 *@return pair<string, string>  separated strings of headers and non-headers
 *@cite https://www.geeksforgeeks.org/html-parser-in-c-cpp/
 **/
pair<string, string> filterHTMLContent(const string& htmlContent) {
    string headersString = "";
    string bodyString = "";

    size_t pos = 0;

    int condition = 0;

    while (pos != string::npos && pos < htmlContent.length()) {
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
        if ((tag.substr(0, 3) == "<h1" || tag.substr(0, 3) == "<h2" || tag.substr(0, 3) == "<h3") 
             && tag.back() == '>') {
            condition = HEADER;
        } 
        else {
            condition = NON_HEADER;
        }

        size_t startPos = endTagPos + 1;
        size_t endPos = htmlContent.find('<', startPos);

        while (htmlContent[startPos] == '<'){
            string auxiliarTag = htmlContent.substr(startPos, 4);
            // Check for "hidden" header tags
            if (auxiliarTag.substr(0, 3) == "<h1" || auxiliarTag.substr(0, 3) == "<h2" 
                || auxiliarTag.substr(0, 3) == "<h3"){
                condition = HEADER;
            }
            startPos = htmlContent.find('>', startPos) + 1;
            endPos = htmlContent.find('<', startPos);
        }
        
        if (endPos == string::npos){
            endPos = htmlContent.length();
        }
        string textToAdd = htmlContent.substr(startPos, endPos - startPos);
        
        switch (condition){
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
        pos = endPos;
    }

    return std::make_pair(headersString, bodyString);
}

/**
 *@brief Puts html file into a single string
 *
 *@param filePath                   
 *@return stirng        raw html text

 **/
string readHTMLFile(const std::string& filePath) {
    ifstream file(filePath);
    if (!file.is_open()) {
        cerr << "Failed to open file: " << filePath << endl;
        return "";
    }

    string htmlContent((istreambuf_iterator<char>(file)), istreambuf_iterator<char>());
    file.close();

    return htmlContent;
}

int main() {
    string filePath = PATH_CORRECTION_HTML"Yogur.html";
    string htmlContent = readHTMLFile(filePath);

    pair<std::string, std::string> filteredContent = filterHTMLContent(htmlContent);

    cout << "Headers:\n" << filteredContent.first << endl;
    cout << "Body:\n" << filteredContent.second << endl;

    return 0;
}

void print(string s)
{
    cout << s;
}

int fail()
{
    cout << "FAIL" << endl;
    return 1;
}

int pass()
{
    cout << "PASS" << endl;
    return 0;
}
