# Defining command line arguments

In order to use Ookii.CommandLine, you must use the [`ookii::parser_builder`][] class to create a
command line argument parser. Using the builder, you will specify the names, types and attributes
(required, positional, description, default value, etc.) of each argument.

There are two ways to define arguments: manually using the [`ookii::parser_builder`][] class, or using a
[code generation script](Scripts.md) with a specially-annotated struct or class.

## Using the parser builder

Before you can define an argument, you must provide a place for its value to be stored. This can be
a local or global variable, or a field of a class or struct. The type of this variable or field
will determine the type of the argument.

Then, you must create the [`ookii::parser_builder`][], supplying the executable name of your
application, which will be used when [generating usage help](UsageHelp.md). You can use the
[`command_line_parser::get_executable_name()`][] method to extract just the file name from `argv[0]`,
which is usually the value you want to use for this.

You must then define arguments using the builder, by using the [`parser_builder::add_argument()`][parser_builder::add_argument()_0],
[`parser_builder::add_multi_value_argument()`][parser_builder::add_multi_value_argument()_1] and [`parser_builder::add_action_argument()`][parser_builder::add_action_argument()_1] methods.
Each of these methods take a reference to the storage for the argument, and the name of the
argument. They return an argument builder which allows you to further customize the argument.
The parser builder and argument builder methods all chain, so you can easily set properties on the
parser builder and define multiple arguments in one statement.

The storage locations for the arguments must remain valid as long as the [`command_line_parser`][] is
being used.

