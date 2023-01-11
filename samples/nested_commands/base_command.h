#ifndef OOKII_BASE_COMMAND_H_
#define OOKII_BASE_COMMAND_H_

#pragma once

#include <ookii/command_line.h>
#include <nlohmann/json.hpp>
#include <fstream>

// This block defines attributes for the command manager created with the generated
// register_commands() function.
//
// [global]
// [name_transform: PascalCase]
// [version_info: Ookii.CommandLine Nested Commands Sample 2.0]
// [common_help_argument: -Help]
// Nested subcommands sample for Ookii.CommandLine.

// This is a base class that adds an argument and some functionality that is common to all the
// commands in this application.
//
// [command, no_register]
class base_command : public ookii::command
{
public:
    base_command(builder_type &builder);

    // Helper function to load the json file.
    nlohmann::json load()
    {
        std::ifstream file{_path};
        if (file)
        {
            nlohmann::json j;
            file >> j;
            if (!file)
            {
                throw std::runtime_error("Reading the file failed.");
            }

            return j;
        }

        return {};
    }

    // Helper function to save the json file.
    void save(const nlohmann::json &db)
    {
        std::ofstream file{_path};
        file << db;
        if (!file)
        {
            throw std::runtime_error("Writing the file failed.");
        }
    }

    // Helper function to determine the next ID based on existing values.
    static int get_next_id(const nlohmann::json &object)
    {
        if (object.is_object())
        {
            auto it = std::max_element(object.items().begin(), object.items().end(),
                [](const auto &left, const auto &right)
                {
                    return std::stoi(left.key()) < std::stoi(right.key());
                });

            if (it != object.items().end())
            {
                return std::stoi(it.key()) + 1;
            }
        }

        return 1;
    }

private:
    // The path argument can be used by any command that inherits from this class.
    // [argument]
    // [default: "data.json"]
    // The json file holding the data.
    std::string _path;
};

#endif