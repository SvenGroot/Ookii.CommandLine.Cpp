// This file is generated by New-Parser.ps1; do not edit manually.
#include <filesystem>
#include <ookii/command_line.h>
#include "../input/arguments.h"
#include "../input/arguments2.h"
    
std::optional<my_arguments> my_arguments::parse(int argc, const char *const argv[], ookii::basic_usage_writer<char> *options)
{
    std::basic_string<char> name;
    if (argc > 0) { name = std::filesystem::path{argv[0]}.filename().string(); }
    my_arguments args{};
    auto parser = ookii::basic_parser_builder<char>{name}
        .description("Description of the arguments with a line break.\n\nAnd a paragraph.")
        .add_argument(args.test_arg, "testArg").required().positional().description("Argument description with a line break.\n\nAnd another paragraph.")
        .add_argument(args.__test__arg2__, "testArg2").positional().default_value(1).value_description("desc").alias("test").description("Short description.")
        .add_multi_value_argument(args.test_arg3, "foo").alias("t").alias("v")
        .add_argument(args._testArg4, "testArg4").cancel_parsing()
        .add_argument(args.TestArg5, "testArg5").default_value("foo")
        .build();

    if (parser.parse(argc, argv, options))
    {
        return args;
    }

    return {};
}

std::optional<other_arguments> other_arguments::parse(int argc, const char *const argv[], ookii::basic_usage_writer<char> *options)
{
    auto name = "name";
    other_arguments args{};
    auto parser = ookii::basic_parser_builder<char>{name}
        .prefixes({ "--", "-" })
        .case_sensitive(true)
        .allow_whitespace_separator(false)
        .allow_duplicate_arguments(true)
        .argument_value_separator('=')
        .add_argument(args.some_arg, "someArg")
        .build();

    if (parser.parse(argc, argv, options))
    {
        return args;
    }

    return {};
}

std::optional<third_arguments> third_arguments::parse(int argc, const char *const argv[], ookii::basic_usage_writer<char> *options)
{
    std::basic_string<char> name;
    if (argc > 0) { name = std::filesystem::path{argv[0]}.filename().string(); }
    third_arguments args{};
    auto parser = ookii::basic_parser_builder<char>{name}
        .add_argument(args.other_arg, "otherArg")
        .build();

    if (parser.parse(argc, argv, options))
    {
        return args;
    }

    return {};
}


