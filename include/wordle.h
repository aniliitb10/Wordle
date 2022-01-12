#pragma once

#include <vector>
#include <string>
#include <algorithm>
#include <util.h>
#include <type_traits>
#include <sstream>
#include <exception>

/*
 * A program to play and win Wordle (https://www.powerlanguage.co.uk/wordle/)
 * This helps you in filtering out the rejected chars, use the known positions
 * tried with wordle of Jan 7, 2021 and found 'slump' in 4th attempt
 * */
class Wordle
{
public:
    Wordle(std::size_t word_size, const std::vector<std::string> &words) :
            _word_size(word_size)
    {
        for (auto &word: words)
        {
            if (word.size() == _word_size)
                _words.push_back(word);
        }
    }

    explicit Wordle(std::size_t word_size, const std::string &file_path = "../dictionary/words.txt") :
            Wordle(word_size, read_lines(file_path))
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
        if (status.find_first_not_of(ALLOWED_STATUS_CHARS) != std::string::npos)
        {
            std::ostringstream os;
            os << "Invalid status characters in [" << status << "], status characters must be from: ["
               << ALLOWED_STATUS_CHARS << "]\n";
            throw std::invalid_argument{os.str()};
        }

        if (str.size() != _word_size || status.size() != _word_size)
        {
            std::ostringstream os;
            os << "Invalid number of characters in [" << str << "], and [" << status << "], they must contain exactly ["
               << _word_size << "] characters\n";
            throw std::invalid_argument{os.str()};
        }

        for (std::size_t i = 0; i < _word_size; ++i)
        {
            const char c = str[i];
            const char s = status[i];
            if (s == 'b')
            {
                remove(c, i);
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
                        remove(c, ii);
                        ++y_count;
                    }

                    else if (status[ii] == 'g')
                    {
                        exists(c, ii);
                        ++g_count;
                    }
                }

                if (y_count == 0 && g_count == 0)   remove(c);
                if (y_count > 0)                    remove_n(c, y_count + 1);
                if (g_count > 0)                    remove_n(c, g_count + 1);

                // Already processed all @c in the string, so let's just mark them processed
                for (std::size_t itr{0}; itr < _word_size; ++itr)
                {
                    if (str[itr] == c)
                    {
                        str[itr] = '\0';
                        status[itr] = '\0';
                    }
                }

            } else if (s == 'y')
            {
                exists(c);
                remove(c, i);
            } else if (s == 'g')
            {
                exists(c, i);
            }
        }
        return clean_up().size();
    }


    [[nodiscard]] const std::vector<std::string> &get_words() const
    {
        return _words;
    }

private:

    // A function to filter the existing set of words
    // func is applied on every word, all words for which func returns true, get filtered out
    template<class Func>
    void filter(Func func)
    {
        for (auto &word: _words)
        {
            if (func(word))
            {
                word.erase();
            }
        }
        clean_up();
    }

    // index must be <= @_word_size
    void validate_index(std::size_t index) const
    {
        if (index >= _word_size)
        {
            std::ostringstream os;
            os << "char index: [" << index << "] must be less than word size: [" << _word_size << "]\n";
            throw std::runtime_error{os.str()};
        }
    }

    // @c exists in the string, not sure about its position though
    void exists(char c) noexcept
    {
        return filter([c](const std::string &word)
                      { return word.find(c) == std::string::npos; });
    }

    // @c exists in the string at @index
    void exists(char c, std::size_t index)
    {
        validate_index(index);
        return filter([c, index](const std::string &word)
                      { return word[index] != c; });
    }

    // removes all words which contain @c at @index
    void remove(char c, std::size_t index)
    {
        validate_index(index);
        return filter([c, index](const std::string &word)
                      { return word[index] == c; });
    }

    // removes all words which contain @c
    void remove(char c)
    {
        return filter([c](const std::string &word)
                      { return word.find(c) != std::string::npos; });
    }

    /**
     * Removes all words for which count of char @c in the word >= n
     * @param c: character to compare
     * @param n: if count of @c is >= @n, then the word should be removed
     */
    void remove_n(char c, std::size_t n)
    {
        return filter([c, n](const std::string &word)
                      { return std::count(word.cbegin(), word.cend(), c) >= n; });
    }

    // cleans up: removes empty strings from the vector efficiently and returns updated list
    std::vector<std::string> &clean_up()
    {
        auto end_pos = std::remove_if(std::begin(_words), std::end(_words),
                                      [](const std::string &str)
                                      { return str.empty(); });
        _words.erase(end_pos, _words.end());
        return _words;
    }

    std::size_t _word_size;
    std::vector<std::string> _words;
};