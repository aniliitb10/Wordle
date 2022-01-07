#include <gtest/gtest.h>
#include <wordle.h>
#include <iostream>

int main(int argc_, char** argv_)
{
    // use case:
    Wordle wordle{5};
    wordle.update("stink", "bbbgg");
    wordle.update("drunk", "bgbgg");
    wordle.update("prank", "bgggg");
    wordle.update("frank", "bgggg");
    // wordle.update("crank", "ggggg");

    std::cout << "Possible words\n" << wordle.get_words() << "\n";
    std::cout << "Possible words count: " << wordle.get_words().size() << "\n";

    testing::InitGoogleTest(&argc_, argv_);
    return RUN_ALL_TESTS();
}