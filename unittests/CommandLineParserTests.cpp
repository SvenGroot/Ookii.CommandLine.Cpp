#include "common.h"
#include "framework.h"
#include <ookii/command_line_builder.h>
#include "custom_types.h"
using namespace std;
using namespace ookii;

class CommandLineParserTests : public test::TestClass
{
public:
    TEST_CLASS(CommandLineParserTests);

    // This test must be disabled on clang because std::ranges seem to be broken on it.
    TEST_METHOD(TestEnumerateArguments)
    {
        tstring arg1;
        tstring arg2;
        auto parser = basic_parser_builder<tchar_t>{TEXT("TestCommand")}
            .add_argument(arg1, TEXT("Arg1")).alias(TEXT("a"))
            .add_argument(arg2, TEXT("Arg2"))
            .build();

        int index = 0;
        for (const auto &arg : parser.arguments())
        {
            VERIFY_TRUE(index < 2);
            switch (index)
            {
            case 0:
                VERIFY_EQUAL(TEXT("Arg1"), arg.name());
                break;

            case 1:
                VERIFY_EQUAL(TEXT("Arg2"), arg.name());
                break;
            }

            ++index;
        }
    }

    TEST_METHOD(TestNamed)
    {
        tstring arg1;
        tstring arg2;
        auto parser = basic_parser_builder<tchar_t>{TEXT("TestCommand")}
            .add_argument(arg1, TEXT("Arg1"))
            .add_argument(arg2, TEXT("Arg2"))
            .build();

        VERIFY_TRUE(parser.allow_white_space_separator());
        VERIFY_FALSE(parser.allow_duplicate_arguments());
        VERIFY_EQUAL(TEXT(':'), parser.argument_value_separator());
        VERIFY_EQUAL(TEXT("TestCommand"), parser.command_name());

        VerifyArgument(parser, TEXT("Arg1"), false, false, false, {});
        VerifyArgument(parser, TEXT("Arg2"), false, false, false, {});

        // Specify no arguments.
        VerifyParseResult(parser.parse(std::vector<tstring>{}));
        VERIFY_EQUAL(TEXT(""), arg1);
        VERIFY_EQUAL(TEXT(""), arg2);

        // Specify arg1 by name.
        arg1 = {};
        arg2 = {};
        VerifyParseResult(parser.parse({ TEXT("-Arg1"), TEXT("Value1") }));
        VERIFY_EQUAL(TEXT("Value1"), arg1);
        VERIFY_EQUAL(TEXT(""), arg2);

        // Specify arg2 by name.
        arg1 = {};
        arg2 = {};
        VerifyParseResult(parser.parse({ TEXT("-Arg2"), TEXT("Value2") }));
        VERIFY_EQUAL(TEXT(""), arg1);
        VERIFY_EQUAL(TEXT("Value2"), arg2);

        // Specify both.
        arg1 = {};
        arg2 = {};
        VerifyParseResult(parser.parse({ TEXT("-Arg1"), TEXT("Value3"), TEXT("-Arg2"), TEXT("Value4") }));
        VERIFY_EQUAL(TEXT("Value3"), arg1);
        VERIFY_EQUAL(TEXT("Value4"), arg2);
        VerifyParseResult(parser.parse({ TEXT("-Arg2"), TEXT("Value3"), TEXT("-Arg1"), TEXT("Value4") }));
        VERIFY_EQUAL(TEXT("Value4"), arg1);
        VERIFY_EQUAL(TEXT("Value3"), arg2);

        // Case insensitive
        arg1 = {};
        arg2 = {};
        VerifyParseResult(parser.parse({ TEXT("-arg1"), TEXT("Value1") }));
        VERIFY_EQUAL(TEXT("Value1"), arg1);
        VERIFY_EQUAL(TEXT(""), arg2);

        // Using separator instead of space
        arg1 = {};
        arg2 = {};
        VerifyParseResult(parser.parse({ TEXT("-Arg1:Value2") }));
        VERIFY_EQUAL(TEXT("Value2"), arg1);
        VERIFY_EQUAL(TEXT(""), arg2);

        // positional.
        VerifyParseResult(parser.parse({ TEXT("Value1") }), parse_error::too_many_arguments, {});

        // Missing value.
        VerifyParseResult(parser.parse({ TEXT("-Arg1") }), parse_error::missing_value, TEXT("Arg1"));
        VerifyParseResult(parser.parse({ TEXT("-Arg1"), TEXT("-Arg2") }), parse_error::missing_value, TEXT("Arg1"));
        
        // Unknown argument name.
        VerifyParseResult(parser.parse({ TEXT("-Unknown"), TEXT("Value") }), parse_error::unknown_argument, TEXT("Unknown"));

        // Duplicate.
        VerifyParseResult(parser.parse({ TEXT("-Arg1"), TEXT("Value"), TEXT("-Arg1"), TEXT("Value") }), parse_error::duplicate_argument, TEXT("Arg1"));
    }

