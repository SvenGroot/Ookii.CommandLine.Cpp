// This file is generated by New-Parser.ps1; do not edit manually.
#include <ookii/command_line.h>
#include "../input/auto_arguments.h"
    
std::optional<my_arguments> my_arguments::parse(int argc, const char *const argv[], ookii::basic_usage_writer<char> *options, ookii::basic_localized_string_provider<char> *string_provider, const std::locale &locale)
{
    auto name = ookii::basic_command_line_parser<char>::get_executable_name(argc, argv);
    my_arguments args{};
    auto parser = ookii::basic_parser_builder<char>{name, string_provider}
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
        .build();

    if (parser.parse(argc, argv, options))
    {
        return args;
    }

    return {};
}

