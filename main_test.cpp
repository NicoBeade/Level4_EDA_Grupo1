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

void print(string s);
int fail();
int pass();

using namespace std;

std::pair<std::string, std::string> filterHTMLContent(const std::string& htmlContent) {
    std::string headersString;
    std::string bodyString;

    size_t pos = 0;

    while (pos != std::string::npos && pos < htmlContent.length()) {
        // Find the start of the next HTML tag
        size_t startTagPos = htmlContent.find('<', pos);
        if (startTagPos == std::string::npos)
            break;

        // Find the end of the current HTML tag
        size_t endTagPos = htmlContent.find('>', startTagPos);
        if (endTagPos == std::string::npos)
            break;

        // Extract the current HTML tag
        std::string tag = htmlContent.substr(startTagPos, endTagPos - startTagPos + 1);

        // Check if it is a header tag
        if (tag.substr(0, 2) == "<h" && tag.back() == '>') {
            // Extract the header text and add it to the headers string
            size_t headerStartPos = endTagPos + 1;
            size_t headerEndPos = htmlContent.find('<', headerStartPos);
            if (headerEndPos == std::string::npos)
                headerEndPos = htmlContent.length();
            std::string headerText = htmlContent.substr(headerStartPos, headerEndPos - headerStartPos);
            headersString += headerText + '\n';

            // Move the position after the current header tag
            pos = headerEndPos;
        } else {
            // Move the position after the current HTML tag
            pos = endTagPos + 1;
        }
    }

    // Extract the body text by skipping HTML tags
    while (pos != std::string::npos && pos < htmlContent.length()) {
        // Find the start of the next HTML tag
        size_t startTagPos = htmlContent.find('<', pos);
        if (startTagPos == std::string::npos)
            break;

        // Find the end of the current HTML tag
        size_t endTagPos = htmlContent.find('>', startTagPos);
        if (endTagPos == std::string::npos)
            break;

        // Move the position after the current HTML tag
        pos = endTagPos + 1;
    }

    // Extract the remaining text as the body string
    if (pos != std::string::npos && pos < htmlContent.length()) {
        bodyString = htmlContent.substr(pos);
    }

    return std::make_pair(headersString, bodyString);
}

std::string readHTMLFile(const std::string& filePath) {
    std::ifstream file(filePath);
    if (!file.is_open()) {
        std::cerr << "Failed to open file: " << filePath << std::endl;
        return "";
    }

    std::string htmlContent((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    file.close();

    return htmlContent;
}

int main() {
    std::string filePath = PATH_CORRECTION_HTML"Yogur.html";
    std::string htmlContent = readHTMLFile(filePath);

    std::pair<std::string, std::string> filteredContent = filterHTMLContent(htmlContent);

    std::cout << "Headers:\n" << filteredContent.first << std::endl;
    std::cout << "Body:\n" << filteredContent.second << std::endl;

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
