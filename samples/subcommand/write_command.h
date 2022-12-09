#ifndef OOKII_WRITE_COMMAND_H_
#define OOKII_WRITE_COMMAND_H_

#pragma once

#include <ookii/command_line.h>

// This is a sample command that can be invoked by specifying "write" as the first argument to the
// sample application.
// 
// Subcommand argument parsing uses the command_line_parser, using the arguments that the command
// class's constructor creates. After the arguments have been parsed, the run method is invoked to
// execute the command.
class write_command : public ookii::command
{
public:
    write_command(builder_type &builder);

    virtual int run() override;

    // The name used to invoke the command can be specified using a static method like this.
    // It can also be specified when calling add_command(), or if neither is used, it defaults to
    // the name of the class.
    static std::string name()
    {
        return "write";
    }

    // The description for the command can be specified using a static method like this. It can
    // also be specified when calling add_command()
    static std::string description()
    {
        return "Writes lines to a file, wrapping them to the specified width.";
    }

private:
    // These fields will be used to hold the command's argument values.
    std::string _file_name;
    std::vector<std::string> _lines;
    size_t _maximum_line_length;
    bool _overwrite;
};

#endif