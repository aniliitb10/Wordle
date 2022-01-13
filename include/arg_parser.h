#pragma once

#include <string>
#include <vector>
#include <unordered_set>
#include <map>
#include <exception>
#include <sstream>
#include <optional>
#include <charconv>
#include <algorithm>
#include <type_traits>
#include <tuple>

/*
 * A header only library to parse command line arguments
 * Sample use case:
 *      ArgParser argParser{" a command line app"}; // description which also becomes part of help message
 *      argParser.add("-l", "--logFile", "log file path");
 *      argParser.add("-w", "--waitTime", "Wait time for application");
 *      argParser.parse(argc, argv);
 *      if (argParser.needHelp())
 *      {
 *          log(argParser.helpMsg()); // in this case, @parse() didn't parse anything as there was nothing to parse
 *          return 0;
 *      }
 *
 * Retrieving values:
 *      std::string logFile = *argParser.retrieve("l") // or *argParser.retrieve("logFile")
 *      const auto valueOpt = argParser.retrieve<int>(w); // or argParser.retrieve<int>("waitTime")
 *      if(valueOpt) // assert that conversion succeeded
 *      {
 *          const auto waitTime = *valueOpt
 *      }
 *
 *      @Note that @retrieve returns std::optional<T> where status of std::optional represents
 *       - if the arg was configured and found command line options
 *       - and if the conversion from std::string to T was successful
 *      So, the status MUST be checked before using the retrieved value
 *
 *      @retrieve has been specialized for: std::string, bool and char
 *      std::string -> no need to check the status of optional because conversion status will always be true
 *      bool -> if source string is (case sensitive) "true"/"false", then conversion succeeds to corresponding values
 *              otherwise conversion fails and returned value is std::nullopt
 *
 * Help message:
 *      If application is run with -h or --help flag, then @needHelp returns true and user should call @helpMsg
 *      and must return because, in this case when parse() was called, no actual parsing was done
 *      Otherwise, any attempt to retrieve any value will throw exception
 *
 * Additional details:
 *      - Functions which are not noexcept might throw exception
 *      - arguments must only consist of alphanumeric characters and underscores
 *      - short option must be shorter in size than the long option
 * */
class ArgParser
{
public:
    ArgParser();

    // This @description becomes part of help message
    explicit ArgParser(std::string description);

    // To add an argument, pass true for the boolean to make the argument mandatory e.g.:
    // 1) argParser.addArgumentWithDefault("-w", "--waitTime", "Wait time");
    // 2) argParser.addArgumentWithDefault("-l", "--logfile", "Log file path", true);  // now it is mandatory
    // It will throw exception if:
    //   - format is invalid, expected format is as shown above in examples
    //   - passed arguments are already configured
    ArgParser &addArgument(const std::string &shortOpt, const std::string &longOpt, const std::string &helpMsg,
                           bool isMandatory = false);

    // to add an argument, with default value
    // 1) argParser.addArgumentWithDefault("-w", "--waitTime", "Wait time", 10);
    // 2) argParser.addArgumentWithDefault("-l", "--logfile", "Log file", "/var/tmp/app.log");
    template<typename T = std::string>
    ArgParser &addArgumentWithDefault(const std::string &shortOpt, const std::string &longOpt,
                                      const std::string &helpMsg, const T &defaultValue);

    // To parse the command line arguments
    // Should be called with the arguments of main function
    // It will throw exception if:
    //  - Any of the mandatory configured argument are not passed
    //  - Configured arguments are repeated
    void parse(int argc, char *argv[]);

    // As the first command line argument is always application path, it returns the same
    const std::string &getAppPath() const noexcept;

    // To retrieve value corresponding to @arg
    // If returned value is std::nullopt, it could be because:
    //  - @arg was not configured/passed
    //  - conversion from std::string to type T failed
    // All values of arguments are stored as std::string, hence conversion to std::string always succeeds
    // throws iff it is called before calling parse()
    template<typename T = std::string>
    std::optional<T> retrieve(const std::string &arg) const;

    // Retrieves the value for @arg and throws if:
    //   - conversion to type T fails
    //   - the arg was not configured/passed
    //   - this was called before parse()
    template<typename T = std::string>
    T retrieveMayThrow(const std::string &arg) const;

    // To get the help message for this application with configured arguments
    std::string helpMsg() const noexcept;

