#ifndef OOKII_COURSE_COMMANDS_H_
#define OOKII_COURSE_COMMANDS_H_

#pragma once

#include "parent_command.h"
#include "base_command.h"

// Command to add courses. Since it inherits from base_command, it has a Path argument in addition
// to the arguments created here.
//
// The attributes can't be used to set the name and description of the command, since that's only
// used if you call the generated register_commands function, which we don't use in this sample.
// [command, no_register]
class add_course_command : public base_command
{
public:
    add_course_command(builder_type &builder);

    virtual int run() override
    {
        auto db = load();
        int new_id = 1;
        auto courses = db["courses"];
        if (courses.is_object())
        {
            auto it = std::max_element(courses.items().begin(), courses.items().end(),
                [](const auto &left, const auto &right)
                {
                    return std::stoi(left.key()) < std::stoi(right.key());
                });

            if (it != courses.items().end())
            {
                new_id = std::stoi(it.key()) + 1;
            }
        }

        db["courses"][std::to_string(new_id)] = { { "name", _name}, { "teacher", _teacher} };
        save(db);
        std::cout << "Added new course with ID: " << new_id << std::endl;
        return 0;
    }

    static std::string name()
    {
        return "add";
    }

    static std::string description()
    {
        return "Adds a course to the database.";
    }

private:
    // [argument, required, positional]
    // The name of the course.
    std::string _name;

    // [argument, required, positional]
    // The name of the teacher of the course.
    std::string _teacher;
};

// Command to remove courses. Since it inherits from base_command, it has a Path argument in
// addition to the arguments created here.
// [command, no_register]
class remove_course_command : public base_command
{
public:
    remove_course_command(builder_type &builder);

    virtual int run() override
    {
        auto db = load();
        auto id = std::to_string(_id);
        if (db["courses"].contains(id))
        {
            db["courses"].erase(id);
            save(db);
            return 0;
        }
        else
        {
            std::cout << "No such course." << std::endl;
            return 1;
        }
    }

    static std::string name()
    {
        return "remove";
    }

    static std::string description()
    {
        return "Removes a course from the database.";
    }

private:
    // [argument, required, positional]
    // The ID of the course to remove.
    int _id;
};

// This is the top-level "course" command. It derives from parent_command so all it has to do is
// register its children.
// Because this is a command with custom parsing, it doesn't use code generation, and therefore has
// no [command] attribute.
class course_command : public parent_command<course_command>
{
public:
    virtual void register_children(ookii::command_manager &manager) override
    {
        manager
            .add_command<add_course_command>()
            .add_command<remove_course_command>();
    }

    static std::string name()
    {
        return "course";
    }

    static std::string description()
    {
        return "Add or remove a course.";
    }
};

#endif