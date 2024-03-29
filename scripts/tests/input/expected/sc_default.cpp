// This file is generated by New-Subcommand.ps1; do not edit manually.
#include <ookii/command_line.h>
#include <ookii/command_line_generated.h>
#include "../input/subcommand.h"
#include "../input/subcommand2.h"
    
my_command::my_command(my_command::builder_type &builder)
    : ookii::command{builder}
{
    builder
        .add_argument(this->test_arg, "test_arg").required().positional().description("Argument description with a line break.\n\nAnd another paragraph.")
        .add_argument(this->__test__arg2__, "__test__arg2__").positional().default_value(1).value_description("desc").alias("test").description("Short description.")
        .add_multi_value_argument(this->test_arg3, "foo").alias("t").alias("v")
        .add_argument(this->_testArg4, "_testArg4").cancel_parsing()
        .add_argument(this->TestArg5, "TestArg5").default_value("foo")
    ;
}

other_command::other_command(other_command::builder_type &builder)
    : ookii::command{builder}
{
    builder
        .prefixes({ "--", "-" })
        .case_sensitive(true)
        .allow_whitespace_separator(false)
        .allow_duplicate_arguments(true)
        .argument_value_separator('=')
        .add_argument(this->_some_arg, "_some_arg")
    ;
}

third_command::third_command(third_command::builder_type &builder)
    : other_command{builder}
{
    builder
        .add_argument(this->_other_arg, "_other_arg")
    ;
}

ookii::basic_command_manager<char> ookii::register_commands(std::basic_string<char> application_name, ::ookii::basic_localized_string_provider<char> *string_provider, const std::locale& locale)
{
    basic_command_manager<char> manager{application_name, false, locale, string_provider};
    manager
        .add_command<my_command>("name", "Description of the command with a line break.\n\nAnd a paragraph.")
        .add_command<third_command>({}, {})
    ;

    return manager;
}