    // Returns true if application was called with -h or --help
    bool needHelp() const noexcept;

    // To get the description of application, the argument of the parameterized constructor
    const std::string &getDescription() const noexcept;

    // Returns true if the application was run with @arg as one of command line arguments
    bool contains(const std::string &arg) const noexcept;

private:

    // Following is a set of helper classes and enum
    // We don't need to expose these to the user, hence made them private
    //  - little difficult to read and maintain, but better for user
    //  - user just sees the public function of ArgParser
    enum class ArgTypeEnum
    {
        Short,
        Long,
        Invalid
    };

    struct ParsedArg
    {
        ArgTypeEnum argType;
        std::string parsedArg;

        static ParsedArg parse_arg(const std::string &arg);
    };

    // A class which deals with parsing configured arguments
    class Arg
    {
    public:

        explicit Arg(const std::string &shortArg, const std::string &longArg, std::string helpMsg,
                     bool isMandatory = false);

        explicit Arg(const std::string &shortArg, const std::string &longArg, std::string helpMsg,
                     std::string defaultValue);

        bool operator==(const Arg& rhs) const noexcept;

        bool operator!=(const Arg& rhs) const noexcept;

        bool operator<(const Arg& rhs) const noexcept;  // useful when Arg will be key inside std::map

        [[nodiscard]] const std::string &getShortOpt() const noexcept;

        [[nodiscard]] const std::string &getLongOpt() const noexcept;

        [[nodiscard]] bool match(const ParsedArg &arg) const;

        [[nodiscard]] bool match(const std::string &arg) const noexcept;

        [[nodiscard]] bool hasDefaultValue() const noexcept;

        // throws exception if no default value is set
        [[nodiscard]] const std::string &getDefaultValue() const;

        [[nodiscard]] std::string toString() const noexcept;

        [[nodiscard]] std::string toVerboseString() const noexcept;

        // Returns true if the argument is mandatory
        [[nodiscard]] bool isMandatory() const noexcept;

    private:
        void init_args(const std::string &shortArg, const std::string &longArg);

        std::string shortOpt{};
        std::string longOpt{};
        std::string helpMsg{};
        std::string defaultValue{};
        bool hasDefault{false};
        bool mandatory{false};
    };

    // To convert a type into std::string, can't be dependent on just std::string
    // as it is callable only on numeric types
    // a helper for @concatenate method
    template<typename T>
    static std::string to_string(T &&arg) noexcept;

    // To concatenate strings, fmt::format would have been ideal
    // but we need to keep this header only library
    template<typename ...Ts>
    static std::string concatenate(Ts &&...args) noexcept;

    // preprocess to ensure that help works as expected
    void init() noexcept;

    // ensures that parse() was called or if there was application was called with -h or --help
    void validateRetrieval() const;

    // check if all the mandatory arguments have been passed, throws otherwise
    void checkMandatoryArgs() const;

    // parses a key=value pair, throws iff format is incorrect
    static std::pair<ParsedArg, std::string> argValueParser(const std::string &arg);

    // finds the arg from list of configured args
    std::optional<Arg> findArg(const ParsedArg &arg) const noexcept;

    static bool isHelpString(const std::string &arg) noexcept;

    ArgParser &addArgumentImpl(Arg &&arg);

    // To convert from std::string to type T
    // Following is how it is different from std::from_string:
    //  - conversion to bool from "true" or "false" is supported
    //  - conversion to char succeeds only if arg.size() is 1
    //  - of course, if T is std::string, arg is returned
    // for all numerical types, the behavior is same as that of std::from_string
    template<typename T>
    static std::optional<T> from_string(const std::string &arg) noexcept;

    // To describe the app
    std::string description;

    // To keep the configured arguments
    std::vector<Arg> configuredArgs{};

    // Used to ensure that any combination of configured arguments don't match with others
    std::unordered_set<std::string> argKeys{};

    // Used to keep the configured argument against their passed values in command line arguments
    std::map<ArgParser::Arg, std::string> parsedArgs{};
    std::string appPath{};
    bool isOnlyHelpString{false};

    static constexpr auto truthy = "true";
    static constexpr auto falsy = "false";
};

inline
ArgParser &ArgParser::addArgument(const std::string &shortOpt, const std::string &longOpt, const std::string &helpMsg,
                                  bool isMandatory)
{
    return addArgumentImpl(Arg{shortOpt, longOpt, helpMsg, isMandatory});
}

