#include <gtest/gtest.h>
#include <words/frequent_words.h>
#include <words/words.h>

struct WordsTest : public ::testing::Test
{
    static void validate_frequency_order(const std::vector<std::pair<std::string, std::size_t>>& data)
    {
        std::vector<std::size_t> freq{};
        std::transform(data.begin(), data.end(), std::back_inserter(freq),
                       [](const std::pair<std::string, std::size_t>& entry) { return entry.second; });

        EXPECT_FALSE(freq.empty());
        EXPECT_TRUE(std::is_sorted(freq.cbegin(), freq.cend(), std::greater<>{}));
    }

};

TEST_F(WordsTest, SimpleTest)
{
    FrequentWords words{};
    EXPECT_EQ(words.size(), 7872);
    EXPECT_EQ(words.get_n_words(1)[0], "about");
    EXPECT_EQ(words.data()[0].first, "about");
    EXPECT_EQ(words.data()[0].second, 1226734006u);
}

TEST_F(WordsTest, ExistsTest)
{
    FrequentWords words{};
    const auto& data = words.data();

    words.exists('a');
    for (const auto& entry : data)
    {
        EXPECT_NE(entry.first.find('a'), std::string::npos);
    }
    validate_frequency_order(words.data());
}

TEST_F(WordsTest, ExistsWithPosTest)
{
    FrequentWords words{};
    const auto& data = words.data();

    words.exists('a', 1);
    for (const auto& entry : data)
    {
        EXPECT_EQ(entry.first[1], 'a');
    }
    validate_frequency_order(data);
}

TEST_F(WordsTest, DoesNotExistTest)
{
    FrequentWords words{};
    const auto& data = words.data();

    words.does_not_exist('b');
    for (const auto& entry : data)
    {
        EXPECT_EQ(entry.first.find('b'), std::string::npos);
    }
    validate_frequency_order(data);
}

TEST_F(WordsTest, DoesNotExistWithPosTest)
{
    FrequentWords words{};
    const auto& data = words.data();

    words.does_not_exist('b', 1);
    for (const auto& entry : data)
    {
        EXPECT_NE(entry.first[1], 'b');
    }
    validate_frequency_order(data);
}