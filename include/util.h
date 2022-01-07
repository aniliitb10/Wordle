#pragma once

/*
 * This utility file contains utility functions to make life
 * easier of a C++ programmer, e.g. read a file line by line
 * into a vector of strings
 * */


#include <vector>
#include <string>
#include <fstream>

inline
std::vector<std::string> read_lines(const std::string& filepath, bool ignore_empty = true)
{
    std::vector<std::string> lines{};
    if (std::ifstream file{filepath}; file.is_open())
    {
        std::string line;
        while (getline(file, line)) // returns reference to file, so even if read line is empty, it should be fine
        {
            if (!line.empty() && line.back() == '\n')
                line.erase(std::next(line.end(), -1));

            if (!line.empty() && line.back() == '\r')
                line.erase(std::next(line.end(), -1));

            if(ignore_empty)
            {
                if (!line.empty())
                    lines.push_back(std::move(line));
            }
            else
                lines.push_back(std::move(line));
        }
    }
    else
        std::cerr << "Failed to open the file: " << filepath << "\n";

    return lines;
}


// print a vector!
inline
std::ostream& operator<< (std::ostream& ostream, const std::vector<std::string>& strings)
{
    for (auto& str : strings)
    {
        ostream << str << "\n";
    }
    return ostream;
}