template<typename T>
ArgParser &ArgParser::addArgumentWithDefault(const std::string &shortOpt, const std::string &longOpt,
                                             const std::string &helpMsg, const T &defaultValue)
{
    return addArgumentImpl(Arg{shortOpt, longOpt, helpMsg, to_string(defaultValue)});
}

inline
ArgParser &ArgParser::addArgumentImpl(Arg &&arg)
{
    if (!argKeys.emplace(arg.getShortOpt()).second || !argKeys.emplace(arg.getLongOpt()).second)
    {
        throw std::runtime_error{concatenate("Duplicate arguments: ", arg.toString())};
    }
    configuredArgs.emplace_back(std::move(arg));
    return *this;
}

inline
void ArgParser::parse(int argc, char *argv[])
{
    if (argc < 1 || argv == nullptr)
    {
        throw std::runtime_error{"Invalid command line arguments"};
    }

    appPath = argv[0];
    if (argc == 2 && isHelpString(argv[1]))
    {
        isOnlyHelpString = true;
        return;
    }

    for (int i = 1; i < argc; ++i)
    {
        const auto argValPair = argValueParser(argv[i]);
        const auto argOpt = findArg(argValPair.first);
        if (!argOpt)
        {
            throw std::runtime_error(concatenate("Unknown argument: ", argValPair.first.parsedArg));
        }
        if (!parsedArgs.emplace(*argOpt, argValPair.second).second)
        {
            throw std::runtime_error{concatenate("Received multiple values for: ", argOpt->toString())};
        }
    }
    // now add those arguments which have default values
    for (const auto &arg: configuredArgs)
    {
        if (arg.hasDefaultValue())
        {
            parsedArgs.emplace(arg, arg.getDefaultValue()); // it won't override if arg already exists
        }
    }

    checkMandatoryArgs();
}

inline
std::pair<ArgParser::ParsedArg, std::string> ArgParser::argValueParser(const std::string &arg)
{
    static constexpr char SEP{'='};
    auto pos = arg.find(SEP);
    if (pos == 0 || pos == std::string::npos)
    {
        throw std::runtime_error(concatenate("Separator [", SEP, "] is supposed to separate arg and value in: ", arg));
    }

    const auto parsedArg{ParsedArg::parse_arg(arg.substr(0, pos))};
    return std::make_pair(parsedArg, arg.substr(pos + 1));
}

inline
std::optional<ArgParser::Arg> ArgParser::findArg(const ParsedArg &arg) const noexcept
{
    auto argItr = std::find_if(configuredArgs.cbegin(), configuredArgs.cend(),
                               [&arg](const Arg &eachArg)
                               { return eachArg.match(arg); });
    if (argItr != configuredArgs.cend())
    {
        return *argItr;
    }
    return std::nullopt;
}

template<typename T>
std::optional<T> ArgParser::retrieve(const std::string &arg) const
{
    validateRetrieval();
    const auto itr = std::find_if(parsedArgs.cbegin(), parsedArgs.cend(),
                                  [&arg](const std::pair<Arg, std::string> &argValuePair)
                                  { return argValuePair.first.match(arg); });
    if (itr != parsedArgs.cend())
    {
        return from_string<T>(itr->second);
    }
    return std::nullopt;
}

template<typename T>
inline
std::optional<T> ArgParser::from_string(const std::string &arg) noexcept
{
    if constexpr(std::is_same_v<T, std::string>)
    {
        return arg;
    }
    else if constexpr(std::is_same_v<T, bool>)
    {
        if (arg == truthy) return true;
        if (arg == falsy)  return false;
        return std::nullopt;
    }
    else if constexpr(std::is_same_v<T, char>)
    {
        return (arg.size() == 1) ? std::optional<char>(arg[0]) : std::nullopt;
    }
    else if constexpr(std::is_arithmetic_v<std::remove_pointer_t<std::remove_reference_t<T>>>)
    {
        T result{};
        auto end_ptr = arg.c_str() + arg.size();
        auto[ptr, ec] = std::from_chars(arg.c_str(), end_ptr, result);
        if (ptr == end_ptr && ec == std::errc{})
        {
            return result;
        }
    }

    return std::nullopt;
}

