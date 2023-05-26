/**
 * @file main_test.cpp
 * @authors Nicolás Beade - Franco Dorfman - Vito Pensa Piccolo - Federico Gentile
 * @brief EDAoogle - test module
 * @version 0.2
 *
 * @copyright Copyright (c) 2022-2023
 *
 * NOTE: All other functions and methods where tested while the program was being designed 
 * and developed. And regarding SQL fucntions, each of these has it's own error handler 
 * which works by themselves as tests. We decided it was not necessary to test these.
 * 
 */


#include <microhttpd.h>

#include <iostream>
#include <string>
#include <fstream>
#include <chrono>
#include <filesystem>
#include <unordered_map>
#include <algorithm>
#include <sstream>
#include <codecvt>

using namespace std;


void print(string s);
int fail();
int pass();
int termFreqCallbackTest(void *data, int argc, char **argv, char **columnNames);

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
    int result = termFreqCallbackTest(&termFrequencies, argc, argv, columnNames);

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

int termFreqCallbackTest(void *data, int argc, char **argv, char **columnNames)
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