#ifndef OOKII_LIST_COMMAND_H_
#define OOKII_LIST_COMMAND_H_

#pragma once

#include "base_command.h"

// A top-level command that lists all the values in the database. Since it inherits from
// base_command, it has a Path argument even though no arguments are defined here.
// [command]
class list_command : public base_command
{
public:
    list_command(builder_type &builder);

    virtual int run() override
    {
        auto output = ookii::line_wrapping_ostream::for_cout();
        output << "Students:" << std::endl;
        auto db = load();
        for (const auto &[id, student] : db["students"].items())
        {
            output << id << ": " << student["last"].get<std::string>() << ", " << student["first"].get<std::string>()
                << "; major: " << student["major"] .get<std::string>() << std::endl;

            if (student["courses"].size() > 0)
            {
                output << "  Courses:" << std::endl;
                output << ookii::set_indent(4);
                for (const auto &course : student["courses"])
                {
                    auto course_def = db["courses"][std::to_string(course["course"].get<int>())];
                    std::string name;
                    if (course_def.is_object())
                    {
                        output << course_def["name"].get<std::string>();
                    }
                    else
                    {
                        output << "Unknown ID:" << course["course"].get<int>();
                    }

                    output << ": grade " << course["grade"].get<float>() << std::endl;
                }

                output << ookii::set_indent(0) << ookii::reset_indent;
            }
        }

        output << std::endl << "Courses:" << std::endl;
        for (const auto &[id, course] : db["courses"].items())
        {
            output << id << ": " << course["name"].get<std::string>() << "; teacher " << course["teacher"].get<std::string>()
                << std::endl;
        }

        return 0;
    }

    static std::string name()
    {
        return "list";
    }

    static std::string description()
    {
        return "Lists all students and courses.";
    }

private:
};

#endif