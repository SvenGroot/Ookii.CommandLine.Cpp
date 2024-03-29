// This file is generated by New-Parser.ps1; do not edit manually.
#include <ookii/command_line.h>
#include "../input/arguments.h"
#include "../input/arguments2.h"
    
ookii::basic_parser_builder<char> my_arguments::create_builder(std::basic_string<char> command_name, ookii::basic_localized_string_provider<char> *string_provider, const std::locale &locale)
{
    command_name = "name";
    ookii::basic_parser_builder<char> builder{command_name, string_provider};
    builder
        .locale(locale)
        .description("Description of the arguments with a line break.\n\nAnd a paragraph.")
        .add_argument(this->test_arg, "test_arg").required().positional().description("Argument description with a line break.\n\nAnd another paragraph.")
        .add_argument(this->__test__arg2__, "__test__arg2__").positional().default_value(1).value_description("desc").alias("test").description("Short description.")
        .add_multi_value_argument(this->test_arg3, "foo").alias("t").alias("v")
        .add_argument(this->_testArg4, "_testArg4").cancel_parsing()
        .add_argument(this->TestArg5, "TestArg5").default_value("foo")
    ;

    return builder;
}

ookii::basic_parser_builder<char> other_arguments::create_builder(std::basic_string<char> command_name, ookii::basic_localized_string_provider<char> *string_provider, const std::locale &locale)
{
    ookii::basic_parser_builder<char> builder{command_name, string_provider};
    builder
        .locale(locale)
        .prefixes({ "--", "-" })
        .case_sensitive(true)
        .allow_whitespace_separator(false)
        .allow_duplicate_arguments(true)
        .argument_value_separator('=')
        .add_argument(this->some_arg, "some_arg")
    ;

    return builder;
}

ookii::basic_parser_builder<char> third_arguments::create_builder(std::basic_string<char> command_name, ookii::basic_localized_string_provider<char> *string_provider, const std::locale &locale)
{
    ookii::basic_parser_builder<char> builder{command_name, string_provider};
    builder
        .locale(locale)
        .add_argument(this->other_arg, "other_arg")
    ;

    return builder;
}


