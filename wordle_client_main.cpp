#include <wordle_client.h>
#include <iostream>
#include <util.h>

int main(int argc, char** argv)
{
    if (argc != 1 && argc != 3)
    {
        std::cerr << "Invalid arguments, there should be either 0 or 2(for word size and display-limit) arguments\n";
        return -1;
    }

    const std::size_t word_size{ (argc == 1) ? 5 : from_string(argv[1])};
    const std::size_t display_limit{ (argc == 1) ? 10 : from_string(argv[2])};
    WordleClient client{word_size, display_limit};
    client.run();
}