inline
std::string ArgParser::helpMsg() const noexcept
{
    std::ostringstream os{};

    if (!getDescription().empty())
    {
        os << getDescription() << "\n";
    }

    os << "Following is a list of configured arguments:\n";

    for (const auto &arg : configuredArgs)
    {
        os << arg.toVerboseString() << "\n";
    }
    return os.str();
}

inline
bool ArgParser::isHelpString(const std::string &arg) noexcept
{
    static constexpr auto ShortHelpString{"-h"};
    static constexpr auto LongHelpString{"--help"};

    return arg == ShortHelpString || arg == LongHelpString;
}

inline
bool ArgParser::needHelp() const noexcept
{
    return isOnlyHelpString;
}

inline
ArgParser::ArgParser(std::string description) : description(std::move(description))
{
    init();
}

inline
const std::string &ArgParser::getDescription() const noexcept
{
    return description;
}

template<typename T>
T ArgParser::retrieveMayThrow(const std::string &arg) const
{
    const auto retrieved_opt = retrieve<T>(arg);
    if (retrieved_opt)
    {
        return *retrieved_opt;
    }
    throw std::runtime_error{concatenate("Type conversion failed for ", arg)};
}

inline
bool ArgParser::contains(const std::string &arg) const noexcept
{
    // although iterating a map is not ideal but not command line arguments are always only few
    return std::find_if(parsedArgs.cbegin(), parsedArgs.cend(), [&arg](const auto &parsedArg)
    {
        return parsedArg.first.match(arg);
    }) != parsedArgs.cend();
}

inline
void ArgParser::init() noexcept
{
    // the default help argument
    Arg helpArg{"-h", "--help", "to get this message"};
    argKeys.emplace(helpArg.getShortOpt());
    argKeys.emplace(helpArg.getLongOpt());
    configuredArgs.emplace_back(std::move(helpArg));
}

inline
ArgParser::ArgParser()
{
    init();
}

inline
void ArgParser::validateRetrieval() const
{
    if (appPath.empty())
    {
        // this means parse() was never called
        throw std::runtime_error{"parse() must be called with command line arguments before retrieving values"};
    }

    if (needHelp())
    {
        throw std::runtime_error{"Application was run with '-h' or '--help', retrieving values is not allowed. "
                                 "Should call @helpMsg and return"};
    }
}

inline
void ArgParser::checkMandatoryArgs() const
{
    for (const auto &configuredArg : configuredArgs)
    {
        if (configuredArg.isMandatory())
        {
            if (parsedArgs.find(configuredArg) == parsedArgs.end())
            {
                throw std::runtime_error{concatenate("Couldn't find [", configuredArg.toString(),
                                                     "] mandatory argument in passed arguments")};
            }
        }
    }
}

inline
const std::string &ArgParser::getAppPath() const noexcept
{
    return appPath;
}

template<typename T>
std::string ArgParser::to_string(T &&arg) noexcept
{
    // std::decay to remove cv and reference qualifiers
    if constexpr(std::is_same_v<std::decay_t<T>, std::string>) return arg;
    else if constexpr(std::is_constructible_v<std::string, std::decay_t<T>>) return std::string{std::forward<T>(arg)};
    else if constexpr(std::is_same_v<char, std::decay_t<T>>) return {arg};
    else if constexpr(std::is_same_v<bool, std::decay_t<T>>) return arg ? truthy : falsy;
    // otherwise all we support is arithmetic values, let std::to_string take care of issues
    else return std::to_string(std::forward<T>(arg)); // wow! this was fun! without else, return fails to compile!
}

template<typename ...Ts>
std::string ArgParser::concatenate(Ts &&...args) noexcept
{
    // This is inefficient compared to fmt::format, but we need to keep this header only library
    // and btw, this happens only in error cases
    return (ArgParser::to_string(std::forward<Ts>(args)) + ...);
}

inline
ArgParser::Arg::Arg(const std::string &shortArg, const std::string &longArg, std::string helpMsg, bool isMandatory) :
        helpMsg(std::move(helpMsg)),
        hasDefault(false),
        mandatory(isMandatory)
{
    init_args(shortArg, longArg);
}

inline
ArgParser::Arg::Arg(const std::string &shortArg, const std::string &longArg, std::string helpMsg,
                    std::string defaultValue) :
        helpMsg(std::move(helpMsg)),
        defaultValue(std::move(defaultValue)),
        hasDefault(true)
{
    init_args(shortArg, longArg);
}

