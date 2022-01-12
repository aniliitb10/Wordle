#pragma once

/*
 * This utility file contains utility functions to make life
 * easier of a C++ programmer, e.g. read a file line by line
 * into a vector of strings
 * */


#include <vector>
#include <string>
#include <iostream>
#include <fstream>
#include <charconv>
#include <exception>
#include <sstream>

// read a file, line by line and strip carriage/new-line characters
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

// If input is not valid, it asks for input again and never throws
template<class Func>
[[nodiscard]] std::string get_valid_input(std::size_t word_size, const Func &validator) noexcept
{
    std::string input{};
    std::getline(std::cin, input);

    auto itr = std::find_if_not(input.begin(), input.end(), validator);
    while (input.size() != word_size || itr != input.end())
    {
        std::cout << "Invalid input [" << input << "] is received, please try again: ";
        std::getline(std::cin, input);
        itr = std::find_if_not(input.begin(), input.end(), validator);
    }
    return input;
}

// if input is not valid, it throws
template <typename T = std::size_t >
T from_string(const std::string& src)
{
    T value{};

    auto res = std::from_chars(src.data(), src.data() + src.size(), value);
    if (res.ec != std::errc() || res.ptr != src.data() + src.size())
    {
        std::stringstream  os;
        os << "Invalid string received [" << src << "]";
        throw std::invalid_argument(os.str());
    }
    
    return value;
}