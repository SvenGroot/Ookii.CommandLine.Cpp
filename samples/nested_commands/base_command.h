#ifndef OOKII_BASE_COMMAND_H_
#define OOKII_BASE_COMMAND_H_

#pragma once

#include <ookii/command_line.h>
#include <nlohmann/json.hpp>
#include <fstream>

// This is a base class that adds an argument and some functionality that is common to all the
// commands in this application.
// Although we won't call the generated register_commands() function, we must still use [no_register],
// because this class is abstract, and would therefore cause compile errors if that function tried
// to add it to a command_manager.
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