    TEST_METHOD(TestPositional)
    {
        tstring arg1;
        tstring arg2;
        auto parser = basic_parser_builder<tchar_t>{TEXT("TestCommand")}
            .add_argument(arg1, TEXT("Arg1")).positional()
            .add_argument(arg2, TEXT("Arg2")).positional()
            .build();

        VerifyArgument(parser, TEXT("Arg1"), false, false, false, 0);
        VerifyArgument(parser, TEXT("Arg2"), false, false, false, 1);

        // Specify no arguments.
        VerifyParseResult(parser.parse(std::vector<tstring>{}));
        VERIFY_EQUAL(TEXT(""), arg1);
        VERIFY_EQUAL(TEXT(""), arg2);

        // positional arguments.
        VerifyParseResult(parser.parse({ TEXT("Value1"), TEXT("Value2") }));
        VERIFY_EQUAL(TEXT("Value1"), arg1);
        VERIFY_EQUAL(TEXT("Value2"), arg2);

        // positional argument by name.
        VerifyParseResult(parser.parse({ TEXT("-Arg1"), TEXT("Value3"), TEXT("-Arg2"), TEXT("Value4") }));
        VERIFY_EQUAL(TEXT("Value3"), arg1);
        VERIFY_EQUAL(TEXT("Value4"), arg2);
        VerifyParseResult(parser.parse({ TEXT("Value1"), TEXT("-Arg2"), TEXT("Value2") }));
        VERIFY_EQUAL(TEXT("Value1"), arg1);
        VERIFY_EQUAL(TEXT("Value2"), arg2);
        VerifyParseResult(parser.parse({ TEXT("-Arg2"), TEXT("Value4"), TEXT("Value3") }));
        VERIFY_EQUAL(TEXT("Value3"), arg1);
        VERIFY_EQUAL(TEXT("Value4"), arg2);
        VerifyParseResult(parser.parse({ TEXT("-Arg1"), TEXT("Value4"), TEXT("Value3") }));
        VERIFY_EQUAL(TEXT("Value4"), arg1);
        VERIFY_EQUAL(TEXT("Value3"), arg2);

        // Too many arguments
        VerifyParseResult(parser.parse({ TEXT("Value3"), TEXT("Value4"), TEXT("TooMany") }), parse_error::too_many_arguments, {});
        VerifyParseResult(parser.parse({ TEXT("-Arg1"), TEXT("Value3"), TEXT("-Arg2"), TEXT("Value4"), TEXT("TooMany") }), parse_error::too_many_arguments, {});

        // Duplicate
        VerifyParseResult(parser.parse({ TEXT("Value1"), TEXT("-Arg1"), TEXT("Value2") }), parse_error::duplicate_argument, TEXT("Arg1"));
    }

