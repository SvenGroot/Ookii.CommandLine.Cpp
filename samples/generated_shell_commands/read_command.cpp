#include <iostream>
#include <fstream>
#include "read_command.h"

// Run the command, which reads from the specified file.
int read_command::run()
{
    std::ifstream file{_file_name};
    if (!file)
    {
        std::cerr << "Failed to open file " << _file_name << std::endl;
        return 1;
    }

    std::string line;
    for (int count = 0;
        std::getline(file, line) && (!_max_lines || count < *_max_lines);
        ++count)
    {
        std::cout << line << std::endl;
    }

    return 0;
}