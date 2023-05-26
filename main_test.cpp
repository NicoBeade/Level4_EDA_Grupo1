/**
 * @file main_test.cpp
 * @authors Nicolás Beade - Franco Dorfman - Vito Pensa Piccolo - Federico Gentile
 * @brief EDAoogle - test module
 * @version 0.2
 *
 * @copyright Copyright (c) 2022-2023
 *
 */

#include <microhttpd.h>

#include "CommandLineParser.h"
#include "HttpServer.h"
#include "EDAoogleHttpRequestHandler.h"

using namespace std;


void print(string s);
int fail();
int pass();

void testTermFreqCallback()
{
    // Create test data
    vector<pair<string, float>> termFrequencies;
    termFrequencies.emplace_back("path1", 0.5f);
    termFrequencies.emplace_back("path2", 0.25f);
    termFrequencies.emplace_back("path3", 0.75f);

    // Create test arguments
    const int argc = 2;
    char *argv[argc] = { "path2", "0.1" };
    char *columnNames[argc] = { "Path", "Frequency" };

    // Call the termFreqCallback
    int result = termFreqCallback(&termFrequencies, argc, argv, columnNames);

    // Print the updated term frequencies
    for (const auto &pair : termFrequencies)
    {
        cout << "Path: " << pair.first << ", Frequency: " << pair.second << endl;
    }

    // Print the result
    if (result == 0)
    {
        pass();
    }
    else
    {
        fail();
    }
}

int main()
{
    testTermFreqCallback();
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
