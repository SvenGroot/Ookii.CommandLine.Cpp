# Long/short mode sample

This sample alters the behavior of Ookii.CommandLine to be more like the POSIX conventions for
command line arguments. To do this, it enables the alternate long/short parsing mode, uses a name
transformation to make all the argument names lower case with dashes between the words, and uses
case-sensitive argument names.

This sample uses the same arguments as the [parser Sample](../parser), so see that sample's source
for more details about each argument.

This sample uses the [code generation scripts](../../docs/Scripts.md), so to build this sample, you
must install [PowerShell 6 or later](https://github.com/PowerShell/PowerShell) for your platform
(including Windows; the built-in Windows PowerShell cannot be used).

After running the build, you can find the file `generated/long_short_generated.cpp` in the CMake
build output, if you want to see what the generated code looks like.

In long/short mode, each argument can have a long name, using the `--` prefix, and a one-character
short name, using the `-` prefix (and `/` on Windows). The prefixes can be customized if desired.

When in this mode, the default usage help has a slightly different format to accommodate the short
names.

```text
Sample command line application. The application parses the command line and prints the results, but
otherwise does nothing and none of the arguments are actually used for anything.

Usage: long_short [--source] <string> [--destination] <string> [[--operation-index] <int>] [--count
   <number>] [--help] [--process] [--value <string>...] [--verbose] [--version]

    -s, --source <string>
            The source data.

    -d, --destination <string>
            The destination data.

        --operation-index <int>
            The operation's index. Default value: 1.

    -c, --count <number>
            Provides the count for something to the application.

    -?, --help [<bool>] (-h)
            Displays this help message.

    -p, --process [<bool>]
            Does the processing.

        --value <string>
            This is an example of a multi-value argument, which can be repeated multiple times to
            set more than one value.

    -v, --verbose [<bool>]
            Print verbose information; this is an example of a switch argument.

        --version [<bool>]
            Displays version information.
```

Long/short mode allows you to combine switches with short names, so running `long_short -vp` sets
both `--verbose` and `--process` to true.
