// This file is generated by New-Parser.ps1; do not edit manually.
#include <ookii/command_line.h>
#include "../input/long_short.h"
    
ookii::basic_parser_builder<wchar_t> my_arguments::create_builder(std::basic_string<wchar_t> command_name, ookii::basic_localized_string_provider<wchar_t> *string_provider, const std::locale &locale)
{
    ookii::basic_parser_builder<wchar_t> builder{command_name, string_provider};
    builder
        .locale(locale)
        .mode(ookii::parsing_mode::long_short)
        .case_sensitive(true)
        .add_argument(this->test_arg, L"test-arg").short_name().required().positional()
        .add_argument(this->test_arg2, L"test-arg2").short_name(L's').short_alias(L'x').short_alias(L'y')
        .add_argument(this->foo, L'f')
        .add_argument(this->bar, L'o')
    ;

    return builder;
}


