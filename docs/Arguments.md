# Command line arguments

Command line arguments are passed to your application when it is started, and are typically accessed
through the parameter of the `int main(int argc, char *argv)` method (or `wmain` in Windows
applications using Unicode). This provides the arguments as an array of strings, which is not
terribly useful by itself. What Ookii.CommandLine allows you to do is to convert that array of
strings into a strongly typed set of named values, which are stored in the variables you specify.

The method used to extract values from the array of string arguments is determined by the command
line argument parsing rules. Ookii.CommandLine for C++ uses the same parsing rules as [Ookii.CommandLine for .Net](https://github.com/SvenGroot/ookii.commandline),
which are very similar to how Microsoft PowerShell parses arguments for cmdlets, so if you have used
PowerShell these rules will be familiar to you.

Command line arguments follow the name of your application on the console, and typically take
the following form:

```text
-ArgumentName ArgumentValue
```

The argument name is preceded by the _argument name prefix_. This prefix is configurable, but
Ookii.CommandLine defaults to accepting a dash (`-`) and a forward slash (`/`) on Windows, and only
a dash (`-`) on other platforms such as Linux or MacOS.

The argument value follows the name, separated either by a space or a colon (`:`). You can configure
how argument names and values can be separated by using the `ookii::parser_builder::allow_white_space_separator()`
and the `ookii::parser_builder::argument_value_separator()` methods.

Not all arguments require values; those that do not are called [_switch arguments_](#switch-arguments)
and have a value determined by their presence or absence on the command line.

An argument can have one or more aliases: alternative names that can also be used to specify the
argument. For example, an argument named “Verbose” might use the alias “v” as a shorter to type
alternative.

## Positional arguments

An argument can be _positional_, which means that its value can be specified either by name as
indicated above, or by its position on the command line. In this case the name of the argument is
not required, and the argument’s value can be supplied by specifying it in the correct position in
relation to other positional arguments.

If an argument value is encountered without being preceded by a name, it is matched to the
positional argument at the current position. For example, take the following command line arguments:

```text
Value1 –ArgumentName Value2 Value3
```

In this case, Value1 is not preceded by a name; therefore, it is matched to the first positional
argument. Value2 follows a name, so it is matched to the argument with the name “ArgumentName”.
Finally, Value3 is matched to the second positional argument.

A positional argument can still be supplied by explicitly supplying its name. If a positional
argument is supplied by name, it cannot also be specified by position; in the previous example,
if the argument named “ArgumentName” was the second positional argument, then Value3 becomes the
value for the third positional argument, because the value for “ArgumentName” was already specified
by name.

## Required arguments

A command line argument that is required must be present on all invocations of the application.
If a required argument is not present, the `ookii::command_line_parser` class will return an
error during parsing.

Any argument can be made required. Usually, it is recommended for any required argument to also be a
positional argument, but this is not mandatory.

## Switch arguments

A switch argument is an argument with a Boolean type (`bool`). Its value is determined by its
presence or absence on the command line; the value will be true if the argument is supplied, and
false if not. The following sets the switch argument named “Switch” to true:

```text
-Switch
```

A switch argument’s value can be specified explicitly, as in the following example:

```text
-Switch:true
```

You must use a colon (or your custom name-value separator if configured) to specify an explicit
value for a switch argument; you cannot use white space to separate the name and the value.

If you use `std::optional<bool>` as the type of the argument, it will be `std::nullopt` if omitted,
true if supplied, and false only if explicitly set to false using `-Switch:false`.

## Arguments with multiple values

Some arguments can take multiple values; these are _multi-value arguments_. These arguments can be
supplied multiple times, and each value is added to the set of values. For example, consider the
following command line arguments:

```text
-ArgumentName Value1 –ArgumentName Value2 –ArgumentName Value3
```

In this case, if "ArgumentName" is a multi-value argument, the value of the argument named will be a
list holding all three values.

It’s possible to specify a separator for multi-value arguments using the `ookii::parser_builder::argument_builder::multi_value_separator()`
method. This makes it possible to specify multiple values for the argument while the argument itself
is specified only once. For example, if the separator is set to a comma, you can specify the values
as follows:

```text
-ArgumentName Value1,Value2,Value3
```

In this case, the value of the argument named “ArgumentName” will be a list with the three values
“Value1”, “Value2” and “Value3”.

**Note:** if you specify a separator for a multi-value argument, it is _not_ possible to have an
argument value containing the separator. There is no way to escape the separator. Therefore, make
sure you pick a separator that will never be used in the argument values, and be extra careful with
culture-sensitive argument types (for example, if you use a comma as the separator for a multi-value
argument of floating point numbers, locales that use a comma as the decimal separator will not be
able to specify values properly).

If an argument is not a multi-value argument, it is an error to supply it more than once, unless
duplicate arguments were set to allowed using `ookii::parser_builder::allow_duplicate_arguments()`,
in which case only the last value is used.

If a multi-value argument is positional, it must be the last positional argument. All remaining
positional argument values will be considered values for the multi-value argument.

If a multi-value argument is required, it means it must have at least one value.

If the type of the argument is an container of Boolean values (e.g. `std::vector<bool>`), it will
act as a multi-value argument and a switch. A value of true (or the explicit value if one is given)
gets added to the list for every time that the argument is supplied.

## Argument value conversion

Ookii.CommandLine allows you to define arguments with any type; the only requirement is that it is
possible to convert that type to and from a string.

String conversion is performed using the `ookii::lexical_cast` template. The default implementation
uses stream extraction (`operator>>` on an `std::istringstream`), so if such an operator is defined
for your type, this is sufficient. Note that the operator must consume the entire contents of the
stream for conversion to be considered successful.

You can also provide a template specialization of `ookii::lexical_cast` to perform conversion
without depending on streams.

It is possible to override the default conversion for a type by specifying a custom conversion
function using the `ookii::parser_builder::argument_builder::converter()` function. Note that even
if you supply a custom converter, a default one must still exist otherwise your code will not
compile. The custom converter is intended for situations where a default conversion exists, but you
wish to deviate from that behavior.

In order to display default values, it must also be possible to convert the type back to a string
using the `<format>` library (or libfmt if your compiler doesn't provide the `<format>` header).
In this case it's necessary to provide a specialization of `std::formatter` (or `fmt::formatter` if
using libfmt) for the type. An example of this can be seen in the [custom_types.h](../unittests/custom_types.h)
file used by the unit tests.

If the argument is a multi-value argument, string conversion must be available for the type indicated
by the container's `value_type`. Usually, this is the type of element in the container (e.g. for
`std::vector<int>` it would be `int`).

If the argument uses the type `std::optional<T>`, string conversion must be available for the type T
contained in the `std::optional<T>`.

For many types, the conversion can be locale dependent. For example, converting numbers or dates
depends on the locale which defines the accepted formats and how they’re interpreted; some locales
might use a period as the decimal separators, while others use a comma.

The locale used for argument value conversions is specified by the `ookii::parser_builder::locale()`
method. If not specified, it defaults to the global locale set by `std::locale::global()`, which
itself defaults to the invariant "C" locale.

- [Defining Command Line Arguments](DefiningArguments.md)
- [Parsing Command Line Arguments](ParsingArguments.md)
- [Generating Usage Help](UsageHelp.md)
