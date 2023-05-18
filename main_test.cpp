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

#include <microhttpd.h>

#include "CommandLineParser.h"
#include "HttpServer.h"
#include "EDAoogleHttpRequestHandler.h"

using namespace std;

string parseHTML(const std::string& html) {
    string parsedHTML;
    bool insideTag = false;
    bool ignoreContent = false;

    for (size_t i = 0; i < html.length(); ++i) {
        if (!ignoreContent) {
            if (html[i] == '<' && html[i + 1] == '\\') {
                insideTag = false;
                ignoreContent = true;
                ++i;
            }
            else if (html[i] == '<') {
                insideTag = true;
                continue;
            }
            else if (html[i] == '>') {
                insideTag = false;
                continue;
            }
        }
        else {
            if (html[i] == '>') {
                ignoreContent = false;
            }
            continue;
        }

        if (!insideTag && !ignoreContent) {
            parsedHTML += html[i];
        }
    }
    return parsedHTML;
}

void print(string s);
int fail();
int pass();

int main()
{
    string prueba = "<html>\n"
                       "<head>\n"
                       "<title>Sample Page</title>\n"
                       "</head>\n"
                       "<body>\n"
                       "<h1>Hello, World!</h1>\n"
                       "<p>This is a sample paragraph.</p>\n"
                       "This <\\ is not a tag.\n"
                       "</body>\n"
                       "</html>";
    cout << parseHTML(prueba) << endl;
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
