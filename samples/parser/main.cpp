// This sample shows how to define arguments using the parser_builder, and parse the supplied
// command line arguments using the resulting command_line_parser.
#include <iostream>
#include <filesystem>
#include <ookii/command_line.h>
#include "../common.h"

int main(int argc, char *argv[])
{
    std::string source;
    std::string destination;
    int operation_index;
    std::optional<float> count;
    bool verbose;
    std::vector<std::string> values;

    // Use the parser_builder (or wparser_builder for wide characters) to define arguments. The
    // constructor argument is the application executable name, used in the usage help. This can
    // usually be derived from the first argument in the argv array.
    auto name = ookii::command_line_parser::get_executable_name(argc, argv);
    auto parser = ookii::parser_builder{name}
        // Set an application description, used at the top of the usage help.
        .description("Sample command line application. The application parses the command line and prints the results, but otherwise does nothing and none of the arguments are actually used for anything.")
        // Since this sample has quite a few arguments, the usage help can obscure the error message
        // if a parsing error occurs. To avoid that, we only show the usage syntax, instead of the
        // full help, on errors. To see the full help, the user has to provide the "-Help" argument.
        .show_usage_on_error(ookii::usage_help_request::syntax_only)
        // This defines a required positional argument called "-Source".
        //
        // It can be set by name as e.g. "-Source value", or by position by supplying "value" as the
        // first positional argument.
        //
        // By default, argument names are case insensitive, so you can also use "-source value".
        //
        // On Windows, you can also use "/Source", but on other platforms only "-Source" works by
        // default.
        //
        // This argument is required, so if it is not supplied, an error message is printed and
        // parsing fails.
        //
        // We add a description that will be shown when displaying usage help.
        .add_argument(source, "Source").required().positional().description("The source data.")
        // This property defines a required positional argument called "-Destination".
        .add_argument(destination, "Destination").required().positional().description("The destination data.")
        // This defines a optional positional argument called "-OperationIndex". If the argument is
        // not supplied, this property will be set to the default value 1.
        //
        // The argument's type is "int", so only valid integer values will be accepted. Anything
        // else will cause an error.
        //
        // For types other than string, command_line_parser will use the ookii::lexical_convert
        // template for the argument's type to try to convert the string to the correct type. By
        // default, this template will use stream extraction (operator >>), so any type that defines
        // such an operator can be used.
        .add_argument(operation_index, "OperationIndex").positional().default_value(1).description("The operation's index.")
        // This defines an argument named "-Count". This argument is not positional, so it can only
        // be supplied by name, for example as "-Count 5.2".
        //
        // This argument uses std::optional<T>, so it will remain as std::nullopt if the value is
        // not supplied, rather than having to choose a default value.
        //
        // The type conversion from string to float is locale dependent. Which locale is used is
        // indicated by the parser_builder::locale() method. By default, the current global locale
        // is used (which is the "C" locale if not explicitly set before using parser_builder).
        //
        // Typically, you probably want to use an invariant locale, rather than something depending
        // on the user's environment, to ensure a consistent parsing experience for all users.
        .add_argument(count, "Count").value_description("number").description("Provides the count for something to the application.")
        // This defines a switch argument named "-Verbose".
        //
        // Non-positional arguments whose type is "bool" act as a switch; if they are supplied on
        // the command line, their value will be true, otherwise it will be false. You don't need to
        // specify a value, just specify "-Verbose" to set it to true. You can explicitly set the
        // value by using "-Verbose:true" or "-Verbose:false" if you want, but it is not needed.
        //
        // If you give an argument the type std::optional<bool>, it will be true if present,
        // std::nullopt if omitted, and false only when explicitly set to false using
        // "-Verbose:false".
        //
        // This argument has an alias, so it can also be specified using "-v" instead of its regular
        // name. An argument can have multiple aliases by calling the alias() method attribute more
        // than once. Aliases will be included in the usage help by default.
        .add_argument(verbose, "Verbose").alias("v").description("Print verbose information; this is an example of a switch argument.")
        // This defines a multi-value argument named "-Value". A multi-value argument can be
        // specified multiple times, and every time it is specified, a value will be added to the
        // container.
        // 
        // To set multiple values, simply repeat the argument, e.g. "-Value foo -Value bar -Value
        // baz" will set it to an vector containing { "foo", "bar", "baz" }
        // 
        // Since no default value is specified, the vector will be empty is not supplied at all.
        // 
        // Multi-value arguments can be use any compatible container type, such as std::vector,
        // std::list, and others.
        .add_multi_value_argument(values, "Value").description("This is an example of a multi-value argument, which can be repeated multiple times to set more than one value.")
        // This defines an argument named "-Version", which will call the lambda and cancel parsing
        // when invoked.
        //
        // If building a Windows application, you can use add_win32_version_argument() to show the
        // product name, version and copyright information from the executable's VERSIONINFO
        // resource.
        .add_version_argument([]() { std::cout << "Ookii.CommandLine Sample 2.0" << std::endl; })
        .build();

    // This method will parse the arguments, and print an error message and usage help if needed.
    // Exit the application if it failed.
    if (!parser.parse(argc, argv, {}))
    {
        return 1;
    }

    std::cout << std::boolalpha;
    std::cout << "The following argument values were provided:" << std::endl;
    std::cout << "Source: " << source << std::endl;
    std::cout << "Destination: " << destination << std::endl;
    std::cout << "OperationIndex: " << operation_index << std::endl;
    std::cout << "Count: " << count << std::endl;
    std::cout << "Verbose: " << verbose << std::endl;
    std::cout << "Values: " << values << std::endl;

    return 0;
}
