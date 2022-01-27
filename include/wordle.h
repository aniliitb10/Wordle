#pragma once

#include <utility>
#include <vector>
#include <string>
#include <algorithm>
#include <util.h>
#include <type_traits>
#include <sstream>
#include <exception>
#include <words/frequent_words.h>
#include <words/words.h>

/*
 * A class to predict the words for Wordle (https://www.powerlanguage.co.uk/wordle/)
 * It expects an implementation of Words which can handle a potential list of words
 * and updates the potential list based on tye feedback from game
 * */
class Wordle
{
public:
    Wordle(std::size_t word_size, Words::Ptr word_ptr) :
            _word_size(word_size),
            _word_ptr(std::move(word_ptr))
    {}

    /**
     * By default, it will use FrequentWords implementation of Words
     * and hence, the default file path is also the one with frequencies
     * @param word_size: size of every word
     * @param file_path: a file where each row contains comma separated words and their frequencies
     */
    explicit Wordle(std::size_t word_size, const std::string &file_path = "../dictionary/5_words_freq.txt") :
            Wordle(word_size, std::make_shared<FrequentWords>(file_path, word_size))
    {}

    /**
     * A function which should be called after every update to filter possible words
     * @param str: the string which was tested
     * @param status: the string containing the output for test string,
     *                e.g. in "bbggy", first two characters were black, next two were green and last one was yellow
     * @return the size of remaining set of words
     */
    std::size_t update(std::string str, std::string status)
    {
        static const std::string ALLOWED_STATUS_CHARS{"byg"};

        assert_statement<std::invalid_argument>(str.size() == _word_size && status.size() == _word_size,
                                                "Invalid number of characters in [", str, "], and/or [", status,
                                                "], they must contain exactly [", _word_size, "] characters");

        assert_statement<std::invalid_argument>(status.find_first_not_of(ALLOWED_STATUS_CHARS) == std::string::npos,
                                                "Invalid status characters in [", status,
                                                "], status characters must be from: [",
                                                ALLOWED_STATUS_CHARS, "]");

        for (std::size_t i = 0; i < _word_size; ++i)
        {
            const char c = str[i];
            const char s = status[i];
            if (s == 'b')
            {
                _word_ptr->does_not_exist(c, i);
                // Count all the occurrence where same character has appeared with 'y' as status
                // only allow that many occurrences of the character in a word
                // of course, this is valid only if at least one such character's status is 'b'
                // e.g. in if test string is 'apple' and status of one 'p' is 'b' but it is 'y' for the other 'p'
                // then, no string with two or more 'p's are allowed, but strings with one 'p' are allowed
                std::size_t y_count{0}; // initialized with 0 as what we have found is 'b' but we are looking for 'y'
                std::size_t g_count{0}; // initialized with 0 as what we have found is 'b' but we are looking for 'g'
                for (std::size_t ii = 0; ii < _word_size; ++ii)
                {
                    if (str[ii] != c) continue;

                    if (status[ii] == 'y')
                    {
                        _word_ptr->does_not_exist(c, ii);
                        ++y_count;
                    }

                    else if (status[ii] == 'g')
                    {
                        _word_ptr->exists(c, ii);
                        ++g_count;
                    }
                }

                if (y_count == 0 && g_count == 0) _word_ptr->does_not_exist(c);
                if (y_count > 0) _word_ptr->remove_if_ge_n(c, y_count + 1);
                if (g_count > 0) _word_ptr->remove_if_ge_n(c, g_count + 1);

                // Already processed all @c in the string, so let's just mark them processed
                for (std::size_t itr{0}; itr < _word_size; ++itr)
                {
                    if (str[itr] == c)
                    {
                        str[itr] = '\0';
                        status[itr] = '\0';
                    }
                }

            }
            else if (s == 'y')
            {
                _word_ptr->exists(c);
                _word_ptr->does_not_exist(c, i);
            }
            else if (s == 'g')
            {
                _word_ptr->exists(c, i);
            }
        }
        return _word_ptr->size();
    }


    [[nodiscard]] std::vector<std::string> get_words() const
    {
        return _word_ptr->strings();
    }

    [[nodiscard]] std::vector<std::string> get_n_words(std::size_t n) const
    {
        return _word_ptr->get_n_words(n);
    }

    [[nodiscard]] std::size_t size() const noexcept
    {
        return _word_ptr->size();
    }

private:

    std::size_t _word_size;
    Words::Ptr _word_ptr;
};
