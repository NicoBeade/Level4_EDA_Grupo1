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

class HTMLParser {
public:
  HTMLParser(string html) {
    this->html = html;
    this->current_index = 0;
  }

  string parse() {
    string text = "";
    while (current_index < html.length()) {
      char c = html[current_index];
      if (c != '<') {
        text += c;
      } else {
        // Skip the tag
        while (html[current_index] != '>') {
          current_index++;
        }
      }
      current_index++;
    }
    return text;
  }

private:
  string html;
  int current_index;
};

void print(string s);
int fail();
int pass();

int main() {
  string html = "<html><head><title>This is a title</title></head><body><p>This is a paragraph</p></body></html>";
  HTMLParser parser(html);
  string text = parser.parse();
  cout << text << endl;
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
