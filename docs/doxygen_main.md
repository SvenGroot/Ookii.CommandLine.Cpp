# Ookii.CommandLine for C++ Documentation

This is the header file documentation for [Ookii.CommandLine for C++](https://github.com/SvenGroot/Ookii.CommandLine.Cpp),
a comprehensive header-only library for parsing command line arguments and generating usage help
in C++ applications.

The primary parsing functionality is provided by the ookii::basic_command_line_parser class, which is
constructed using the ookii::parser_builder class.

Functionality for applications that have multiple subcommands, each with their own arguments, is
provided using the ookii::basic_shell_command class.

To use any of the headers provided here, you should typically include \<ookii/command_line.h> or 
\<ookii/shell_command.h> instead of including other headers directly.

Additional helper types are availabe, which are used by the library internally but can also be used
for other purposes. Chief among these is the ookii::basic_line_wrapping_ostream, a stream wrapper
that can be used to white-space wrap text on output at a specified line length.
