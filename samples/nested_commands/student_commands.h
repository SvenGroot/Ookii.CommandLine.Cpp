#ifndef OOKII_student_COMMANDS_H_
#define OOKII_student_COMMANDS_H_

#pragma once

#include "parent_command.h"
#include "base_command.h"

// Command to add students. Since it inherits from base_command, it has a Path argument in addition
// to the arguments created here.
//
// The attributes can't be used to set the name and description of the command, since that's only
// used if you call the generated register_commands function, which we don't use in this sample.
// [command]
class add_student_command : public base_command
{
public:
    add_student_command(builder_type &builder);

    virtual int run() override
    {
        auto db = load();
        int new_id = get_next_id(db["students"]);
        db["students"][std::to_string(new_id)] = { { "first", _first_name}, { "last", _last_name}, { "major", _major } };

        save(db);
        std::cout << "Added new student with ID: " << new_id << std::endl;
        return 0;
    }

    static std::string name()
    {
        return "add";
    }

    static std::string description()
    {
        return "Adds a student to the database.";
    }

private:
    // [argument, required, positional]
    // The first name of the student.
    std::string _first_name;

    // [argument, required, positional]
    // The first name of the student.
    std::string _last_name;

    // [argument, positional]
    // The student's major.
    std::string _major;
};

// Command to remove students. Since it inherits from base_command, it has a Path argument in
// addition to the arguments created here.
// [command]
class remove_student_command : public base_command
{
public:
    remove_student_command(builder_type &builder);

    virtual int run() override
    {
        auto db = load();
        auto id = std::to_string(_id);
        if (db["students"].contains(id))
        {
            db["students"].erase(id);
            save(db);
            return 0;
        }
        else
        {
            std::cout << "No such student." << std::endl;
            return 1;
        }
    }

    static std::string name()
    {
        return "remove";
    }

    static std::string description()
    {
        return "Removes a student from the database.";
    }

private:
    // [argument, required, positional]
    // The ID of the student to remove.
    int _id;
};

// Command to add a course to a student. Since it inherits from base_command, it has a Path argument
// in addition to the arguments created here.
// [command]
class add_student_course_command : public base_command
{
public:
    add_student_course_command(builder_type &builder);

    virtual int run() override
    {
        auto db = load();
        auto &student = db["students"][std::to_string(_student_id)];
        if (!student.is_object())
        {
            std::cout << "No such student." << std::endl;
            return 1;
        }

        if (!db["courses"].contains(std::to_string(_course_id)))
        {
            std::cout << "No such course." << std::endl;
            return 1;
        }

        if (!student["courses"].is_array())
        {
            student["courses"] = nlohmann::json::array();
        }

        student["courses"].push_back({ { "course", _course_id }, { "grade", _grade } });
        save(db);
        return 0;
    }

    static std::string name()
    {
        return "add-course";
    }

    static std::string description()
    {
        return "Adds a course for a student.";
    }

private:
    // [argument, required, positional]
    // The ID of the student.
    int _student_id;

    // [argument, required, positional]
    // The ID of the course.
    int _course_id;

    // [argument, required, positional]
    // The grade achieved in the course.
    float _grade;
};

// This is the top-level "student" command. It derives from parent_command so all it has to do is
// register its children.
// Because this is a command with custom parsing, it doesn't use code generation, and therefore has
// no [command] attribute.
class student_command : public parent_command<student_command>
{
public:
    virtual void register_children(ookii::command_manager &manager) override
    {
        manager
            .add_command<add_student_command>()
            .add_command<remove_student_command>()
            .add_command<add_student_course_command>();
    }

    static std::string name()
    {
        return "student";
    }

    static std::string description()
    {
        return "Add or remove a student.";
    }
};

#endif