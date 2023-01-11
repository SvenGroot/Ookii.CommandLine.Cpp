# Nested commands sample

While Ookii.CommandLine has no built-in way to nest subcommands, such functionality is easy to
implement using commands with custom parsing.

Commands that have children derive from the [`parent_command`](parent_command.h) class defined in
this sample, which derives from the [`command_with_custom_parsing`][] class so it can do its own
parsing, rather than relying on the [`command_line_parser`][]. This allows it to create a new
[`command_manager`][] with the children of that command, and pass the remaining arguments to that.

The other commands are just regular commands using the [`command_line_parser`][], and don't need to
do anything special.

This sample uses this framework to create a simple "database" application that lets your add and
remove students and courses to a json file. It has top-level commands `student` and `course`, which
both have child commands `add` and `remove` (and a few others).

All the leaf commands use a common base class, so they can specify the path to the json file. This
is the way you add common arguments to multiple commands in Ookii.CommandLine.

When invoked without arguments, we see only the top-level commands:

```text
Nested subcommands sample for Ookii.CommandLine.

Usage: nested_commands <command> [arguments]

The following commands are available:

    course
        Add or remove a course.

    list
        Lists all students and courses.

    student
        Add or remove a student.

    version
        Displays version information.

Run 'nested_commands <command> -Help' for more information about a command.
```

This is completely ordinary help for any application with subcommands.

Now, if we run `./nested_commands student`, we see the following:

```text
Add or remove a student.

Usage: nested_commands student <command> [arguments]

The following commands are available:

    add
        Adds a student to the database.

    add-course
        Adds a course for a student.

    remove
        Removes a student from the database.

Run 'nested_commands student <command> -Help' for more information about a command.
```

You can see the sample has customized the usage help to:

- Show the command description at the top, rather than the application description.
- Include the top-level command name in the usage syntax.

If we run `./nested_commands student -Help`, we get the same output. While the `student` command
doesn't have a help argument (since it uses custom parsing, and not the [`command_line_parser`][]),
there is no command named `-Help` so it still just shows the command list.

If we run `./nested_commands student add -Help`, we get the help for the command's arguments as
usual:

```text
Adds a student to the database.

Usage: nested_commands student add [-FirstName] <string> [-LastName] <string> [[-Major] <string>]
   [-Help] [-Path <string>]

    -FirstName <string>
        The first name of the student.

    -LastName <string>
        The first name of the student.

    -Major <string>
        The student's major.

    -Help [<bool>] (-?, -h)
        Displays this help message.

    -Path <string>
        The json file holding the data. Default value: data.json.
```

We can see the usage syntax shows both command names before the arguments.

This sample uses the [JSON for modern C++](https://github.com/nlohmann/json) library to read and
write JSON files.

[`command_line_parser`]: https://www.ookii.org/docs/commandline-cpp-2.0/classookii_1_1basic__command__line__parser.html
[`command_manager`]: https://www.ookii.org/docs/commandline-cpp-2.0/classookii_1_1basic__command__manager.html
[`command_with_custom_parsing`]: https://www.ookii.org/docs/commandline-cpp-2.0/classookii_1_1basic__command__with__custom__parsing.html
