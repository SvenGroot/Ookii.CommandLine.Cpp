# Generated parser sample

This sample is identical to the [parser sample](../parser), but uses the
[code generation scripts](../../docs/Scripts.md) to define the arguments. It defines the same
arguments as the parser sample, and behaves the same, so this sample is a good way to compare the
manual method of defining arguments against the scripts.

To build this sample, you must install [PowerShell 6 or later](https://github.com/PowerShell/PowerShell)
for your platform (including Windows; the built-in Windows PowerShell cannot be used).

This sample uses the `New-Parser.ps1` script, which reads a header file with a specially annotated
struct containing the arguments. See [arguments.h](arguments.h) to see how the arguments of this
sample are defined.

See [CMakeLists.txt](CMakeLists.txt) to see how PowerShell is invoked and the generated code added
to the build.

We pass the `-EntryPoint` argument to the script to have it generate a main() function, which calls
the function we specified after the arguments have been parsed.

After running the build, you can find the file `generated/parser_generated.cpp` in the CMake build
output, if you want to see what the generated code looks like (it will be very similar to the
code in the parser sample).

For examples of the usage output, please see the [parser sample](../parser).
