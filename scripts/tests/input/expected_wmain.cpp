// This file is generated by New-Parser.ps1; do not edit manually.
#include <ookii/command_line.h>
#include "../input/arguments.h"
    
std::optional<my_arguments> my_arguments::parse(int argc, const wchar_t *const argv[], ookii::basic_usage_writer<wchar_t> *options, ookii::basic_localized_string_provider<wchar_t> *string_provider)
{
    auto name = L"name";
    my_arguments args{};
    auto parser = ookii::basic_parser_builder<wchar_t>{name, string_provider}
        .description(L"Description of the arguments with a line break.\n\nAnd a paragraph.")
        .add_argument(args.test_arg, L"TestArg").required().positional().description(L"Argument description with a line break.\n\nAnd another paragraph.")
        .add_argument(args.__test__arg2__, L"TestArg2").positional().default_value(1).value_description(L"desc").alias(L"test").description(L"Short description.")
        .add_multi_value_argument(args.test_arg3, L"foo").alias(L"t").alias(L"v")
        .add_argument(args._testArg4, L"TestArg4").cancel_parsing()
        .add_argument(args.TestArg5, L"TestArg5").default_value("foo")
        .build();

    if (parser.parse(argc, argv, options))
    {
        return args;
    }

    return {};
}

int wmain(int argc, wchar_t *argv[])
{
    auto args = my_arguments::parse(argc, argv);
    if (!args)
        return 1;

    return my_entry(*args);
}

