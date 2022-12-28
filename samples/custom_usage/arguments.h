#ifndef ARGUMENTS_H_
#define ARGUMENTS_H_

#pragma once

#include <ookii/command_line_generated.h>

// This struct defines the arguments for the sample. It uses the same arguments as the long/short
// sample, so see that sample for more detailed descriptions.
// 
// [arguments, case_sensitive]
// [mode: long_short]
// [name_transform: dash-case]
// [version_info: Ookii.CommandLine Custom Usage Sample 2.0]
// Sample command line application. The application parses the command line and prints the results,
// but otherwise does nothing and none of the arguments are actually used for anything.
struct arguments
{
    // [argument, required, positional, short_name]
    // The source data.
    std::string source;

    // [argument, required, positional, short_name]
    // The destination data.
    std::string destination;

    // [argument, positional]
    // [default: 1]
    // The operation's index.
    int operation_index;

    // [argument, short_name]
    // [value_description: number]
    // Provides the count for something to the application.
    std::optional<float> count;

    // [argument, short_name]
    // Print verbose information; this is an example of a switch argument.
    bool verbose;

    // [argument, short_name]
    // Does the processing.
    bool process;

    // [argument: value]
    // [multi_value]
    // This is an example of a multi-value argument, which can be repeated multiple times to set
    // more than one value.
    std::vector<std::string> values;

    OOKII_DECLARE_PARSE_METHOD(arguments);
};

int ookii_main(arguments args);

#endif