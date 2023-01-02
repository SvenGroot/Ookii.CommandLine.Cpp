# Code-generation scripts

Although the `parser_builder` class provides a simple interface for defining command line
arguments, it's still less convenient than the way [Ookii.CommandLine for .Net](https://github.com/SvenGroot/ookii.commandline)
works. To provide an easier method, which is also more similar to the .Net version,
Ookii.CommandLine for C++ comes with two scripts, one to generate a stand-alone parser, and one to
generate [subcommands](Subcommands.md).

These two scripts require [PowerShell Core or PowerShell 6](https://github.com/powershell/powershell)
or later, which is available for Windows and other platforms such as Linux. Note, even on Windows,
the new cross-platform PowerShell is required; the scripts use features not available in Windows
PowerShell. The scripts were tested with PowerShell 7.2.

Both of these scripts generate code that creates a `parser_builder` and adds arguments to it. It
doesn't offer any features that manually writing code doesn't, it's just a more convenient way to do
it.

For information on how to generate these files as part of your build process, [see here](#including-the-scripts-in-your-build-process).

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

The `arguments` attribute here indicates that the following struct or class contains arguments. The
fields with arguments are also annotated in the same way.

Attributes can have values; for example, the `[arguments]` attribute can specify the name of the
application. To specify a value, follow the attribute by a colon, and then the value. For example,
`[arguments: name]`. If a colon is present, the entire remainder of the annotation is considered
the value.

If an argument doesn't have a value, you can combine multiple on a single line by separating them
with a comma. For example, `[argument, positional]`. This is not possible if the attribute has a
value, unless it's the last attribute. For example, `[positional, default: 5]` is valid, but
`[default: 5, positional]` is _not_, because everything after the colon is part of the value for
`default`.

If there are any lines of comments after the annotations, these specify the description for that
item. They will be concatenated into a single line, though if you have a blank line this will be
preserved in the output.

For example, the following creates an argument named "ArgumentName" whose value will be stored in
the `arg` field. The argument is required, positional, and has a description:

```c++
// [argument: ArgumentName]
// [required, positional]
// This is the description.
std::string arg;
```

### Attributes for arguments types

The following attributes can be applied to the struct or class that contains arguments.

Attribute                     | Description                                                                                                                                                                                                  | Value
------------------------------|--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------|------------------------------------------------------------------------------------------------------------
**arguments**                 | Indicates the following struct or class defines arguments (`New-Parser.ps1` only) .                                                                                                                          | **(optional)** The application executable name; if omitted, defaults to the file name portion of `argv[0]`.
**command**             | Indicates the following class is a subcommand (`New-Subcommand.ps1` only).                                                                                                                              | **(optional)** The command name; if omitted, defaults to the static `name()` method or type name.
**no_register**               | Indicates the subcommand should not be registered with the `command_manager`. Use this for classes you intend to use as a common base class for other subcommands (`New-Subcommand.ps1` only). | **(none)**
**prefixes**                  | Specifies the argument name prefixes to use instead of the defaults.                                                                                                                                         | A comma-separated list of argument name prefixes.
**case_sensitive**            | Indicates argument names should be treated as case sensitive.                                                                                                                                                | **(none)**
**argument_value_separator**  | Specifies the separator to use between argument names and values, instead of the default ':'                                                                                                                 | The separator to use. May only be a single character.
**no_whitespace_separator**   | Indicates that argument names and values cannot be separated by whitespace                                                                                                                                   | **(none)**
**allow_duplicate_arguments** | Indicates that repeating an argument more than once is not an error, but the last value supplied will be used.                                                                                               | **(none)**

### Attributes for arguments

The following attributes can be applied to the fields that define arguments.

Attribute             | Description                                                                                                                                                         | Value
----------------------|---------------------------------------------------------------------------------------------------------------------------------------------------------------------|----------------------------------------------------------------------------------------------------------------------------------------------------------------------
**argument**          | Indicates the following field is a command line argument. This must always be the first attribute on an argument field.                                             | **(optional)** The name of the argument; if omitted, it defaults to the field name, possibly modified according to the script's `-NameTransform` argument.
**required**          | Indicates the argument is [required](Arguments.md#required-arguments).                                                                                              | **(none)**
**positional**        | Indicates the argument is [positional](Arguments.md#positional-arguments). The position depends on the order the positional arguments are defined in.               | **(none)**
**value_description** | Specifies the [value description](UsageHelp.md#value-description) of the argument.                                                                                  | The value description.
**alias**             | Specifies the [aliases](DefiningArguments.md#aliases) of an argument.                                                                                               | A comma-separated list of aliases.
**default**           | Specifies the [default value](DefiningArguments.md#default-values) of an argument.                                                                                  | The default value. This is copied into the generated code verbatim, so it must be a valid C++ expression or literal; if the value is a string, this must have quotes.
**multi_value**       | Indicates the argument is a [multi-value argument](Arguments.md#arguments-with-multiple-values). Requires the argument uses a suitable type (e.g. `std::vector<T>`) | **(none)**
**cancel_parsing**    | Indicates the argument, when supplied, will [cancel parsing](DefiningArguments.md#arguments-that-cancel-parsing).                                                   | **(none)**

### Parsing limitations

The code-generation scripts don't use a full-fledged C++ parser to interpret your code. Instead,
they use a simple regular expressions based parser that looks for annotations and the struct and
field definitions. As such, you must use the following conventions when declaring your argument
struct or class.

- Annotation comments, including the descriptions, must use single-line comments (`//`). Comment
  blocks like `/* ... */` are not recognized. You can use any number of slashes to start the comment.
- The struct or class must follow the annotation comment with `[arguments]` or `[command]`,
  and have the form `(struct|class) name` with nothing preceding it on that line. Everything after
  the name is ignored, so you are free to add a base class or something like that.
  - For subcommands, the base class must be specified on the same line with `class name : public base_name`.
    You may use the `public`, `private`, `protected` and `virtual` keywords before the base class
    name. See [subcommand base classes](#shell-command-base-classes).
- The struct or class must end with a single line containing nothing but `};`.
- The struct or class may not be in a namespace.
- The struct or class may not be a template.
- Every argument field must be on a single line (preceded by lines with annotation comments), with
  the last word before the semi-colon, or before an `=` or `{` token for initialization, being the
  name of the field. So, `int foo;`, `int foo = 0;`, and `int foo{};` are all supported.

### Example

Here's an example of a struct that defines a number of arguments, using the annotations:

```c++
// [arguments]
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
    std::optional<int> count;

    // [argument]
    // [alias: v]
    // Print verbose information; this is an example of a switch argument.
    bool verbose;

    // [argument: Value]
    // [multi_value]
    // This is an example of a multi-value argument, which can be repeated multiple times to set
    // more than one value.
    std::vector<std::string> values;

    // [argument, cancel_parsing, alias: ?]
    // Displays this help message.
    bool help;

    OOKII_DECLARE_PARSE_METHOD(arguments);
};
```

These are the arguments used by the [included sample application](../samples/generated_parser).

## New-Parser.ps1

To generate an argument parser, you use the [`scripts/New-Parser.ps1`](../scripts/New-Parser.ps1)
script. This script takes as input a C++ header file that contains a struct or class that defines
the arguments. The struct or class and its fields must be annotated as described above.

In addition, the struct must declare a static `parse` method with the following signature:

```c++
static std::optional<type> parse(int argc, const char* const argv[], const ookii::usage_options &options = {});
```

Where `type` is the type of your arguments struct or class. By having the parse method be a member
of the struct, it makes it possible to use private fields for arguments if you wish.

To make it easy to declare the method, you can include `<ookii/command_line_generated.h>` and use
the following macro:

```c++
OOKII_DECLARE_PARSE_METHOD(type);
```

The script's output, written to the file specified by `-OutputPath`, will be a C++ source file
that contains a definition for that parse method. Add this generated file to your compiler inputs
to use the generated parse method.

The generated `parse` method will create a `command_line_parser` using the information extracted
from the annotation comments. It will then parse the arguments. If successful, it returns an instance
of the arguments struct or class. If an error occurred, it will print the error message and usage
help according to the supplied `usage_options`, and return `std::nullopt`.

Use the script's `-NameTransform` argument to indicate that argument names derived from field names
should be changed according to the supplied method. The following values are supported:

Value          | Description                                                                                                                                                                                                                                                  | Example
---------------|--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------|----------------------------------------------------------------
**None**       | Field names are used as-is, without changing them. This is the default.                                                                                                                                                                                      |
**PascalCase** | Field names are transformed to PascalCase. This removes all underscores, and the first character and every character after an underscore is changed to uppercase. The case of other characters is not changed.                                               | `some_name`, `_some_name`, `_someName` => SomeName
**CamelCase**  | Field names are transformed to camelCase. Similar to PascalCase, but the first character will not be uppercase.                                                                                                                                             | `some_name`, `_some_name`, `_SomeName`=> someName
**SnakeCase**  | Field names are transformed to snake_case. This removes leading and trailing underscores, changes all characters to lower-case, and reduces consecutive underscores to a single underscore. An underscore is inserted before previously capitalized letters. | `some_name`, `_some__name`, `_someName` => some_name
**DashCase**   | Field names are transformed to dash-case. Similar to SnakeCase, but uses a dash instead of an underscore.                                                                                                                                                    | `some_name`, `_some__name`, `_someName` => some-name
**Trim**       | Removes leading and trailing underscores, but leaves the rest of the name alone.                                                                                                                                                                             | `some_name`, `_some_name` => some_name; `_someName` => someName

This is convenient if your naming convention for fields in C++ is different than the convention you wish to use
for argument names, and you don't want to specify every argument name manually. Keep in mind that argument names are
case insensitive by default, so the capitalization of the names only affects how they are displayed in the usage help;
they can still be specified using any case, unless you use `[case_sensitive]` on the arguments struct.

If you do specify an argument name manually using `[argument: name]`, these are not affected by
`-NameTransform`.

By using the script's `-EntryPoint` argument, you can specify a function name that should serve as
the entry point for your application. The script will then generate a `main()` function for you,
which parses the arguments and invokes the specified function, passing it the arguments struct. This
function should take the form `int entry_point(arguments args)`. Make sure to declare this function
in the header you pass as input as well, so it's available when the generated file is compiled.

The `-WideChar` argument can be used on Windows to generate code that uses wide characters (`wchar_t`)
for the arguments. Make sure to define `_UNICODE` if you use `<ookii/command_line_generated.h>` so
the `OOKII_DECLARE_PARSE_METHOD` macro will declare a method using `wchar_t` as well. When combined
with `-EntryPoint`, this will generate a `wmain()` instead of `main()`.

The generated source file will include the headers for Ookii.CommandLine, as well as the header(s)
the script was passed as input. If you need to include additional headers, like for example a
pre-compiled header file, use the `-AdditionalHeaders` argument. These will be included before
any other headers. Usually, it's easier to include other headers from the header(s) containing the
argument structs or classes.

For more information on how to use the script, run `Get-Help ./New-Parser.ps1`.

A [full sample](../samples/generated_parser) is included, which also demonstrates how to incorporate
the code generation as a build step in CMake.

## New-Subcommand.ps1

The [`scripts/New-Subcommand.ps1`](../scripts/New-Subcommand.ps1) script works very similar
to the `New-Parser.ps1` script, but generates [subcommands](Subcommands.md) instead of a
stand-alone parser.

It takes as input one or more C++ headers, which contain declarations of the subcommands, and
generates argument parsers for them, as well as the `ookii::register_commands` function, which
registers all the subcommands it found and returns a `command_manager`. To use this
function, include `<ookii/command_line_generated.h>` _after_ you include `<ookii/command_line.h>`

For the purposes of the code-generation scripts, the major differences between a subcommand and
a regular arguments struct or class are that:

- The class must be annotated using the `[command]` attribute instead of `[arguments]`.
- The class must derive from `ookii::command` as normal, or from another subcommand class.
- You must declare the normal subcommand constructor taking a `ookii::command::builder_type &`
  as an argument; this constructor will be defined by the generated code.
- You should _not_ declare a `parse` method.

The `[command]` attribute can specify the name of the command, like `[command: name]`.
If not specified, the [normal ways of determining the name](Subcommands.md#shell-command-names-and-descriptions)
are used. The same is true for the description if there is none in the comment following the
attribute.

Here is an example of an annotated subcommand, taken from the [included sample](../samples/generated_subcommand/):

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

Like `New-Parser.ps1`, the `New-Subcommand.ps1` script supports the `-NameTransform`, `-WideChar`
and `-AdditionalHeaders` arguments, with the same behavior. Note that `-NameTransform` affects the
auto-generated names of arguments, but does not affect the names of commands (which, if not explicitly
specified, are determined at runtime and not by the script).

Use the `-GenerateMain` argument to include a `main()` function in the generated output file. This
main method will call `ookii::register_commands`, and then use `command_manager::run_command`
to run one of the commands. You don't need to define an entry point at all when using this; just the
commands. If `-WideChar` is present, this generates a `wmain()` function instead.

For more information on how to use the script, run `Get-Help ./New-Subcommand.ps1`.

A [full sample](../samples/generated_subcommand) is included, which also demonstrates how to
incorporate the code generation as a build step in CMake.

### subcommand base classes

If you have common arguments, or common parser attributes you wish to use for every command, you
may wish to use a common base class for your subcommands. `New-Subcommand.ps1` supports this
scenario.

To create the base class, add the `[no_register]` attribute to the class to prevent it from being
registered as a command. Alternatively, you can manually write the base class instead of generating
it. The base class must have a constructor that takes a `ookii::command::builder_type &`
argument same as a normal subcommand.

When creating the derived class, just inherit from your base class. The generated constructor will
invoke the base class constructor.

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

In this example, all commands deriving from `base_command` will have a "Path" argument, and use `=`
as the separator between argument names and values.

## Script limitations

Some functionality is not available when using the code-generation scripts.

- You cannot specify custom type converters for an argument.
- You cannot specify a separator for a multi-value argument.
- You cannot assign descriptions using localized resources or any other method that isn't just
  string literals.
- You cannot add arguments that aren't defined using the annotations.

If you generate a main method using `-EntryPoint` or `-GenerateMain`, the following additional
limitations apply:

- You cannot customize the `usage_options`.
- You cannot change the global locale before parsing happens; the standard "C" locale is always
  used.
- For `New-Subcommand.ps1`, you cannot register commands that aren't defined using the
  annotations.

If you need any of this functionality to be added, please file an issue, or even better, submit
a pull request.

## Including the scripts in your build process

### CMake

If you use CMake, you can use a custom command to invoke the PowerShell scripts as part of the code
generation process. For example, you can use the following:

```cmake
find_program(POWERSHELL_PATH NAMES pwsh NO_PACKAGE_ROOT_PATH NO_CMAKE_PATH NO_CMAKE_ENVIRONMENT_PATH NO_CMAKE_SYSTEM_PATH NO_CMAKE_FIND_ROOT_PATH)

set(GENERATED_DIR "${CMAKE_BINARY_DIR}/generated")
set(GENERATED_OUTPUT "${GENERATED_DIR}/parser_generated.cpp")
set(GENERATED_INPUT "${CMAKE_CURRENT_LIST_DIR}/arguments.h")

make_directory("${GENERATED_DIR}")

add_custom_command(
    OUTPUT ${GENERATED_OUTPUT}
    COMMAND ${POWERSHELL_PATH} -ExecutionPolicy Bypass "${FETCHCONTENT_BASE_DIR}/ookiicl-src/scripts/New-Parser.ps1" -Path "${GENERATED_INPUT}" -OutputPath "${GENERATED_OUTPUT}" -NameTransform PascalCase
    DEPENDS ${GENERATED_INPUT}
    VERBATIM
)
```

This assumes you used `FetchContent` to add Ookii.CommandLine to your library, otherwise you may
need to adjust the path to the scripts.

Also see the [generated parser sample](../samples/generated_parser/CMakeLists.txt) and the
[generated subcommand sample](../samples/generated_subcommand/CMakeLists.txt) for working
examples of how to do this.

### Visual Studio

If you use a Visual Studio project and are using the [NuGet package](https://www.nuget.org/packages/Ookii.CommandLine.Cpp)
to add Ookii.CommandLine to your project, the scripts are included in the package.

First, you can of course use the pre-build command or a custom build step to invoke the scripts.
The scripts are located in the `packages\Ookii.CommandLine.Cpp.1.0.0\tools` folder under your
solution root directory. The downside of this is that you would have to change the path if the
package version number changes.

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

Doing this will automatically generate a command line parser for the specified header file, and
include the generated file in the build. By default, the file is called "ookii.parser.generated.cpp"
and will be placed in the intermediate output directory of your project (e.g. "x64\Debug"), but
this can be overridden with the `OokiiParserOutput` property.

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
    <Import Project="..\packages\Ookii.CommandLine.Cpp.1.0.0\build\native\Ookii.CommandLine.Cpp.targets" Condition="Exists('..\packages\Ookii.CommandLine.Cpp.1.0.0\build\native\Ookii.CommandLine.Cpp.targets')" />
  </ImportGroup>
```
