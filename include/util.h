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
#include <random>

// throws only if status is false
// no char* to std::string conversion unless status is actually false
template<typename T=std::runtime_error>
void assert_statement(bool status, std::string_view details)
{
    if (!status)
    {
        throw T{std::string{details}};
    }
}

// read a file, line by line and strip carriage/new-line characters
inline
std::vector<std::string> read_lines(const std::string &filepath, bool ignore_empty = true)
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

            if (ignore_empty)
            {
                if (!line.empty())
                    lines.push_back(std::move(line));
            } else
                lines.push_back(std::move(line));
        }
    } else
        std::cerr << "Failed to open the file: " << filepath << "\n";

    return lines;
}

// print a vector!
inline
std::ostream &operator<<(std::ostream &ostream, const std::vector<std::string> &strings)
{
    for (auto &str: strings)
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

// to select a random element from a std::vector
template<typename T>
const T &select_a_random_element(const std::vector<T> &elements)
{
    assert_statement<std::length_error>(!elements.empty(), "There are no elements in the container to choose from!");
    std::mt19937_64 gen{std::random_device{}()};
    // -1 as right end is also inclusive
    std::uniform_int_distribution<std::size_t> dist{0, elements.size() - 1};
    return elements.at(dist(gen));
}

// selects a maximum of n elements
// if vector contains fewer elements, all original elements are returned
template<typename T>
std::vector<T> select_n_random_elements(const std::vector<T> &elements, std::size_t n)
{
    std::vector<T> out{};
    std::sample(elements.begin(), elements.end(), std::back_inserter(out), n,
                std::mt19937_64{std::random_device{}()});
    return out;
}
