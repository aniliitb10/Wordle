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
#include <random>
#include <string>
#include <optional>
#include <type_traits>

// a helper function to be used in concatenate function
template<typename T>
std::string to_string(T arg)
{
    if constexpr(std::is_same_v<std::decay_t<T>, std::string>) return arg;
    else if constexpr(std::is_constructible_v<std::string, std::decay_t<T>>) return std::string{std::forward<T>(arg)};
    else if constexpr(std::is_same_v<char, std::decay_t<T>>) return {arg};
    else if constexpr(std::is_same_v<bool, std::decay_t<T>>) return arg ? "truthy" : "falsy";
    // otherwise, all arithmetic values are supported by std::to_string
    else return std::to_string(std::forward<T>(arg));
}

// a function to concatenate string-ified version of different data types
template<typename... Ts>
std::string concatenate(Ts &&... args)
{
    return (to_string(std::forward<Ts>(args)) + ...);
}

// throws only if status is false
// no char* to std::string conversion unless status is actually false
template<typename T=std::runtime_error, typename ... Ts>
void assert_statement(bool status, Ts... args)
{
    if (!status)
    {
        throw T{concatenate(std::forward<Ts>(args)...)};
    }
}

// Reads a file, line by line and strip carriage/new-line characters
// It might appear that 3rd argument is making it unnecessarily complicated but
// without that, we will have to first store a vector of strings
// and then transform them in another vector
template<typename T=std::string>
std::vector<T> read_lines(const std::string &filepath, bool ignore_empty = true,
                          const std::function<T(const std::string &)> &
                          convertor = [](const std::string &arg) { return arg; })
{
    std::vector<T> lines{};
    if (std::ifstream file{filepath}; file.is_open())
    {
        std::string line;
        while (getline(file, line)) // returns reference to file, so even if read line is empty, it should be fine
        {
            if (!line.empty() && line.back() == '\n')
                line.erase(std::prev(line.end()));

            if (!line.empty() && line.back() == '\r')
                line.erase(std::prev(line.end()));

            if (ignore_empty && line.empty()) continue;

            lines.template emplace_back(convertor(line));
        }
    }
    else
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

// if input is not valid, it throws
template<typename T = std::size_t>
T from_string(const std::string &src)
{
    T value{};
    auto res = std::from_chars(src.data(), src.data() + src.size(), value);
    assert_statement<std::invalid_argument>(res.ec == std::errc() && res.ptr == src.data() + src.size(),
                     "Invalid string received [", src, "]");
    return value;
}
