# Subcommand sample

This sample is a simple demonstration of subcommands. The sample application defines two commands,
`read`, and `write`, which can be used to read or write a file, respectively.

For detailed information, check the source files, which have comments describing what they do.

When invoked without arguments, a subcommand application prints the list of commands.

```text
Subcommand sample for Ookii.CommandLine.

Usage: subcommand <command> [arguments]

The following commands are available:

    read
        Reads and displays data from a file, optionally limiting the number of lines.

    version
        Displays version information.

    write
        Writes lines to a file, wrapping them to the specified width.

Run 'subcommand <command> -Help' for more information about a command.
```

Like the usage help format for arguments, the command list format can also be customized using the
[`usage_writer`][] class. If the console is capable, the command list also uses color.

If we run `./subcommand write -Help`, we get the following:

```text
Writes lines to a file, wrapping them to the specified width.

Usage: subcommand write [-FileName] <string> [[-Lines] <string>...] [-Help] [-MaximumLineLength
   <number>] [-Overwrite]

    -FileName <string>
        The name of the file to write to.

    -Lines <string>
        The lines of text to write to the file; if no lines are specified, this application will
        read from standard input instead.

    -Help [<bool>] (-?, -h)
        Displays this help message.

    -MaximumLineLength <number> (-Length)
        The maximum length of the lines in the file, or zero to have no limit. Default value: 79.

    -Overwrite [<bool>]
        When this option is specified, the file will be overwritten if it already exists.
```

This is just like normal usage help for an application using the [`command_line_parser`][] class
directly, but note that it shows the command description, not the application description, and the
command name is included in the usage syntax.

Instead of a `-Version` argument, this sample uses a `version` command, which has the same function.
We can see that if we run `./subcommand version`:

```text
Ookii.CommandLine Subcommand Sample 2.0
```

[`command_line_parser`]: https://www.ookii.org/docs/commandline-cpp-2.0/classookii_1_1basic__command__line__parser.html
[`usage_writer`]: https://www.ookii.org/docs/commandline-cpp-2.0/classookii_1_1basic__usage__writer.html
