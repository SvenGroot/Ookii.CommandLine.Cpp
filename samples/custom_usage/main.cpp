// This sample shows how to customize the usage help. It uses the New-Parser.ps1 script to create
// the arguments parser.
//
// After running the build, you can find the file generated/custom_usage_generated.cpp in the CMake build
// output, if you want to see what the generated code looks like.
#include <iostream>
#include <chrono>
#include <filesystem>
#include <ookii/command_line.h>
#include "../common.h"
#include "custom_usage_writer.h"
#include "arguments.h"

void show_args(arguments args)
{
    std::cout << std::boolalpha;
    std::cout << "The following argument values were provided:" << std::endl;
    std::cout << "Source: " << args.source << std::endl;
    std::cout << "Destination: " << args.destination << std::endl;
    std::cout << "OperationIndex: " << args.operation_index << std::endl;
    std::cout << "Count: " << args.count << std::endl;
    std::cout << "Verbose: " << args.verbose << std::endl;
    std::cout << "Process: " << args.process << std::endl;
    std::cout << "Values: " << args.values << std::endl;
}

// In this case we don't have the script generate a main method, because we want to use our custom
// usage writer (see custom_usage_writer.h for its implementation).
int main(int argc, char *argv[])
{
    custom_usage_writer writer;
    auto args = arguments::parse(argc, argv, &writer);
    if (!args)
    {
        return 1;
    }

    show_args(*args);
    return 0;
}
