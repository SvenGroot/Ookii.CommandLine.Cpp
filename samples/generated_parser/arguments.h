// This file contains the arguments used by this sample. It's passed to the New-Parser.ps1 script
// as part of the build in CMakeLists.txt, which looks for the attributes in square brackets, and
// generates an implementation of the parse() method that builds a parser and parses the arguments.
// The generated source file is then included in the compilation.
//
// See https://github.com/SvenGroot/Ookii.CommandLine.cpp/blob/main/docs/Scripts.md for information
// on how to use these scripts and attributes are available. 
#ifndef ARGUMENTS_H_
#define ARGUMENTS_H_

#pragma once

#include <ookii/command_line_generated.h>

// This struct defines the sample's command line arguments. The rest of the comment under the
// attribute provides the description for the application used in the generated usage help.
//
// The [version_info] attribute is used to create a "-Version" argument that will print the
// specified string. You can show multiple lines by repeating this attribute. On Windows, you can
// also use [win32_version] to display version information from the VERSION_INFO resource. If you
// use both [win32_version] and [version_info], the latter will be used on non-Windows platforms
// only.
// 
// [arguments]
// [version_info: Ookii.CommandLine Sample 2.0]
// Sample command line application. The application parses the command line and prints the results,
// but otherwise does nothing and none of the arguments are actually used for anything.
struct arguments
{
    // This field defines a required positional argument called "Source". It can be set by name as
    // e.g. "-Source value", or by position by specifying "value" as the first positional argument.
    // Note that by default command line argument names are case insensitive, so this argument can
    // also be specified as e.g. "-source value". On Windows, the default command name prefixes are
    // "/" and "-", so the argument can also be specified as e.g. "/Source value". On other platforms,
    // only "-" is accepted by default.
    // 
    // The argument's name is Source because CMakeLists.txt uses -NameTransform PascalCase argument
    // with the New-Parser.ps1 script, which transforms the field name to PascalCase. Other
    // possible values are CamelCase (camelCase), SnakeCase (snake_case), DashCase (dash-case),
    // Trim (just removes leading and trailing underscores), and None (use the field name as-is).
    // Try changing it in CMakeLists.txt to see the results.
    // 
    // The description of the argument is taken from the rest of the comment under the attributes.
    // 
    // [argument, required, positional]
    // The source data.
    std::string source;

    // This field defines a required positional argument called "Destination". It can be set by
    // name as e.g. "-Destination value", or by position by specifying "value" as the second
    // positional argument.
    // 
    // [argument, required, positional]
    // The destination data.
    std::string destination;

    // This field defines a optional positional argument called "OperationIndex". It can be set by
    // name as e.g. "-Index 5", or by position by specifying e.g. "5" as the third positional
    // argument. If the argument is not specified, this field will be set to the default value 1.
    // That default value will be included in the generated usage help for this application.
    // 
    // For types other than std::string, command_line_parser will use the lexical_convert template
    // to try to convert the string argument to the correct type. The default implementation tries
    // to use stream extraction (operator>>). You can use your own custom types, as long as you
    // either implement stream extraction or provide a lexical_convert specialization.
    // 
    // [argument, positional]
    // [default: 1]
    // The operation's index.
    int operation_index;

    // This field defines an argument named "Count". This argument is not positional, so it can
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
    // 
    // [argument]
    // [value_description: number]
    // Provides the count for something to the application.
    std::optional<float> count;

    // This field defines a switch argument named "Verbose".
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
    // 
    // [argument]
    // [alias: v]
    // Print verbose information; this is an example of a switch argument.
    bool verbose;

    // This field defines a multi-value argument named "Value". Its name is specified explicitly
    // so it differs from the field name. A multi-value argument can be specified multiple times,
    // and every time it is specified, a value will be added to the container.
    // 
    // To set multiple values, simply repeat the argument, e.g. "-Value foo -Value bar -Value baz"
    // will set it to an vector containing { "foo", "bar", "baz" }
    // 
    // Since no default value is specified, the vector will be empty is not supplied at all.
    // 
    // Multi-value arguments can be use any compatible container type, include std::vector and
    // std::list.
    // 
    // [argument: Value]
    // [multi_value]
    // This is an example of a multi-value argument, which can be repeated multiple times to set
    // more than one value.
    std::vector<std::string> values;

    // Structs used with the New-Parser.ps1 struct must have a parse method, whose implementation
    // will be provided by the script. Use the OOKII_DECLARE_PARSE_METHOD macro as an easy way to
    // declare the method with the correct signature. Call this method to parse the specified
    // arguments.
    //
    // This macro is defined in <ookii/command_line_generated.h>, which must be included separately.
    OOKII_DECLARE_PARSE_METHOD(arguments);
};

// In CMakeLists.txt, the argument "-EntryPoint ookii_main" is passed to the New-Parser.ps1 script.
// This means that the script will generate a main() method for us, which will parse the arguments,
// and, if successful, invoke the specified function. The generated source file must have access to
// the declaration of that function, so it's declared here.
int ookii_main(arguments args);

#endif