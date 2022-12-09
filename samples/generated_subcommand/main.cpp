// This sample shows how to use the New-Subcommand.ps1 script to generate a command line argument
// parser. This script is used with PowerShell 6+ (on any platform, including Linux and Windows)
// to create parsers from a specially annotated command classes. These structs can be found in
// read_command.h and write_command.h.
// 
// For each command, New-Subcommand.ps1 will generate a constructor that defines the arguments.
// It will also generate an ookii::register_commands() function, which registers the commands
// and returns a command_manager.
//
// See https://github.com/SvenGroot/Ookii.CommandLine.cpp/blob/main/docs/Scripts.md for information
// on how to use these scripts and attributes are available. 
//
// This main.cpp file is empty (and indeed, doesn't need to exist at all) because New-Subcommand.ps1 is
// invoked with the "-GenerateMain" argument in CMakeLists.txt, which creates a main() method which
// registers and runs the commands. There is no need to provide an entry point function for the
// application in this case.
//
// If you don't use "-GenerateMain", you can call ookii::register_commands() manually. Include
// <ookii/command_line_generated.h> to get the declaration of this function.
//
// After running the build, you can find the file generated/commands_generated.cpp in the CMake
// build output, if you want to see what the generated code looks like.
