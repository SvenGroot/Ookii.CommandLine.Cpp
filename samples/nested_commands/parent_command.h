#ifndef OOKII_PARENT_COMMAND_H_
#define OOKII_PARENT_COMMAND_H_

#pragma once

#include <ookii/command_line.h>

// This is the base class for all the commands that have child commands. It performs all
// the work necessary to find and run subcommands; all derived classes have to do it provide the
// list of their child commands.
//
// This class uses command_with_custom_parsing, so command_line_parser won't be used to create it.
// Instead, command_manager will just instantiate it and call the parse() method, where we can
// do whatever we want. In this case, we create another command_manager to find and create a
// child command.
//
// Although this sample doesn't do this, you can use this to nest commands more than one level deep
// just as easily.
template<typename Command>
class parent_command : public ookii::command_with_custom_parsing
{
public:
    virtual bool parse(std::span<const char *const> args, const ookii::command_manager &manager, ookii::usage_writer *usage) override
    {
        // Append the name of this command to the application name so child command usage is correct.
        ookii::command_manager child_manager{manager.application_name() + ' ' + Command::name(),
            manager.case_sensitive(),
            manager.locale(),
            &manager.string_provider()};

        child_manager
            .common_help_argument("-Help")
            .description(Command::description());

        register_children(child_manager);
        _child = child_manager.create_command(args, usage);
        return static_cast<bool>(_child);
    }

    virtual int run() override
    {
        // This shouldn't be invoked if the child wasn't successfully created, but just in case.
        if (!_child)
        {
            return 1;
        }

        return _child->run();
    }

    virtual void register_children(ookii::command_manager &manager) = 0;

private:
    std::unique_ptr<ookii::command> _child;
};

#endif