# Subcommands

Ookii.CommandLine allows you to create applications that have multiple commands, each with their own
arguments. This is a common pattern used by many applications; for example, `git` uses it with
commands like `git pull` and `git cherry-pick`.

Ookii.CommandLine makes it trivial to define and use subcommands, using the same techniques we've
already seen for defining and parsing arguments. Subcommand specific functionality is all in the
`<subcommand.h>` header, which is automatically included if you include `<command_line.h>`.

In an application using subcommands, the first argument to the application is the name of the
command. The remaining arguments are arguments to that command. You cannot have arguments that are
not associated with a command using the subcommand functionality in Ookii.CommandLine, though you
can still easily define [common arguments](#multiple-commands-with-common-arguments).

For example, the [subcommand sample](../samples/subcommand) can be invoked as follows:

```text
./subcommand read file.txt -MaxLines 10
```

This command line invokes the command named `read`, and passes the remaining arguments to that
command.

## Defining subcommands

To create a command for your application, you define a class that derives from [`ookii::command`][].

This class must have a constructor that takes a reference to a [`parser_builder`][], which you must
use to specify the arguments for your command. This [`parser_builder`][] will have been initialized
with the name and description of your command, and a [`case_sensitive()`][case_sensitive()_1] value
and and [`locale()`][locale()_2] value that match the [`command_manager`][]'s (see below).

> Note: do not call [`parser_builder::build()`][]; [`command_manager`][] will do that for you.

Here is a simple example of a command with a single argument:

```c++
class sample_command : public ookii::command
{
public:
    my_command(builder_type &builder)
    {
        builder.add_argument(_sample_argument, "SampleArgument").required().positional()
            .description("A sample argument for the sample command.");
    }

    virtual int run() override
    {
        // Command functionality goes here.
        return 0;
    }

private:
    int _sample_argument{};
};
```

This code creates a subcommand which has a single positional required argument.

The [`ookii::command`][] class defines a single method, [`command::run()`][], which all subcommands must
implement. This function is invoked to run your command. The return value is typically used as the
exit code for the application, after the command finishes running.

When using the [`command_manager`][] class as [shown below](#using-subcommands), the class will be
created using the constructor defined above, after which the resulting [`command_line_parser`][] is used
to parse all the arguments except for the command name. Then, the [`command::run()`][] method will be called.

All of the functionality and options available with the [`command_line_parser`][] types are available
with commands too, including [usage help generation](#subcommand-usage-help),
[long/short mode](Arguments.md#longshort-mode), all kinds of arguments, etc.

### Multiple commands with common arguments

You may have multiple commands that have one or more arguments in common. For example, you may have
a database application where every command needs the connection string as an argument. The easiest
way to accomplish this is to use a common base class.

```c++
class database_command : public ookii::command
{
public:
    database_command(ookii::parser_builder &builder)
    {
        builder.add_argument(_connection_string, "ConnectionString").required().positional();
    }

private:
    std::string _connection_string;
};

class add_command : public database_command
{
public:
    add_command(ookii::parser_builder &builder)
        : database_command{builder}
    {
        builder.add_argument(_new_value, "NewValue").required().positional();
    }

    virtual int run() override
    {
        /* Omitted */
    }

private:
    std::string _new_value;
};

class delete_command : public database_command
{
public:
    delete_command(ookii::parser_builder &builder)
        : database_command{builder}
    {
        builder.add_argument(_id, "Id").required().positional()
            .add_argument(_force, "Force");
    }

    virtual int run() override
    {
        /* Omitted */
    }

private:
    int _id{};
    bool _force{};
};
```

The two commands, `add_command` and `delete_command` both inherit the `-ConnectionString` argument,
and add their own additional arguments.

The `database_command` class is not a valid subcommand by itself, because it still has a pure
virtual method and cannot be instantiated. This isn't a problem, since you'll only register the
derived classes with the [`command_manager`][].

### Custom parsing

In some cases, you may want to create commands that do not use the [`command_line_parser`][] class
to parse their arguments. For this purpose, you can derive from the
[`ookii::command_with_custom_parsing`][] class instead.

This class must have a constructor with no parameters, and implement the
[`command_with_custom_parsing::parse()`][] method, which will be called before [`command::run()`][]
to allow you to parse the command line arguments.

In this case, it is up to the command to handle argument parsing, and handle errors and display
usage help if appropriate.

For example, you may have a command that launches an external executable, and wants to pass the
arguments to that executable.

```c++
class launch_command : public ookii::command_with_custom_parsing
{
public:
    virtual bool parse(std::span<const char *const> args, const ookii::command_manager &manager, ookii::usage_writer *usage) override
    {
        _args = args;
        return true;
    }

    virtual int run() override
    {
        auto pid = fork();
        if (pid == 0)
        {
            std::vector<const char *> args;
            args.reserve(_args.size() + 2);
            args.push_back("executable");
            std::copy(_args.begin(), _args.end(), std::back_inserter(args));
            args.push_back(nullptr);
            execv("/path/to/executable", const_cast<char *const *>(args.data()));
            std::cout << "Error running executable: " << errno << std::endl;
            return 1;
        }

        int status;
        waitpid(pid, &status, 0);
        return WEXITSTATUS(status);
    }

private:
    std::span<const char *const> _args;
};
```

## Using subcommands

To write an application that uses subcommands, you use the [`command_manager`][] class in the `main()`
function of your application.

Before you can run a command, you must tell the [`command_manager`][] which commands exist, by
registering them using the [`add_command()`][] method.

This method takes as a template parameter the type of subcommand class you wish to register, and
takes two parameters: the first is the name of the command, which is used to invoke it from the
command line, and the second is the description of the command, used in the usage help. Like the
[`parser_builder`][] methods, calls to [`add_command()`][] can be chained together.

```c++
int main(int argc, char *argv[])
{
    auto name = ookii::command_line_parser::get_executable_name(argc, argv);
    ookii::command_manager manager{name};
    manager
        .add_command<sample_command>("sample", "Description for the sample command.")
        .add_command<another_command>("another", "Description for another command.");

    return manager.run_command(argc, argv).value_or(1);
}
```

This code does the following:

1. Creates a command manager, passing the application executable name, and registers two commands.
2. Calls the [`command_manager::run_command()`][command_manager::run_command()_1] method, which:
   1. Uses the first argument to determine the command name.
   2. Creates a [`parser_builder`][] instance, and passes it to the constructor of the command class.
   3. Calls [`parser_builder::build()`][] to create a [`command_line_parser`][], and uses it to parse the
      arguments for the command.
   4. invokes the [`command::run()`][] method, and returns its return value.
   5. If the command could not be created, for example because no command name was supplied, an
      unknown command name was supplied, or an error occurred parsing the command's arguments, it
      will print the error message and usage help, similar to the [`command_line_parser::parse()`][command_line_parser::parse()_0]
      overloads that take a [`usage_writer`][], and return [`std::nullopt`][].
3. If [`run_command()`][run_command()_2] returned [`std::nullopt`][], it returns an error exit code using
   [`std::optional::value_or()`][].

Check out the [tutorial](Tutorial.md) and the [subcommand sample](../samples/subcommand) for
more detailed examples of how to create and use commands.

### Subcommand names and descriptions

In the above example, the name of the command was specified by passing it to the [`add_command()`][]
method. This is not the only way you can set the name of a command.

There are three ways the name of a command can be determined.

1. If the name argument of [`add_command()`][] is a non-empty string, it's used as the name.

    ```c++
    manager.add_command<my_command>("name");
    ```

2. If the name argument is omitted or an empty string, the compiler looks for a static method called
   `name()` on the subcommand class, and if found, calls it to get the name.
3. If no such method exists, the type name (without the namespace prefix) is used as the name.

The description is determined in a similar way:

1. If the description argument of [`add_command()`][] is a non-empty string, it's used as the
   description.

    ```c++
    manager.add_command<my_command>("name", "The description of the command.");
    ```

2. If the description argument is omitted or an empty string, the compiler looks for a static method
   called `description()` on the subcommand class, and if found, calls it to get the description.
3. If no such method exists, the description will be empty.

Here is an example of a class that uses the static methods to set its name and description:

```c++
class sample_command : public ookii::command
{
public:
    my_command(builder_type &builder)
    {
        builder.add_argument(_sample_argument, "SampleArgument").required().positional()
            .description("A sample argument for the sample command.");
    }

    virtual int run() override
    {
        // Command functionality goes here.
        return 0;
    }

    static std::string name()
    {
        return "sample";
    }

    static std::string description()
    {
        return "Description for the sample command.";
    }

private:
    int _sample_argument{};
};
```

This allows you to keep the metadata of your command with the command's class, rather than having
to specify it in a separate location.

### Subcommand options

Just like when you use [`command_line_parser`][] directly, there are many options available to customize
the parsing behavior.

Some options can be set on the [`command_manager`][] itself. For example, the [`command_manager`][]
constructor takes a parameter that indicates whether or not command names are case sensitive (by
default, they are case insensitive). This option is then also used to determine whether argument
names are case sensitive for the commands.

Most options must still be set on the [`parser_builder`][], however, and usually you'll want options
like the [parsing mode](Arguments.md#longshort-mode) to be the same for all commands.

We've [already seen](#multiple-commands-with-common-arguments) we can use a base class to create
common arguments, and this can also be used to set common options.

Another option is to use the [`command_manager::configure_parser()`][] method. This method lets you
specify a callback that will be invoked whenever a [`parser_builder`][] is created for a command, before
the constructor of the command is called.

```c++
ookii::command_manager manager{name, true};
manager
    .configure_parser([](auto &builder)
        {
            builder.mode(ookii::parsing_mode::long_short);
        })
    .add_command<sample_command>("sample");
```

This sample also sets the case-sensitive parameter to `true`.

### Custom error handling

The [`command_manager::run_command()`][command_manager::run_command()_1] and
[`command_manager::create_command()`][command_manager::create_command()_0] methods will handle
errors and display usage help on the console if requested. However, if you need more control over
the error handling process, there are several options.

If you simply wish to customize the error messages, you can create a class that derives from
[`localized_string_provider`][], and pass the instance to the [`command_manager`][] constructor.

If you need access to the error message or usage help output, for example to display it using
something other than the console (like in a GUI app), the easiest way to accomplish this is to
use the [`line_wrapping_ostringstream`][] class in combination with the [`usage_writer`][] class to redirect
where error messages and usage help are written.

```c++
int main(int argc, char *argv[])
{
    auto name = ookii::command_line_parser::get_executable_name(argc, argv);
    ookii::command_manager manager{name};
    /* Set options and add commands here */

    // You can specify a desired line wrapping width, or use 0 for no wrapping.
    ookii::line_wrapping_ostringstream error{0};
    ookii::line_wrapping_ostringstream output{0};
    ookii::usage_writer usage{output, error};
    auto result = manager.run_command(argc, argv, &usage);
    if (!result)
    {
        // This still uses the console, but it's for demonstration purposes.
        if (!error.str().empty())
        {
            std::cerr << error.str() << std::endl;
        }

        if (!output.str().empty())
        {
            std::cout << output.str() << std::endl;
        }

        return 1;
    }

    return *result;
}
```

Finally, if you really need fine-grained control, you can manually handle creating a command class
and parsing arguments. This means you have to handle things such as commands with custom parsing
manually (unless your application does not use that), which can get rather complex. Below is an
example of what this would look like.

```c++
int main(int argc, char *argv[])
{
    auto name = ookii::command_line_parser::get_executable_name(argc, argv);
    ookii::command_manager manager{name};
    /* Set options and add commands here */

    auto info = argc < 2 ? nullptr : manager.get_command(argv[1]);
    if (info == nullptr)
    {
        manager.write_usage();
        return 1;
    }

    std::unique_ptr<ookii::command> command;
    std::span args{argv, static_cast<size_t>(argc)};
    if (info->use_custom_argument_parsing())
    {
        command = info->create_custom_parsing();
        auto custom_parsing = static_cast<ookii::command_with_custom_parsing *>(command.get());
        if (!custom_parsing->parse(args.subspan(2), manager, nullptr))
        {
            // How parsing errors and usage are handled for a command with custom parsing is up to
            // the command.
            return 1;
        }
    }
    else
    {
        auto builder = manager.create_parser_builder(*info);
        command = info->create(builder);
        auto parser = builder.build();
        auto result = parser.parse(args.subspan(2));
        if (!result)
        {
            if (result.error != ookii::parse_error::parsing_cancelled)
            {
                std::cerr << result.get_error_message() << std::endl << std::endl;
            }

            if (parser.help_requested())
            {
                parser.write_usage();
            }

            return 1;
        }
    }

    return command->run();
}
```

## Subcommand usage help

Since subcommands are created using the [`command_line_parser`][], they support showing usage help when
parsing errors occur, or the `-Help` argument is used. For example, with the [subcommand sample](../samples/subcommand)
you could run the following to get help on the `read` command:

```text
./Subcommand read -help
```

In addition, the [`command_manager`][] also prints usage help if no command name was supplied, or the
supplied command name did not match any command defined in the application. In this case, it prints
a list of commands, with their descriptions. This is what that looks like for the sample:

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

Usage help for a [`command_manager`][] is also created using the [`usage_writer`][], and can be
customized by setting the subcommand-specific fields of that class. In addition, you can set a few
options on the [`command_manager`][] itself.

The [`command_manager::description()`][command_manager::description()_1] method sets an application description which will be included
before the command list usage. The [`command_manager::common_help_argument()`][command_manager::common_help_argument()_1] method sets the name
of a help argument (including its prefix) that is common to all commands, which adds the bottom
line to the usage help seen above.

Fields on the [`usage_writer`][] let you configure indentation and colors, among others.

The actual help is created using a number of protected virtual methods on the [`usage_writer`][], so
this can be further customized by deriving your own class from the [`usage_writer`][] class. Creating
command list usage help is driven by the [`write_command_list_usage_core()`][] method. You can also
override other methods to customize parts of the usage help, such as
[`write_command_list_usage_syntax()`][], [`write_command_description()`][write_command_description()_0], and
[`write_command_list_usage_footer()`][], to name just a few.

## Automatic commands

The [`command_manager`][] provides an option to add an automatic `version` command to the list of
commands.

You can add this command using the [`add_version_command()`][] method. It will have a default name
and description (which can be customized using the [`localized_string_provider`][] class), and will
invoke the specified callback to show version information.

```c++
ookii::command_manager manager{name};

manager
    .add_version_command([]()
        {
            std::cout << "Awesome Application 1.0" << std::endl;
        });
```

On Windows only, you can call the [`add_win32_version_command()`][] method to add a `version`
command that reads information from your executable's `VERSIONINFO` resource, and displays it on the
console. It will print the product name, version, and copyright information if it's present.

## Nested subcommands

Ookii.CommandLine does not natively support nested subcommands. However, the
[`ookii::command_with_custom_parsing`][] class provides the tools needed to implement support for this
fairly easily.

The [nested commands sample](../samples/nested_commands) shows a complete implementation of this
functionality.

## Code-generation scripts

Just like a stand-alone parser, it's possible to generate the argument parser for a subcommand
class by adding special annotations and using the provided [code-generation scripts](Scripts.md),
which is what we'll cover next.

[`add_command()`]: https://www.ookii.org/docs/commandline-cpp-2.0/classookii_1_1basic__command__manager.html#ae521f57e933e688bed1700e2fc0c2157
[`add_version_command()`]: https://www.ookii.org/docs/commandline-cpp-2.0/classookii_1_1basic__command__manager.html#a3703e2c1eebdeecddcac20b6089e3601
[`add_win32_version_command()`]: https://www.ookii.org/docs/commandline-cpp-2.0/classookii_1_1basic__command__manager.html#a4782d6ea7e38e943214bd11feb33f8bb
[`command_line_parser`]: https://www.ookii.org/docs/commandline-cpp-2.0/classookii_1_1basic__command__line__parser.html
[`command_manager::add_command()`]: https://www.ookii.org/docs/commandline-cpp-2.0/classookii_1_1basic__command__manager.html#ae521f57e933e688bed1700e2fc0c2157
[`command_manager::configure_parser()`]: https://www.ookii.org/docs/commandline-cpp-2.0/classookii_1_1basic__command__manager.html#a1a68ed8729ad0dfa2300a2a74691a0c6
[`command_manager`]: https://www.ookii.org/docs/commandline-cpp-2.0/classookii_1_1basic__command__manager.html
[`command_with_custom_parsing::parse()`]: https://www.ookii.org/docs/commandline-cpp-2.0/classookii_1_1basic__command__with__custom__parsing.html#a870de32c0335e9c4dfc84141a9ae56c2
[`command::run()`]: https://www.ookii.org/docs/commandline-cpp-2.0/classookii_1_1basic__command.html#a1f14c66512418948c9cafc81fd7b881b
[`line_wrapping_ostringstream`]: https://www.ookii.org/docs/commandline-cpp-2.0/classookii_1_1basic__line__wrapping__ostringstream.html
[`localized_string_provider`]: https://www.ookii.org/docs/commandline-cpp-2.0/classookii_1_1basic__localized__string__provider.html
[`ookii::command_with_custom_parsing`]: https://www.ookii.org/docs/commandline-cpp-2.0/classookii_1_1basic__command__with__custom__parsing.html
[`ookii::command`]: https://www.ookii.org/docs/commandline-cpp-2.0/classookii_1_1basic__command.html
[`parser_builder::build()`]: https://www.ookii.org/docs/commandline-cpp-2.0/classookii_1_1basic__parser__builder.html#af66361855468fde2eb545fbe1631e042
[`parser_builder`]: https://www.ookii.org/docs/commandline-cpp-2.0/classookii_1_1basic__parser__builder.html
[`std::nullopt`]: https://en.cppreference.com/w/cpp/utility/optional/nullopt
[`std::optional::value_or()`]: https://en.cppreference.com/w/cpp/utility/optional/value_or
[`usage_writer`]: https://www.ookii.org/docs/commandline-cpp-2.0/classookii_1_1basic__usage__writer.html
[`write_command_list_usage_core()`]: https://www.ookii.org/docs/commandline-cpp-2.0/classookii_1_1basic__usage__writer.html#a82d4afe6fb751f019218bfc50770d1dc
[`write_command_list_usage_footer()`]: https://www.ookii.org/docs/commandline-cpp-2.0/classookii_1_1basic__usage__writer.html#abfd66f1180a81e007abe452a521bfaa8
[`write_command_list_usage_syntax()`]: https://www.ookii.org/docs/commandline-cpp-2.0/classookii_1_1basic__usage__writer.html#a9a80648ecc397c903665bee123c84105
[case_sensitive()_1]: https://www.ookii.org/docs/commandline-cpp-2.0/classookii_1_1basic__parser__builder.html#a47a0c39a17d1dcca8b99455dab68dbb0
[command_line_parser::parse()_0]: https://www.ookii.org/docs/commandline-cpp-2.0/classookii_1_1basic__command__line__parser.html#ae0c7b9990c29f41343182ac3d9918af7
[command_manager::common_help_argument()_1]: https://www.ookii.org/docs/commandline-cpp-2.0/classookii_1_1basic__command__manager.html#a34c551257a363486a212a2bed5db444a
[command_manager::create_command()_0]: https://www.ookii.org/docs/commandline-cpp-2.0/classookii_1_1basic__command__manager.html#ac15dc514b638e514036256a42aa0a353
[command_manager::description()_1]: https://www.ookii.org/docs/commandline-cpp-2.0/classookii_1_1basic__command__manager.html#a1ce6bb7ffd642322db2b8315036ab555
[command_manager::run_command()_1]: https://www.ookii.org/docs/commandline-cpp-2.0/classookii_1_1basic__command__manager.html#a4deb89f49a7ce6d03ed41cdbf8769d58
[locale()_2]: https://www.ookii.org/docs/commandline-cpp-2.0/classookii_1_1basic__parser__builder.html#a4f1626f8fdf059f4bfde502bcb78befc
[run_command()_2]: https://www.ookii.org/docs/commandline-cpp-2.0/classookii_1_1basic__command__manager.html#a4deb89f49a7ce6d03ed41cdbf8769d58
[write_command_description()_0]: https://www.ookii.org/docs/commandline-cpp-2.0/classookii_1_1basic__usage__writer.html#a51f655b2323f8b33e34ab668a36e599c