An argument created by the builder is by default optional and not positional, has no description,
the default [value description](#value-descriptions) for its type, and no default value.

When you are done defining arguments, call the [`build()`][] method to create an
[`ookii::command_line_parser`][].

```c++
auto name = ookii::command_line_parser::get_executable_name(argc, argv);
int some_argument{};
std::string other_argument{};
auto parser = ookii::parser_builder{name}
    .add_argument(some_argument, "SomeArgument")
    .add_argument(other_argument, "OtherArgument")
    .build();
```

### Long/short mode

To enable [long/short mode](Arguments.md#longshort-mode), you typically want to do three things if
you want to mimic typical POSIX conventions: enable the mode itself, enable case sensitive argument
names, and give all your arguments dash-case names.

When using long/short mode, the string passed to [`add_argument()`][add_argument()_0] and similar methods is the long
name. The argument does not have a short name unless you set one.

To set a short name, call the [`parser_builder::argument_builder_common::short_name()`][parser_builder::argument_builder_common::short_name()_1] method. You
can call this method without parameters to derive the short name from the first letter of the long
name, or you can pass a `char` to set an explicit long name.

You can also define an argument that only has a short name, and no long name, by calling the
overload of the [`add_argument()`][add_argument()_1], [`add_multi_value_argument()`][add_multi_value_argument()_0] and [`add_action_argument()`][add_action_argument()_0] methods
that take a `char` as their second argument.

```c++
std::string file_name;
int foo{};
bool bar{};
auto parser = ookii::parser_builder{name}
    .mode(ookii::parsing_mode::long_short)
    .case_sensitive(true)
    .add_argument(file_name, "file-name").short_name()
    .add_argument(foo, "foo").short_name('F')
    .add_argument(bar, 'b')
    .build();
```

This example defines an argument with the long name `--file-name` and the short name `-f`, taken
from the first character. The second argument has the long name `--foo` and the explicit short name
`-F`, which is distinct from `-f` because case sensitivity is turned on. The third argument has
the short name `-b`, and no long name.

### Required and positional arguments

To create a [required argument](Arguments.md#required-arguments), call the
[`parser_builder::argument_builder_common::required()`][] method.

To create a [positional argument](Arguments.md#positional-arguments), call the
[`parser_builder::argument_builder_common::positional()`][] method. You must add positional arguments in
the order you wish them to have.

```c++
.add_argument(some_argument, "SomeArgument").required().positional();
```

### Switch arguments

If the type of an argument is `bool`, an [`std::optional<bool>`][], or a multi-value argument of either
type, this defines a switch argument.

```c++
bool switch_argument{};
auto parser = ookii::parser_builder{name}
    .add_argument(switch_argument, "Switch")
    .build();
```

An [`std::optional<bool>`][] can be used to distinguish between an omitted switch and an explicit value
of false.

```c++
std::optional<bool> switch_argument{};
auto parser = ookii::parser_builder{name}
    .add_argument(switch_argument, "Switch")
    .build();
```

This variable will be `std::nullopt` if the argument was not supplied, true if the argument was
present or explicitly set to true with `-Switch:true`, and false only if the user supplied
`-Switch:false`.

### Multi-value arguments

To define a [multi-value argument](Arguments.md#arguments-with-multiple-values), you must use the
[`parser_builder::add_multi_value_argument()`][parser_builder::add_multi_value_argument()_1] method.

```c++
std::vector<std::string> multi_value;
auto parser = ookii::parser_builder{name}
    .add_multi_value_argument(multi_value, "MultiValue")
    .build();
```

The type of a multi-value argument must be a container type which has at least the methods
`push_back()` and `clear()`, and the typedef `value_type`. For example, you can use [`std::vector<T>`][]
or [`std::list<T>`][].

A multi-value switch argument can be created with a container of `bool` (e.g. [`std::vector<bool>`][]).

### Default values

You can set a default value for an argument using the [`parser_builder::typed_argument_builder::default_value()`][]
method. Default values are applied only if the argument is not required, and it was not supplied on
the command line.

```c++
int some_argument{};
auto parser = ookii::parser_builder{name}
    .add_argument(some_argument, "SomeArgument").default_value(5)
    .build();
```

If no default value is set for an argument, the [`command_line_parser`][] will not assign any value
to the variable used for the argument. This allows you to initialize the variable to the desired
value, and that value will not be changed.

```c++
int some_argument = 5;
auto parser = ookii::parser_builder{name}
    .add_argument(some_argument, "SomeArgument")
    .build();
```

This has essentially the same effect as setting the default value. However, if this method is used,
the default value cannot be included in the usage help.

If you wish to be able to tell whether an argument was supplied, you should use [`std::optional<T>`][]
as the type of the argument. If the argument was not supplied, the variable will remain empty
([`std::nullopt`][]).

### Argument descriptions

You can add a description to an argument with the
[`parser_builder::argument_builder_common::description()`][] method. These descriptions will be used for
the [usage help](UsageHelp.md).

```c++
std::string path;
auto parser = ookii::parser_builder{name}
    .add_argument(path, "Path").description("Provides the name of a file to read.")
    .build();
```

It's strongly recommended to always add descriptions to all your arguments.

### Value descriptions

The value description is a short, often one-word description of the type of values your argument
accepts. It's shown in the [usage help](UsageHelp.md) after the name of your argument, and defaults
to the name of the argument type (in the case of a multi-value argument, the element type, or for
[`std::optional<T>`][], the underlying type).

This should *not* be used for the description of the argument's purpose; use the
[`parser_builder::argument_builder_common::description()`][] method for that.

To specify a custom value description, use the
[`parser_builder::argument_builder_common::value_description()`][] method.

```c++
int some_argument{};
auto parser = ookii::parser_builder{name}
    .add_argument(some_argument, "SomeArgument").value_description("number")
    .build();
```

### Custom type conversion

If you want to use a non-default conversion from string, you can specify a custom type converter
using the [`parser_builder::typed_argument_builder::converter()`][] method.

```c++
bool switch_argument{};
auto parser = ookii::parser_builder{name}
    .add_argument(switch_argument, "Switch")
        .converter([](auto value, const auto &loc) -> std::optional<bool>
            {
                if (value == "yes" || value == "true")
                {
                    return true;
                }
                else if (value == "no" || value == "false")
                {
                    return false;
                }

                return {};
            })
    .build();
```

The above defines a custom converter for booleans that accepts "yes" and "no" in addition to "true"
and "false".

### Arguments that cancel parsing

You can indicate that argument parsing should stop, and usage help should be printed immediately,
when an argument is supplied by using the
[`parser_builder::argument_builder_common::cancel_parsing()`][] method.

Arguments that use this method cause parsing to stop when the argument is encountered. The rest of
the command line is not processed, and
[`command_line_parser::parse()`][command_line_parser::parse()_0] will return a [`parse_result`][]
with [`parse_error::parsing_cancelled`][]. The
[`command_line_parser::parse()`][command_line_parser::parse()_0] overloads that take a
[`usage_writer`][] will automatically print usage help.

This can be used to implement a custom `-Help` argument, if you don't wish to use the
[default one](#automatic-arguments).

```c++
bool help{};
auto parser = ookii::parser_builder{name}
    .add_argument(help, "Help").cancel_parsing()
    .build();
```

### Aliases

An alias is an alternative name that can be used to specify a command line argument. Aliases can be
added to a command line argument by using the [`parser_builder::argument_builder_common::alias()`][]
method.

For example, the following code defines a switch argument that can be specified using either the
name `-Verbose` or the alias `-v`:

```c++
bool verbose{};
auto parser = ookii::parser_builder{name}
    .add_argument(verbose, "Verbose").alias("v")
    .build();
```

To specify more than one alias for an argument, simply call the
[`parser_builder::argument_builder_common::alias()`][] method multiple times.

When using [long/short mode](Arguments.md#longshort-mode), the
[`parser_builder::argument_builder_common::alias()`][] method specifies long name aliases, and will be
ignored if the argument doesn't have a long name. Use the
[`parser_builder::argument_builder_common::short_alias()`][] method to specify short aliases. These will
be ignored if the argument doesn't have a short name.

### Case sensitive argument names

By default, argument names are case insensitive. This means that an argument named "Foo" could
be specified using `-foo`, `-FOO`, or any other variation. Use the [`parser_builder::case_sensitive()`][]
method to make argument names case sensitive instead. When enabled, you can also define arguments
whose names differ only by case (so `-foo` and `-FOO` could refer to different arguments).

### Action arguments

An action argument is an argument that doesn't set a variable, but instead invokes a callback
function when the argument is supplied. The function is called immediately when the argument is
parsed, without waiting for the entire command line to be finished parsing. To create an action
argument, you call the [`parser_builder::add_action_argument()`][parser_builder::add_action_argument()_1] method.

The callback function must have the following signature:

```c++
bool action(argument_type value, ookii::command_line_parser &parser);
```

The type of the `value` parameter is the type of the argument, and this can use any type that is
valid for a variable used with the [`parser_builder::add_argument()`][parser_builder::add_argument()_0] method. A type of `bool` creates
a switch argument.

Multi-value action arguments are not supported, so the parameter may not have a container type like
[`std::vector<T>`][].

The return value of the callback determines whether parsing will continue. Returning `false` will
cancel parsing. Unlike the [`cancel_parsing()`][cancel_parsing()_1] method, this will *not* automatically display usage
help. If you do want to show help, call the [`command_line_parser::help_requested()`][command_line_parser::help_requested()_1] method
before returning `false`.

```c++
auto action = [](bool value, ookii::command_line_parser &parser)
{
    std::cout << "Some amazingly useful information." << std::endl;
    parser.help_requested(true);
    return false;
};

auto parser = ookii::parser_builder{name}
    .add_action_argument(action, "MoreHelp")
    .build();
```

Action arguments allow all the same options as regular or multi-value arguments, except they cannot
have a default value. The method will never be invoked if the argument is not explicitly specified
by the user.

### Automatic arguments

Besides the arguments you define, the [`parser_builder`][] will, by default, add one automatic arguments
to your application: `-Help`.

The `-Help` argument will cancel parsing, and immediately show usage help without displaying an
error message. It has two aliases, `-?` and `-h`. When using [long/short mode](Arguments.md#longshort-mode),
it has the long name `--Help`, the short name `-?`, and the short alias `-h`.

If you manually define an argument with a name that conflicts with the automatic help argument or
any of its aliases, the automatic help argument will not be added. You can also explicitly disable
the automatic help argument with the [`parser_builder::automatic_help_argument()`][] method.

In addition, you can also add an automatic `-Version` argument by using the
[`parser_builder::add_version_argument()`][] method, which takes as its parameter a function that will
be called to display version information. When supplied, this argument will call the function and
cancel parsing, but will not show help.

The `-Version` argument does not have any aliases or a short name by default.

On Windows only, you can call the [`parser_builder::add_win32_version_argument()`][] method to add a
`-Version` argument that reads information from your executable's `VERSIONINFO` resource, and
displays it on the console. It will print the product name, version, and copyright information if
it's present.

To ensure consistent argument naming, both arguments will adjust their capitalization to match the
first manually defined argument. If this argument does not start with a capital letter, the arguments
will be called `-help` and `-version`.

You can also customize the name, aliases, and descriptions of the automatic arguments using the
[`localized_string_provider`][] class.

## Using the code generation script

You can also create a parser by writing a specially-annotated struct or class, and using one of the
code generation scripts to generate a parser. This script can also be used to generate a `main()`
function for your application which parses arguments. For more information, see the documentation
for the [code generation scripts](Scripts.md).

Next, we'll take a look at how to [parse the arguments we've defined](ParsingArguments.md)

[`build()`]: https://www.ookii.org/docs/commandline-cpp-2.0/classookii_1_1basic__parser__builder.html#af66361855468fde2eb545fbe1631e042
[`command_line_parser::get_executable_name()`]: https://www.ookii.org/docs/commandline-cpp-2.0/classookii_1_1basic__command__line__parser.html#a614b989de00c3c26fe035d6ded845edb
[`command_line_parser`]: https://www.ookii.org/docs/commandline-cpp-2.0/classookii_1_1basic__command__line__parser.html
[`localized_string_provider`]: https://www.ookii.org/docs/commandline-cpp-2.0/classookii_1_1basic__localized__string__provider.html
[`ookii::command_line_parser`]: https://www.ookii.org/docs/commandline-cpp-2.0/classookii_1_1basic__command__line__parser.html
[`ookii::parser_builder`]: https://www.ookii.org/docs/commandline-cpp-2.0/classookii_1_1basic__parser__builder.html
[`parse_error::parsing_cancelled`]: https://www.ookii.org/docs/commandline-cpp-2.0/namespaceookii.html#afae8f4d80dbe46a2344bb46c522982ef
[`parse_result`]: https://www.ookii.org/docs/commandline-cpp-2.0/structookii_1_1parse__result.html
[`parser_builder::add_version_argument()`]: https://www.ookii.org/docs/commandline-cpp-2.0/classookii_1_1basic__parser__builder.html#a9abfabd3ea77bdda6b8c9c53010c6f9d
[`parser_builder::add_win32_version_argument()`]: https://www.ookii.org/docs/commandline-cpp-2.0/classookii_1_1basic__parser__builder.html#a8757faab5a8c2c011cf53a9609538bbb
[`parser_builder::argument_builder_common::alias()`]: https://www.ookii.org/docs/commandline-cpp-2.0/classookii_1_1basic__parser__builder_1_1argument__builder__common.html#a44d77984b1cd12b04764f7f2741269d4
[`parser_builder::argument_builder_common::cancel_parsing()`]: https://www.ookii.org/docs/commandline-cpp-2.0/classookii_1_1basic__parser__builder_1_1argument__builder__common.html#a70953e9876bbede9132754595f76b6b3
[`parser_builder::argument_builder_common::description()`]: https://www.ookii.org/docs/commandline-cpp-2.0/classookii_1_1basic__parser__builder_1_1argument__builder__common.html#a1589adafc67093261b0a73237339593f
[`parser_builder::argument_builder_common::positional()`]: https://www.ookii.org/docs/commandline-cpp-2.0/classookii_1_1basic__parser__builder_1_1argument__builder__common.html#aff6eb66bc5610def9090da9579ef3233
[`parser_builder::argument_builder_common::required()`]: https://www.ookii.org/docs/commandline-cpp-2.0/classookii_1_1basic__parser__builder_1_1argument__builder__common.html#a85060c36d2d231431b3d4bdccdd796d3
[`parser_builder::argument_builder_common::short_alias()`]: https://www.ookii.org/docs/commandline-cpp-2.0/classookii_1_1basic__parser__builder_1_1argument__builder__common.html#a30ecafbbf18070a6bd79581754b4182a
[`parser_builder::argument_builder_common::value_description()`]: https://www.ookii.org/docs/commandline-cpp-2.0/classookii_1_1basic__parser__builder_1_1argument__builder__common.html#acb9d6fe9dea36f365894b98b373ebe73
[`parser_builder::automatic_help_argument()`]: https://www.ookii.org/docs/commandline-cpp-2.0/classookii_1_1basic__parser__builder.html#acf3a2fe08de1bdef7a69cf26cf94ba12
[`parser_builder::case_sensitive()`]: https://www.ookii.org/docs/commandline-cpp-2.0/classookii_1_1basic__parser__builder.html#a47a0c39a17d1dcca8b99455dab68dbb0
[`parser_builder::typed_argument_builder::converter()`]: https://www.ookii.org/docs/commandline-cpp-2.0/classookii_1_1basic__parser__builder_1_1typed__argument__builder.html#a301675f8e8b4811e9581efdbfd9732dd
[`parser_builder::typed_argument_builder::default_value()`]: https://www.ookii.org/docs/commandline-cpp-2.0/classookii_1_1basic__parser__builder_1_1typed__argument__builder.html#ab517dcc6b4ac48d8a46e31156c1736f4
[`parser_builder`]: https://www.ookii.org/docs/commandline-cpp-2.0/classookii_1_1basic__parser__builder.html
[`std::list<T>`]: https://en.cppreference.com/w/cpp/container/list
[`std::nullopt`]: https://en.cppreference.com/w/cpp/utility/optional/nullopt
[`std::optional<bool>`]: https://en.cppreference.com/w/cpp/utility/optional
[`std::optional<T>`]: https://en.cppreference.com/w/cpp/utility/optional
[`std::vector<bool>`]: https://en.cppreference.com/w/cpp/container/vector
[`std::vector<T>`]: https://en.cppreference.com/w/cpp/container/vector
[`usage_writer`]: https://www.ookii.org/docs/commandline-cpp-2.0/classookii_1_1basic__usage__writer.html
[add_action_argument()_0]: https://www.ookii.org/docs/commandline-cpp-2.0/classookii_1_1basic__parser__builder.html#a8a1077883eaa6f0cfbab84232061d98e
[add_argument()_0]: https://www.ookii.org/docs/commandline-cpp-2.0/classookii_1_1basic__parser__builder.html#aa4046a3e0aef810e41501c17572ced3d
[add_argument()_1]: https://www.ookii.org/docs/commandline-cpp-2.0/classookii_1_1basic__parser__builder.html#a16b69672a4dd924cf5a402c419be3d05
[add_multi_value_argument()_0]: https://www.ookii.org/docs/commandline-cpp-2.0/classookii_1_1basic__parser__builder.html#ad3c06f01bab1d95b61daa8e4a048700b
[cancel_parsing()_1]: https://www.ookii.org/docs/commandline-cpp-2.0/classookii_1_1basic__parser__builder_1_1argument__builder__common.html#a70953e9876bbede9132754595f76b6b3
[command_line_parser::help_requested()_1]: https://www.ookii.org/docs/commandline-cpp-2.0/classookii_1_1basic__command__line__parser.html#af9b9817ad33398563e583e1a1a0ad630
[command_line_parser::parse()_0]: https://www.ookii.org/docs/commandline-cpp-2.0/classookii_1_1basic__command__line__parser.html#ae0c7b9990c29f41343182ac3d9918af7
[parser_builder::add_action_argument()_1]: https://www.ookii.org/docs/commandline-cpp-2.0/classookii_1_1basic__parser__builder.html#a979611d8fe9c5ac50ff300243355d662
[parser_builder::add_argument()_0]: https://www.ookii.org/docs/commandline-cpp-2.0/classookii_1_1basic__parser__builder.html#aa4046a3e0aef810e41501c17572ced3d
[parser_builder::add_multi_value_argument()_1]: https://www.ookii.org/docs/commandline-cpp-2.0/classookii_1_1basic__parser__builder.html#a35953740728918a4d3daa0f69d052e16
[parser_builder::argument_builder_common::short_name()_1]: https://www.ookii.org/docs/commandline-cpp-2.0/classookii_1_1basic__parser__builder_1_1argument__builder__common.html#acf0df83ada201975cc845ac63725904f