    TEST_METHOD(TestRequired)
    {
        tstring arg1;
        tstring arg2;
        auto parser = basic_parser_builder<tchar_t>{TEXT("TestCommand")}
            .add_argument(arg1, TEXT("Arg1")).positional().required()
            .add_argument(arg2, TEXT("Arg2")).positional()
            .build();

        VerifyArgument(parser, TEXT("Arg1"), true, false, false, 0);
        VerifyArgument(parser, TEXT("Arg2"), false, false, false, 1);

        // No arguments.
        VerifyParseResult(parser.parse(std::vector<tstring>{}), parse_error::missing_required_argument, TEXT("Arg1"));

        // required argument present.
        VerifyParseResult(parser.parse({ TEXT("Value1") }));
        VERIFY_EQUAL(TEXT("Value1"), arg1);
        VERIFY_EQUAL(TEXT(""), arg2);
        VerifyParseResult(parser.parse({ TEXT("-Arg1"), TEXT("Value2") }));
        VERIFY_EQUAL(TEXT("Value2"), arg1);
        VERIFY_EQUAL(TEXT(""), arg2);
        VerifyParseResult(parser.parse({ TEXT("Value1"), TEXT("-Arg2"), TEXT("Value2") }));
        VERIFY_EQUAL(TEXT("Value1"), arg1);
        VERIFY_EQUAL(TEXT("Value2"), arg2);

        // required argument missing.
        VerifyParseResult(parser.parse({ TEXT("-Arg2"), TEXT("Value2") }), parse_error::missing_required_argument, TEXT("Arg1"));
    }

    TEST_METHOD(TestDefaultValue)
    {
        int arg1;
        int arg2 = 47;
        auto parser = basic_parser_builder<tchar_t>{TEXT("TestCommand")}
            .add_argument(arg1, TEXT("Arg1")).positional().default_value(42)
            .add_argument(arg2, TEXT("Arg2")).positional()
            .build();

        VerifyArgument(parser, TEXT("Arg1"), false, false, false, 0);
        VerifyArgument(parser, TEXT("Arg2"), false, false, false, 1);

        // No arguments.
        VerifyParseResult(parser.parse<tstring>({}));
        VERIFY_EQUAL(42, arg1);
        VERIFY_EQUAL(47, arg2); // Unchanged if not specified.

        // Arguments present.
        VerifyParseResult(parser.parse({ TEXT("5"), TEXT("6") }));
        VERIFY_EQUAL(5, arg1);
        VERIFY_EQUAL(6, arg2);
    }

    TEST_METHOD(TestNegativeValue)
    {
        int arg1{};
        int arg2{};
        auto parser = basic_parser_builder<tchar_t>{TEXT("TestCommand")}
            .add_argument(arg1, TEXT("Arg1")).positional()
            .add_argument(arg2, TEXT("Arg2")).positional()
            .build();

        VerifyArgument(parser, TEXT("Arg1"), false, false, false, 0);
        VerifyArgument(parser, TEXT("Arg2"), false, false, false, 1);

        // Negative value is not seen as an argument name even though it starts with -
        VerifyParseResult(parser.parse({ TEXT("-Arg2"), TEXT("-42") }));
        VERIFY_EQUAL(0, arg1);
        VERIFY_EQUAL(-42, arg2);
    }

    TEST_METHOD(TestSwitch)
    {
        bool sw{};
        auto parser = basic_parser_builder<tchar_t>{TEXT("TestCommand")}
            .add_argument(sw, TEXT("Switch"))
            .build();

        VerifyArgument(parser, TEXT("Switch"), false, true, false, {});

        // No arguments.
        VerifyParseResult(parser.parse(std::vector<tstring>{}));
        VERIFY_FALSE(sw);

        // Present.
        VerifyParseResult(parser.parse({ TEXT("-Switch") }));
        VERIFY_TRUE(sw);

        // Explicit value.
        VerifyParseResult(parser.parse({ TEXT("-Switch:false") }));
        VERIFY_FALSE(sw);
        VerifyParseResult(parser.parse({ TEXT("-Switch:true") }));
        VERIFY_TRUE(sw);
        VerifyParseResult(parser.parse({ TEXT("-Switch:False") }));
        VERIFY_FALSE(sw);
        VerifyParseResult(parser.parse({ TEXT("-Switch:True") }));
        VERIFY_TRUE(sw);
        VerifyParseResult(parser.parse({ TEXT("-Switch:0") }));
        VERIFY_FALSE(sw);
        VerifyParseResult(parser.parse({ TEXT("-Switch:1") }));
        VERIFY_TRUE(sw);

        // Can't use space as a separator.
        VerifyParseResult(parser.parse({ TEXT("-Switch"), TEXT("false") }), parse_error::too_many_arguments, {});

        // Invalid values.
        VerifyParseResult(parser.parse({ TEXT("-Switch:f") }), parse_error::invalid_value, TEXT("Switch"));
        VerifyParseResult(parser.parse({ TEXT("-Switch:2") }), parse_error::invalid_value, TEXT("Switch"));
    }

