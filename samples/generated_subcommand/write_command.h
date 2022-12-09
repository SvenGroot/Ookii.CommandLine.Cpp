#ifndef OOKII_WRITE_COMMAND_H_
#define OOKII_WRITE_COMMAND_H_

#pragma once

#include <ookii/command_line.h>

// This is a sample command that can be invoked by specifying "write" as the first argument to the
// sample application.
// 
// The command's name is given as a value to the [command] attribute, and the description is
// taken from the remainder of the comment. If omitted, they can also be defined using static
// methods same as in the regular command sample.
// 
// [command: write]
// Writes lines to a file, wrapping them to the specified width.
class write_command : public ookii::command
{
public:
    // The implementation of the constructor, which defines the arguments, is generated by
    // New-Subcommand.ps1.
    write_command(builder_type &builder);

    virtual int run() override;

private:
    // These fields will be used to hold the command's argument values. They use the same
    // attributes as in the generated_parser sample.

    // [argument, required, positional]
    // The name of the file to write to.
    std::string _file_name;

    // [argument, positional, multi_value]
    // The lines of text to write to the file.
    std::vector<std::string> _lines;

    // [argument, positional]
    // [value_description: number]
    // [default: 79]
    // The maximum length of the lines in the file, or zero to have no limit.
    size_t _maximum_line_length;

    // [argument]
    // When this option is specified, the file will be overwritten if it already exists.
    bool _overwrite;
};

#endif