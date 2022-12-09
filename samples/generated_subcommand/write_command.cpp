#include <iostream>
#include <fstream>
#include <filesystem>
#include "write_command.h"

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