#pragma once


#include <utility>
#include <vector>
#include <string>
#include <map>
#include <algorithm>
#include <util.h>
#include <memory>

class Words
{
public:

    using Ptr = std::shared_ptr<Words>;

    /**
     * Ensure that @c exists in all words.
     * If there is a word which doesn't contain @c,
     *  it should be removed from the list of potential candidates
     * @param c: the character which exists in all words
     */
    virtual void exists(char c) noexcept = 0;

    /**
     * Ensure that @c exists in all words at index=@pos
     * If there is a word which doesn't contain @c at index=@pos,
     *  it should be removed from the list of potential candidates
     * @param c: the character which exists in all words
     * @param pos: the index at which @c exists in all words
     */
    virtual void exists(char c, std::size_t pos) = 0;

    /**
     * Ensure that if a word contains @c at any index, it is no longer a potential candidate
     * @param c: the character which should not be in any potential candidates
     */
    virtual void does_not_exist(char c) noexcept = 0;

    /**
     * Ensure that if a word contains @c at index=@pos, it is no longer a potential candidate
     * @param c: the character which should not be at index=@pos in any potential candidates
     * @param pos: if @c exists at @pos in a word, then that word is no longer a potential candidate
     */
    virtual void does_not_exist(char c, std::size_t pos) = 0;

    /**
     * Used to get n words from remaining potential candidates
     * If n > number of potential words, then all words are returned
     * If there are no potential candidates, empty vector is returned
     * @param n: the number of requested words
     * @return a vector of @n words which are potential candidates
     */
    [[nodiscard]] virtual std::vector<std::string> get_n_words(std::size_t n) const noexcept = 0;

    /**
     * Returns the size of the potential candidates
     * @return: size of the potential candidates
     */
    [[nodiscard]] virtual std::size_t size() const noexcept = 0;

    /**
     * Gets the size of each word
     * @return size of each word
     */
    [[nodiscard]] virtual std::size_t get_each_word_size() const noexcept = 0;

    /**
     * Remove a word from list of potential candidates if it has greater than or equal to @n occurrences of @c in it
     * @param c the character whose occurrences is being checked in every word
     * @param n the count of occurrences which makes @c eligible for removal from potential candidates
     */
    virtual void remove_if_ge_n(char c, std::size_t n) noexcept = 0;

    /**
     * Returns a list of all potential candidate strings
     * @return a list of all potential candidate strings
     */
    [[nodiscard]] virtual std::vector<std::string> strings() const noexcept = 0;

protected:
    virtual void validate_index(std::size_t pos) const
    {
        const auto each_word_size {get_each_word_size()};
        assert_statement(pos < each_word_size, "Index [", pos, "] must be less than word size [", each_word_size, "]");
    }
};
