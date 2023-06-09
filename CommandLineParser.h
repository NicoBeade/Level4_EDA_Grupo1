/**
 * @file CommandLineParser.h
 * @author Marc S. Ressl
 * @authors Nicolás Beade - Franco Dorfman - Vito Pensa Piccolo - Federico Gentile
 * @brief Quick and dirty command line parser
 * @version 0.2
 *
 * @copyright Copyright (c) 2022-2023
 *
 */

#ifndef COMMANDLINEPARSER_H
#define COMMANDLINEPARSER_H

#include <string>
#include <vector>

class CommandLineParser
{
public:
    CommandLineParser(int argc, const char *argv[]);

    bool hasOption(const std::string &option);
    std::string getOption(const std::string &option);

private:
    std::vector<std::string> arguments;
};

#endif
