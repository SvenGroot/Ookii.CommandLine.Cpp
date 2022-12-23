#pragma once

#include <ookii/command_line.h>

struct UsageArguments
{
    ookii::tstring stringArg;
    int intArg;
    int intArg2;
    float floatArg;
    bool switchArg;
    std::vector<ookii::tstring> multiArg;
    std::optional<bool> optionalSwitchArg;

    auto create_parser(ookii::usage_help_request on_error = ookii::usage_help_request::full)
    {
        return ookii::basic_parser_builder<ookii::tchar_t>{TEXT("TestCommand")}
            .description(TEXT("Application description."))
            .show_usage_on_error(on_error)
            .add_argument(stringArg, TEXT("StringArg")).positional().required().description(TEXT("String argument description."))
            .add_argument(intArg, TEXT("IntArg")).required() // No description so it doesn't show up in the detailed help.
            .add_argument(intArg2, TEXT("IntArg2")).default_value(4) // No description but the default value causes it to show up for information mode.
            .add_argument(floatArg, TEXT("FloatArg")).description(TEXT("Float argument description that is really quite long and probably needs to be wrapped.")).value_description(TEXT("number")).default_value(10.0f)
            .add_argument(switchArg, TEXT("SwitchArg")).description(TEXT("Switch argument description.\nWith a new line.")).alias(TEXT("s"))
            .add_argument(optionalSwitchArg, TEXT("OptionalSwitchArg")).description(TEXT("Optional switch argument."))
            .add_multi_value_argument(multiArg, TEXT("MultiArg")).description(TEXT("Multi-value argument description.")).alias(TEXT("multi")).alias(TEXT("m"))
            .build();
    }
};

struct LongShortArguments
{
    int foo{};
    int bar{};
    int arg1{};
    int arg2{};
    bool switch1{};
    bool switch2{};
    bool switch3{};

    auto create_parser()
    {
        return ookii::basic_parser_builder<ookii::tchar_t>{TEXT("TestCommand")}
            .mode(ookii::parsing_mode::long_short)
            .add_argument(foo, TEXT("foo")).short_name().positional().default_value(0).description(TEXT("Foo description."))
            .add_argument(bar, TEXT("bar")).default_value(0).positional().description(TEXT("Bar description."))
            .add_argument(arg1, TEXT("Arg1")).short_alias(TEXT('c')).description(TEXT("Arg1 description."))
            .add_argument(arg2, TEXT("Arg2")).short_name(TEXT('a')).positional().short_alias(TEXT('b')).alias(TEXT("Baz")).description(TEXT("Arg2 description."))
            .add_argument(switch1, TEXT("Switch1")).short_name().description(TEXT("Switch1 description."))
            .add_argument(switch2, TEXT("Switch2")).short_name(TEXT('k')).description(TEXT("Switch2 description."))
            .add_argument(switch3, TEXT('u')).description(TEXT("Switch3 description."))
            .build();
    }
};

struct ActionArguments
{
    static int action_value;
    bool called{};

    static bool static_action(int value, ookii::basic_command_line_parser<ookii::tchar_t> &)
    {
        action_value = value;
        return true;
    }

    static bool static_action2(bool, ookii::basic_command_line_parser<ookii::tchar_t> &parser)
    {
        parser.help_requested(true);
        return false;
    }

    bool action(bool)
    {
        called = true;
        return false;
    }

    auto create_parser()
    {
        return ookii::basic_parser_builder<ookii::tchar_t>{TEXT("TestCommand")}
            .add_action_argument(static_action, TEXT("StaticAction"))
            .add_action_argument(static_action2, TEXT("StaticAction2"))
            .add_action_argument([this](bool value, ookii::basic_command_line_parser<ookii::tchar_t> &)
                {
                    return action(value);
                },
                TEXT("Action"))
            .build();
    }
};