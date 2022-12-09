#ifndef OOKII_READ_COMMAND_H_
#define OOKII_READ_COMMAND_H_

#pragma once

#include <ookii/command_line.h>

// This is a sample command that can be invoked by specifying "read" as the first argument to the
// sample application.
// 
// The command's name is given as a value to the [command] attribute, and the description is
// taken from the remainder of the comment. If omitted, they can also be defined using static
// methods same as in the regular command sample.
// 
// 
// [command: read]
// Reads and displays data from a file, optionally limiting the number of lines.
class read_command : public ookii::command
{
public:
    // The implementation of the constructor, which defines the arguments, is generated by
    // New-Subcommand.ps1.
    read_command(builder_type &builder);

    virtual int run() override;

private:
    // These fields will be used to hold the command's argument values. They use the same
    // attributes as in the generated_parser sample.

    // [argument, required, positional]
    // The name of the file to read.
    std::string _file_name;

    // [argument]
    // The maximum number of lines to read.
    std::optional<int> _max_lines;
};

#endif