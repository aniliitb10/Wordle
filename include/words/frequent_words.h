#pragma once

#include <utility>
#include <vector>
#include <string>
#include <utility>
#include <map>
#include <algorithm>
#include <util.h>
#include <words/words.h>


/**
 * A class which suggests words based on how frequently they are used in the language
 * - source: https://www.kaggle.com/rtatman/english-word-frequency
 * - it also updates potential list of words as per the feedback from the game
 */

class FrequentWords : public Words
{
public:
    explicit FrequentWords(std::vector<std::pair<std::string, std::size_t>> words, std::size_t word_size = 5);

    explicit FrequentWords(const std::string &filepath = "../dictionary/5_words_freq.txt", size_t word_size = 5);

    /**
     * Get the data used by this class internally
     * @return a vector of pair of words and its frequency
     */
    [[nodiscard]] const std::vector<std::pair<std::string, std::size_t>> &data() const noexcept;

    /**
     * Used to get n words from remaining potential candidates
     * If n > number of potential words, then all words are returned
     * If there are no potential candidates, empty vector is returned
     * This will be expensive if n is too high, as it will be created on the fly
     * @param n: the number of requested words
     * @return a vector of @n words which are potential candidates
     */
    [[nodiscard]] std::vector<std::string> get_n_words(std::size_t n) const noexcept override;

    /**
     * Ensure that @c exists in all words.
     * If there is a word which doesn't contain @c,
     *  it should be removed from the list of potential candidates
     * @param c: the character which exists in all words
     */
    void exists(char c) noexcept override;

    /**
     * Ensure that @c exists in all words at index=@pos
     * If there is a word which doesn't contain @c at index=@pos,
     *  it should be removed from the list of potential candidates
     * @param c: the character which exists in all words
     * @param pos: the index at which @c exists in all words
     */
    void exists(char c, std::size_t pos) override;

    /**
     * Ensure that if a word contains @c at any index, it is no longer a potential candidate
     * @param c
     */
    void does_not_exist(char c) noexcept override;

    /**
     * Ensure that if a word contains @c at index=@pos, it is no longer a potential candidate
     * @param c: the character which should not be at index=@pos in any potential candidates
     * @param pos: if @c exists at @pos in a word, then that word is no longer a potential candidate
     */
    void does_not_exist(char c, std::size_t pos) override;

    /**
     * Returns the size of the potential candidates
     * @return: size of the potential candidates
     */
    [[nodiscard]] std::size_t size() const noexcept override;

    /**
     * Gets the size of each word
     * @return size of each word
     */
    [[nodiscard]] std::size_t get_each_word_size() const noexcept override;

    /**
     * Remove a word from list of potential candidates if it has greater than or equal to @n occurrences of @c in it
     * @param c the character whose occurrences is being checked in every word
     * @param n the count of occurrences which makes @c eligible for removal from potential candidates
     */
    void remove_if_ge_n(char c, std::size_t n) noexcept override;

    /**
     * Returns a list of all potential candidate strings
     * - this is a HIGHLY EXPENSIVE call, since strings are generated on the fly in this implementation
     * - so it must be used judiciously
     * @return a list of all potential candidate strings
     */
    [[nodiscard]] std::vector<std::string> strings() const noexcept override;

private:
    template<class Func>
    void filter(const Func &func);

    // some helpful aliases
    using word_count_t = std::pair<std::string, std::size_t>;
    // using count_word_t = std::pair<std::size_t, std::string>;

    std::size_t _word_size;
    std::vector<word_count_t> _word_count_list{};
};

inline
FrequentWords::FrequentWords(const std::string &filepath, size_t word_size) :
        FrequentWords(read_lines<word_count_t>(filepath, true, [](const std::string &str)
        {
            auto pos = str.find(',');
            assert_statement(pos != std::string::npos, "Couldn't find separator ',' in ", str);
            auto word = str.substr(0, pos);
            auto count = from_string(str.substr(pos + 1));
            return word_count_t{word, count};
        }), word_size)
{}

inline
FrequentWords::FrequentWords(std::vector<std::pair<std::string, std::size_t>> words, std::size_t word_size) :
        _word_count_list(std::move(words)),
        _word_size(word_size)
{
    filter([this](const word_count_t &entry) { return entry.first.size() != _word_size; });
}

template<class Func>
void FrequentWords::filter(const Func &func)
{
    // std::erase(_word_count_list, func);
    auto itr = std::remove_if(_word_count_list.begin(), _word_count_list.end(), func);
    _word_count_list.erase(itr, _word_count_list.end());
}

inline
const std::vector<std::pair<std::string, std::size_t>> &FrequentWords::data() const noexcept
{
    return _word_count_list;
}

inline
void FrequentWords::exists(char c) noexcept
{
    filter([c](const word_count_t &entry) { return entry.first.find(c) == std::string::npos; });
}

inline
void FrequentWords::exists(char c, std::size_t pos)
{
    validate_index(pos);
    filter([c, pos](const word_count_t &entry) { return entry.first[pos] != c; });
}

inline
void FrequentWords::does_not_exist(char c) noexcept
{
    filter([c](const word_count_t &entry) { return entry.first.find(c) != std::string::npos; });
}

inline
void FrequentWords::does_not_exist(char c, std::size_t pos)
{
    validate_index(pos);
    filter([c, pos](const word_count_t &entry) { return entry.first[pos] == c; });
}

inline
std::vector<std::string> FrequentWords::get_n_words(std::size_t n) const noexcept
{
    std::vector<std::string> words{};
    auto begin_itr = _word_count_list.cbegin();
    auto end_itr = std::next(begin_itr, static_cast<int>(std::min(n, _word_count_list.size())));

    std::transform(begin_itr, end_itr, std::back_inserter(words),
                   [](const word_count_t &entry) { return entry.first; });
    return words;
}

inline
std::size_t FrequentWords::size() const noexcept
{
    return _word_count_list.size();
}

inline
std::size_t FrequentWords::get_each_word_size() const noexcept
{
    return _word_size;
}

inline
void FrequentWords::remove_if_ge_n(char c, std::size_t n) noexcept
{
    filter([c, n](const word_count_t &entry) { return std::count(entry.first.begin(), entry.first.end(), c) >= n; });
}

inline
std::vector<std::string> FrequentWords::strings() const noexcept
{
    std::vector<std::string> strings{};
    std::transform(_word_count_list.begin(), _word_count_list.end(), std::back_inserter(strings),
                   [](const word_count_t &entry) { return entry.first; });

    return strings;
}
