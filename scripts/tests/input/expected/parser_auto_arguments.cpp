// This file is generated by New-Parser.ps1; do not edit manually.
#include <ookii/command_line.h>
#include "../input/auto_arguments.h"
    
ookii::basic_parser_builder<char> my_arguments::create_builder(std::basic_string<char> command_name, ookii::basic_localized_string_provider<char> *string_provider, const std::locale &locale)
{
    ookii::basic_parser_builder<char> builder{command_name, string_provider};
    builder
        .locale(locale)
        .show_usage_on_error(ookii::usage_help_request::syntax_only)
        .automatic_help_argument(false)
#ifdef _WIN32
        .add_win32_version_argument()
#else
        .add_version_argument([]()
        {
            ookii::console_stream<char>::cout() << "First line" << std::endl;
            ookii::console_stream<char>::cout() << "Second line" << std::endl;
        })
#endif
    ;

    return builder;
}