    TEST_METHOD(TestOptionalSwitch)
    {
        optional<bool> sw;
        auto parser = basic_parser_builder<tchar_t>{TEXT("TestCommand")}
            .add_argument(sw, TEXT("switch"))
            .build();

        VerifyArgument(parser, TEXT("switch"), false, true, false, {});

        // No arguments.
        VerifyParseResult(parser.parse(std::vector<tstring>{}));
        VERIFY_NULL(sw);

        // Present.
        sw = {};
        VerifyParseResult(parser.parse({ TEXT("-switch") }));
        VERIFY_NOT_NULL(sw);
        VERIFY_TRUE(*sw);

        // Explicit value.
        sw = {};
        VerifyParseResult(parser.parse({ TEXT("-switch:false") }));
        VERIFY_NOT_NULL(sw);
        VERIFY_FALSE(*sw);
    }

    TEST_METHOD(TestArgumentTypes)
    {
        int number;
        float floatNumber;
        point p;
        std::optional<animal> a;
        auto parser = basic_parser_builder<tchar_t>{TEXT("TestCommand")}
            .add_argument(number, TEXT("Number"))
            .add_argument(floatNumber, TEXT("FloatNumber"))
            .add_argument(p, TEXT("Point"))
            .add_argument(a, TEXT("Animal"))
            .build();

        VerifyArgument(parser, TEXT("Number"), false, false, false, {});
        VerifyArgument(parser, TEXT("FloatNumber"), false, false, false, {});
        VerifyArgument(parser, TEXT("Point"), false, false, false, {});
        VerifyArgument(parser, TEXT("Animal"), false, false, false, {});

        VerifyParseResult(parser.parse({ TEXT("-Number"), TEXT("42"), TEXT("-FloatNumber"), TEXT("-3.142"), TEXT("-Point"), TEXT("5,6") }));
        VERIFY_EQUAL(42, number);
        VERIFY_EQUAL(-3.142f, floatNumber);
        VERIFY_EQUAL(5, p.x);
        VERIFY_EQUAL(6, p.y);
        VERIFY_NULL(a);

        // Specify animal.
        VerifyParseResult(parser.parse({ TEXT("-Animal"), TEXT("cat") }));
        VERIFY_NOT_NULL(a);
        VERIFY_EQUAL((int)animal::cat, (int)*a);

        // Different base.
        VerifyParseResult(parser.parse({ TEXT("-Number"), TEXT("0x42") }));
        VERIFY_EQUAL(0x42, number);

        // Invalid value.
        VerifyParseResult(parser.parse({ TEXT("-Number"), TEXT("42a") }), parse_error::invalid_value, TEXT("Number"));
        VerifyParseResult(parser.parse({ TEXT("-Point"), TEXT("42") }), parse_error::invalid_value, TEXT("Point"));
        VerifyParseResult(parser.parse({ TEXT("-Animal"), TEXT("cat2") }), parse_error::invalid_value, TEXT("Animal"));
    }

    TEST_METHOD(TestCustomConverter)
    {
        int arg;
        auto parser = basic_parser_builder<tchar_t>{TEXT("TestCommand")}
            .add_argument(arg, TEXT("Arg")).positional().converter([](auto value, auto) -> std::optional<int>
                {
                    size_t pos;
                    auto converted = stoi(tstring{value}, &pos) + 1;
                    if (pos != value.length())
                        return {};

                    return converted;
                })
            .build();

        VerifyArgument(parser, TEXT("Arg"), false, false, false, 0);

        // Valid value.
        VerifyParseResult(parser.parse({ TEXT("5") }));
        VERIFY_EQUAL(6, arg);

        // Invalid value.
        VerifyParseResult(parser.parse({ TEXT("5b") }), parse_error::invalid_value, TEXT("Arg"));
    }

