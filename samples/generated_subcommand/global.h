// This header file isn't included anywhere, because it doesn't contain any code. It just contains
// global settings for the New-Subcommand.ps1 script. These settings will be applied to the
// command_manager. These don't have to be in a separate header, but I did that here because they
// don't belong to either read_command.h or write_command.h.
//
// We use the global settings to set an application description, generate a "version" command, and
// an instruction to invoke a command with "-Help" for more information. We also set the name
// transformation used for all arguments of all commands.
//
// This can also be used to set options that apply to all commands, such as parsing mode, case
// sensivity, prefixes, etc.
// 
// The global block must end with a non-comment line

// [global]
// [version_info: Ookii.CommandLine Subcommand Sample 2.0]
// [common_help_argument]
// [name_transform: PascalCase]
// Subcommand sample for Ookii.CommandLine.