inline
bool ArgParser::Arg::operator==(const Arg &rhs) const noexcept
{
    return shortOpt == rhs.shortOpt && longOpt == rhs.longOpt;
}

inline
bool ArgParser::Arg::operator!=(const Arg &rhs) const noexcept
{
    return !(*this == rhs);
}

inline
bool ArgParser::Arg::operator<(const Arg &rhs) const noexcept
{
    return std::tie(shortOpt, longOpt) < std::tie(rhs.shortOpt, rhs.longOpt);
}

inline
const std::string &ArgParser::Arg::getShortOpt() const noexcept
{
    return shortOpt;
}

inline
const std::string &ArgParser::Arg::getLongOpt() const noexcept
{
    return longOpt;
}

inline
bool ArgParser::Arg::match(const ParsedArg &arg) const
{
    switch (arg.argType)
    {
        case ArgTypeEnum::Short:
            return arg.parsedArg == this->shortOpt;

        case ArgTypeEnum::Long:
            return arg.parsedArg == this->longOpt;

        case ArgTypeEnum::Invalid:
            throw std::runtime_error{concatenate("Attempting to match with Invalid argument")};
    }
    return false; // just to make compiler happy
}

inline
bool ArgParser::Arg::match(const std::string &arg) const noexcept
{
    return arg == shortOpt || arg == longOpt;
}

inline
std::string ArgParser::Arg::toString() const noexcept
{
    return concatenate('-', shortOpt, ", --", longOpt);
}

inline
bool ArgParser::Arg::hasDefaultValue() const noexcept
{
    return hasDefault;
}

inline
const std::string &ArgParser::Arg::getDefaultValue() const
{
    if (hasDefaultValue())
    {
        return defaultValue;
    }
    throw std::runtime_error{concatenate("There was no default set for: ", toString())};
}

inline
void ArgParser::Arg::init_args(const std::string &shortArg, const std::string &longArg)
{
    // @shortArg and @longArg are only being forwarded till ParsedArg::parse_arg
    // hence, it is better to accept them as reference to const std::string instead of keep moving
    const auto shortParsedArg = ParsedArg::parse_arg(shortArg);
    const auto longParsedArg = ParsedArg::parse_arg(longArg);

    if (shortParsedArg.argType != ArgTypeEnum::Short)
    {
        throw std::runtime_error{concatenate("Invalid short option: ", shortArg)};
    }

    if (longParsedArg.argType != ArgTypeEnum::Long)
    {
        throw std::runtime_error{concatenate("Invalid long option: ", longArg)};
    }

    // both can't be same parsed arguments
    if (shortParsedArg.parsedArg.size() >= longParsedArg.parsedArg.size())
    {
        throw std::runtime_error{
                concatenate("Short option [", shortArg, "] must be shorter than Long option [", longArg, ']')};
    }
    shortOpt = shortParsedArg.parsedArg;
    longOpt = longParsedArg.parsedArg;
}

inline
std::string ArgParser::Arg::toVerboseString() const noexcept
{
    if (hasDefaultValue())
    {
        return concatenate(toString(), "\n\tdescription: ", helpMsg, ", default: ", getDefaultValue());
    }

    if (isMandatory())
    {
        return concatenate(toString(), "\n\tdescription: ", helpMsg, ", mandatory: ",
                           isMandatory() ? "true" : "false");
    }

    return concatenate(toString(), "\n\tdescription: ", helpMsg);
}

inline
bool ArgParser::Arg::isMandatory() const noexcept
{
    return mandatory;
}

inline
ArgParser::ParsedArg ArgParser::ParsedArg::parse_arg(const std::string &arg)
{
    static constexpr char us = '_';

    const auto pos = arg.find_first_not_of('-');
    if (pos == 0 || pos == std::string::npos)
    {
        return ParsedArg{ArgTypeEnum::Invalid, arg};
    }

    if (!std::all_of(std::next(arg.cbegin(), static_cast<long>(pos)), arg.cend(),
                     [](unsigned char c) { return std::isalnum(c) || c == us; }))
    {
        return ParsedArg{ArgTypeEnum::Invalid, arg};
    }

    if (pos == 1)
    {
        return ParsedArg{ArgTypeEnum::Short, arg.substr(pos)};
    }

    if (pos == 2)
    {
        return ParsedArg{ArgTypeEnum::Long, arg.substr(pos)};
    }

    return ParsedArg{ArgTypeEnum::Invalid, arg};
}