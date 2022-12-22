// This sample shows how to define arguments using the parser_builder, and parse the supplied
// command line arguments using the resulting command_line_parser.
#include <iostream>
#include <filesystem>
#include <ookii/command_line.h>
#include "../common.h"

void show_version()
{
    std::cout << "Ookii.CommandLine Sample 2.0" << std::endl;
}

int main(int argc, char *argv[])
{
    std::string source;
    std::string destination;
    int operation_index;
    std::optional<float> count;
    bool verbose;
    std::vector<std::string> values;

    std::string name;
    if (argc > 0) 
    {
        name = std::filesystem::path{argv[0]}.filename().string(); 
    }

    auto parser = ookii::basic_parser_builder<char>{name}
        .description("Sample command line application. The application parses the command line and prints the results, but otherwise does nothing and none of the arguments are actually used for anything.")
        // This defines a required positional argument called "Source". It can be set by name as
        // e.g. "-Source value", or by position by specifying "value" as the first positional argument.
        // Note that by default command line argument names are case insensitive, so this argument can
        // also be specified as e.g. "-source value". On Windows, the default command name prefixes are
        // "/" and "-", so the argument can also be specified as e.g. "/Source value". On other platforms,
        // only "-" is accepted by default.
        .add_argument(source, "Source").required().positional().description("The source data.")
        // This defines a required positional argument called "Destination". It can be set by
        // name as e.g. "-Destination value", or by position by specifying "value" as the second
        // positional argument.
        .add_argument(destination, "Destination").required().positional().description("The destination data.")
        // This defines a optional positional argument called "OperationIndex". It can be set by name as
        // e.g. "-Index 5", or by position by specifying e.g. "5" as the third positional argument. If
        // the argument is not specified, this variable will be set to the default value 1. That default
        // value will be included in the generated usage help for this application.
        // 
        // For types other than std::string, command_line_parser will use the lexical_convert template
        // to try to convert the string argument to the correct type. The default implementation tries
        // to use stream extraction (operator>>). You can use your own custom types, as long as you
        // either implement stream extraction or provide a lexical_convert specialization.
        .add_argument(operation_index, "OperationIndex").positional().default_value(1).description("The operation's index.")
        // This defines an argument named "Count". This argument is not positional, so it can
        // only be supplied by name, for example as "-Count 5.2".
        // 
        // This argument uses std::optional<T>, so it will remain as std::nullopt if the value is not
        // supplied, rather than having to choose a default value.
        // 
        // The type conversion from string to float is locale dependent. Which locale is used is
        // indicated by the parser_builder::locale() method. By default, the current global locale is
        // used (which is the "C" locale if not explicitly set before using parser_builder).
        // 
        // Typically, you probably want to use an invariant locale, rather than something depending
        // on the user's environment, to ensure a consistent parsing experience for all users.
        .add_argument(count, "Count").value_description("number").description("Provides the count for something to the application.")
        // This defines a switch argument named "Verbose".
        // 
        // Non-positional arguments whose type is "bool" act as a switch; if they are supplied on the
        // command line, their value will be true, otherwise it will be false. You don't need to
        // specify a value, just specify "-Verbose" to set it to true. You can explicitly set the value
        // by using "-Verbose:true" or "-Verbose:false" if you want, but it is not needed.
        // 
        // If you give an argument the type std::optional<bool>, it will be true if present,
        // std::nullopt if omitted, and false only when explicitly set to false using "-Verbose:false".
        // 
        // This argument has an alias, so it can also be specified using "-v" instead of its regular
        // name. An argument can have multiple aliases by specifying the Alias attribute more than once.
        // Aliases will be included in the usage help by default.
        .add_argument(verbose, "Verbose").alias("v").description("Print verbose information; this is an example of a switch argument.")
        // This defines a multi-value argument named "Value". Its name is specified explicitly
        // so it differs from the variable name. A multi-value argument can be specified multiple times,
        // and every time it is specified, a value will be added to the container.
        // 
        // To set multiple values, simply repeat the argument, e.g. "-Value foo -Value bar -Value baz"
        // will set it to an vector containing { "foo", "bar", "baz" }
        // 
        // Since no default value is specified, the vector will be empty is not supplied at all.
        // 
        // Multi-value arguments can be use any compatible container type, include std::vector and
        // std::list.
        .add_multi_value_argument(values, "Value").description("This is an example of a multi-value argument, which can be repeated multiple times to set more than one value.")
        // This defines an argument named "Version", which will call the function and cancel parsing
        // when invoked.
        //
        // If building a Windows application, you can use add_win32_version_argument() to show the
        // product name, version and copyright information from the executable's VERSION_INFO
        // resource.
        .add_version_argument(show_version)
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
