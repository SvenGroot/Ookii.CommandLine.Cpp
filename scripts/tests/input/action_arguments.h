// [arguments]
// [name_transform: PascalCase]
struct my_arguments
{
    // [argument, required, positional]
    static bool static_action(bool value, ookii::command_line_parser &parser)
    {
        return false;
    }

    // [argument]
    bool instance_action(some<template_type<with, nesting>> value, ookii::command_line_parser &parser);
    {
        return true;
    }

    OOKII_DECLARE_PARSE_METHOD(my_arguments);
};
