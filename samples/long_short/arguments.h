#ifndef ARGUMENTS_H_
#define ARGUMENTS_H_

#pragma once

#include <ookii/command_line_generated.h>

// This struct defines the arguments for the sample. It uses the same arguments as the parser
// sample, so see that sample for more detailed descriptions.
//
// This sample uses the alternate long/short parsing mode, transforms argument names to dash-case,
// and uses case-sensitive argument name matching. This makes the behavior similar to POSIX
// conventions for command line arguments.
//
// The name transformation is applied to all automatically derived names.
// 
// [arguments, case_sensitive]
// [mode: long_short]
// [name_transform: dash-case]
// [version_info: Ookii.CommandLine Long/Short Sample 2.0]
// Sample command line application. The application parses the command line and prints the results,
// but otherwise does nothing and none of the arguments are actually used for anything.
struct arguments
{
    // This argument has a short name, derived from the first letter of its long name. The long
    // name is "--source", and the short name is "-s".
    // 
    // [argument, required, positional, short_name]
    // The source data.
    std::string source;

    // Similarly, this argument has a long name "--destination", and a short name "-d".
    // 
    // [argument, required, positional, short_name]
    // The destination data.
    std::string destination;

    // This argument does not have a short name. Its long name is "--operation-index".
    // 
    // [argument, positional]
    // [default: 1]
    // The operation's index.
    int operation_index;

    // This argument's long name is "--count", with the short name "-c".
    // 
    // [argument, short_name]
    // [value_description: number]
    // Provides the count for something to the application.
    std::optional<float> count;

    // This argument's long name is "--verbose", with the short name "-v".
    //
    // Instead of the alias used in the parser sample, this argument now has a short name. Note
    // that you can still use aliases in long/short mode. Long name aliases are given with the
    // [alias] attribute, and short name aliases with the [short_alias] attribute.
    // 
    // [argument, short_name]
    // Print verbose information; this is an example of a switch argument.
    bool verbose;

    // Another switch argument, called "--process" with the short name "-v". Switch arguments with
    // short names can be combined; for example, "-vp" sets both the verbose and process switch
    // (this only works for switch arguments).
    //
    // [argument, short_name]
    // Does the processing.
    bool process;

    // This argument's long name is "--value", with no short name.
    //
    // The name here is explicitly specified, and explicit names aren't subject to the NameTransform,
    // so it must be explicitly given as lower case to match the other arguments.
    // 
    // [argument: value]
    // [multi_value]
    // This is an example of a multi-value argument, which can be repeated multiple times to set
    // more than one value.
    std::vector<std::string> values;

    OOKII_GENERATED_METHODS(arguments);
};

int ookii_main(arguments args);

#endif