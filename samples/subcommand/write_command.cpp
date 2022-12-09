#include <iostream>
#include <fstream>
#include <filesystem>
#include "write_command.h"

// Create the command line arguments for the command. The builder will have been initialized with
// the name and description of the command, as well as the locale and case sensitivity settings
// used by the command_manager. If we want to override any settings, that can be done here
// too.
write_command::write_command(write_command::builder_type &builder)
{
    builder.add_argument(_file_name, "FileName").positional().required().description("The name of the file to write to.")
        .add_multi_value_argument(_lines, "Lines").positional().description("The lines of text to write to the file; if no lines are specified, this application will read from standard input instead.")
        .add_argument(_maximum_line_length, "MaximumLineLength").default_value(79).alias("Length").value_description("number").description("The maximum length of the lines in the file, or zero to have no limit.")
        .add_argument(_overwrite, "Overwrite").description("When this option is specified, the file will be overwritten if it already exists.");
}

// Run the command, which writes to the specified file.
int write_command::run()
{
    if (!_overwrite && std::filesystem::exists(_file_name))
    {
        std::cerr << "File already exists." << std::endl;
        return 1;
    }

    std::ofstream file{_file_name};
    if (!file)
    {
        std::cerr << "Failed to open file " << _file_name << std::endl;
        return 1;
    }

    ookii::line_wrapping_ostream stream{file, _maximum_line_length};
    for (auto line : _lines)
    {
        stream << line << std::endl;
    }

    if (!stream)
    {
        std::cerr << "Failed to write to file." << std::endl;
        return 1;
    }

    return 0;
}