#include <vector>
#include <string>

// A macro to test exceptions
#define EXPECT_EXCEPTION( TRY_BLOCK, EXCEPTION_TYPE, MESSAGE )        \
try                                                                   \
{                                                                     \
    TRY_BLOCK;                                                        \
    FAIL() << "exception '" << MESSAGE << "' not thrown at all!";     \
}                                                                     \
catch( const EXCEPTION_TYPE& e )                                      \
{                                                                     \
    EXPECT_EQ( std::string(MESSAGE), e.what() )                       \
        << " exception message is incorrect. Expected the following " \
           "message:\n\n"                                             \
        << MESSAGE << "\n";                                           \
}                                                                     \
catch( ... )                                                          \
{                                                                     \
    FAIL() << "exception '" << MESSAGE                                \
           << "' not thrown with expected type '" << #EXCEPTION_TYPE  \
           << "'!";                                                   \
}


namespace util
{
    template <typename C1, typename C2>
    bool compare (const C1& c1, const C2& c2)
    {
        auto b1 = std::begin(c1);
        auto b2 = std::begin(c2);

        auto e1 = std::end(c1);
        auto e2 = std::end(c2);

        while (b1 != e1 && b2 != e2)
        {
            if (*b1 != *b2) return false;
            std::advance(b1, 1);
            std::advance(b2, 1);
        }

        if (b1 != e1 || b2 != e2) return false;
        return true;
    }

    inline
    std::vector<std::string> get_vector(const std::initializer_list<std::string>& strings)
    {
        std::vector<std::string> result{};
        for (const auto& str: strings)
        {
            result.push_back(str);
        }
        return result;
    }
}