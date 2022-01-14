#include <gtest/gtest.h>
#include <wordle.h>
#include <vector>
#include <string>

#include "test_util.h"

using strings_t = std::vector<std::string>;

struct WordleTest : ::testing::Test
{
};

TEST_F(WordleTest, SimpleTest)
{
    strings_t strings{"abc", "bcd", "pqr", "abf", "abr"};
    Wordle wordle{3, strings};
    EXPECT_TRUE(util::compare(wordle.get_words(), strings));

    wordle.update("abf", "ggb");
    EXPECT_TRUE(util::compare(wordle.get_words(), util::get_vector({"abc", "abr"})));

    wordle.update("abc", "ggb");
    EXPECT_TRUE(util::compare(wordle.get_words(), util::get_vector({"abr"})));
}

TEST_F(WordleTest, SimpleTest2)
{
    strings_t strings{"abc", "bcd", "pqr", "abf", "abr"};
    Wordle wordle{3, strings};
    EXPECT_TRUE(util::compare(wordle.get_words(), strings));

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