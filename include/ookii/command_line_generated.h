//! \file command_line_generated.h
//! \brief Provides helpers for using the New-Parser.ps1 and New-Subcommand.ps1 scripts.
#ifndef OOKII_COMMAND_LINE_GENERATED_H_
#define OOKII_COMMAND_LINE_GENERATED_H_

#pragma once

#include "command_line.h"

//! \brief A script to declare the static parse member function that the New-Parser.ps1 script will
//!        generate, using the specified character type.
//! 
//! When using New-Parser.ps1, the definition of this method will be generated. This macro makes
//! it easy to define a method that matches the exact signature expected.
//! 
//! \param type The type of the struct or class that contains the arguments.
//! \param char_type The character type to use for strings.
#define OOKII_DECLARE_PARSE_METHOD_EX(type, char_type) \
    static ::std::optional<type> parse(int argc, const char_type* const argv[], ::ookii::basic_usage_writer<char_type> *usage = nullptr, ::ookii::basic_localized_string_provider<char_type> *string_provider = nullptr, const std::locale &locale = {})

//! \brief A script to declare the static parse member function that the New-Parser.ps1 script will
//!        generate, using the default character type.
//! 
//! When using New-Parser.ps1, the definition of this method will be generated. This macro makes
//! it easy to define a method that matches the exact signature expected.
//! 
//! The default character type is `wchar_t` if _UNICODE is defined; otherwise, it's `char`.
//! 
//! \param type The type of the struct or class that contains the arguments.
#define OOKII_DECLARE_PARSE_METHOD(type) OOKII_DECLARE_PARSE_METHOD_EX(type, ookii::details::default_char_type)

namespace ookii
{
    //! \brief Function that registers all the subcommands generated by New-Subcommand.ps1.
    //! 
    //! When using New-Subcommand.ps1, the definition of this method will be generated.
    //! 
    //! This method will be declared for use with `wchar_t` if _UNICODE is defined; otherwise, it
    //! uses `char`.
    //! 
    //! \param application_name The name of the application's executable, for use with the usage
    //!        help.
    //! \param string_provider The basic_localized_string_provider to use for error messages and
    //!        other strings, or `nullptr` to use the default.
    //! \param locale The locale to use when converting argument values. The default is a copy
    //!        of the current global locale.
    basic_command_manager<details::default_char_type> register_commands(
        std::basic_string<details::default_char_type> application_name,
        ::ookii::basic_localized_string_provider<details::default_char_type> *string_provider = nullptr,
        const std::locale& locale = {});
}

#endif