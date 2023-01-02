# Subcommands

Ookii.CommandLine allows you to create applications that have multiple commands, each with their own
arguments. This is a common pattern used by many applications; for example, `git` uses it with
commands like `git pull` and `git cherry-pick`.

Ookii.CommandLine makes it trivial to define and use subcommands, using the same techniques we've
already seen for defining and parsing arguments. Subcommand specific functionality is all in the
`<subcommands.h>` header, which is automatically included if you include `<command_line.h>`.

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

To create a command for your application, you define a class that derives from `ookii::command`.

This class must have a constructor that takes a reference to a `parser_builder`, which you must
use to specify the arguments for your command. This `parser_builder` will have been initialized with
the name and description of your command, and a `case_sensitive()` and `locale()`
value that match the `command_manager`'s (see below).

> Note: do not call `parser_builder::build()`; `command_manager` will do that for you.

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

The `ookii::command` interface defines a single method, `command::run()`, which all subcommands must
implement. This function is invoked to run your command. The return value is typically used as the
exit code for the application, after the command finishes running.

When using the `command_manager` class as [shown below](#using-subcommands), the class will be
created using the constructor defined above, after which the resulting `command_line_parser` is used
to parse all the arguments except for the command name. Then, the `command::run()` method will be called.

All of the functionality and options available with the `command_line_parser` types are available
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
derived classes with the `command_manager`.

### Custom parsing

In some cases, you may want to create commands that do not use the `command_line_parser` class to
parse their arguments. For this purpose, you can derive from the
`ookii::command_with_custom_parsing` class instead.

This cklass must have a constructor with no parameters, and implement the
`command_with_custom_parsing::parse()` method, which will be called before `command::run()` to allow
you to parse the command line arguments.

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

To write an application that uses subcommands, you use the `command_manager` class in the `main()`
function of your application.

Before you can run a command, you must tell the `command_manager` which commands exist, by
registering them using the `add_command()` method.

This method takes as a template parameter the type of subcommand class you wish to register, and
takes two parameters: the first is the name of the command, which is used to invoke it from the
command line, and the second is the description of the command, used in the usage help. Like the
`parser_builder` methods, calls to `add_command()` can be chained together.

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
2. Calls the `command_manager::run_command` method, which:
   1. Uses the first argument to determine the command name.
   2. Creates a `parser_builder` instance, and passes it to the constructor of the command class.
   3. Calls `parser_builder::build()` to create a `command_line_parser`, and uses it to parse the
      arguments for the command.
   4. invokes the `command::run()` method, and returns its return value.
   5. If the command could not be created, for example because no command name was supplied, an
      unknown command name was supplied, or an error occurred parsing the command's arguments, it
      will print the error message and usage help, similar to the `command_line_parser::parse()`
      overloads that take a `usage_writer`, and return `std::nullopt`.
3. If `run_command()` returned `std::nullopt`, it returns an error exit code using
   `std::optional::value_or()`.

The `command_manager::add_command` method takes two arguments: the first is the name of the command,
which is used to invoke it from the command line, and the second is the description of the command,
used in the usage help.

Check out the [tutorial](Tutorial.md) and the [subcommand sample](../samples/subcommand) for
more detailed examples of how to create and use commands.

### Subcommand names and descriptions

In the above example, the name of the command was specified by passing it to the `add_command()`
method. This is not the only way you can set the name of a command.

There are three ways the name of a command can be determined.

1. If the name argument of `add_command()` is a non-empty string, it's used as the name.

    ```c++
    manager.add_command<my_command>("name");
    ```

2. If the name argument is omitted or an empty string, the compiler looks for a static method called
   `name()` on the subcommand class, and if found, calls it to get the name.
3. If no such method exists, the type name (without the namespace prefix) is used as the name.

The description is determined in a similar way:

1. If the description argument of `add_command()` is a non-empty string, it's used as the
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

Just like when you use `command_line_parser` directly, there are many options available to customize
the parsing behavior.

Some options can be set on the `command_manager` itself. For example, the `command_manager`
constructor takes a parameter that indicates whether or not command names are case sensitive (by
default, they are case insensitive). This option is then also used to determine whether argument
names are case sensitive for the commands.

Most options must still be set on the `parser_builder`, however, and usually you'll want options
like the [parsing mode](Arguments.md#longshort-mode) to be the same for all commands.

We've [already seen](#multiple-commands-with-common-arguments) we can use a base class to create
common arguments, and this can also be used to set common options.

Another option is to use the `command_manager::configure_parser` method. This method lets you
specify a callback that will be invoked whenever a `parser_builder` is created for a command, before
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

TODO

## Subcommand usage help

TODO

## Automatic commands

TODO

## Nested subcommands

TODO

## Code-generation scripts

Just like a stand-alone parser, it's possible to generate the argument parser for a subcommand
class by adding special annotations and using the provided [code-generation scripts](Scripts.md),
which is what we'll cover next.
