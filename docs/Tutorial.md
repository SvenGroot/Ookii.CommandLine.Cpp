# Tutorial: getting started with Ookii.CommandLine for C++

This tutorial will show you the basics of how to use Ookii.CommandLine. It will show you how to
create a basic application that parses the command line and shows usage help, how to customize some
of the options—including the new long/short mode—and how to use subcommands.

Refer to the [documentation](README.md) for more detailed information.

## Creating a project

Create a directory called "tutorial" for the project, and create a C++ project using whatever
your preferred tool set is. Then add Ookii.CommandLine to your project using one of the options
outlined [here](../README.md#requirements).

From here on, I'll assume you're using CMake, but if not, all the C++ code is the same regardless.

If you do use CMake, create a CMakeLists.txt file with the following contents:

```cmake
cmake_minimum_required(VERSION 3.15)

project(tutorial)

include(CheckIncludeFileCXX)
include(FetchContent)

# C++ 20 or later is required for Ookii.CommandLine
set(CMAKE_CXX_STANDARD 20)

FetchContent_Declare(OOKIICL
    GIT_REPOSITORY "https://github.com/SvenGroot/Ookii.CommandLine.Cpp"
    GIT_TAG "v2.0")

FetchContent_MakeAvailable(OOKIICL)

add_executable(tutorial "main.cpp" "arguments.h")
target_link_libraries(tutorial PRIVATE Ookii.CommandLine::OOKIICL)

check_include_file_cxx("format" HAVE_FORMAT)
if(NOT HAVE_FORMAT)
  find_package(fmt)
  target_link_libraries(tutorial PRIVATE fmt::fmt)
endif()
```

This CMake file creates a tutorial project, downloads and adds Ookii.CommandLine to that project,
and if the compiler doesn't have the C++20 `<format>` header, tries to find and link to
[libfmt](https://github.com/fmtlib/fmt).

## Defining and parsing command line arguments

Add a file to your project called arguments.h, and insert the following code:

```c++
#include <ookii/command_line.h>

struct arguments
{
    std::string path;

    static std::optional<arguments> parse(int argc, char *argv[])
    {
        arguments args{};
        auto name = ookii::command_line_parser::get_executable_name(argc, argv);
        auto parser = ookii::parser_builder{name}
            .add_argument(args.path, "Path").positional().required()
            .build();

        if (!parser.parse(argc, argv, {}))
        {
            return {};
        }

        return args;
    }
};
```

> Feel free to add include guards or `#pragma once` to the header if you like; I've omitted it for
> simplicity.

In Ookii.CommandLine, you define arguments by creating variables that will hold the parsed values,
and then using the [`parser_builder`][] class to define the arguments and create a [`command_line_parser`][].

The code above uses a struct to hold the arguments, and a method in that struct to parse them, but
this isn't required. You can use local variables just as easily. This is just an easy pattern that
keeps your code organized, and makes it easier to switch to using [code generation](#using-code-generation)
later.

The code above defines a single argument called "Path", indicates it's the first positional
argument, and makes it required. I like to use PascalCase argument names when using the default
parsing mode, but of course, you can use any naming convention your prefer.

The actual parsing is done when we call [`command_line_parser::parse()`][command_line_parser::parse()_0]. An important thing to notice
here is the empty third parameter in the call. Adding that parameter calls the overload that takes
a [`usage_writer`][], and that overload will handle errors and print usage help for us. In this case,
we're using the default [`usage_writer`][] by not explicitly providing one.

If you remove that parameter, it instead calls an overload that doesn't do those things, and you
would have to inspect the [`parse()`][parse()_0] method's return value in more detail to handle errors manually.

Now add a file named main.cpp, with the following contents:

```c++
#include <fstream>
#include "arguments.h"

int read_file(const arguments &args)
{
    std::ifstream file{args.path};
    std::string line;
    while (std::getline(file, line))
    {
        std::cout << line << std::endl;
    }
}

int main(int argc, char *argv[])
{
    auto args = arguments::parse(argc, argv);
    if (!args)
    {
        return 1;
    }

    read_file(*args);
    return 0;
}
```

This code parses the arguments we defined, returns an error code if it was unsuccessful, and writes
the contents of the file specified by the path argument to the console. Note that error handling
was omitted for simplicity, which because of how C++ file streams work means that it will silently
fail if you e.g. specify a path that doesn't exist.

So, let's build and run our application. For CMake, run the following from inside the "tutorial"
directory.

```sh
mkdir build
cd build
cmake ..
cmake --build .
```

If all went well, that should've built without errors. Then, we can run the project, passing it a
text file as an argument:

```sh
./tutorial ../main.cpp
```

This will print the contents of the main.cpp file, as shown above.

So far, so good. But what happens if we invoke the application without arguments? After all, we
made the `-Path` argument required. To try this, run the following command:

```text
./tutorial
```

This gives the following output:

```text
The required argument 'Path' was not supplied.

Usage: tutorial [-Path] <string> [-Help]

    -Help [<bool>] (-?, -h)
        Displays this help message.
```

> The actual usage help uses color if your console supports it. See [here](images/color.png) for
> an example.

As you can see, the [`command_line_parser::parse()`][command_line_parser::parse()_0] method lets us know what's wrong (we didn't
supply the required argument), and shows the usage help.

The usage syntax (the line starting with "Usage:") includes the argument we defined. However, the
list of argument descriptions below it does not. That's because our argument doesn't have a
description, and only arguments with descriptions are shown in that list by default. We'll add some
descriptions [below](#expanding-the-usage-help).

You can also see that there is another argument that we didn't define: `-Help`. This argument is
automatically added by Ookii.CommandLine. So, what does it do?

If you use the `-Help` argument (`./tutorial -Help`), it shows the same message as before. The only
difference is that there's no error message, even if you omitted the `-Path` argument. And even if
you do supply a path together with `-Help`, it still shows the help and exits, it doesn't run the
application. Basically, the presence of `-Help` will override anything else.

> If you define an argument called "Help", the automatic argument won't be added. Also, you can
> disable the automatic argument by calling [`automatic_help_argument(false)`][] on the
> [`parser_builder`][].

Note that in the usage syntax, your "Path" argument still has its name shown as `-Path`, even though
it's positional and can be used without a name. That's because every argument, even positional ones,
can still be supplied by name. So if you run this:

```text
./tutorial -path ../main.cpp
```

The output is the same as above.

> Argument names are case insensitive by default, so even though I used `-path` instead of `-Path`
> above, it still worked.

## Arguments with other types

Arguments don't have to be strings. In fact, they can have any type as long as there's a way to
[convert to that type](Arguments.md#argument-value-conversion) from a string. All of the basic C++
types are supported (like `int`, `float`, `bool`, etc.), and you can add support for custom types as
well.

Let's try this out by adding more arguments in the `arguments` struct. First add the following
fields:

```c++
int max_lines;
bool inverted;
```

And add the arguments to the [`parser_builder`][]:

```c++
.add_argument(args.max_lines, "MaxLines").alias("Max").default_value(50)
.add_argument(args.inverted, "Inverted")
```

This defines two new arguments. The first, `-MaxLines`, uses `int` as its type, so it will only
accept integer numbers. This argument is not positional (you must use the name), and it's optional,
so we've set a default value which will be used if it isn't supplied.

If you don't set a default value, the variable will not be set if the argument isn't supplied, so it
will remain at its initial value. If you want to be able to easily tell if the argument was supplied
or not, you can use [`std::optional<T>`][].

Since `-MaxLines` might be a bit verbose, we've given it an alias `-Max`, which can be used as an
alternative name to supply the argument. An argument can have any number of aliases; just repeat the
[`alias()`][] method.

The second argument, `-Inverted`, is a boolean, which means it's a switch argument. Switch arguments
don't need values, you either supply them or you don't.

Now, let's update `read_file` to use the new arguments:

```csharp
void read_file(const arguments &args)
{
    auto support = ookii::vt::virtual_terminal_support::enable_color(ookii::standard_stream::output);
    if (support && args.inverted)
    {
        std::cout << ookii::vt::text_format::negative;
    }

    std::ifstream file{args.path};
    std::string line;
    for (int count = 0;
         std::getline(file, line) && count < args.max_lines;
         ++count)
    {
        std::cout << line << std::endl;
    }

    if (support && args.inverted)
    {
        std::cout << ookii::vt::text_format::default_format;
    }
}
```

This code uses virtual terminal sequences to change the console color; to do this, we must check if
it's supported and, on Windows, enable support. The [`virtual_terminal_support`][] class provided with
Ookii.CommandLine does this for us.

Now we can run the application like this:

```text
./tutorial ../main.cpp -max 5 -inverted
```

And it'll only show the first five lines of the file, using inverted colors.

If you supply a value that's not a valid integer for `-MaxLines`, you'll once again get an error
message and the usage help.

## Expanding the usage help

We saw before that our custom arguments were included in the usage syntax, but didn't have any
descriptions. Typically, you'll want to add descriptions to your arguments. This is done using the
[`description()`][description()_1] method when using the [`parser_builder`][].

Let's add some for our arguments:

```c++
auto parser = ookii::parser_builder{name}
    .description("Reads a file and displays the contents on the command line.")
    .add_version_argument([]()
        {
            std::cout << "Ookii.CommandLine Tutorial 1.0" << std::endl;
        })
    .add_argument(args.path, "Path").required().positional()
        .description("The path of the file to read.")
    .add_argument(args.max_lines, "MaxLines").alias("Max").default_value(50).value_description("number")
        .description("The maximum number of lines to output.")
    .add_argument(args.inverted, "Inverted")
        .description("Invert the console colors for the output.")
    .build();
```

I've also added a description to the parser itself. That description is shown before the usage
syntax as part of the usage help. Use it to provide a description for your application as a whole.

The `MaxLines` argument now also sets its *value description*. The value description is a short,
typically one-word description of the type of values the argument accepts, which is shown in angle
brackets in the usage help. It defaults to the type name, but "int" might not be very meaningful
to people who aren't programmers, so we've changed it to "number" instead.

Another thing we did is add a `-Version` argument. This argument is another standard argument, like
`-Help`, supported by Ookii.CommandLine, but to use it you need to tell the [`parser_builder`][] what
to do if it's supplied.

Now, let's run the application using `./tutorial -help`:

```text
Reads a file and displays the contents on the command line.

Usage: tutorial [-Path] <string> [-Help] [-Inverted] [-MaxLines <number>] [-Version]

    -Path <string>
        The path of the file to read.

    -Help [<bool>] (-?, -h)
        Displays this help message.

    -Inverted [<bool>]
        Invert the console colors for the output.

    -MaxLines <number> (-Max)
        The maximum number of lines to output. Default value: 50.

    -Version [<bool>]
        Displays version information.
```

Now our usage help looks a lot better! All the arguments are present in the description list. Also
note how the default value we used was automatically added to the description of `-MaxLines` (this
can be disabled if you want).

The `-MaxLines` argument also has its alias listed, just like the `-Help` argument. Both the `-Help`
and `-Version` argument have standard descriptions (these can be changed with the
[`localized_string_provider`][] class).

> Don't like the way the usage help looks? It can be fully customized! Check out the
> [custom usage sample](../samples/custom_usage) for an example of hwo to do that.

Now that we have a `-Version` argument, let's try to use it by running `./tutorial -Version`:

```text
Ookii.CommandLine Tutorial 1.0
```

When the version argument is supplied, the lambda we passed to the [`add_version_argument()`][] method
is executed, which displays the message. Note that when the `-Version` argument is supplied, the
remaining arguments won't be parsed and the [`parse()`][parse()_0] method returns a failure result, so the
application won't run regardless of what other arguments are present.

## Long/short mode and other customizations

Ookii.CommandLine offers many options to customize the way it parses the command line. For example,
you can disable the use of white space as a separator between argument names and values, and specify
a custom separator. You can specify custom argument name prefixes, instead of `-` which is the
default (on Windows only, `/` is also accepted by default). You can make the argument names case
sensitive. And there's more.

These options are specified using methods on the [`parser_builder`][] class. Let's add some options:

```c++
auto parser = ookii::parser_builder{name}
    .mode(ookii::parsing_mode::long_short)
    .case_sensitive(true)
    .description("Reads a file and displays the contents on the command line.")
    .add_version_argument([]()
        {
            std::cout << "Ookii.CommandLine Tutorial 1.0" << std::endl;
        })
    .add_argument(args.path, "path").required().positional()
        .description("The path of the file to read.")
    .add_argument(args.max_lines, "max-lines").alias("max").short_name()
        .default_value(50).value_description("number")
        .description("The maximum number of lines to output.")
    .add_argument(args.inverted, "inverted").short_name()
        .description("Invert the console colors for the output.")
    .build();
```

We've done a few things here: we've turned on an alternative set of parsing rules by setting the
[`mode()`][mode()_1] to [`parsing_mode::long_short`][], and we've made argument names case sensitive. We've also
changed the argument names to lower case with dashes (i.e. dash-case).

These options combined make the application's parsing behavior very similar to common POSIX
conventions; the same conventions followed by tools such as `cargo` or `git`, and many others. For
a cross-platform application, you may prefer these conventions over the default, but it's up to you
of course.

Long/short mode is the key to this behavior. It enables every argument can have two separate names:
a long name, using the `--` prefix by default, and a single-character short name using the `-`
prefix (and `/` on Windows).

When using long/short mode, all arguments have long names by default, but you'll need to indicate
which arguments have short names. We've done that here with the `--max-lines` and `--inverted`
arguments, by calling [`short_name()`][short_name()_0]. This gives them a short name using the first character of
their long name, so `-m` and `-i` in this case. You can also specify a custom short name by
supplying a parameter to the [`short_name()`][short_name()_2] method.

If you want an argument to only have a short name, there are overloads of [`add_argument()`][add_argument()_1] that
take a short name.

With all these changes, we now have an argument with the long name `--max-lines`, and the short name
`-m`, and the long alias `--max` (use the [`short_alias()`][] method to create a short alias). We also
have an argument with the long name `--inverted`, and the short name `-i`. Finally, `--path` only
has a long name, and is still positional. All of these names are now case sensitive.

Now, the usage help looks like this:

```text
Reads a file and displays the contents on the command line.

Usage: tutorial [--path] <string> [--help] [--inverted] [--max-lines <number>] [--version]

        --path <string>
            The path of the file to read.

    -?, --help [<bool>] (-h)
            Displays this help message.

    -i, --inverted [<bool>]
            Invert the console colors for the output.

    -m, --max-lines <number> (--max)
            The maximum number of lines to output. Default value: 50.

        --version [<bool>]
            Displays version information.
```

As you can see, the format is slightly different, giving more prominence to the short names.

Notice how the `--help` and `--version` argument also have lower case names now. For these arguments,
Ookii.CommandLine will match the casing to the first manually defined argument, so they will be
consistent without having to explicitly customize them.

## Using code generation

Ookii.CommandLine comes with a code-generation script, `New-Parser.ps1`, that provides an easy
alternative to manually using the [`parser_builder`][] class. This script looks for a specially
annotated struct in a header file, using attributes in the comments to indicate what the arguments
are and what options to use.

The script uses the cross-platform version of [PowerShell](https://github.com/PowerShell/PowerShell),
which must be installed on your system. Even on Windows, you must install this version; the script
uses some features that aren't available on the built-in Windows PowerShell.

Let's modify our application to use code generation. If you're following along, *please make a backup
of your current code*, as we'll go back to the non-generated version after this section.

Since our arguments are already in a struct in a header file, it's very easy to adapt it to use
code generation (it's almost like I did that on purpose). All we need to do is add the attributes,
and remove the `parse()` method we wrote to replace it with the one the script generates.

```c++
// [arguments]
// [mode: long_short]
// [case_sensitive]
// [name_transform: dash-case]
// [version_info: Ookii.CommandLine Tutorial 1.0]
// Reads a file and displays the contents on the command line.
struct arguments
{
    // [argument, required, positional]
    // Reads a file and displays the contents on the command line.
    std::string path;

    // [argument, short_name]
    // [alias: max]
    // [default: 50]
    // he maximum number of lines to output.
    int max_lines;

    // [argument, short_name]
    // Invert the console colors for the output.
    bool inverted;

    OOKII_DECLARE_PARSE_METHOD(arguments);
};
```

The script uses attributes that are enclosed in square brackets, in the comment block before the
item they apply to. The first attribute must always be `[arguments]`, which indicates the struct
(or class) after it contains arguments. Additional attributes on the struct set options for the
parser; in this case, we set the long/short mode, enable case sensitivity, and set a string with
information for the `--version` argument.

If there are any comment lines after the attributes, these are treated as the description. In the
case of the struct itself, this is the application description.

Each argument is a field, and must be marked with the `[argument]` attribute. This attribute can
have a value to explicitly set a name for the argument (e.g. `[argument: name]`). If it doesn't, the
name of the field is used.

Rather than requiring you to use the field names verbatim, the script can transform the names to
follow a difference convention, which we've done here with the `[name_transform]` attribute on the
struct. All of the field names will be automatically transformed to dash-case, without having to
set explicit names. Other possible name transformations are PascalCase, camelCase, snake_case, and
trim (which just removes leading and trailing underscores, without other changes).

The script will generate a `parse()` method for the struct, but this method must be declared in the
struct first. We can't use the previous parse method, because it doesn't match the exact signature
the script expects. The signature used by the script is as follows:

```c++
static std::optional<arguments_type> parse(int argc, const char* const argv[], ookii::usage_writer *usage = nullptr,
    ookii::localized_string_provider *string_provider = nullptr, const std::locale &locale = {})
```

To make sure you use the correct signature, use the `OOKII_DECLARE_PARSE_METHOD` macro to declare
the method, rather than doing it manually.

Of course, we need to run the script and add the generated output file to the build. You could do
this manually, but it's much better to add this to the build. When using CMake, this can be done
using the `find_program` and `add_custom_command` commands. Remove the existing `add_executable`
command, and add the following to your CMakeLists.txt file:

```cmake
find_program(POWERSHELL_PATH NAMES pwsh NO_PACKAGE_ROOT_PATH NO_CMAKE_PATH NO_CMAKE_ENVIRONMENT_PATH NO_CMAKE_SYSTEM_PATH NO_CMAKE_FIND_ROOT_PATH)
if (POWERSHELL_PATH)
    message("-- Found PowerShell at ${POWERSHELL_PATH}")
else()
    message(FATAL_ERROR "-- PowerShell (pwsh) was not found.")
endif()

set(GENERATED_OUTPUT "${CMAKE_BINARY_DIR}/generated.cpp")
set(GENERATED_INPUT "${CMAKE_CURRENT_LIST_DIR}/arguments.h")

# Invoke the PowerShell script to generate the command line parser.
add_custom_command(
    OUTPUT ${GENERATED_OUTPUT}
    COMMAND ${POWERSHELL_PATH}
        -ExecutionPolicy Bypass
        "${Ookii.CommandLine.Cpp_SOURCE_DIR}/scripts/New-Parser.ps1"
        -Path "${GENERATED_INPUT}"
        -OutputPath "${GENERATED_OUTPUT}"
    DEPENDS ${GENERATED_INPUT}
    VERBATIM
)

add_executable(tutorial "main.cpp" "arguments.h" ${GENERATED_OUTPUT})
```

You can now reconfigure and build your project, and the result should be identical to the version
without code generation. If you want to see what code was generated, check the build/generated.cpp
file after you build the project. You'll see it looks almost identical to the code we wrote
ourselves.

Because we already used a struct with a `parse()` method, no changes to our `main()` method were
needed to use the generated code. However, the script can make our life even easier, and generate
the main method for us. To do this, first put a declaration of the `read_file()` method in
arguments.h, so the generated code can use it:

```c++
int read_file(const arguments &args);
```

Yes, I've changed the return type of the function to `int`, so make the corresponding change in
main.cpp and just add a `return 0;` to the end of the function. The script requires a function
returning `int` if it generates a `main()` method.

Next, delete the existing `main()` function from main.cpp, and add the following argument to the
script invocation in CMakeLists.txt:

```cmake
add_custom_command(
    OUTPUT ${GENERATED_OUTPUT}
    COMMAND ${POWERSHELL_PATH}
        -ExecutionPolicy Bypass
        "${Ookii.CommandLine.Cpp_SOURCE_DIR}/scripts/New-Parser.ps1"
        -Path "${GENERATED_INPUT}"
        -OutputPath "${GENERATED_OUTPUT}"
        -EntryPoint read_file
    DEPENDS ${GENERATED_INPUT}
    VERBATIM
)
```

The `-EntryPoint` argument tells the script to generate a `main()` method that parses the arguments
and calls a function named `read_file` if it was successful.

Before continuing, make sure to revert to the non-code-generated version of the tutorial project,
since that's what we'll be using below. We'll return to using code generation later.

Check out the [code generation script documentation](../docs/Scripts.md) for more information.

## Using subcommands

Many applications have multiple functions, which are invoked through subcommands. Think for example
of `git`, which has commands like `git pull` or `git cherry-pick`. Each command does something
different, and needs its own command line arguments.

Creating subcommands with Ookii.CommandLine is very similar to what we've been doing already. A
subcommand is a class that derives from the [`ookii::command`][] type. It defines arguments using the
[`parser_builder`][], but instead of using a `parse()` method, it does so in its constructor.

Let's change the example we've built so far to use subcommands. I'm going to continue with the
POSIX-like long/short mode, but if you prefer the defaults, you can go back to that version too.

To do this, we'll rename arguments.h to read_command.h (and make the appropriate change in
CMakeLists.txt and main.cpp), and change the [`arguments`][] struct to a `read_command` class. We'll
turn the `parse()` method into a public constructor that takes a [`parser_builder`][] as an argument.

```c++
class read_command : public ookii::command
{
public:
    read_command(ookii::parser_builder &builder)
    {
        builder
            .add_argument(_path, "path").required().positional()
                .description("The path of the file to read.")
            .add_argument(_max_lines, "max-lines").alias("max").short_name()
                .default_value(50).value_description("number")
                .description("The maximum number of lines to output.")
            .add_argument(_inverted, "inverted").short_name()
                .description("Use black text on a white background.");
    }

    int run() override;

private:
    std::string _path;
    int _max_lines{};
    bool _inverted{};
};
```

> I've changed the field names to match my convention for private fields; this is optional of course.

The constructor has a few differences from the old `parse()` method: I've removed the calls to
[`mode()`][mode()_1] and [`case_sensitive()`][case_sensitive()_1]; while we could do that here, we probably want to have those options
apply to all commands. I've also removed the description and the version argument.

Crucially, the constructor *does not call [`build()`][]*. The [`command_manager`][] class will do that for
us.

The [`ookii::command`][] class has one pure virtual member that we must implement, the [`run()`][] method.
To implement it, we'll adapt the `read_file` method in main.cpp.

```c++
int read_command::run()
{
    auto support = ookii::vt::virtual_terminal_support::enable_color(ookii::standard_stream::output);
    if (support && _inverted)
    {
        std::cout << ookii::vt::text_format::negative;
    }

    std::ifstream file{_path};
    std::string line;
    for (int count = 0;
         std::getline(file, line) && count < _max_lines;
         ++count)
    {
        std::cout << line << std::endl;
    }

    if (support && _inverted)
    {
        std::cout << ookii::vt::text_format::default_format;
    }

    return 0;
}
```

The [`run()`][] method is like the `main()` method for your command, and its return value should be
treated like the exit code returned from `main()`, because typically, you will return the executed
command's return value from `main()`.

And that's it: we've now defined a command. However, we still need to change the `main()` method to
use commands instead of just parsing arguments from a single class. Fortunately, this is very
simple. Update your `main()` method as follows:

```csharp
int main(int argc, char *argv[])
{
    auto name = ookii::command_line_parser::get_executable_name(argc, argv);
    ookii::command_manager manager{name, true};
    manager
        .description("An application to read and write files.")
        .common_help_argument("--help")
        .configure_parser([](auto &builder)
            {
                builder.mode(ookii::parsing_mode::long_short);
            })
        .add_version_command([]()
            {
                std::cout << "Ookii.CommandLine Tutorial 1.0" << std::endl;
            })
        .add_command<read_command>("read", "Reads a file and displays the contents on the command line.");

    return manager.run_command(argc, argv).value_or(1);
}
```

The [`command_manager`][] class handles finding, creating and running of your commands. You can use it
to set various options common to each command as well. As you can see, the syntax to do so is
similar to the [`parser_builder`][] class, with methods that chain together. Let's walk through what we
did here.

We instantiate the command manager with the application executable name, and the second parameter
is a boolean indicating the commands are case sensitive. This makes both the command names and the
argument names for each command case sensitive.

The call to [`description()`][description()_2] sets a description for the application as a whole, rather than any
individual command. It's used in the usage help when the list of commands is shown.

We then tell the command manager that every command has an argument named `--help`. This allows the
command manager to show a message in the usage help telling the user how to get help for a command.

The [`configure_parser()`][] method lets you specify a function that will be called when the
[`parser_builder`][] is created for each command. We can use this to specify parser options that are
common to each command, such as the parsing mode in this case ([`case_sensitive`][case_sensitive_1] isn't needed because
the command manager is already case sensitive, but you could use it here if you wanted e.g. case
insensitive command names but case sensitive argument names).

We then add a `version` command, similar to the `--version` argument before. With subcommands, it
typically makes more sense to have a version command rather than an argument on each command.

Finally, we add our command, giving it the name "read" and the same description we used earlier.

> You can also set the name and description of a command by giving the class `static std::string name()`
> and `static std::string description()` methods. Then you don't need to specify them here. If
> you don't give a command a name at all, the class name is used.

After the manager is set up, we can run a command. The [`run_command()`][run_command()_2] method takes the provided
arguments, and uses the first argument as the command name. If a command with that name exists, it
creates a [`parser_builder`][], initializing it with the case sensitivity option used by the command
manager, and the description for the command, before calling first the [`configure_parser()`][]
callback, and then invoking the command's constructor. It then builds a [`command_line_parser`][],
and parses the arguments.

If anything goes wrong, it will either display a list of commands, or if a command has been found,
the help for that command. The return value is the value returned from [`ookii::command::run()`][], or
[`std::nullopt`][] if parsing failed, in which case in this sample we return a non-zero exit code to
indicate failure.

> If you want to customize any of these steps, there are methods like [`get_command()`][] and
> [`create_command()`][create_command()_0] that you can call to do this manually.

If we build our application, and run it without arguments again (`./tutorial`), we see the
following:

```text
An application to read and write files.

Usage: tutorial <command> [arguments]

The following commands are available:

    read
        Reads a file and displays the contents on the command line.

    version
        Displays version information.

Run 'tutorial <command> --help' for more information about a command.
```

When no command, or an unknown command, is supplied, a list of commands is printed, including their
descriptions.

Let's see the usage help for our command:

```text
./tutorial read --help
```

Which gives the following output:

```text
Reads a file and displays the contents on the command line.

Usage: tutorial read [--path] <string> [--help] [--inverted] [--max-lines <number>]

        --path <string>
            The path of the file to read.

    -?, --help [<bool>] (-h)
            Displays this help message.

    -i, --inverted [<bool>]
            Use black text on a white background.

    -m, --max-lines <number> (--max)
            The maximum number of lines to output. Default value: 50.
```

There are two differences to spot from the earlier version: the usage syntax now says `tutorial read`
before the arguments, indicating you have to use the command, and the `--version` argument is gone
since we replaced it with the `version` command.

So we turned our application into a command, but we still have only one command ("version" doesn't
count), and the application description we just added is lying (the application only reads files).

## Multiple commands

An application with only one subcommand doesn't really need to use subcommands, so let's add a
second one. Create a new file in your project called write_command.h (add it to CMakeLists.txt as
well), and add the following code:

```c++
#include <ookii/command_line.h>

class write_command : public ookii::command
{
public:
    write_command(ookii::parser_builder &builder)
    {
        builder
            .add_argument(_path, "path").required().positional()
                .description("The path of the file to write.")
            .add_multi_value_argument(_text, "text").required().positional()
                .description("The text to write to the file.")
            .add_argument(_append, "append").short_name()
                .description("Append to the file instead of overwriting it.");
    }

    int run() override;

private:
    std::string _path;
    std::vector<std::string> _text;
    bool _append{};
};
```

There's one thing here that we haven't seen before, and that's a multi-value argument. The `--text`
argument has an container type ([`std::vector<string>`][]) and is added with the
[`add_multi_value_argument()`][add_multi_value_argument()_1] method, which means it can have multiple values by supplying it
multiple times. We could, for example, use `--text foo --text bar` to assign the values "foo" and
"bar" to it. Because it's also a positional argument, we can also simply use `foo bar` to do the
same.

> Positional multi-value arguments must always be the last positional argument.

This command will take the values from the `--text` argument and write them as lines to the specified
file, optionally appending to the file.

We need to implement the [`run()`][] method, of course, so add `#include <write_command.h>` to the
main.cpp file, and add the following:

```c++
int write_command::run()
{
    auto mode = std::ios_base::out;
    if (_append)
    {
        mode |= std::ios_base::app;
    }

    std::ofstream file{_path, mode};
    for (const auto &line : _text)
    {
        file << line << std::endl;
    }

    return 0;
}
```

Finally, we have to add our new command to the [`command_manager`][]:

```c++
.add_command<write_command>("write", "Writes text to a file.");
```

Let's build and run our application again, without arguments:

```text
./tutorial
```

Which now gives the following output:

```text
An application to read and write files.

Usage: tutorial <command> [arguments]

The following commands are available:

    read
        Reads a file and displays the contents on the command line.

    version
        Displays version information.

    write
        Writes text to a file.

Run 'tutorial <command> --help' for more information about a command.
```

If you run `./tutorial write --help`, you'll see the usage help for your new command:

```text
Writes text to a file.

Usage: tutorial write [--path] <string> [--text] <string>... [--append] [--help]

        --path <string>
            The path of the file to write.

        --text <string>
            The text to write to the file.

    -a, --append [<bool>]
            Append to the file instead of overwriting it.

    -?, --help [<bool>] (-h)
            Displays this help message.
```

We can test out our new command like this:

```text
$ ./tutorial write test.txt "Hello!" "Ookii.CommandLine is pretty neat." "At least I think so."
$ ./tutorial write test.txt "Thanks for using it!" -a
$ ./tutorial read test.txt
Hello!
Ookii.CommandLine is pretty neat.
At least I think so.
Thanks for using it!
```

Here, we wrote three lines of text to a file, then appended one more line, and read them back using
the "read" command.

## Code generation for commands

As with the regular parser, there is also a code-generation script for subcommands, `New-Subcommand.ps1`.
This script works in a similar way to the `New-Parser.ps1` script, except it will generate the
constructor, rather than a `parse()` method for your commands. It will also generate a function
to create [`command_manager`][] with all your commands.

So, let's change this sample to use code generation once again. In this case, for both `read_command`
and `write_command`, we will remove the constructor definition, and add the necessary annotations.

So, in read_command.h, make the following changes:

```c++
#include <ookii/command_line_generated.h>

// [command: read]
// Reads a file and displays the contents on the command line.
class read_command : public ookii::command
{
public:
    read_command(ookii::parser_builder &builder);

    int run() override;

private:
    // [argument, required, positional]
    // The path of the file to read.
    std::string _path;

    // [argument, short_name]
    // [default: 50]
    // The maximum number of lines to output.
    int _max_lines{};

    // [argument, short_name]
    // Use black text on a white background.
    bool _inverted{};
};
```

This is essentially the same as we did when generating a regular parser, except instead of
`[arguments]`, subcommands need to start with the `[command]` attribute. The argument to that
attribute specified the name of the command, and any comment lines after the attributes are the
description of the command.

We also didn't declare a `parse()` method; we just need to declare (but not define) the constructor.

We can do the same in write_command.h:

```c++
#include <ookii/command_line_generated.h>

// [command: write]
// Writes text to a file.
class write_command : public ookii::command
{
public:
    write_command(ookii::parser_builder &builder);

    int run() override;

private:
    // [argument, required, positional]
    // The path of the file to write.
    std::string _path;

    // [argument, multi_value, required, positional]
    // The text to write to the file.
    std::vector<std::string> _text;

    // [argument, short_name]
    // Append to the file instead of overwriting it.
    bool _append{};
};
```

Note that the `--text` argument has to have an explicit `[multi_value]` attribute.

What about all the other settings, like the parsing mode, case sensitivity, or things like the
version command? We can set those with a comment block marked `[global]`. Add the following to
either header file (or a separate one, if you prefer):

```c++
// [global]
// [mode: long_short]
// [case_sensitive]
// [common_help_argument: --help]
// [name_transform: dash-case]
// [version_info: Ookii.CommandLine Tutorial 1.0]
// An application to read and write files.
```

If it's not the end of a file, make sure you end the block with a blank line. Here, we've set all
the options we used before, as well as the application description. Note also the `[common_help_argument]`
attribute, which only applied to `[global]` blocks for the command manager.

We can now simplify our main method:

```c++
int main(int argc, char *argv[])
{
    auto name = ookii::command_line_parser::get_executable_name(argc, argv);
    auto manager = ookii::register_commands(name);

    return manager.run_command(argc, argv).value_or(1);
}
```

The [`ookii::register_commands()`][] function is declared in `<ookii/command_line_generated.h>`, and
its definition is generated by the `New-Subcommand.ps1` script.

Finally, we have to modify CMakeLists.txt again to invoke the script.

```cmake
set(GENERATED_OUTPUT "${CMAKE_BINARY_DIR}/generated.cpp")
set(GENERATED_INPUT "${CMAKE_CURRENT_LIST_DIR}/read_command.h" "${CMAKE_CURRENT_LIST_DIR}/write_command.h")
list(JOIN GENERATED_INPUT "," GENERATED_INPUT_LIST)

# Invoke the PowerShell script to generate the subcommands
add_custom_command(
    OUTPUT ${GENERATED_OUTPUT}
    COMMAND ${POWERSHELL_PATH}
        -ExecutionPolicy Bypass
        -Command "&{ \
            ${Ookii.CommandLine.Cpp_SOURCE_DIR}/scripts/New-Subcommand.ps1 \
                -Path ${GENERATED_INPUT_LIST} \
                -OutputPath ${GENERATED_OUTPUT} \
        }"
    DEPENDS ${GENERATED_INPUT}
    VERBATIM
)

add_executable(tutorial "main.cpp" "read_command.h" "write_command.h" ${GENERATED_OUTPUT})
```

This is slightly different from before. Instead of invoking the script directly, we have to use the
`pwsh -Command` argument so we can pass multiple input files as an array to the script.

Now, you can build your project, and it will behave the same as before. You can also check the
build/generated.cpp file again to see what code was generated.

Like the `New-Parser.ps1` script, the `New-Subcommand.ps1` script can also generate a `main()`
function for us. This is accomplished with the `-GenerateMain` parameter:

```cmake
add_custom_command(
    OUTPUT ${GENERATED_OUTPUT}
    COMMAND ${POWERSHELL_PATH}
        -ExecutionPolicy Bypass
        -Command "&{ \
            ${Ookii.CommandLine.Cpp_SOURCE_DIR}/scripts/New-Subcommand.ps1 \
                -Path ${GENERATED_INPUT_LIST} \
                -OutputPath ${GENERATED_OUTPUT} \
                -GenerateMain \
        }"
    DEPENDS ${GENERATED_INPUT}
    VERBATIM
)
```

We can then remove the `main()` function from main.cpp. The generated function will do the same
thing as our manual one did: register all the commands, and try to run one.

## Common arguments for commands

Sometimes, you'll want some arguments to be available to all commands. With Ookii.CommandLine, the
way to do this is to make a common base class.

For example, if we wanted to make a common base class to share the `--path` argument between the
"read" and "write" commands, we could do so like this (without code generation):

```c++
class base_command : public ookii::command
{
public:
    base_command(ookii::parser_builder &builder)
    {
        builder
            .add_argument(_path, "path").required().positional()
                .description("The path of the file.");
    }

protected:
    std::string _path;
};

class read_command : public base_command
{
public:
    read_command(ookii::parser_builder &builder)
        : base_command{builder}
    {
        builder
            .add_argument(_max_lines, "max-lines").alias("max").short_name()
                .default_value(50).value_description("number")
                .description("The maximum number of lines to output.")
            .add_argument(_inverted, "inverted").short_name()
                .description("Use black text on a white background.");
    }

    int run() override;

private:
    int _max_lines{};
    bool _inverted{};
};
```

The `write_command` changes would be identical; just change the base class, remove the `_path`
field, and call the base class constructor.

Now both commands share the `--path` argument defined in the base class, in addition to the
arguments they define themselves. Note that `base_command` is not itself a valid command, because
it's an abstract class (the [`run()`][] method is still pure virtual), so trying to add it to the
[`command_manager`][] would cause compilation errors.

If you apply other options to the [`parser_builder`][] in the base class constructor, this is another
way to share them between commands without having to use the [`configure_parser()`][] method, a
`[global]` block when using code generation.

If using the code generation script, the common base class would look like this:

```c++
// [command, no_register]
class base_command : public ookii::command
{
public:
    base_command(ookii::parser_builder &builder);

protected:
    // [argument, required, positional]
    // The path of the file.
    std::string _path;
};

// [command: read]
// Reads a file and displays the contents on the command line.
class read_command : public base_command
{
    /* Remove the _path field, leave everything else as is */
}

// [command: write]
// Reads a file and displays the contents on the command line.
class write_command : public base_command
{
    /* Remove the _path field, leave everything else */
}
```

Because the base_command class cannot be used with the [`command_manager`][] (and we don't want it to),
the `[no_register]` attribute was added. It tells the code generation script not to register that
class.

In this case, you do not need to change anything about the constructors of the derived classes,
because the code generation script will automatically call the base class constructor.

> The code generation script always calls the base class constructor; for this reason,
> [`ookii::command`][] also has a constructor that takes a [`parser_builder`][], but it's empty so there's
> no need to call it when not using the script.

## More information

I hope this tutorial helped you get started with Ookii.CommandLine. To learn more, check out the
following resources:

- [Usage documentation](README.md)
- [Class library documentation](https://www.ookii.org/Link/CommandLineCppDoc)
- [Sample applications](../samples)

[`add_version_argument()`]: https://www.ookii.org/docs/commandline-cpp-2.0/classookii_1_1basic__parser__builder.html#a9abfabd3ea77bdda6b8c9c53010c6f9d
[`alias()`]: https://www.ookii.org/docs/commandline-cpp-2.0/classookii_1_1basic__parser__builder_1_1argument__builder__common.html#a44d77984b1cd12b04764f7f2741269d4
[`arguments`]: https://www.ookii.org/docs/commandline-cpp-2.0/classookii_1_1basic__command__line__parser.html#ab29d6b51c259b4c868f52501614c76ad
[`automatic_help_argument(false)`]: https://www.ookii.org/docs/commandline-cpp-2.0/classookii_1_1basic__parser__builder.html#acf3a2fe08de1bdef7a69cf26cf94ba12
[`build()`]: https://www.ookii.org/docs/commandline-cpp-2.0/classookii_1_1basic__parser__builder.html#af66361855468fde2eb545fbe1631e042
[`command_line_parser`]: https://www.ookii.org/docs/commandline-cpp-2.0/classookii_1_1basic__command__line__parser.html
[`command_manager`]: https://www.ookii.org/docs/commandline-cpp-2.0/classookii_1_1basic__command__manager.html
[`configure_parser()`]: https://www.ookii.org/docs/commandline-cpp-2.0/classookii_1_1basic__command__manager.html#a1a68ed8729ad0dfa2300a2a74691a0c6
[`get_command()`]: https://www.ookii.org/docs/commandline-cpp-2.0/classookii_1_1basic__command__manager.html#af7c67cf9f0a6ffda1e870268968b4229
[`localized_string_provider`]: https://www.ookii.org/docs/commandline-cpp-2.0/classookii_1_1basic__localized__string__provider.html
[`ookii::command::run()`]: https://www.ookii.org/docs/commandline-cpp-2.0/classookii_1_1basic__command.html#a1f14c66512418948c9cafc81fd7b881b
[`ookii::command`]: https://www.ookii.org/docs/commandline-cpp-2.0/classookii_1_1basic__command.html
[`ookii::register_commands()`]: https://www.ookii.org/docs/commandline-cpp-2.0/namespaceookii.html#ac514246d38c58d21cc168406737b4865
[`parser_builder`]: https://www.ookii.org/docs/commandline-cpp-2.0/classookii_1_1basic__parser__builder.html
[`parsing_mode::long_short`]: https://www.ookii.org/docs/commandline-cpp-2.0/namespaceookii.html#abf0aa62e29f4953f7cba303a3da407fd
[`run()`]: https://www.ookii.org/docs/commandline-cpp-2.0/classookii_1_1basic__command.html#a1f14c66512418948c9cafc81fd7b881b
[`short_alias()`]: https://www.ookii.org/docs/commandline-cpp-2.0/classookii_1_1basic__parser__builder_1_1argument__builder__common.html#a30ecafbbf18070a6bd79581754b4182a
[`std::nullopt`]: https://en.cppreference.com/w/cpp/utility/optional/nullopt
[`std::optional<T>`]: https://en.cppreference.com/w/cpp/utility/optional
[`std::vector<string>`]: https://en.cppreference.com/w/cpp/container/vector
[`usage_writer`]: https://www.ookii.org/docs/commandline-cpp-2.0/classookii_1_1basic__usage__writer.html
[`virtual_terminal_support`]: https://www.ookii.org/docs/commandline-cpp-2.0/classookii_1_1vt_1_1virtual__terminal__support.html#af9d2adc67cfa52c358a70864f75efb92
[add_argument()_1]: https://www.ookii.org/docs/commandline-cpp-2.0/classookii_1_1basic__parser__builder.html#a16b69672a4dd924cf5a402c419be3d05
[add_multi_value_argument()_1]: https://www.ookii.org/docs/commandline-cpp-2.0/classookii_1_1basic__parser__builder.html#a35953740728918a4d3daa0f69d052e16
[case_sensitive_1]: https://www.ookii.org/docs/commandline-cpp-2.0/classookii_1_1basic__parser__builder.html#a47a0c39a17d1dcca8b99455dab68dbb0
[case_sensitive()_1]: https://www.ookii.org/docs/commandline-cpp-2.0/classookii_1_1basic__parser__builder.html#a47a0c39a17d1dcca8b99455dab68dbb0
[command_line_parser::parse()_0]: https://www.ookii.org/docs/commandline-cpp-2.0/classookii_1_1basic__command__line__parser.html#ae0c7b9990c29f41343182ac3d9918af7
[create_command()_0]: https://www.ookii.org/docs/commandline-cpp-2.0/classookii_1_1basic__command__manager.html#ac15dc514b638e514036256a42aa0a353
[description()_1]: https://www.ookii.org/docs/commandline-cpp-2.0/classookii_1_1basic__parser__builder_1_1argument__builder__common.html#a1589adafc67093261b0a73237339593f
[description()_2]: https://www.ookii.org/docs/commandline-cpp-2.0/classookii_1_1basic__command__manager.html#a1ce6bb7ffd642322db2b8315036ab555
[mode()_1]: https://www.ookii.org/docs/commandline-cpp-2.0/classookii_1_1basic__parser__builder.html#a160689dea7f096c4e644634c48bae752
[parse()_0]: https://www.ookii.org/docs/commandline-cpp-2.0/classookii_1_1basic__command__line__parser.html#ae0c7b9990c29f41343182ac3d9918af7
[run_command()_2]: https://www.ookii.org/docs/commandline-cpp-2.0/classookii_1_1basic__command__manager.html#a4deb89f49a7ce6d03ed41cdbf8769d58
[short_name()_0]: https://www.ookii.org/docs/commandline-cpp-2.0/classookii_1_1basic__parser__builder_1_1argument__builder__common.html#acf0df83ada201975cc845ac63725904f
[short_name()_2]: https://www.ookii.org/docs/commandline-cpp-2.0/classookii_1_1basic__parser__builder_1_1argument__builder__common.html#a2b7a59866bc23888381b631b5f087f8e
