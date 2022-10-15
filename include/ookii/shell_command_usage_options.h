//! \file shell_command_usage_options.h
//! \brief Provides the ookii::shell_command_usage_options class.
#ifndef SHELL_COMMAND_USAGE_OPTIONS_H_
#define SHELL_COMMAND_USAGE_OPTIONS_H_

#pragma once

#include "usage_options.h"

namespace ookii
{

    //! \brief Provides options for how to format usage help for applications using shell commands.
    //!
    //! This type is used by the basic_shell_command_manager to determine how to show errors, a
    //! list of commands, or usage help for a specific command.
    //! 
    //! Two typedefs for common character types are provided:
    //! 
    //! Type                                  | Definition
    //! ------------------------------------- | ---------------------------------------------------
    //! `ookii::shell_command_usage_options`  | `ookii::basic_shell_command_usage_options<char>`
    //! `ookii::wshell_command_usage_options` | `ookii::basic_shell_command_usage_options<wchar_t>`
    //! 
    //! \tparam CharType The character type used for arguments and other strings.
    //! \tparam Traits The character traits to use for strings. Defaults to `std::char_traits<CharType>`.
    //! \tparam Alloc The allocator to use for strings. Defaults to `std::allocator<CharType>`.
    template<typename CharType, typename Traits = std::char_traits<CharType>, typename Alloc = std::allocator<CharType>>
    class basic_shell_command_usage_options : public basic_usage_options<CharType, Traits, Alloc>
    {
    public:
        //! \brief Provides default values for the fields of basic_shell_command_usage_options.
        struct defaults
        {
            //! \brief Default value for basic_shell_command_usage_options::command_usage_format.
            static constexpr auto command_usage_format = literal_cast<CharType>("{} <command> [args...]");
            //! \brief Default value for basic_shell_command_usage_options::command_format.
            static constexpr auto command_format = literal_cast<CharType>("    {}\n{}\n");
            //! \brief Default value for basic_shell_command_usage_options::available_commands_header.
            static constexpr auto available_commands_header = literal_cast<CharType>("The following commands are available:");
            //! \brief Default value for basic_shell_command_usage_options::command_indent.
            static constexpr size_t command_indent = 8;
        };

        //! \brief The concrete base class of thes type.
        using base_type = basic_usage_options<CharType, Traits, Alloc>;
        //! \brief The concrete string type used.
        using string_type = typename base_type::string_type;
        //! \brief The concrete stream type used.
        using stream_type = typename base_type::stream_type;

        //! \brief Initializes a new instance of the basic_shell_command_usage_options class.
        //!
        //! This instance will write to a line_wrapping_ostream for the standard output stream
        //! and the standard input stream.
        basic_shell_command_usage_options() = default;

        //! \brief Initializes a new instance of the basic_shell_command_usage_options class with the
        //!        specified stream.
        //!
        //! This instance will write both errors and usage to the same stream.
        //!
        //! \param output The stream used for usage help and errors.
        basic_shell_command_usage_options(stream_type &output)
            : base_type(output)
        {
        }

        //! \brief Initializes a new instance of the basic_shell_command_usage_options class with the
        //!        specified output and error streams.
        //!
        //! \param output The stream used for usage help.
        //! \param error The stream used for errors.
        basic_shell_command_usage_options(stream_type &output, stream_type &error)
            : base_type(output, error)
        {
        }

        //! \brief The usage syntax format shown when no command was specified, or an unknown
        //!        command was specified.
        //!
        //! This value must contain one `{}` placeholder for the application name.
        string_type command_usage_format{defaults::command_usage_format.data()};

        //! \brief The format for showing a command in the list of commands.
        //!
        //! This value must contain `{}` placeholders for the command name and the command
        //! description.
        string_type command_format{defaults::command_format.data()};

        //! \brief The header to show above the list of available commands.
        string_type available_commands_header{defaults::available_commands_header.data()};

        //! \brief The level of indentation to use when writing the list of commands.
        //!
        //! Note that the first line of each command is not indented.
        //!
        //! This value has no effect if the output stream is not using a line_wrapping_streambuf.
        size_t command_indent{defaults::command_indent};
    };

    //! \brief Typedef for basic_shell_command_usage_options using `char` as the character type.
    using shell_command_usage_options = basic_shell_command_usage_options<char>;
    //! \brief Typedef for basic_shell_command_usage_options using `wchar_t` as the character type.
    using wshell_command_usage_options = basic_shell_command_usage_options<wchar_t>;
}

#endif