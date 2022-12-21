#pragma once

#include <ookii/command_line.h>

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