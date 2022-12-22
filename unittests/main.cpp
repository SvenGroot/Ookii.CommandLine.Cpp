#include "common.h"
#include <ookii/command_line.h>
#include "framework.h"
using namespace std;
using namespace ookii;

std::mutex ookii::test::details::g_logMutex;
bool ookii::test::g_verbose{};

struct Arguments
{
    tstring Pattern;
    bool Verbose{};

    static std::optional<Arguments> Parse(int argc, tchar_t *argv[])
    {
        auto name = basic_command_line_parser<tchar_t>::get_executable_name(argc, argv);
        Arguments result;
        auto parser = basic_parser_builder<tchar_t>{name}.add_argument(result.Pattern, TEXT("Pattern"))
            .positional().value_description(TEXT("Regex"))
                .description(TEXT("Regular expression used to determine which tests to run. This is matched against the full name of the test (e.g. TestClass::TestMethod). If omitted, all tests are run."))
            .add_argument(result.Verbose, TEXT("Verbose")).alias(TEXT("v"))
                .description(TEXT("Print detailed information about successful tests."))
#ifdef _WIN32
            .add_win32_version_argument()
#endif
            .build();

        if (parser.parse(argc, argv, {}))
            return result;

        return {};
    }
};

int TMAIN(int argc, tchar_t *argv[])
{
    auto parsedArgs = Arguments::Parse(argc, argv);
    if (!parsedArgs)
    {
        return 1;
    }

    try
    {
        std::locale loc{"nl_NL.UTF-8"};
    }
    catch (std::runtime_error &)
    {
        tcout << TEXT("Failed to create locale nl_NL.UTF-8, which is needed for some tests.") << endl;
        tcout << TEXT("On Linux, run 'sudo locale-gen nl_NL.UTF-8'.") << endl;
    }

    std::basic_regex<tchar_t> pattern;
    std::basic_regex<tchar_t> *patternToUse{};
    if (parsedArgs->Pattern.length() > 0)
    {
        pattern = parsedArgs->Pattern;
        patternToUse = &pattern;
    }

    ookii::test::g_verbose = parsedArgs->Verbose;
    if (!test::TestDriver::Instance().RunTests(patternToUse))
    {
        return 1;
    }

    return 0;
}