#include <gtest/gtest.h>
#include <wordle.h>
#include <vector>
#include <string>
#include <words/frequent_words.h>

#include "test_util.h"

using strings_t = std::vector<std::pair<std::string, std::size_t>>;

struct WordleTest : ::testing::Test
{
    static std::vector<std::string> extract_strings(const std::vector<std::pair<std::string, std::size_t>>& data)
    {
        std::vector<std::string> result{};
        std::transform(data.begin(), data.end(), std::back_inserter(result),
                       [](const std::pair<std::string, std::size_t>& entry) {return entry.first; });
        return result;
    }
};

TEST_F(WordleTest, SimpleTest)
{
    strings_t strings{{"abc", 1}, {"bcd", 1}, {"pqr",1}, {"abf",1}, {"abr",1}};
    Wordle wordle{3, std::make_shared<FrequentWords>(strings, 3)};
    EXPECT_TRUE(util::compare(wordle.get_words(), extract_strings(strings)));

    wordle.update("abf", "ggb");
    EXPECT_TRUE(util::compare(wordle.get_words(), util::get_vector({"abc", "abr"})));

    wordle.update("abc", "ggb");
    EXPECT_TRUE(util::compare(wordle.get_words(), util::get_vector({"abr"})));
}

TEST_F(WordleTest, SimpleTest2)
{
    strings_t strings{{"abc",1}, {"bcd",1}, {"pqr",1}, {"abf",1}, {"abr",1}};
    Wordle wordle{3, std::make_shared<FrequentWords>(strings, 3)};
    EXPECT_TRUE(util::compare(wordle.get_words(), extract_strings(strings)));

    wordle.update("abf", "ggb");
    EXPECT_TRUE(util::compare(wordle.get_words(), util::get_vector({"abc", "abr"})));

    wordle.update("abc", "ggb");
    EXPECT_TRUE(util::compare(wordle.get_words(), util::get_vector({"abr"})));
}

TEST_F(WordleTest, ComplicatedTest)
{
    Wordle wordle{5};
    wordle.update("stink", "bbbgg");

    for (auto& word : wordle.get_words())
    {
        EXPECT_EQ(word.find('s'), std::string::npos);
        EXPECT_EQ(word.find('t'), std::string::npos);
        EXPECT_EQ(word.find('i'), std::string::npos);

        EXPECT_EQ(word[3], 'n');
        EXPECT_EQ(word[4], 'k');
    }

    wordle.update("drunk", "bgbgg");
    for (auto& word : wordle.get_words())
    {
        // from previous operations
        EXPECT_EQ(word.find('s'), std::string::npos);
        EXPECT_EQ(word.find('t'), std::string::npos);
        EXPECT_EQ(word.find('i'), std::string::npos);


        EXPECT_EQ(word.find('d'), std::string::npos);
        EXPECT_EQ(word[1], 'r');

        EXPECT_EQ(word.find('u'), std::string::npos);

        EXPECT_EQ(word[3], 'n');
        EXPECT_EQ(word[4], 'k');
    }

    wordle.update("prank", "bgggg");
    for (auto& word : wordle.get_words())
    {
        // from previous operations
        EXPECT_EQ(word.find('s'), std::string::npos);
        EXPECT_EQ(word.find('t'), std::string::npos);
        EXPECT_EQ(word.find('i'), std::string::npos);
        EXPECT_EQ(word.find('d'), std::string::npos);
        EXPECT_EQ(word.find('u'), std::string::npos);

        EXPECT_EQ(word.find('p'), std::string::npos);

        EXPECT_EQ(word[1], 'r');
        EXPECT_EQ(word[2], 'a');
        EXPECT_EQ(word[3], 'n');
        EXPECT_EQ(word[4], 'k');
    }

    wordle.update("frank", "bgggg");
    for (auto& word : wordle.get_words())
    {
        // from previous operations
        EXPECT_EQ(word.find('s'), std::string::npos);
        EXPECT_EQ(word.find('t'), std::string::npos);
        EXPECT_EQ(word.find('i'), std::string::npos);
        EXPECT_EQ(word.find('d'), std::string::npos);
        EXPECT_EQ(word.find('u'), std::string::npos);
        EXPECT_EQ(word.find('p'), std::string::npos);

        EXPECT_EQ(word.find('f'), std::string::npos);

        EXPECT_EQ(word[1], 'r');
        EXPECT_EQ(word[2], 'a');
        EXPECT_EQ(word[3], 'n');
        EXPECT_EQ(word[4], 'k');
    }

    wordle.update("crank", "ggggg");
    for (auto& word : wordle.get_words())
    {
        // from previous operations
        EXPECT_EQ(word.find('s'), std::string::npos);
        EXPECT_EQ(word.find('t'), std::string::npos);
        EXPECT_EQ(word.find('i'), std::string::npos);
        EXPECT_EQ(word.find('d'), std::string::npos);
        EXPECT_EQ(word.find('u'), std::string::npos);
        EXPECT_EQ(word.find('p'), std::string::npos);
        EXPECT_EQ(word.find('f'), std::string::npos);

        EXPECT_EQ(word[0], 'c');
        EXPECT_EQ(word[1], 'r');
        EXPECT_EQ(word[2], 'a');
        EXPECT_EQ(word[3], 'n');
        EXPECT_EQ(word[4], 'k');
    }
}

TEST_F(WordleTest, InvalidNumberOfCharsExceptionTest)
{
    strings_t strings{{"abc", 1}, {"bcd", 1}, {"pqr",1}, {"abf",1}, {"abr",1}};
    Wordle wordle{3, std::make_shared<FrequentWords>(strings, 3)};
    EXPECT_EXCEPTION(wordle.update("abcd", "gggg"), std::invalid_argument,
                     "Invalid number of characters in [abcd], and/or [gggg], they must contain exactly [3] characters");

    EXPECT_EXCEPTION(wordle.update("abcd", ""), std::invalid_argument,
                     "Invalid number of characters in [abcd], and/or [], they must contain exactly [3] characters");

    EXPECT_EXCEPTION(wordle.update("abcd", "g"), std::invalid_argument,
                     "Invalid number of characters in [abcd], and/or [g], they must contain exactly [3] characters");

    EXPECT_EXCEPTION(wordle.update("a", "ggg"), std::invalid_argument,
                     "Invalid number of characters in [a], and/or [ggg], they must contain exactly [3] characters");

    EXPECT_EXCEPTION(wordle.update("ab", "ggg"), std::invalid_argument,
                     "Invalid number of characters in [ab], and/or [ggg], they must contain exactly [3] characters");
}

TEST_F(WordleTest, InvalidStatusExceptionTest)
{
    strings_t strings{{"abc", 1}, {"bcd", 1}, {"pqr",1}, {"abf",1}, {"abr",1}};
    Wordle wordle{3, std::make_shared<FrequentWords>(strings, 3)};

    EXPECT_EXCEPTION(wordle.update("abc", "abc"), std::invalid_argument,
                     "Invalid status characters in [abc], status characters must be from: [byg]");

    EXPECT_EXCEPTION(wordle.update("abc", "aaa"), std::invalid_argument,
                     "Invalid status characters in [aaa], status characters must be from: [byg]");

    EXPECT_EXCEPTION(wordle.update("abc", "pqr"), std::invalid_argument,
                     "Invalid status characters in [pqr], status characters must be from: [byg]");
}
