#include <gtest/gtest.h>
#include <vector>
#include <string>

#include "test_util.h"

struct TestUtilTest : ::testing::Test
{};

TEST_F(TestUtilTest, GetVectorTest)
{
    std::vector<std::string> strings0{};
    auto generated_vec0 = util::get_vector({});
    EXPECT_EQ(strings0, generated_vec0);

    std::vector<std::string> strings1{"item1"};
    auto generated_vec1 = util::get_vector({"item1"});
    EXPECT_EQ(strings1, generated_vec1);

    std::vector<std::string> strings2{"item1", "item2"};
    auto generated_vec2 = util::get_vector({"item1", "item2"});
    EXPECT_EQ(strings2, generated_vec2);

    std::vector<std::string> strings3{"item1", "item2", "item3"};
    auto generated_vec3 = util::get_vector({"item1", "item2", "item3"});
    EXPECT_EQ(strings3, generated_vec3);

    EXPECT_NE(strings1, strings0);
    EXPECT_NE(strings2, strings1);
    EXPECT_NE(strings3, strings2);
    EXPECT_NE(strings3, strings0);
    EXPECT_NE(strings0, strings2);
    EXPECT_NE(strings3, strings1);
}

TEST_F(TestUtilTest, CompareTest)
{
    std::vector<std::string> strings0{};
    EXPECT_TRUE(util::compare(strings0, util::get_vector({})));

    std::vector<std::string> strings1{"item1"};
    EXPECT_TRUE(util::compare(strings1, util::get_vector({"item1"})));

    std::vector<std::string> strings2{"item1", "item2"};
    EXPECT_TRUE(util::compare(strings2, util::get_vector({"item1", "item2"})));

    EXPECT_TRUE(util::compare(strings2, strings2));

    EXPECT_FALSE(util::compare(strings2, util::get_vector({"item1"})));
    EXPECT_FALSE(util::compare(strings1, strings2));
    EXPECT_FALSE(util::compare(strings1, strings0));
    EXPECT_FALSE(util::compare(strings0, strings2));
}