//! \file parse_result.h
//! \brief Provides error handling for the ookii::basic_command_line_parser class.
#ifndef OOKII_PARSE_RESULT_H_
#define OOKII_PARSE_RESULT_H_

#pragma once

#include "localized_string_provider.h"

namespace ookii
{
    namespace details
    {
        template<typename CharType>
        struct error_formats
        {
            static constexpr auto invalid_value = literal_cast<CharType>("The value provided for the argument '{}' was invalid.");
            static constexpr auto unknown_argument = literal_cast<CharType>("Unknown argument name '{}'.");
            static constexpr auto missing_value = literal_cast<CharType>("No value was supplied for the argument '{}'.");
            static constexpr auto duplicate_argument = literal_cast<CharType>("The argument '{}' was supplied more than once.");
            static constexpr auto too_many_arguments = literal_cast<CharType>("Too many arguments were supplied.");
            static constexpr auto missing_required_argument = literal_cast<CharType>("The required argument '{}' was not supplied.");
            static constexpr auto unknown = literal_cast<CharType>("An unknown error has occurred.");
        };
    }

    //! \brief The type of error that occurred while parsing the command line.
    enum class parse_error
    {
        //! \brief No error occurred.
        none,

        //! \brief Parsing was cancelled by an argument using basic_parser_builder::argument_builder::cancel_parsing(),
        //!        or the by the basic_command_line_parser::on_parsed() callback.
        //! 
        //! While this is treated as a parsing failure, this is not technically an error, and no
        //! error message is associated with it.
        parsing_cancelled,

        //! \brief A supplied value could not be converted to the argument's type.
        invalid_value,

        //! \brief An argument name was supplied that doesn't exist.
        unknown_argument,

        //! \brief A named argument, other than a switch argument, was supplied without a value.
        missing_value,

        //! \brief An argument, other than a multi-value argument, was supplied more than once, and
        //!        basic_parser_builder::allow_duplicate_arguments() was not enabled.
        duplicate_argument,

        //! \brief More positional arguments were supplied than were defined.
        too_many_arguments,

        //! \brief One of the required arguments was not supplied.
        missing_required_argument
    };

    //! \brief Provides the result, success or error, of a command line argument parsing operation.
    //! 
    //! This type is returned from the various basic_command_line_parser::parse() overloads to indicate
    //! success or failure. Inspect the value to see if parsing was successful.
    //! 
    //! \warning Command line argument parsing is not atomic; if a value indicating failure is
    //!          returned, some of the arguments could have already been successfully parsed and
    //!          their variables set to the supplied values.
    //! 
    //! \tparam CharType The character type used for argument names and error strings.
    //! \tparam Traits The character traits to use for strings. Defaults to `std::char_traits<CharType>`.
    //! \tparam Alloc The allocator to use for strings. Defaults to `std::allocator<CharType>`.
    template<typename CharType, typename Traits = std::char_traits<CharType>, typename Alloc = std::allocator<CharType>>
    struct [[nodiscard]] parse_result
    {
        //! \brief The concrete string type used by this structure.
        using string_type = std::basic_string<CharType, Traits, Alloc>;
        //! \brief The specialized type of basic_localized_string_provider used.
        using string_provider_type = basic_localized_string_provider<CharType, Traits, Alloc>;

        //! \brief Initializes a new instance of the parse_result structure.
        //! 
        //! \param string_provider The basic_localized_string_provider to use for error messages.
        //! \param error The type of error that occurred, or parse_error::none to indicate no
        //!        error.
        //! \param error_arg_name The name of the argument that caused the error, or a blank string
        //!        if there was no error or the error doesn't relate to a specific argument.
        parse_result(string_provider_type &string_provider, parse_error error = parse_error::none, string_type error_arg_name = {})
            : string_provider{&string_provider},
              error{error},
              error_arg_name{error_arg_name}
        {
        }

        //! \brief The string provider used to get error messages.
        string_provider_type *string_provider;

        //! \brief The type of error that occurred, or parse_error::none to indicate no error.
        parse_error error;

        //! \brief The name of the argument that caused the error, or a blank string if there was
        //!        no error or the error doesn't relate to a specific argument.
        string_type error_arg_name;

        //! \brief Checks if the result was successful.
        //! \return `true` only if the error is parse_error::none; otherwise, `false`.
        operator bool() const noexcept
        {
            return error == parse_error::none;
        }

        //! \brief Gets a default, English language error message for the current error.
        //! 
        //! If appropriate, the message will include the name of the argument that caused the error.
        //! 
        //! \return The error message, or a blank string for parse_error::none and
        //! parse_error::parsing_cancelled.
        string_type get_error_message() const
        {
            switch (error)
            {
            case parse_error::none:
            case parse_error::parsing_cancelled:
                return {};

            case parse_error::invalid_value:
                return string_provider->invalid_value(error_arg_name);

            case parse_error::unknown_argument:
                return string_provider->unknown_argument(error_arg_name);

            case parse_error::missing_value:
                return string_provider->missing_value(error_arg_name);

            case parse_error::duplicate_argument:
                return string_provider->duplicate_argument(error_arg_name);

            case parse_error::too_many_arguments:
                return string_provider->too_many_arguments();

            case parse_error::missing_required_argument:
                return string_provider->missing_required_argument(error_arg_name);

            default:
                return string_provider->unknown_error();
            }
        }
    };
}

#endif