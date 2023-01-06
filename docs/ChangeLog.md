# What's new in Ookii.CommandLine for C++

## Ookii.CommandLine for C++ 2.0

- Argument parsing
  - Added support for a [new parsing mode](Arguments.md#longshort-mode) where arguments can have a
    separate long name using the `--` prefix (customizable, of course) and single-character short
    name using the `-` prefix. This allows you to use a parsing style that's similar to POSIX
    conventions.
  - You can use [functions to define arguments](DefiningArguments.md#action-arguments).
  - [Automatically](DefiningArguments.md#automatic-arguments) add `-Help` argument if not defined,
    and support easily adding a `-Version` argument.
  - Added the ability to customize error messages and other strings.
- Subcommands
  - Renamed "shell commands" to "subcommands" because I never liked the old name.
    - Header and class names have been changed accordingly.
  - Support [commands with custom parsing](Subcommands.md#custom-parsing).
  - Easily add a [`version` subcommand](Subcommands.md#automatic-commands).
  - Set options for all commands using the `command_manager::configure_parser()` method, or the
    `[global]` block when using [code generation](Scripts.md).
  - Subcommand functionality is included by default with `<ookii/command_line.h>`, no need for a
    separate header.
- Usage help
  - [Color output](UsageHelp.md#color-output) support.
  - Greatly expanded [usage help customization options](UsageHelp.md#customizing-the-usage-help)
    with the new `usage_writer` class, including abbreviated syntax, description
    list ordering and filtering, the ability to override any string or format, and more.
  - Arguments that have no description but that have other information not shown in the usage syntax
    (like aliases, a default value, or validators) will be included in the description list by
    default.
- `line_wrapping_ostream` has an option to flush all content, including the last unfinished line.
- Added `line_wrapping_ostringstream` class.
- [Code generation](Scripts.md) supports the new functionality.
- Updated and improved documentation.
- More [samples](../samples) with descriptions and explanations.
- No longer uses the C++20 `<ranges>` header because it didn't work in Clang.
- Various bug fixes.

There are a number of breaking API changes from version 1.0:

- The callback used by `command_line_parser::on_parsed()` takes an `std::optional<std::string_view>`
  for the value, instead of just `std::string_view`.
- The `command_line_parser::get_argument()` method returns `nullptr` instead of throwing an
  exception for a non-existent argument.
- Default values for arguments are now displayed with stream insertion (`operator<<`), so custom
  types must define that instead of a formatter.
- The `for_each_argument_in_usage_order()` method has been removed; the `arguments()` method returns
  the arguments in that order.
- The `<shell_command.h>` header was renamed to `<subcommand.h>`.
  - Recommended: include `<command_line.h>` instead; it now includes subcommands.
- The `shell_command` class was renamed to `command`.
- The `shell_command_manager` class was renamed to `command_manager`.
- Several overloads of the `create_command()` and `run_command()` methods have been removed. This
  was necessary to support commands with custom argument parsing.
- The `command_manager::run_command()` method returns an `std::optional<int>`.
- Several changes to the code generation scripts mean that any generated files must be regenerated.
- The `OOKII_DECLARE_PARSE_METHOD()` macro has been replaced with the `OOKII_GENERATED_METHODS()`
  macro.

## Ookii.CommandLine for C++ 1.0

- Initial release
- Feature parity with [Ookii.CommandLine 2.4 for .Net](https://github.com/SvenGroot/Ookii.CommandLine),
  except for dictionary arguments.
