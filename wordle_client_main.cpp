#include <wordle_client.h>
#include <iostream>
#include <external/arg_parser.h>

int main(int argc, char **argv)
{

    ArgParser arg_parser{"A Wordle client!"};
    arg_parser.addArgumentWithDefault<std::size_t>("-w", "--width", "Width of each word in the game", 5);
    arg_parser.addArgumentWithDefault<bool>("-a", "--auto", "To enable auto mode, set to false by default", false);
    arg_parser.addArgumentWithDefault<std::size_t>("-d", "--display_limit",
                                                   "Number of suggestions for next word, useful only if auto-mode is off",
                                                   10);
    arg_parser.parse(argc, argv);

    if (arg_parser.needHelp())
    {
        std::cout << arg_parser.helpMsg() << "\n";
        return 0;
    }

    WordleClient client{arg_parser.retrieveMayThrow<std::size_t>("w"),
                        arg_parser.retrieveMayThrow<std::size_t>("d"),
                        arg_parser.retrieveMayThrow<bool>("a")};
}