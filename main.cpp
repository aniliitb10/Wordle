#include <gtest/gtest.h>
#include <wordle.h>
#include <iostream>
#include <wordle_client.h>

int main(int argc_, char** argv_)
{
    testing::InitGoogleTest(&argc_, argv_);
    return RUN_ALL_TESTS();
}