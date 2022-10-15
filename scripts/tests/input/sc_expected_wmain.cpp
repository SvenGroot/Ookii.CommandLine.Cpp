// This file is generated by New-ShellCommand.ps1; do not edit manually.
#include <filesystem>
#include <ookii/shell_command.h>
#include <ookii/command_line_generated.h>
#include "../input/shell_command.h"
#include "../input/shell_command2.h"
    
my_command::my_command(my_command::builder_type &builder)
    : ookii::shell_command{builder}
{
    builder
        .add_argument(this->test_arg, L"TestArg").required().positional().description(L"Argument description with a line break.\n\nAnd another paragraph.")
        .add_argument(this->__test__arg2__, L"TestArg2").positional().default_value(1).value_description(L"desc").alias(L"test").description(L"Short description.")
        .add_multi_value_argument(this->test_arg3, L"foo").alias(L"t").alias(L"v")
        .add_argument(this->_testArg4, L"TestArg4").cancel_parsing()
        .add_argument(this->TestArg5, L"TestArg5").default_value("foo");
}

other_command::other_command(other_command::builder_type &builder)
    : ookii::shell_command{builder}
{
    builder
        .prefixes({ L"--", L"-" })
        .case_sensitive(true)
        .allow_whitespace_separator(false)
        .allow_duplicate_arguments(true)
        .argument_value_separator(L'=')
        .add_argument(this->_some_arg, L"SomeArg");
}

third_command::third_command(third_command::builder_type &builder)
    : other_command{builder}
{
    builder
        .add_argument(this->_other_arg, L"OtherArg");
}

ookii::basic_shell_command_manager<wchar_t> ookii::register_shell_commands(std::basic_string<wchar_t> application_name)
{
    basic_shell_command_manager<wchar_t> manager{application_name};
    manager
        .add_command<my_command>(L"name", L"Description of the command with a line break.\n\nAnd a paragraph.")
        .add_command<third_command>({}, {});

    return manager;
}

int wmain(int argc, wchar_t *argv[])
{
    std::basic_string<wchar_t> name;
    if (argc > 0)
        name = std::filesystem::path{argv[0]}.filename().wstring();
    auto manager = ookii::register_shell_commands(name);
    return manager.run_command(argc, argv);
}

