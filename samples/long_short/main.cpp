// This sample shows how to use the New-Parser.ps1 script in an application using long/short mode.
//
// After running the build, you can find the file generated/long_short_generated.cpp in the CMake build
// output, if you want to see what the generated code looks like.
#include <iostream>
#include <chrono>
#include <filesystem>
#include <ookii/command_line.h>
#include "../common.h"
#include "arguments.h"

// New-Parser.ps1 generates the actual main function, which will parse the arguments and then
// invoke this function (which can have any name, specified by the script's -EntryPoint argument).
//
// If "-EntryPoint" is not specified, you can call the struct's parse method manually to use the
// generated parser.
int ookii_main(arguments args)
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
    return 0;
}