    TEST_METHOD(TestMultiValue)
    {
        std::vector<tstring> args;
        int other;
        auto parser = basic_parser_builder<tchar_t>{TEXT("TestCommand")}
            .add_argument(other, TEXT("Other")).positional()
            .add_multi_value_argument(args, TEXT("Arg")).positional()
            .build();

        VerifyArgument(parser, TEXT("Arg"), false, false, true, 1);

        // No arguments.
        VerifyParseResult(parser.parse(std::vector<tstring>{}));
        VERIFY_EQUAL(0u, args.size());

        // One argument.
        VerifyParseResult(parser.parse({ TEXT("-Arg"), TEXT("Value1") }));
        std::vector<tstring> expected = { TEXT("Value1")};
        VERIFY_RANGE_EQUAL(expected, args);

        // Multiple arguments.
        VerifyParseResult(parser.parse({ TEXT("42"), TEXT("Value2"), TEXT("Value3"), TEXT("-Arg"), TEXT("Value4")}));
        expected = { TEXT("Value2"), TEXT("Value3"), TEXT("Value4") };
        VERIFY_RANGE_EQUAL(expected, args);
    }

    TEST_METHOD(TestMultiValueRequired)
    {
        std::vector<tstring> args;
        auto parser = basic_parser_builder<tchar_t>{TEXT("TestCommand")}
            .add_multi_value_argument(args, TEXT("Arg")).positional().required()
            .build();

        VerifyArgument(parser, TEXT("Arg"), true, false, true, 0);

        // No arguments.
        VerifyParseResult(parser.parse(std::vector<tstring>{}), parse_error::missing_required_argument, TEXT("Arg"));
        VERIFY_EQUAL(0u, args.size());

        // One argument.
        VerifyParseResult(parser.parse({ TEXT("Value1") }));
        std::vector<tstring> expected = { TEXT("Value1") };
        VERIFY_RANGE_EQUAL(expected, args);

        // Multiple arguments.
        VerifyParseResult(parser.parse({ TEXT("Value2"), TEXT("Value3"), TEXT("-Arg"), TEXT("Value4") }));
        expected = { TEXT("Value2"), TEXT("Value3"), TEXT("Value4") };
        VERIFY_RANGE_EQUAL(expected, args);
    }

    TEST_METHOD(TestMultiValueSwitch)
    {
        std::vector<bool> args;
        auto parser = basic_parser_builder<tchar_t>{TEXT("TestCommand")}
            .add_multi_value_argument(args, TEXT("Arg"))
            .build();

        VerifyArgument(parser, TEXT("Arg"), false, true, true, {});

        // No arguments.
        VerifyParseResult(parser.parse(std::vector<tstring>{}));
        VERIFY_EQUAL(0u, args.size());

        // Multiple arguments.
        VerifyParseResult(parser.parse({ TEXT("-Arg"), TEXT("-Arg:false"), TEXT("-Arg:true") }));
        std::vector<bool> expected = { true, false, true };
        VERIFY_RANGE_EQUAL(expected, args);
    }

    TEST_METHOD(TestMultiValueDefault)
    {
        std::vector<int> args;
        auto parser = basic_parser_builder<tchar_t>{TEXT("TestCommand")}
            .add_multi_value_argument(args, TEXT("Arg")).default_value(42)
            .build();

        VerifyArgument(parser, TEXT("Arg"), false, false, true, {});

        // No arguments.
        VerifyParseResult(parser.parse<tstring>({}));
        std::vector<int> expected = { 42 };
        VERIFY_RANGE_EQUAL(expected, args);

        // Multiple arguments.
        VerifyParseResult(parser.parse({ TEXT("-Arg"), TEXT("5"), TEXT("-Arg"), TEXT("6") }));
        expected = { 5, 6 };
        VERIFY_RANGE_EQUAL(expected, args);
    }

    TEST_METHOD(TestMultiValueSeparator)
    {
        std::vector<int> args;
        auto parser = basic_parser_builder<tchar_t>{TEXT("TestCommand")}
            .add_multi_value_argument(args, TEXT("Arg")).separator(TEXT(';'))
            .build();

        VerifyArgument(parser, TEXT("Arg"), false, false, true, {});

        VerifyParseResult(parser.parse({ TEXT("-Arg"), TEXT("5;6;7") }));
        std::vector<int> expected = { 5, 6, 7};
        VERIFY_RANGE_EQUAL(expected, args);
    }

