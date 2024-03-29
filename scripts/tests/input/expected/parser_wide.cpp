// This file is generated by New-Parser.ps1; do not edit manually.
#include <ookii/command_line.h>
#include "../input/arguments.h"
#include "../input/arguments2.h"
    
ookii::basic_parser_builder<wchar_t> my_arguments::create_builder(std::basic_string<wchar_t> command_name, ookii::basic_localized_string_provider<wchar_t> *string_provider, const std::locale &locale)
{
    command_name = L"name";
    ookii::basic_parser_builder<wchar_t> builder{command_name, string_provider};
    builder
        .locale(locale)
        .description(L"Description of the arguments with a line break.\n\nAnd a paragraph.")
        .add_argument(this->test_arg, L"TestArg").required().positional().description(L"Argument description with a line break.\n\nAnd another paragraph.")
        .add_argument(this->__test__arg2__, L"TestArg2").positional().default_value(1).value_description(L"desc").alias(L"test").description(L"Short description.")
        .add_multi_value_argument(this->test_arg3, L"foo").alias(L"t").alias(L"v")
        .add_argument(this->_testArg4, L"TestArg4").cancel_parsing()
        .add_argument(this->TestArg5, L"TestArg5").default_value("foo")
    ;

    return builder;
}

ookii::basic_parser_builder<wchar_t> other_arguments::create_builder(std::basic_string<wchar_t> command_name, ookii::basic_localized_string_provider<wchar_t> *string_provider, const std::locale &locale)
{
    ookii::basic_parser_builder<wchar_t> builder{command_name, string_provider};
    builder
        .locale(locale)
        .prefixes({ L"--", L"-" })
        .case_sensitive(true)
        .allow_whitespace_separator(false)
        .allow_duplicate_arguments(true)
        .argument_value_separator(L'=')
        .add_argument(this->some_arg, L"SomeArg")
    ;

    return builder;
}

ookii::basic_parser_builder<wchar_t> third_arguments::create_builder(std::basic_string<wchar_t> command_name, ookii::basic_localized_string_provider<wchar_t> *string_provider, const std::locale &locale)
{
    ookii::basic_parser_builder<wchar_t> builder{command_name, string_provider};
    builder
        .locale(locale)
        .add_argument(this->other_arg, L"OtherArg")
    ;

    return builder;
}


