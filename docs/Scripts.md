# Code-generation scripts

Although the [`parser_builder`][] class provides a simple interface for defining command line
arguments, it's still less convenient than the way [Ookii.CommandLine for .Net](https://github.com/SvenGroot/ookii.commandline)
works. To provide an easier method, which is also more similar to the .Net version,
Ookii.CommandLine for C++ comes with two scripts, one to generate a stand-alone parser, and one to
generate [subcommands](Subcommands.md).

These two scripts require [PowerShell 6](https://github.com/powershell/powershell)
or later, which is available for Windows and other platforms such as Linux. Note, even on Windows,
the new cross-platform PowerShell is required; the scripts use features not available in Windows
PowerShell. The scripts were tested with PowerShell 7.3.

These scripts generate code that use the [`parser_builder`][] and [`command_manager`][] class to
define arguments and subcommands. They do not offer any features that manually writing code doesn't;
they're just a convenient alternative.

This article first describes the attributes used to annotate your code, before explaining how each
script must be used.

For information on how to include code generation as part of your build process,
[see here](#including-the-scripts-in-your-build-process).

## Argument attributes

To generate an argument parser, you must create a struct or class that contains your arguments.
This struct or class, and its fields, must use special annotations to indicate the arguments and
their options (like required, positional, etc.).

These annotations take the form of a single-line comment with an attribute in square brackets. For
example:

```c++
// [arguments]
struct args
{
    // ...
};
```

The `[arguments]` attribute here indicates that the following struct or class contains arguments. The
fields with arguments are also annotated in the same way.

Attributes can have values; for example, the `[arguments]` attribute can specify the name of the
application's executable. To specify a value, follow the attribute by a colon, and then the value;
for example, `[arguments: name]`. If a colon is present, the entire remainder of the annotation is
considered the value.

If an argument doesn't have a value, you can combine multiple on a single line by separating them
with a comma. For example, `[argument, positional]`. This is not possible if the attribute has a
value, unless it's the last attribute. For example, `[positional, default: 5]` is valid, but
`[default: 5, positional]` is _not_, because everything after the colon is part of the value for
`[default]`.

If there are any lines of comments after the annotations, these specify the description for that
item. They will be concatenated into a single line, though if you have a blank line this will be
preserved in the output (avoid using blank lines in argument descriptions, as they do not get
indented correctly with the default usage help format).

For example, the following creates an argument named "ArgumentName" whose value will be stored in
the `arg` field. The argument is required, positional, and has a description:

```c++
// [argument: ArgumentName]
// [required, positional]
// This is the description.
std::string arg;
```

The code generation scripts also support the use of [action arguments](DefiningArguments.md#action-arguments).
To create an action argument, add the `[argument]` attribute (and any other attributes, except for
`[default]`) to a method declaration. This method should have one of the following signatures:

```c++
/// [argument]
static bool action_arg1(argument_type value, ookii::command_line_parser &parser);

/// [argument]
bool action_arg2(argument_type value, ookii::command_line_parser &parser);
```

A static method will be invoked directly. For an instance method, the script will generate a lambda
capturing the `this` pointer to invoke the method.

### Attributes for arguments types

The following attributes can be applied to the struct or class that contains arguments.

Attribute                     | Description                                                                                                                                                                                                                                                                                                                                                  | Value
------------------------------|--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------|-----------------------------------------------------------------------------------------------------------------------
**arguments**                 | Indicates the following struct or class defines arguments (`New-Parser.ps1` only) .                                                                                                                                                                                                                                                                          | **(optional)** The application executable name; if omitted, defaults to the file name portion of `argv[0]`.
**command**                   | Indicates the following class is a subcommand (`New-Subcommand.ps1` only).                                                                                                                                                                                                                                                                                   | **(optional)** The command name; if omitted, defaults to the static `name()` method or type name.
**global**                    | Specifies options that apply to all commands, or the [`command_manager`][] itself (`New-Subcommand.ps1` only)                                                                                                                                                                                                                                                | **(none)**
**allow_duplicate_arguments** | Indicates that repeating an argument more than once is not an error, but the last value supplied will be used.                                                                                                                                                                                                                                               | **(none)**
**argument_value_separator**  | Specifies the separator to use between argument names and values, instead of the default ':'.                                                                                                                                                                                                                                                                | The separator to use. May only be a single character.
**case_sensitive**            | Indicates argument names should be treated as case sensitive.                                                                                                                                                                                                                                                                                                | **(none)**
**common_help_argument**      | Specifies the name of a help argument that is common to all subcommands, so that an instruction on how to get help for a command can be added to the usage help (`New-Subcommand.ps1`, in a `[global]` block, only).                                                                                                                                         | The name, including prefix, of the help argument (e.g. `-Help`).
**long_prefix**               | Specifies the prefix to use for long argument names in [long/short mode](Arguments.md#longshort-mode), instead of the default `--`.                                                                                                                                                                                                                          | The long argument prefix.
**mode**                      | Specifies the [parsing mode](Arguments.md#longshort-mode) to use.                                                                                                                                                                                                                                                                                            | Either `default` or `long_short`.
**name_transform**            | Specifies the [name transformation](#name-transformation) to use when generating argument names from member names. If present, this overrides the `-NameTransform` script argument.                                                                                                                                                                          | One of the following values: `none` (the default), `PascalCase`, `camelCase`, `snake_case`, `dash-case`, or `trim`.
**no_auto_help**              | Do not create the [automatic `-Help` argument](DefiningArguments.md#automatic-arguments)                                                                                                                                                                                                                                                                     | **(none)**
**no_register**               | Indicates the subcommand should not be registered with the [`command_manager`][]. Use this for classes you intend to use as a common base class for other subcommands (`New-Subcommand.ps1` only).                                                                                                                                                           | **(none)**
**no_whitespace_separator**   | Indicates that argument names and values cannot be separated by whitespace                                                                                                                                                                                                                                                                                   | **(none)**
**prefixes**                  | Specifies the argument name prefixes to use instead of the defaults. These are the short prefixes if using [long/short mode](Arguments.md#longshort-mode)                                                                                                                                                                                                    | A comma-separated list of argument name prefixes.
**show_usage_on_error**       | Indicates how usage help should be printed if a parsing error occurs.                                                                                                                                                                                                                                                                                        | One of the following values: `full` (the default), `syntax_only`, or `none`.
**version_info**              | Creates a `-Version` argument, or a `version` command (if used in a `[global]` block).                                                                                                                                                                                                                                                                       | The string to display when the argument or command is used. For multiple lines, repeat the `[version_info]` attribute.
**win32_version**             | Creates a `-Version` argument, or a `version` command (if used in a `[global]` block), which shows version information extracted from the executable's `VERSIONINFO` resource. If used in combination with `[version_info]`, an `#ifdef` directive is generated so that the `[win32_version]` is used for Windows, and `[version_info]` for other platforms. | **(none)**

### Attributes for arguments

The following attributes can be applied to the fields that define arguments.

Attribute             | Description                                                                                                                                                                                                                               | Value
----------------------|-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------|------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
**argument**          | Indicates the following member is a command line argument. This must always be the first attribute on an argument field.                                                                                                                  | **(optional)** The name of the argument; if omitted, it defaults to the member name, possibly modified according to the name transformation in use. If using [long/short mode](Arguments.md#longshort-mode), this is the long name.
**alias**             | Specifies the [aliases](DefiningArguments.md#aliases) of an argument. If using  [long/short mode](Arguments.md#longshort-mode), these are the long aliases, and are ignored if the argument has no long name.                             | A comma-separated list of aliases.
**cancel_parsing**    | Indicates the argument, when supplied, will [cancel parsing](DefiningArguments.md#arguments-that-cancel-parsing).                                                                                                                         | **(none)**
**default**           | Specifies the [default value](DefiningArguments.md#default-values) of an argument.                                                                                                                                                        | The default value. This is copied into the generated code verbatim, so it must be a valid C++ expression or literal; if the value is a string, this must have quotes.
**multi_value**       | Indicates the argument is a [multi-value argument](Arguments.md#arguments-with-multiple-values). Requires the argument uses a suitable type (e.g. [`std::vector<T>`][])                                                                   | **(none)**
**no_long_name**      | Indicates the argument has no long name, but only a short name. If not using  [long/short mode](Arguments.md#longshort-mode), this will create an argument using the short name as its name. Requires that the argument has a short name. | **(none)**
**positional**        | Indicates the argument is [positional](Arguments.md#positional-arguments). The position depends on the order the positional arguments are defined in.                                                                                     | **(none)**
**required**          | Indicates the argument is [required](Arguments.md#required-arguments).                                                                                                                                                                    | **(none)**
**short_alias**       | Specifies the [short aliases](DefiningArguments.md#aliases) of an argument. Ignored if [long/short mode](Arguments.md#longshort-mode) is not used or the argument has no short name.                                                      | A comma-separated list of single-character short aliases.
**short_name**        | Specifies the short name of an argument. Ignored if [long/short mode](Arguments.md#longshort-mode) is not used.                                                                                                                           | **(optional)** The single-character short name; if omitted, it defaults to the first character of the long name.
**value_description** | Specifies the [value description](UsageHelp.md#value-descriptions) of the argument.                                                                                                                                                       | The value description.

### Name transformation

You can use the `-NameTransform` argument to either script, or the `[name_transform]` attribute, to
indicate how argument names should be derived from the member names of the struct or class, if you
didn't specify an explicit name.

Value          | Description                                                                                                                                                                                                                                                  | Example
---------------|--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------|----------------------------------------------------------------
**None**       | Field names are used as-is, without changing them. This is the default.                                                                                                                                                                                      |
**PascalCase** | Field names are transformed to PascalCase. This removes all underscores, and the first character and every character after an underscore is changed to uppercase. The case of other characters is not changed.                                               | `some_name`, `_some_name`, `_someName` => SomeName
**CamelCase**  | Field names are transformed to camelCase. Similar to PascalCase, but the first character will not be uppercase.                                                                                                                                              | `some_name`, `_some_name`, `_SomeName`=> someName
**SnakeCase**  | Field names are transformed to snake_case. This removes leading and trailing underscores, changes all characters to lower-case, and reduces consecutive underscores to a single underscore. An underscore is inserted before previously capitalized letters. | `some_name`, `_some__name`, `_someName` => some_name
**DashCase**   | Field names are transformed to dash-case. Similar to SnakeCase, but uses a dash instead of an underscore.                                                                                                                                                    | `some_name`, `_some__name`, `_someName` => some-name
**Trim**       | Removes leading and trailing underscores, but leaves the rest of the name alone.                                                                                                                                                                             | `some_name`, `_some_name` => some_name; `_someName` => someName

This is convenient if your naming convention for fields in C++ is different than the convention you
wish to use for argument names, and you don't want to specify every argument name manually. Keep in
mind that argument names are case insensitive by default, so the capitalization of the names only
affects how they are displayed in the usage help; they can still be specified using any case, unless
you use `[case_sensitive]` on the arguments struct.

If you do specify an argument name manually using `[argument: name]`, these are not affected by name
transformations.

### Example

Here's an example of a struct that defines a number of arguments, using the annotations:

```c++
// [arguments]
// [name_transform: PascalCase]
// [version_info: Ookii.CommandLine Sample 2.0]
// [show_usage_on_error: syntax_only]
// Sample command line application. The application parses the command line and prints the results,
// but otherwise does nothing and none of the arguments are actually used for anything.
struct arguments
{
    // [argument, required, positional]
    // The source data.
    std::string source;

    // [argument, required, positional]
    // The destination data.
    std::string destination;

    // [argument, positional]
    // [default: 1]
    // The operation's index.
    int operation_index;

    // [argument]
    // [value_description: number]
    // Provides the count for something to the application.
    std::optional<float> count;

    // [argument]
    // [alias: v]
    // Print verbose information; this is an example of a switch argument.
    bool verbose;

    // [argument: Value]
    // [multi_value]
    // This is an example of a multi-value argument, which can be repeated multiple times to set
    // more than one value.
    std::vector<std::string> values;

    OOKII_GENERATED_METHODS(arguments);
};
```

These are the arguments used by the [generated parser sample](../samples/generated_parser).

## New-Parser.ps1

To generate an argument parser, you use the [`scripts/New-Parser.ps1`](../scripts/New-Parser.ps1)
script. This script takes as input a C++ header file that contains a struct or class that defines
the arguments. The struct or class and its fields must be annotated as described above.

In addition, the struct must declare a static `create_builder()` method with the following signature:

```c++
ookii::parser_builder create_builder(std::string command_name, 
    ookii::string_provider *string_provider = nullptr, const std::locale &locale = {})
```

If using the `-WideChar` script argument, this definition should look like this instead:

```c++
ookii::wparser_builder create_builder(std::wstring command_name, 
    ookii::wstring_provider *string_provider = nullptr, const std::locale &locale = {})
```

To make it easy to declare the method, you can include `<ookii/command_line_generated.h>` and use
the following macro:

```c++
OOKII_GENERATED_METHODS(type);
```

The [`OOKII_GENERATED_METHODS`][] macro will declare the `create_builder()` method, and will also
add a `parse()` method for added convenience.

The script's output will be a C++ source file that contains a definition for the `create_builder()`
method. Add this generated file to your compiler inputs to use the generated method.

Most of the time, you'll want to use the `parse()` method added by the macro, which will call the
`create_builder()` method to create a [`command_line_parser`][] using the information extracted from the
annotation comments. It will then parse the arguments. If successful, it returns an instance of the
arguments struct or class. If an error occurred, it will print the error message and usage help
according to the supplied [`usage_writer`][] (if any), and return [`std::nullopt`][].

```c++
int main(int argc, char *argv[])
{
    std::optional<arguments> args = arguments::parse(argc, argv);
    if (!args)
    {
        // Parse error, already handled.
        return 1;
    }

    // Run your application.
    do_something(*args);
    return 0;
}
```

You can also use the `create_builder()` method directly. Since this method returns the builder, it
allows you to further customize the arguments and options before calling [`parser_builder::build()`][].
Doing this also allows you to do things like custom error handling while still using code generation.

### Script arguments for `New-Parser.ps1`

`-OutputPath` indicates the path of the generated C++ file.

Use the script's `-NameTransform` argument to indicate that argument names derived from field names
should be changed according to the supplied [name transformation](#name-transformation).
me alone. This will be ignored if the script uses the `[name_transform]` attribute.

By using the script's `-EntryPoint` argument, you can specify a function name that should serve as
the entry point for your application. The script will then generate a `main()` function for you,
which parses the arguments and invokes the specified function, passing it the arguments struct. This
function should take the form `int entry_point(arguments args)` (passing the arguments by reference
or const reference is also allowed). Make sure to declare this function in the header you pass as
input as well, so it's available when the generated file is compiled.

The `-WideChar` argument can be used on Windows to generate code that uses wide characters (`wchar_t`)
for the arguments. Make sure to define `_UNICODE` if you use `<ookii/command_line_generated.h>` so
the [`OOKII_GENERATED_METHODS`][] macro will declare a method using `wchar_t` as well. When combined
with `-EntryPoint`, this will generate a `wmain()` function instead of `main()`.

The generated source file will include the headers for Ookii.CommandLine, as well as the header(s)
the script was passed as input. If you need to include additional headers, like for example a
pre-compiled header file, use the `-AdditionalHeaders` argument. These will be included before
any other headers. Usually, it's easier to include other headers from the header(s) containing the
argument structs or classes.

A sample invocation of this script could look as follows:

```pwsh
./New-Parser.ps1 arguments.h -OutputPath generated.cpp -EntryPoint my_main
```

For more information on how to use the script, run `Get-Help ./New-Parser.ps1`.

Check the [generated parser sample](../samples/generated_parser) for a full example of how you can
use this script. The [long short sample](../samples/long_short) and [custom usage sample](../samples/custom_usage)
also use it.

## New-Subcommand.ps1

The [`scripts/New-Subcommand.ps1`](../scripts/New-Subcommand.ps1) script works very similar
to the `New-Parser.ps1` script, but generates [subcommands](Subcommands.md) instead of a
stand-alone parser.

It takes as input one or more C++ headers, which contain declarations of the subcommand classes, and
generates argument parsers for them. It also generates a function called
[`ookii::register_commands()`][], which registers all the generated subcommands and returns a
[`command_manager`][] instance. To use this function in your code, include
`<ookii/command_line_generated.h>`.

For the purposes of the code-generation scripts, the major differences between a subcommand class
and a regular arguments struct or class are that:

- The class must be annotated using the `[command]` attribute instead of `[arguments]`.
- The class must derive from [`ookii::command`][] as normal, or from another subcommand class.
- You must declare the normal subcommand constructor taking a `ookii::parser_builder &` as an
  argument; the implementation of this constructor will be generated by the script.
- You should _not_ use the [`OOKII_GENERATED_METHODS`][] macro, or otherwise declare a
  `create_builder()` method.

The `[command]` attribute can specify the name of the command, like `[command: name]`.
If not specified, the [normal ways of determining the name](Subcommands.md#subcommand-names-and-descriptions)
are used. The same is true for the description if there is none in the comment following the
attribute.

Arguments are annotated in the same way as with `New-Parser.ps1`, with the same attributes.

Here is an example of an annotated subcommand, taken from the
[generated subcommand sample](../samples/generated_subcommand/):

```c++
// [command: read]
// Reads and displays data from a file, optionally limiting the number of lines.
class read_command : public ookii::command
{
public:
    read_command(builder_type &builder);

    virtual int run() override;

private:
    // [argument, required, positional]
    // The name of the file to read.
    std::string _file_name;

    // [argument]
    // The maximum number of lines to read.
    std::optional<int> _max_lines;
};
```

### Global options

If you wish to set options that apply to all commands (such as the parsing mode), you can use a
comment block that starts with a `[global]` attribute. This will add a [`configure_parser()`][] call in
the generated [`ookii::register_commands()`][] function with those options, so they are used for every
command.

A `[name_transform]` attribute in the `[global]` block will cause this name transformation to be
used for every generated command, for every header file.

In addition, if the `[global]` block contains `[version_info]` or `[win32_version]` attribute, these
are used to generate a `version` command instead of an argument. The `[common_help_argument]`
attribute is used to specify the name (including prefix) of a help argument shared by every command,
which will be used in the command list usage help.

If the `[global]` block is followed by any comment lines not containing attributes, these specify
the application description to include before the command list usage help.

The `[global]` block must end with either a non-comment line (like a blank line), or the end of the
file. There can be only one `[global]` block across all header files.

### Script arguments for `New-Subcommand.ps1`

Like `New-Parser.ps1`, the `New-Subcommand.ps1` script supports the `-Output`, `-NameTransform`,
`-WideChar` and `-AdditionalHeaders` arguments, with the same behavior. Note that `-NameTransform`
affects the auto-generated names of arguments, but does not affect the names of commands (which, if
not explicitly specified, are determined at runtime and not by the script).

Use the `-GenerateMain` argument to include a `main()` function in the generated output file. This
`main()` function will call [`ookii::register_commands()`][], and then use
[`command_manager::run_command()`][command_manager::run_command()_1] to run one of the commands. You
don't need to define an entry point at all when using this; just the commands. If `-WideChar` is
present, this generates a `wmain()` function instead.

For more information on how to use the script, run `Get-Help ./New-Subcommand.ps1`.

The [generated subcommand sample](../samples/generated_subcommand) shows a full example of how to
use this script, including how to incorporate the code generation as a build step in CMake.

### Subcommand base classes

If you wish to create a [common base class](Subcommands.md#multiple-commands-with-common-arguments)
for some or all of your commands, you can do so using code generation as well. The `New-Subcommand.ps1`
script will invoke your base class constructor automatically.

If the base class itself is generated as well, you should add the `[no_register]` attribute to the
class to prevent it from being treated as a command.

For example:

```c++
// [command, no_register]
// [argument_value_separator: =]
class base_command : public ookii::command
{
public:
    base_command(builder_type &builder);

private:
    // [argument, required, positional]
    // An argument common to all commands.
    std::string _path;
}

// [command: name]
// Description of the command.
class my_command : public base_command
{
public:
    my_command(builder_type &builder);

    int run() override;

private:
    // [argument]
    // A command-specific argument.
    int _value;
};
```

The [nested commands sample](../samples/nested_commands) has a detailed example of using a base
class, as well as mixing generated and hand-written commands.

## Script limitations

The code-generation scripts don't use a full-fledged C++ parser to interpret your code. Instead,
they use a simple parser using regular expressions to identify the annotations and the struct and
member definitions. As such, you must use the following conventions when declaring your arguments
struct or class, or subcommand.

- Annotation comments, including the descriptions, must use single-line comments (`//`). Comment
  blocks like `/* ... */` are not recognized. You can use any number of slashes to start the comment.
- The struct or class must follow the annotation comment with `[arguments]` or `[command]`,
  and have the form `(struct|class) name` with nothing preceding it on that line. Everything after
  the name is ignored, so you are free to add a base class or something like that.
  - For subcommands, the base class must be specified on the same line with `class name : public base_name`.
    You may use the `public`, `private`, `protected` and `virtual` keywords before the base class
    name. See [subcommand base classes](#subcommand-base-classes).
- The struct or class must end with a single line containing nothing but `};` (besides whitespace).
  - There must be no other line like that before the end of the struct, so take care when declaring
    nested types.
- The struct or class may not be in a namespace.
- The struct or class may not be a template.
- Every argument field must be on a single line (preceded by lines with annotation comments), with
  the last word before the semi-colon, or before an `=` or `{` token for initialization, being the
  name of the field. So, `int foo;`, `int foo = 0;`, and `int foo{};` are all supported.
- The method declaration for an action argument must be on a single line, optionally start with
  `static`, and then `bool name(argument_type` followed by a space or comma. Everything beyond that
  is ignored by the script (but must still match the expected signature, of course).

Some functionality is not available when using the code-generation scripts.

- You cannot specify custom type converters for an argument.
- You cannot specify a separator for a multi-value argument.
- You cannot assign descriptions using localized resources or any other method that isn't just
  string literals.

If you generate a main method using `-EntryPoint` or `-GenerateMain`, the following additional
limitations apply:

- You cannot customize the [`usage_writer`][] or [`localized_string_provider`][] used.
- You cannot change the global locale before parsing happens; the default "C" locale is always
  used.
- You cannot add arguments that aren't defined using the annotations.
- For `New-Subcommand.ps1`, you cannot register commands that aren't defined using the
  annotations.

If you need any of this functionality to be added, please file an issue, or even better, submit
a pull request.

## Including the scripts in your build process

Typically, you should include the code generation as a pre-build step in your build process. There
are several ways to do this

### CMake

If you use CMake, you can use a custom command to invoke the PowerShell scripts as part of the code
generation process. For example, you can use the following:

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

add_executable(application "source.cpp" ${GENERATED_OUTPUT})
```

If you need to process multiple header files, you must use the `-Command` argument of the `pwsh`
binary to be able to pass an array to the script. For `New-Subcommand.ps1`, you should not invoke
the script separately for each file, because doing so will generate multiple conflicting definitions
of the [`ookii::register_commands()`][] function.

Here is an example of passing multiple headers to `New-Subcommand.ps1`:

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

add_executable(application "source.cpp" ${GENERATED_OUTPUT})
```

Also see the [various samples](../samples) for working examples of how to do this.

### Visual Studio

If you use a Visual Studio project and are using the [NuGet package](https://www.nuget.org/packages/Ookii.CommandLine.Cpp)
to add Ookii.CommandLine to your project, the scripts are included in the package.

You can of course use a pre-build command or a custom build step in your project settings to invoke
the scripts. The scripts are located in the `packages\Ookii.CommandLine.Cpp.2.0.0\tools` folder
under your solution root directory. The downside of this is that you would have to change the path
if the package version number changes.

Alternatively, the NuGet package includes MSBuild targets that make this easier. These targets are
disabled by default, but can be used by defining specific items. To do this, you have to manually
edit the .vcxproj file for your project; I don't believe there's a way to do this through the
Visual Studio UI.

To enable the use of `New-Parser.ps1`, add the following to your .vcxproj file, _before_ the
Ookii.CommandLine.Cpp.targets file is imported:

```xml
<ItemGroup>
  <OokiiParserInput Include="arguments.h" />
</ItemGroup>
```

Doing this will automatically invoke the script for the specified header file, and include the
generated file in the build. By default, the file is called "ookii.parser.generated.cpp" and will be
placed in the intermediate output directory of your project (e.g. "x64\Debug"), but this can be
overridden with the `OokiiParserOutput` property.

You can process multiple header files by using wild cards, or by using multiple `OokiiParserInput`
items.

You can use the following MSBuild properties to further customize the behavior:

Property                       | Meaning
-------------------------------|--------------------------------------------------------------------------------------------------------------------------------------------------------------------
`OokiiParserEntryPoint`        | Passes the `-EntryPoint` argument to the script with the specified value.
`OokiiParserNameTransform`     | Passes the `-NameTransfom` argument to the script with the specified value.
`OokiiParserAdditionalHeaders` | Passes the `-AdditionalHeaders` argument to the script with the specified value. For multiple headers, separate them with commas.
`OokiiParserOutput`            | Overrides the path of the generated output file.
`OokiiPwshPath`                | Overrides the path to the pwsh.exe executable used to invoke the scripts.
`CharacterSet`                 | If the value is "Unicode", passes the `-WideChar` argument to the script. This property is set by Visual Studio based on the target character set for your project.

For example, the following processes the file "arguments.h", applying the PascalCase transform and
generating main function that invokes "ookii_main":

```xml
<ItemGroup>
  <OokiiParserInput Include="arguments.h" />
</ItemGroup>
<PropertyGroup>
  <OokiiParserNameTransform>PascalCase</OokiiParserNameTransform>
  <OokiiParserEntryPoint>ookii_main</OokiiParserEntryPoint>
</PropertyGroup>
```

To invoke `New-Subcommand.ps1`, specify the `OokiiCommandInput` item instead of `OokiiParserInput`.
The properties you can set for this script are `OokiiCommandNameTransform`, `OokiiCommandAdditionalHeaders`,
`OokiiCommandOutput`, `OokiiPwshPath` and `CharacterSet`, analogous to the above. In addition,
set the `OokiiCommandGenerateMain` property to `true` to pass the `-GenerateMain` argument to the
script.

For example, the following generates subcommands from two headers:

```xml
<ItemGroup>
  <OokiiCommandInput Include="read_command.h" />
  <OokiiCommandInput Include="write_command.h" />
</ItemGroup>
<PropertyGroup>
  <OokiiCommandNameTransform>PascalCase</OokiiCommandNameTransform>
  <OokiiCommandGenerateMain>true</OokiiCommandGenerateMain>
</PropertyGroup>
```

Dependencies are set up for the MSBuild targets, so the files will only be regenerated if the input
header(s) changed, or if you rebuild the project.

Again, make sure you place these definitions _before_ the line to import Ookii.CommandLine.Cpp.targets.
That line is generated by Visual Studio if you're using NuGet and will look something like this:

```xml
  <ImportGroup Label="ExtensionTargets">
    <Import Project="..\packages\Ookii.CommandLine.Cpp.2.0.0\build\native\Ookii.CommandLine.Cpp.targets" Condition="Exists('..\packages\Ookii.CommandLine.Cpp.2.0.0\build\native\Ookii.CommandLine.Cpp.targets')" />
  </ImportGroup>
```

Next, we will look at some [utility types](Utilities.md) included with the library.

[`OOKII_GENERATED_METHODS`]: https://www.ookii.org/docs/commandline-cpp-2.0/command__line__generated_8h.html#a53b626c1994f1addfd297da8072c76f4
[`command_line_parser`]: https://www.ookii.org/docs/commandline-cpp-2.0/classookii_1_1basic__command__line__parser.html
[`command_manager`]: https://www.ookii.org/docs/commandline-cpp-2.0/classookii_1_1basic__command__manager.html
[`configure_parser()`]: https://www.ookii.org/docs/commandline-cpp-2.0/classookii_1_1basic__command__manager.html#a1a68ed8729ad0dfa2300a2a74691a0c6
[`localized_string_provider`]: https://www.ookii.org/docs/commandline-cpp-2.0/classookii_1_1basic__localized__string__provider.html
[`ookii::command`]: https://www.ookii.org/docs/commandline-cpp-2.0/classookii_1_1basic__command.html
[`ookii::register_commands()`]: https://www.ookii.org/docs/commandline-cpp-2.0/namespaceookii.html#ac514246d38c58d21cc168406737b4865
[`parser_builder::build()`]: https://www.ookii.org/docs/commandline-cpp-2.0/classookii_1_1basic__parser__builder.html#af66361855468fde2eb545fbe1631e042
[`parser_builder`]: https://www.ookii.org/docs/commandline-cpp-2.0/classookii_1_1basic__parser__builder.html
[`std::nullopt`]: https://en.cppreference.com/w/cpp/utility/optional/nullopt
[`std::vector<T>`]: https://en.cppreference.com/w/cpp/container/vector
[`usage_writer`]: https://www.ookii.org/docs/commandline-cpp-2.0/classookii_1_1basic__usage__writer.html
[command_manager::run_command()_1]: https://www.ookii.org/docs/commandline-cpp-2.0/classookii_1_1basic__command__manager.html#a4deb89f49a7ce6d03ed41cdbf8769d58
