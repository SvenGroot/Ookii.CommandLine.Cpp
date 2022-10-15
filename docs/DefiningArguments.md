# Defining command line arguments

In order to use Ookii.CommandLine, you must use the `ookii::parser_builder` class to create a
command line argument parser. Using the builder, you will specify the names, types and attributes
(required, positional, description, default value, etc.) of each argument.

There are two ways to define arguments: manually using the `ookii:parser_builder`, or using a
[code generation script](Scripts.md) with a specially-annotated struct or class.

## Using the parser builder

Before you can define an argument, you must provide a place for its value to be stored. This can be
a local or global variable, or a field of a class or struct. The type of this variable or field
will determine the type of the argument.

Then, you must create the `ookii::parser_builder`, supplying the name of your application, which
will be used when [generating usage help](UsageHelp.md). Usually, this should be the executable
name of your application. It's a good idea to use `argv[0]`, which contains the executable name
by convention, for this purpose.

You must then define arguments using the parser, by using the `ookii::parser_builder::add_argument()`
and `ookii::parser_builder::add_multi_value_argument()` methods, which return an `ookii::parser_builder::argument_builder`
which allows you to further customize the argument. The parser builder and argument builder methods
all chain, so you can easily set properties on the parser builder and define multiple arguments
in one statement.

An argument created by the builder is by default optional and not positional, has no description,
the default value description for its type, and no default value.

To create a [required argument](Arguments.md#required-arguments), call the `required()` method on
the argument builder.

To create a [positional argument](Arguments.md#positional-arguments), call the `positional()` method
on the argument builder. You must add positional arguments in the order you wish them to have.

An argument's default value can be set using the builder's `default_value()` method. This value will
be used if the argument is not supplied.

If the type of an argument is `bool` or `std::optional<bool>`, this defines a switch argument.

When you are done defining arguments, call the `build()` method to create an `ookii::command_line_parser`.

Consider the following code:

```c++
std::string some_argument;
int other_argument = 0;
std::vector<int> values;
bool switch_argument = false;
auto parser = ookii::parser_builder{argv[0]}
    .add_argument(some_argument, "SomeArgument").default_value("default")
    .add_argument(other_argument, "OtherName").required().positional()
    .add_multi_value_argument(values, "Value").positional()
    .add_argument(switch_argument, "Switch")
    .build();
```

This code defines four arguments:

- An optional non-positional argument of type string with the name “SomeArgument” and the default
  value “default”.
- A required positional argument of type int with the name “OtherName”. The argument will be the
  first positional argument.
- An optional positional multi-value argument with the name `Value`. This is the second positional
  argument, and since it's multi-value, all remaining positional values will be added to this vector.
- A switch argument, because its type is a `bool`. Switch arguments can be supplied without a value;
  they will be true if present and false is not.

### Multi-value arguments

To define a [multi-value argument](Arguments.md#arguments-with-multiple-values), you must use the
`add_multi_value_argument()` method.

The type of a multi-value argument must be a container type which has at least the methods
`push_back()` and `clear()`, and the typedef `value_type`. For example, you can use `std::vector<T>`
or `std::list<T>`.

A multi-value switch argument can be created with a container of `bool` (e.g. `std::vector<bool>`).

### Default values

Default argument values set by the `default_value()` method property are applied only if the
argument is not required, and it was not supplied on the command line.

If no default value is set for an argument, the `command_line_parser` will not assign any value
to the variable used for the argument. This allows you to initialize the variable to the desired
value, and that value will not be changed. In the example above, the value of other_argument will
remain zero if it wasn't supplied.

However, if this method is used, the default value cannot be included in the usage help
automatically. To do that, you must use the `default_value()` method.

If you wish to be able to tell whether an argument was supplied, you should use `std::optional<T>`
as the type of the argument. If the argument was not supplied, the variable will remain empty
(`std::nullopt`).

### Arguments that cancel parsing

Sometimes, you may wish to show usage help even if the command line is valid (all required arguments
are present and there are no other errors), for example if the user supplied a "-Help" or "-?"
argument.

To enable this behavior, you use the `cancel_parsing()` method on the argument builder. If this
enabled, parsing is stopped when the argument is encountered. The rest of the
command line is not processed, and `command_line_parser::parse()` will return a result with
`parse_error::parsing_cancelled`. The `parse()` methods that take a `usage_options` will automatically
print usage in this case, but will not show an error message.

For example, you could use the following argument definition:

```csharp
bool help;
// ...
    .add_argument(help, "Help").alias("?").cancel_parsing()
// ...
```

### Aliases

An alias is an alternative name that can be used to specify a command line argument. Aliases can be
added to a command line argument by using the `alias()` method on the argument builder. To define
multiple aliases, call the `alias()` method multiple times.

### Other properties

The argument builder has other methods to customize an argument, including setting its description,
its value description (a short, one-word description of the type of values an argument accepts),
a custom string converter, and, for multi-value arguments, an optional separator for the values.

See the [header file documentation](https://www.ookii.org/Link/CommandLineCppDoc) for more
information.

### Case sensitive argument names

By default, argument names are case insensitive. This means that an argument named "Foo" could
be specified using `-foo`, `-FOO`, or any other variation. Use the `parser_builder::case_sensitive()`
method to make argument names case sensitive instead. When enabled, you can also define arguments
whose names differ only by case (so `-foo` and `-FOO` could refer to different arguments).

## Using the code generation script

You can also create a parser by writing a specially-annotated struct or class, and using the
[provided scripts](Scripts.md) to generate a parser. This script can also be used to generate a
main method for your application which parses arguments.

Next, we'll see how to [parse arguments](ParsingArguments.md).
