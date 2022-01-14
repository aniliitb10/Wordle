#pragma once

#include <vector>
#include <string>
#include <random>
#include <wordle.h>
#include <util.h>
#include <algorithm>
#include <iomanip>

class WordleClient
{
public:
    WordleClient(size_t word_size, size_t display_limit, bool auto_mode) : _word_size(word_size),
                                                                           _display_limit(display_limit),
                                                                           _wordle(word_size)
    {
        if (!auto_mode)
        {
            std::cout << "Welcome! word size is: [" << _word_size << "], display limit is: [" << _display_limit
                      << "]\n";
            run();
        } else
        {
            std::cout << "Welcome! word size is: [" << _word_size << "], auto-mode is on\n";
            auto_run();
        }
    }

private:
    // prints status, accepts test string and status
    // runs in infinite loop, unless target word is found or there are no more words in dictionary to suggest
    void run() noexcept
    {
        print_update();
        std::string input{get_word()};
        std::string status{get_status()};
        while (!found(status))
        {
            _wordle.update(input, status);
            if (_wordle.get_words().empty())
            {
                std::cout << "Unable to find any suitable words from dictionary\n";
                return;
            }

            print_update();
            input = get_word();
            status = get_status();
        }
        std::cout << "Congratulations! you eventually found the word!\n";
    }

    void auto_run()
    {
        while (!_wordle.get_words().empty())
        {
            const auto &word = select_a_random_element(_wordle.get_words());
            print_update(word);
            auto status = get_status();
            if (found(status))
            {
                std::cout << "Congratulations! you eventually found the word!\n";
                return;
            }
            _wordle.update(word, status);
        }

        if (_wordle.get_words().empty())
        {
            std::cout << "Unable to find any suitable words from dictionary\n";
        }
    }

    // checks if the target word has been found
    [[nodiscard]] static bool found(const std::string &status) noexcept
    {
        return std::all_of(status.cbegin(), status.cend(), [](char c)
        { return c == 'g'; });
    }

    // reads valid word from terminal
    // if user enters status instead, user gets another chance to enter word again
    [[nodiscard]] std::string get_word() const noexcept
    {
        std::cout << "Enter the selected word: ";
        std::string input = get_valid_input(_word_size, [](unsigned char c)
        { return std::isalpha(c); });
        if (std::all_of(input.cbegin(), input.cend(), [](char c)
        { return c == 'b' || c == 'g' || c == 'y'; }))
        {
            std::cout << "Did you just enter status instead of words (y/n)? ";
            std::string ans = get_valid_input(1u, [](char c)
            { return c == 'y' || c == 'n'; });
            if (ans == "y")
            {
                std::cout << "Okay! Try again (last chance though)! Enter the selected word: ";
                return get_valid_input(_word_size, [](unsigned char c)
                { return std::isalpha(c); });
            }
        }

        return input;
    }

    // reads valid status string from terminal
    [[nodiscard]] std::string get_status() const noexcept
    {
        std::cout << "Enter the status of previous word: ";
        return get_valid_input(_word_size, [](char c)
        { return c == 'b' || c == 'g' || c == 'y'; });
    }

    // prints the status on terminal
    void print_update(const std::string &single_word = "") const noexcept
    {
        const auto &words = _wordle.get_words();
        assert_statement(!words.empty(), "There are no entries to print!");

        if (words.size() == 1u)
        {
            std::cout << "Only possible word is: " << std::quoted(words.front()) << "\n";
        } else if (!single_word.empty())
        {
            std::cout << "\nThere are currently [" << std::setw(5) << words.size() << "] words, try: "
                      << std::quoted(single_word) << "\n";
        } else
        {
            std::cout << "There are " << words.size() << " possible words, try one of these: \n";
            for (const auto &word: select_n_random_elements(words, _display_limit))
            {
                std::cout << word << "\n";
             }
        }
    }


    std::size_t _word_size;
    std::size_t _display_limit;
    Wordle _wordle;
};