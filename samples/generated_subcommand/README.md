# Generated subcommand sample

This sample is identical to the [subcommand sample](../subcommand), but uses the
[code generation scripts](../../docs/Scripts.md) to define the commands. It defines the same
commands as the subcommand sample, and behaves the same, so this sample is a good way to compare the
manual method of defining commands against the scripts.

To build this sample, you must install [PowerShell 6 or later](https://github.com/PowerShell/PowerShell)
for your platform (including Windows; the built-in Windows PowerShell cannot be used).

This sample uses the `New-Subcommand.ps1` script, which reads one or more header files with
specially annotated classes defining the commands. See [read_command.h](read_command.h) and
[write_command.h](write_command.h) to see how the arguments of this sample are defined.

The script also sets several options that apply to all commands; check [global.h](global.h) for
these.

See [CMakeLists.txt](CMakeLists.txt) to see how PowerShell is invoked and the generated code added
to the build.

We pass the `-GenerateMain` argument to the script to have it generate a `main()` function, which
runs the command specified by the first argument. With this, there is no need to write your own
`main()` function at all.

If you don't want to use the generated main function, you can use the [`ookii::register_commands()`][]
function, declared in `<ookii/command_line_generated.h>`, to get a [`command_manager`][] with all
the commands you defined.

After running the build, you can find the file `generated/subcommand_generated.cpp` in the CMake
build output, if you want to see what the generated code looks like (it will be very similar to the
code in the subcommand sample).

For examples of the usage output, please see the [subcommand sample](../subcommand).

[`command_manager`]: https://www.ookii.org/docs/commandline-cpp-2.0/classookii_1_1basic__command__manager.html
[`ookii::register_commands()`]: https://www.ookii.org/docs/commandline-cpp-2.0/namespaceookii.html#ac514246d38c58d21cc168406737b4865
