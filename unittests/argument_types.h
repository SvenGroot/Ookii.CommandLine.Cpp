#pragma once

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
            .add_argument(foo, TEXT("Foo")).short_name(true).default_value(0).description(TEXT("Foo description."))
            .add_argument(bar, TEXT("Bar")).default_value(0).description(TEXT("Bar description."))
            .add_argument(arg1, TEXT("Arg1")).short_alias(TEXT('c')).description(TEXT("Arg1 description."))
            .add_argument(arg2, TEXT("Arg2")).short_name(TEXT('a')).positional().short_alias(TEXT('b')).alias(TEXT("Baz")).description(TEXT("Arg2 description."))
            .add_argument(switch1, TEXT("Switch1")).short_name(true).description(TEXT("Switch1 description."))
            .add_argument(switch2, TEXT("Switch2")).short_name(TEXT('k')).description(TEXT("Switch2 description."))
            .add_argument(switch2, TEXT("Switch3")).short_name(TEXT('u')).long_name(false).description(TEXT("Switch3 description."))
            .build();
    }
};