    TEST_METHOD(TestUsage)
    {
        tstring stringArg;
        int intArg;
        float floatArg;
        bool switchArg;
        std::vector<tstring> multiArg;
        std::optional<bool> optionalSwitchArg;

        auto parser = basic_parser_builder<tchar_t>{TEXT("TestCommand")}.description(TEXT("Application description."))
            .add_argument(stringArg, TEXT("StringArg")).positional().required().description(TEXT("String argument description."))
            .add_argument(intArg, TEXT("IntArg")).required() // No description so it doesn't show up in the detailed help.
            .add_argument(floatArg, TEXT("FloatArg")).description(TEXT("Float argument description that is really quite long and probably needs to be wrapped.")).value_description(TEXT("number")).default_value(10.0f)
            .add_argument(switchArg, TEXT("SwitchArg")).description(TEXT("Switch argument description.\nWith a new line.")).alias(TEXT("s"))
            .add_argument(optionalSwitchArg, TEXT("OptionalSwitchArg")).description(TEXT("Optional switch argument."))
            .add_multi_value_argument(multiArg, TEXT("MultiArg")).description(TEXT("Multi-value argument description.")).alias(TEXT("multi")).alias(TEXT("m"))
            .build();
        
        {
            //std::basic_ostringstream<tchar_t> stream;
            //tline_wrapping_stream wrapStream{stream, 40};
            //parser.write_usage(basic_usage_options<tchar_t>{wrapStream});
            //VERIFY_EQUAL(c_usageExpected, stream.str());
        }

        {
            std::basic_ostringstream<tchar_t> stream;
            tline_wrapping_stream wrapStream{stream, 40};
            basic_usage_writer<tchar_t> writer{wrapStream};
            writer.write_parser_usage(parser);
            VERIFY_EQUAL(c_usageExpected, stream.str());
        }
    }

    TEST_METHOD(TestWindowsOptionPrefix)
    {
        tstring arg;
        bool sw{};
        auto parser = basic_parser_builder<tchar_t>{TEXT("TestCommand")}
            .add_argument(arg, TEXT("Arg")).positional()
            .add_argument(sw, TEXT("Switch"))
            .build();

        VerifyParseResult(parser.parse({ TEXT("/Switch") }));

#ifdef _WIN32
        VERIFY_TRUE(sw);
        VERIFY_EQUAL(0, arg.size());
#else
        VERIFY_FALSE(sw);;
        VERIFY_EQUAL(TEXT("/Switch"), arg);
#endif
    }

    TEST_METHOD(TestCustomPrefixes)
    {
        bool sw{};
        auto parser = basic_parser_builder<tchar_t>{TEXT("TestCommand")}
            .prefixes({ TEXT("++"), TEXT("+") })
            .add_argument(sw, TEXT("Switch"))
            .build();

        VerifyParseResult(parser.parse({ TEXT("++Switch") }));
        VERIFY_TRUE(sw);

        sw = false;
        VerifyParseResult(parser.parse({ TEXT("+Switch") }));
        VERIFY_TRUE(sw);

        // With a - will now be considered a value.
        VerifyParseResult(parser.parse({ TEXT("-Switch") }), parse_error::too_many_arguments);
    }

    // N.B. If this test fails on Linux due to the locale, run "sudo locale-gen nl_NL.UTF-8".
    TEST_METHOD(TestLocale)
    {
        float arg;
        auto parser = basic_parser_builder<tchar_t>{TEXT("TestCommand")}.locale(std::locale{"nl_NL.UTF-8"}).add_argument(arg, TEXT("Arg")).build();
        VerifyParseResult(parser.parse({ TEXT("-Arg"), TEXT("3,142") }));
        VERIFY_EQUAL(3.142f, arg);
    }

