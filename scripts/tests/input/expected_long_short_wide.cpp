// This file is generated by New-Parser.ps1; do not edit manually.
#include <filesystem>
#include <ookii/command_line.h>
#include "../input/long_short.h"
    
std::optional<my_arguments> my_arguments::parse(int argc, const wchar_t *const argv[], ookii::basic_usage_writer<wchar_t> *options)
{
    std::basic_string<wchar_t> name;
    if (argc > 0) { name = std::filesystem::path{argv[0]}.filename().wstring(); }
    my_arguments args{};
    auto parser = ookii::basic_parser_builder<wchar_t>{name}
        .mode(ookii::parsing_mode::long_short)
        .case_sensitive(true)
        .add_argument(args.test_arg, L"test-arg").short_name().required().positional()
        .add_argument(args.test_arg2, L"test-arg2").short_name(L's').short_alias(L'x').short_alias(L'y')
        .add_argument(args.foo, L'f')
        .add_argument(args.bar, L'o')
        .build();

    if (parser.parse(argc, argv, options))
    {
        return args;
    }

    return {};
}

