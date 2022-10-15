# Generating usage help

When you have an application that accepts command line arguments, it’s often useful to be able to
provide the user with information about how to invoke the application, including an overview of all
the arguments.

Creating this kind of usage help text is tedious, and you must make sure it is kept up to date
whenever you change the arguments to your application. For this reason, Ookii.CommandLine can
generate this usage help text automatically. The generated usage help can be customized by adding
descriptions to the arguments.

Usage help can be generated using the `command_line_parser::write_usage()` method. The output can be
customized using the `usage_options` class. Various overloads of the `command_line_parser::parse()`
method that take a parameter of type `usage_options` also automatically print usage help if an
error occurred while parsing.

By default, the `usage_options` class will use an [`ookii::line_wrapping_ostream`](LineWrappingStream.md)
for the standard output, white-space wrapping the generated output at the console line width.

The following example shows the usage help generated for the sample application included with the
Ookii.CommandLine library:

```text
Sample command line application. The application parses the command line and
prints the results, but otherwise does nothing and none of the arguments are
actually used for anything.

Usage: parser [-Source] <string> [-Destination] <string> [[-OperationIndex]
   <int>] [-Count <number>] [-Help] [-Value <string>...] [-Verbose]

    -Source <string>
        The source data.

    -Destination <string>
        The destination data.

    -OperationIndex <int>
        The operation's index. Default value: 1.

    -Count <number>
        Provides the count for something to the application.

    -Help [<bool>] (-?)
        Displays this help message.

    -Value <string>
        This is an example of a multi-value argument, which can be repeated
        multiple times to set more than one value.

    -Verbose [<bool>] (-v)
        Print verbose information; this is an example of a switch argument.
```

The usage help consists of three components: the application description, the argument usage syntax,
and the argument descriptions.

## Application description

The first part of the usage help is a description of your application. This is a short description
that explains what your application does and how it can be used. It can be any text you like, though
it’s recommended to keep it short.

The description is specified by using the `parser_builder::description()` method, as in the
following example:

```c++
auto parser = ookii::parser_builder{argv[0]}
    .description("This is the application description that is included in the usage help.")
    .add_argument(/* ... */);
```

If the description is not specified, no description is included in the usage help.

## Argument usage syntax

The argument syntax shows how your application can be invoked from the command line. The syntax
typically starts with the name of your application, and is followed by all the arguments, indicating
their name and type. It shows which arguments are required or optional, and whether they allow
multiple values. For positional arguments, the order is indicated as well.

The syntax for a single argument has the following format options:

1. For a required, non-positional argument:

    ```text
    -ArgumentName <ArgumentType>
    ```

2. For an optional, non-positional argument:

    ```text
    [-ArgumentName <ArgumentType>]
    ```

3. For a required, positional argument:

    ```text
    [-ArgumentName] <ArgumentType>
    ```

4. For an optional, positional argument:

    ```text
    [[-ArgumentName] <ArgumentType>]
    ```

5. For a switch argument:

    ```text
    [-ArgumentName]
    ```

6. For a multi-value argument (which can be combined with the other formatting options):

    ```text
    -ArgumentName <ArgumentType>...
    ```

Essentially, anything that's optional is enclosed in square brackets, switch arguments have their
value description (the argument type) omitted, and multi-value arguments are followed by an ellipsis.
The exact formatting can be customized using the `ookii::usage_options` class.

### Value description

Arguments (except switch arguments) are followed by a short description of the type of value they
support, in angle brackets. This is called the _value description_. It's a short, typically one-word
description that describes the kind of value the argument expects. It should not be used for the
longer description of the argument's purpose.

The value description defaults to the type of the argument (e.g. `int` or `string`), stripping off
any namespace prefixes. For multi-value arguments or arguments using `std::optional<T>`, the name
of the value type is used.

To specify a different value description for a particular argument, use the `parser_builder::argument_builder::value_description()`
method:

```c++
int arg;
auto parser = ookii::parser_builder{argv[0]}
    .add_argument(arg, "Arg").value_description("number")
    .build();
```

This will cause the argument's syntax to show `-Arg <number>` instead of `-Arg <int>`.

You can also provide a different default value description for a particular type by specializing
the `ookii::value_description` template:

```c++
template<>
struct ookii::value_description<int>
{
    static std::string get()
    {
        return "number";
    }
};
```

Now, all arguments of type int will use "number" as the value description.

Usually, you should provide a custom value description for any template type, because their type
name will not look very good. Ookii.CommandLine already provides a `value_description` specialization
for `std::basic_string` so the value description looks like "string" and not like "basic_string\<char, std::char_traits\<char>, std::allocator\<char> >"

## Argument descriptions

After the usage syntax, the usage help will write a list of all arguments with their detailed
descriptions. Arguments that do not have descriptions will be omitted from this list.

An argument's description can be specified using the `parser_builder::argument_builder::description()` method:

```c++
int arg;
auto parser = ookii::parser_builder{argv[0]}
    .add_argument(arg, "Arg").description("Provides a value to the application.")
    .build();
```

By default, the list of argument descriptions will include any argument aliases, and their default
values if set. This, along with the format of the descriptions, can be customized using
`usage_options`.

Putting everything together, we can now write the code that generated the usage help shown in the
sample at the top of this page:

```c++
std::string source;
std::string destination;
int operation_index;
std::optional<int> count;
bool verbose;
std::vector<std::string> values;
bool help;
auto parser = ookii::parser_builder{std::filesystem::path{argv[0]}.filename().string()}
    .description("Sample command line application. The application parses the command line and prints the results, but otherwise does nothing and none of the arguments are actually used for anything.")
    .add_argument(source, "Source").required().positional().description("The source data.")
    .add_argument(destination, "Destination").required().positional().description("The destination data.")
    .add_argument(operation_index, "OperationIndex").positional().default_value(1).description("The operation's index.")
    .add_argument(count, "Count").value_description("number").description("Provides the count for something to the application.")
    .add_argument(verbose, "Verbose").alias("v").description("Print verbose information; this is an example of a switch argument.")
    .add_multi_value_argument(values, "Value").description("This is an example of a multi-value argument, which can be repeated multiple times to set more than one value.")
    .add_argument(help, "Help").cancel_parsing().alias("?").description("Displays this help message.")
    .build();
```

Next, we'll take a look at [shell commands](ShellCommands.md).