    TEST_METHOD(TestAlias)
    {
        // The fact that arguments() only returns the argument once is tested in TestEnumerateArguments
        int arg1;
        int arg2;
        auto parser = basic_parser_builder<tchar_t>{TEXT("TestCommand")}
            .add_argument(arg1, TEXT("Arg1")).alias(TEXT("a1"))
            .add_argument(arg2, TEXT("Arg2")).alias(TEXT("a2"))
            .build();

        VerifyArgument(parser, TEXT("Arg1"), false, false, false, {});
        VerifyArgument(parser, TEXT("Arg2"), false, false, false, {});
        VERIFY_EQUAL(TEXT("Arg1"), parser.get_argument(TEXT("a1")).name());
        VERIFY_EQUAL(TEXT("Arg2"), parser.get_argument(TEXT("a2")).name());

        VerifyParseResult(parser.parse({ TEXT("-a1"), TEXT("5"), TEXT("-Arg2"), TEXT("6") }));
        VERIFY_EQUAL(5, arg1);
        VERIFY_EQUAL(6, arg2);

        VerifyParseResult(parser.parse({ TEXT("-Arg1"), TEXT("7"), TEXT("-a2"), TEXT("8") }));
        VERIFY_EQUAL(7, arg1);
        VERIFY_EQUAL(8, arg2);

        VerifyParseResult(parser.parse({ TEXT("-a1"), TEXT("abc") }), parse_error::invalid_value, TEXT("Arg1"));
        VerifyParseResult(parser.parse({ TEXT("-a1"), TEXT("5"), TEXT("-Arg1"), TEXT("6") }), parse_error::duplicate_argument, TEXT("Arg1"));
    }

    TEST_METHOD(TestSeparator)
    {
        int arg;
        auto parser = basic_parser_builder<tchar_t>{TEXT("TestCommand")}
            .allow_whitespace_separator(false)
            .argument_value_separator(TEXT('='))
            .add_argument(arg, TEXT("Arg"))
            .build();

        VERIFY_FALSE(parser.allow_white_space_separator());
        VERIFY_EQUAL('=', parser.argument_value_separator());

        VerifyParseResult(parser.parse({ TEXT("-Arg=5") }));
        VERIFY_EQUAL(5, arg);
        VerifyParseResult(parser.parse({ TEXT("-Arg:5") }), parse_error::unknown_argument, TEXT("Arg:5"));
        VerifyParseResult(parser.parse({ TEXT("-Arg"), TEXT("5") }), parse_error::missing_value, TEXT("Arg"));
    }

    TEST_METHOD(TestAllowDuplicate)
    {
        int arg;
        auto parser = basic_parser_builder<tchar_t>{TEXT("TestCommand")}
            .allow_duplicate_arguments(true)
            .add_argument(arg, TEXT("Arg")).positional()
            .build();

        VerifyParseResult(parser.parse({ TEXT("-Arg"), TEXT("5"), TEXT("-Arg"), TEXT("6") }));
        VERIFY_EQUAL(6, arg);
        VerifyParseResult(parser.parse({ TEXT("7"), TEXT("-Arg"), TEXT("8") }));
        VERIFY_EQUAL(8, arg);
    }

    TEST_METHOD(TestCaseSensitive)
    {
        int arg;
        auto parser = basic_parser_builder<tchar_t>{TEXT("TestCommand")}
            .case_sensitive(true)
            .add_argument(arg, TEXT("Arg"))
            .build();

        VerifyParseResult(parser.parse({ TEXT("-Arg"), TEXT("5") }));
        VERIFY_EQUAL(5, arg);
        VerifyParseResult(parser.parse({ TEXT("-arg"), TEXT("5") }), parse_error::unknown_argument, TEXT("arg"));
        VERIFY_EQUAL(TEXT("Arg"), parser.get_argument(TEXT("Arg")).name());
        VERIFY_THROWS(parser.get_argument(TEXT("arg")), std::out_of_range);
    }

