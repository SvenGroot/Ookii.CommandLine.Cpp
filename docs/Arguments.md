# Command line arguments

If you've gone through the [tutorial](Tutorial.md), you'll already have some idea of how
Ookii.CommandLine parses arguments. This page will explain the rules in detail, including all the
possible kinds or arguments.

Command line arguments are passed to your application when it is started, and are usually accessed
through the parameters of the `int main(int argc, char *argv[])` method (or `int wmain(int argc, wchar_t *argv[])`
or [`CommandLineToArgvW`][] for Windows applications using Unicode). This provides the arguments as an
array of strings, which Ookii.CommandLine will parse to extract strongly-typed, named values that
you can easily access in your application.

The method used to extract values from the array of string arguments is determined by the command
line argument parsing rules. Ookii.CommandLine supports two sets of parsing rules: the default mode,
which uses parsing rules similar to those used by PowerShell, and [long/short mode](#longshort-mode),
which is more POSIX-like, and lets arguments have a long name and a short name, with different
prefixes. Most of the below information applies to both modes, with the differences described at the
end.

## Named arguments

In Ookii.CommandLine, all command line arguments have a name, and can be assigned a value on the
command line using that name. They follow the name of your application's executable on the command
prompt, and typically take the following form:

```text
-ArgumentName ArgumentValue
```

The argument name is preceded by the _argument name prefix_. This prefix is configurable, but
Ookii.CommandLine defaults to accepting a dash (`-`) and a forward slash (`/`) on Windows, and only
a dash (`-`) on other platforms such as Linux or MacOS.

Argument names are case insensitive by default, though this can be customized using the
[`parser_builder::case_sensitive()`][] method.

The argument's value follow the name, separated by either white space (as a separate argument token),
or by the argument name/value separator, which is a colon (`:`) by default. The following is
identical to the previous example:

```text
-ArgumentName:value
```

Whether white-space is allowed to separate the name and value is configured using the
[`parser_builder::allow_whitespace_separator()`][] method, and the argument name/value separator can be
customized using the [`parser_builder::argument_value_separator()`][] method.

Not all arguments require values; those that do not are called [_switch arguments_](#switch-arguments)
and have a value determined by their presence or absence on the command line.

An argument can have one or more aliases: alternative names that can also be used to supply the same
argument. For example, an argument named `-Verbose` might use the alias `-v` as a shorter to type
alternative.

## Positional arguments

An argument can be _positional_, which means in addition to being supplied by name, it can also be
supplied without the name, using the position of the value. Which argument the value belongs to
is determined by its position relative to other positional arguments.

If an argument value is encountered without being preceded by a name, it is matched to the
next positional argument without a value. For example, take the following command line arguments:

```text
value1 –ArgumentName value2 value3
```

In this case, value1 is not preceded by a name; therefore, it is matched to the first positional
argument. Value2 follows a name, so it is matched to the argument with the name `-ArgumentName`.
Finally, value3 is matched to the second positional argument.

A positional argument can still be supplied by name. If a positional argument is supplied by name,
it cannot also be specified by position; in the previous example, if the argument named
`-ArgumentName` was the second positional argument, then value3 becomes the value for the third
positional argument, because the value for `-ArgumentName` was already specified by name. If
`-ArgumentName` is the first positional argument, this would cause an error (unless duplicate
arguments are allowed in the options), because it already had a value set by `value1`.

## Required arguments

A command line argument that is required must be supplied on all invocations of the application. If
a required argument is not supplied, this is considered an error and parsing will fail.

Any argument can be made required. Usually, it is recommended for any required argument to also be a
positional argument, but this is not mandatory.

## Switch arguments

A switch argument, sometimes also called a flag, is an argument with a Boolean type (`bool`). Its
value is determined by its presence or absence on the command line; the value will be true if the
argument is supplied, and false if not. The following sets the switch argument named “Switch” to
true:

```text
-Switch
```

A switch argument’s value can be specified explicitly, as in the following example:

```text
-Switch:false
```

You must use the name/value separator (a colon by default) to specify an explicit value for a switch
argument; you cannot use white space. If the command line contains `-Switch false`, then `false` is
the value of the next positional argument, not the value for `-Switch`.

If you use [`std::optional<bool>`][] as the type of the argument, it will be [`std::nullopt`][] if not
supplied, `true` if supplied, and `false` only if explicitly set to false using `-Switch:false`.

## Arguments with multiple values

Some arguments can take multiple values; these are _multi-value arguments_. These arguments can be
supplied multiple times, and each value is added to the set of values. For example, consider the
following command line arguments:

```text
-ArgumentName value1 –ArgumentName value2 –ArgumentName value3
```

In this case, if `-ArgumentName` is a multi-value argument, the value of the argument will be a list
holding all three values.

It’s possible to specify a separator for multi-value arguments using the
[`parser_builder::multi_value_argument_builder::separator()`][] method. This makes it possible to
specify multiple values for the argument while the argument itself is specified only once. For
example, if the separator is set to a comma, you can specify the values as follows:

```text
-ArgumentName value1,value2,value3
```

In this case, the value of the argument named `-ArgumentName` will be a list with the three values
"value1", "value2" and "value3".

**Note:** if you specify a separator for a multi-value argument, it is _not_ possible to have an
argument value containing the separator. There is no way to escape the separator. Therefore, make
sure you pick a separator that will never be used in the argument values, and be extra careful with
culture-sensitive argument types.

If a multi-value argument is positional, it must be the last positional argument. All remaining
positional argument values will be considered values for the multi-value argument.

If a multi-value argument is required, it means it must have at least one value.

If an argument is not a multi-value argument, it is an error to supply it more than once, unless
duplicate arguments were set to allowed using the [`parser_builder::allow_duplicate_arguments()`][]
method, in which case only the last value is used.

If the type of the argument is a container of Boolean values (e.g. [`std::vector<bool>`][]), it will
act as a multi-value argument and a switch. A value of true (or the explicit value if one is given)
gets added to the list for every time that the argument is supplied.

## Argument value conversion

Ookii.CommandLine allows you to define arguments with any type; the only requirement is that it is
possible to convert that type to and from a string.

String conversion is performed using the [`ookii::lexical_convert`][] template. The default
implementation uses stream extraction ([`operator>>`][] on an [`std::istringstream`][]), so if such an
operator is defined for your type, this is sufficient. Note that the operator must consume the
entire contents of the stream, and leave the stream without [`badbit`][] or [`failbit`][] set, for
conversion to be considered successful.

You can also provide a template specialization of [`ookii::lexical_convert`][] to perform conversion
without depending on streams. This template struct has a single method, [`from_string()`][], which
you must implement. The method returns an [`std::optional<T>`][], and you should return [`std::nullopt`][]
if conversion failed.

```c++
template<>
struct ookii::lexical_convert<your_type, char>
{
    static std::optional<your_type> from_string(std::string_view value, const std::locale &loc)
    {
        // Implement string conversion here.
    }
};
```

> If you are using Unicode arguments on Windows, use `wchar_t` and [`std::wstring_view`][].

It is possible to override the default conversion for a type by specifying a custom conversion
function using the [`parser_builder::typed_argument_builder::converter()`][] method. Note that even
if you supply a custom converter, a default one must still exist otherwise your code will not
compile. The custom converter is intended for situations where a default conversion exists, but you
wish to deviate from that behavior.

In order to display default values, it must also be possible to output the type using stream
insertion ([`operator<<`][]). This is always necessary, even if you don't use a default value for
arguments of that type.

An example of writing implementations of [`operator<<`][] and [`operator>>`][] for a custom type, as well as
an example of directly specializing [`ookii::lexical_convert`][], can be seen in the
[custom_types.h](../unittests/custom_types.h) file used by the unit tests.

If the argument is a multi-value argument, string conversion must be available for the type indicated
by the container's `value_type`. Usually, this is the type of element in the container (e.g. for
[`std::vector<int>`][] it would be `int`).

If the argument uses the type [`std::optional<T>`][], string conversion must be available for the
contained type `T`.

### Conversion locale

For many types, the conversion can be locale dependent. For example, converting numbers or dates
depends on the [`std::locale`][] which defines the accepted formats and how they’re interpreted; some
locales might use a period as the decimal separators, while others use a comma.

The locale used for argument value conversions is specified by the [`ookii::parser_builder::locale()`][]
method. If not specified, it defaults to the global locale set by [`std::locale::global()`][], which
itself defaults to the invariant "C" locale.

For a consistent parsing experience, it's strongly recommended to always use an invariant locale
for command line parsing. If you use the locale based on the user's current culture, the same
command line may not be parsed the same for users with different regional settings (for example,
if you use floating point numbers).

## Long/short mode

POSIX and GNU conventions specify that options use a dash (`-`) followed by a single character, and
define the concept of long options, which use `--` followed by an a multi-character name. This style
is used by many tools like `cmake`, `git`, and many others, and may be preferred if you are writing
a cross-platform application.

Ookii.CommandLine calls this style of parsing "long/short mode," and offers it as an alternative
mode to augment the default parsing rules. In this mode, an argument can have the regular long name
and an additional single-character short name, each with its own argument name prefix. By default,
the prefix `--` is used for long names, and `-` (and `/` on Windows) for short names.

This mode can be enabled by passing [`parsing_mode::long_short`][] to the [`parser_builder::mode()`][]
method.

POSIX conventions also specify the use of lower case argument names, with dashes separating words
("dash-case"). If you are using the [code generation scripts](Scripts.md), you can easily achieve
that using name transformation. It's also common to use case-sensitive argument names in this mode,
which can be enabled with the [`parser_builder::case_sensitive()`][] method.

For example, an argument named `--path` could have a short name `-p`. It could then be supplied
using either name:

```text
--path value
```

Or:

```text
-p value
```

Note that you must use the correct prefix: using `-path` or `--p` will not work.

An argument can have either a short name or a long name, or both.

Arguments in this mode can still have aliases. You can set separate long and short aliases, which
follow the same rules as the long and short names.

For switch arguments with short names, the switches can be combined in a single argument. For
example, given the switches `-a`, `-b` and `-c`, the following command line sets all three switches:

```text
-abc
```

This is equivalent to:

```text
-a -b -c
```

This only works for switch arguments, and does not apply to long names.

Besides these differences, long/short mode follows the same rules and conventions as the default
mode outlined above, with all the same options.

## More information

Next, let's take a look at how to [define arguments](DefiningArguments.md).

[`badbit`]: https://en.cppreference.com/w/cpp/io/ios_base/iostate
[`failbit`]: https://en.cppreference.com/w/cpp/io/ios_base/iostate
[`from_string()`]: https://www.ookii.org/docs/commandline-cpp-2.0/structookii_1_1lexical__convert.html#ac8ef25e2344f404f823e3f6d1046d010
[`ookii::lexical_convert`]: https://www.ookii.org/docs/commandline-cpp-2.0/structookii_1_1lexical__convert.html
[`ookii::parser_builder::locale()`]: https://www.ookii.org/docs/commandline-cpp-2.0/classookii_1_1basic__parser__builder.html#a4f1626f8fdf059f4bfde502bcb78befc
[`operator<<`]: https://en.cppreference.com/w/cpp/io/basic_ostream/operator_ltlt2
[`operator>>`]: https://en.cppreference.com/w/cpp/io/basic_istream/operator_gtgt
[`parser_builder::allow_duplicate_arguments()`]: https://www.ookii.org/docs/commandline-cpp-2.0/classookii_1_1basic__parser__builder.html#a25c0ba446cb1b44cd95b0824c9009215
[`parser_builder::allow_whitespace_separator()`]: https://www.ookii.org/docs/commandline-cpp-2.0/classookii_1_1basic__parser__builder.html#a1fe1337efd0e72b95ab7038894dc43de
[`parser_builder::argument_value_separator()`]: https://www.ookii.org/docs/commandline-cpp-2.0/classookii_1_1basic__parser__builder.html#a43c366262f712b87a9a95473d6603e18
[`parser_builder::case_sensitive()`]: https://www.ookii.org/docs/commandline-cpp-2.0/classookii_1_1basic__parser__builder.html#a47a0c39a17d1dcca8b99455dab68dbb0
[`parser_builder::mode()`]: https://www.ookii.org/docs/commandline-cpp-2.0/classookii_1_1basic__parser__builder.html#a160689dea7f096c4e644634c48bae752
[`parser_builder::multi_value_argument_builder::separator()`]: https://www.ookii.org/docs/commandline-cpp-2.0/classookii_1_1basic__parser__builder_1_1multi__value__argument__builder.html#a26dc9cd6fb572fee640e205ca312c478
[`parser_builder::typed_argument_builder::converter()`]: https://www.ookii.org/docs/commandline-cpp-2.0/classookii_1_1basic__parser__builder_1_1typed__argument__builder.html#a301675f8e8b4811e9581efdbfd9732dd
[`parsing_mode::long_short`]: https://www.ookii.org/docs/commandline-cpp-2.0/namespaceookii.html#abf0aa62e29f4953f7cba303a3da407fd
[`std::istringstream`]: https://en.cppreference.com/w/cpp/io/basic_istringstream
[`std::locale::global()`]: https://en.cppreference.com/w/cpp/locale/locale/global
[`std::locale`]: https://en.cppreference.com/w/cpp/locale/locale
[`std::nullopt`]: https://en.cppreference.com/w/cpp/utility/optional/nullopt
[`std::optional<bool>`]: https://en.cppreference.com/w/cpp/utility/optional
[`std::optional<T>`]: https://en.cppreference.com/w/cpp/utility/optional
[`std::vector<bool>`]: https://en.cppreference.com/w/cpp/container/vector
[`std::vector<int>`]: https://en.cppreference.com/w/cpp/container/vector
[`std::wstring_view`]: https://en.cppreference.com/w/cpp/string/basic_string_view
[`CommandLineToArgvW`]: https://learn.microsoft.com/windows/win32/api/shellapi/nf-shellapi-commandlinetoargvw
