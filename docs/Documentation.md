# Introduction

Ookii.CommandLine is a library that helps you to parse command line arguments for your applications.
It allows you to easily define a set of accepted arguments, and then parse the command line supplied
to your application for those arguments, converting the value of those arguments from a string to
their specified type. In addition, it allows you to generate usage help from the arguments that you
defined, which you can display to the user.

Ookii.CommandLine can be used with any kind of C++ application, whether console or GUI. Although a
limited subset of functionality—particularly related around generating usage help text—is geared
primarily towards console applications that are invoked from the command line, the main command line
parsing functionality is usable in any application that needs to process command line arguments.

To define a set of command line arguments, first you create variables that will hold their values.
Then, you use the ookii::parser_builder class to construct an ookii::command_line_parser to parse
those arguments, specifying things such as the argument names, whether or not an argument is
required, and descriptions used to customize the usage help, among other options.

Two samples are provided with the library, one for basic command line parsing and one for shell
commands. Both have a version using manually written code, and a version that uses the code
generation scripts.

## Contents

- [Tutorial: getting started with Ookii.CommandLine for C++](docs/Tutorial.md)
- [Command Line Arguments Parsing Rules](Arguments.md)
  - [Defining Command Line Arguments](DefiningArguments.md)
  - [Parsing Command Line Arguments](ParsingArguments.md)
  - [Generating Usage Help](UsageHelp.md)
- [Shell Commands](ShellCommands.md)
- [Code-generation scripts](Scripts.md)
- [Line Wrapping Stream](LineWrappingStream.md)
- [Header File Documentation](https://www.ookii.org/Link/CommandLineCppDoc)