    TEST_METHOD(TestCancelParsing)
    {
        bool sw{};
        int arg{};
        auto parser = basic_parser_builder<tchar_t>{TEXT("TestCommand")}
            .add_argument(sw, TEXT("Switch")).cancel_parsing()
            .add_argument(arg, TEXT("Arg"))
            .build();

        VerifyParseResult(parser.parse({ TEXT("-Switch"), TEXT("-Arg"), TEXT("5") }), parse_error::parsing_cancelled, TEXT("Switch"));
        VERIFY_TRUE(sw);
        VERIFY_EQUAL(0, arg);

        // Use on_parsed but don't intervene.
        sw = false;
        bool called{};
        parser.on_parsed([&](auto &arg, auto value)
            {
                if (arg.name() == TEXT("Switch"))
                {
                    VERIFY_EQUAL(TEXT(""), value);
                    called = true;
                }

                return on_parsed_action::none;
            });

        VerifyParseResult(parser.parse({ TEXT("-Switch"), TEXT("-Arg"), TEXT("5") }), parse_error::parsing_cancelled, TEXT("Switch"));
        VERIFY_TRUE(called);
        VERIFY_TRUE(sw);
        VERIFY_EQUAL(0, arg);

        // Use on_parsed to resume anyway.
        sw = false;
        called = false;
        parser.on_parsed([&](auto &arg, auto value)
            {
                if (arg.name() == TEXT("Switch"))
                {
                    VERIFY_EQUAL(TEXT(""), value);
                    called = true;
                    return on_parsed_action::always_continue;
                }

                return on_parsed_action::none;
            });

        VerifyParseResult(parser.parse({ TEXT("-Switch"), TEXT("-Arg"), TEXT("5") }));
        VERIFY_TRUE(called);
        VERIFY_TRUE(sw);
        VERIFY_EQUAL(5, arg);

        // Use on_parsed to cancel on the other arg.
        arg = 0;
        sw = false;
        called = false;
        parser.on_parsed([&](auto &arg, auto value)
            {
                if (arg.name() == TEXT("Arg"))
                {
                    VERIFY_EQUAL(TEXT("5"), value);
                    called = true;
                    return on_parsed_action::cancel_parsing;
                }

                return on_parsed_action::none;
            });

        VerifyParseResult(parser.parse({ TEXT("-Arg"), TEXT("5"), TEXT("-Switch") }), parse_error::parsing_cancelled, TEXT("Arg"));
        VERIFY_TRUE(called);
        VERIFY_FALSE(sw);
        VERIFY_EQUAL(5, arg);
    }

private:
    static void VerifyArgument(const basic_command_line_parser<tchar_t> &parser, const tstring &name, bool required, bool is_switch, bool multi_value, std::optional<size_t> position)
    {
        const auto &arg = parser.get_argument(name);
        VERIFY_EQUAL(name, arg.name());
        VERIFY_EQUAL(required, arg.is_required());
        VERIFY_EQUAL(is_switch, arg.is_switch());
        VERIFY_EQUAL(multi_value, arg.is_multi_value());
        VERIFY_TRUE(position == arg.position());
        if (position)
        {
            VERIFY_TRUE(&parser.get_argument(*position) == &arg);
        }
    }

    static void VerifyParseResult(const parse_result<tchar_t> &result, parse_error expected_error = parse_error::none, tstring_view expected_arg = {})
    {
        if (expected_error != result.error)
        {
            test::LogMessage(test::LogMessageType::Error, TEXT("{}"), result.get_error_message());
        }

        VERIFY_EQUAL((int)expected_error, (int)result.error);
        VERIFY_EQUAL(expected_arg, result.error_arg_name);
    }

    static constexpr tstring_view c_usageExpected = TEXT(R"(Application description.

Usage: TestCommand [-StringArg] <string>
   -IntArg <int> [-FloatArg <number>]
   [-MultiArg <string>...]
   [-OptionalSwitchArg] [-SwitchArg]

    -StringArg <string>
        String argument description.

    -FloatArg <number>
        Float argument description that
        is really quite long and
        probably needs to be wrapped.
        Default value: 10.

    -MultiArg <string> (-multi, -m)
        Multi-value argument
        description.

    -OptionalSwitchArg [<bool>]
        Optional switch argument.

    -SwitchArg [<bool>] (-s)
        Switch argument description.
        With a new line.

)");

};

TEST_CLASS_REGISTRATION(CommandLineParserTests);
