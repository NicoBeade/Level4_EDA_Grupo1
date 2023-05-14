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

#include <microhttpd.h>

#include "CommandLineParser.h"
#include "HttpServer.h"
#include "EDAoogleHttpRequestHandler.h"

using namespace std;

void print(string s);
int fail();
int pass();

int main()
{
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
