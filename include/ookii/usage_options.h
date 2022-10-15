//! \file basic_usage_options.h
//! \brief Provides the ookii::basic_usage_options class.
#ifndef OOKII_basic_usage_options_H_
#define OOKII_basic_usage_options_H_

#pragma once

#include "string_helper.h"
#include "line_wrapping_stream.h"

namespace ookii
{
    //! \brief Provides options for how to format usage help.
    //!
    //! This type is used by the basic_command_line_parser to determine how to show errors or usage help.
    //! 
    //! Two typedefs for common character types are provided:
    //! 
    //! Type                    | Definition
    //! ----------------------- | -------------------------------------
    //! `ookii::usage_options`  | `ookii::basic_usage_options<char>`
    //! `ookii::wusage_options` | `ookii::basic_usage_options<wchar_t>`
    //! 
    //! \tparam CharType The character type to use for arguments and other strings.
    //! \tparam Traits The character traits to use for strings. Defaults to `std::char_traits<CharType>`.
    //! \tparam Alloc The allocator to use for strings. Defaults to `std::allocator<CharType>`.
    template<typename CharType, typename Traits = std::char_traits<CharType>, typename Alloc = std::allocator<CharType>>
    class basic_usage_options
    {
    private:
        using line_wrapping_stream_type = basic_line_wrapping_ostream<CharType, Traits>;

        std::optional<line_wrapping_stream_type> _owned_output;
        std::optional<line_wrapping_stream_type> _owned_error;

    public:
        //! \brief Provides default values for the fields of basic_usage_options.
        struct defaults
        {
            //! \brief Default value for basic_usage_options::usage_prefix_format.
            static constexpr auto usage_prefix_format = literal_cast<CharType>("Usage: {}");
            //! \brief Default value for basic_usage_options::value_description_format.
            static constexpr auto value_description_format = literal_cast<CharType>("<{}>");
            //! \brief Default value for basic_usage_options::optional_argument_format.
            static constexpr auto optional_argument_format = literal_cast<CharType>("[{}]");
            //! \brief Default value for basic_usage_options::multi_value_suffix.
            static constexpr auto multi_value_suffix = literal_cast<CharType>("...");
            //! \brief Default value for basic_usage_options::argument_description_format.
            static constexpr auto argument_description_format = literal_cast<CharType>("    {}{} {}{}\n{}{}\n");
            //! \brief Default value for basic_usage_options::default_value_format.
            static constexpr auto default_value_format = literal_cast<CharType>(" Default value: {}.");
            //! \brief Default value for basic_usage_options::alias_format.
            static constexpr auto alias_format = literal_cast<CharType>(" ({})");
            //! \brief Default value for basic_usage_options::alias_separator.
            static constexpr auto alias_separator = literal_cast<CharType>(", ");
            
            //! \brief Default value for basic_usage_options::syntax_indent.
            static constexpr size_t syntax_indent = 3;
            //! \brief Default value for basic_usage_options::argument_description_indent.
            static constexpr size_t argument_description_indent = 8;
        };

        //! \brief The concrete string type used.
        using string_type = std::basic_string<CharType, Traits, Alloc>;
        //! \brief The concrete stream type used.
        using stream_type = std::basic_ostream<CharType, Traits>;

        //! \brief Initializes a new instance of the basic_usage_options class.
        //!
        //! This instance will write to a line_wrapping_ostream for the standard output stream
        //! and the standard input stream.
        basic_usage_options()
            : _owned_output{line_wrapping_stream_type::for_cout()},
              _owned_error{line_wrapping_stream_type::for_cerr()},
              output{*_owned_output},
              error{*_owned_error}
        {

        }

        //! \brief Initializes a new instance of the basic_usage_options class with the specified stream.
        //!
        //! This instance will write both errors and usage to the same stream.
        //!
        //! \param output The stream used for usage help and errors.
        basic_usage_options(stream_type &output)
            : output{output},
              error{output}
        {
        }

        //! \brief Initializes a new instance of the basic_usage_options class with the specified output
        //!        and error streams.
        //!
        //! \param output The stream used for usage help.
        //! \param error The stream used for errors.
        basic_usage_options(stream_type &output, stream_type &error)
            : output{output},
              error{error}
        {
        }

        //! \brief The stream used to write usage help to.
        stream_type &output;

        //! \brief The stream used to write errors to.
        stream_type &error;

        //! \brief The format to use for the prefix to the usage syntax.
        //! 
        //! This value must contain a `{}` placeholder for the application name.
        string_type usage_prefix_format{defaults::usage_prefix_format.data()};

        //! \brief The format to use for the value description of an argument.
        //! 
        //! This value must contain a `{}` placeholder for the value description.
        string_type value_description_format{defaults::value_description_format.data()};

        //! \brief The format to use for the syntax of an optional argument.
        //! 
        //! This value must contain a `{}` placeholder for the argument name.
        string_type optional_argument_format{defaults::optional_argument_format.data()};

        //! \brief Suffix to add to the syntax of multi-value arguments.
        string_type multi_value_suffix{defaults::multi_value_suffix.data()};

        //! \brief The format to use for the description of an argument.
        //! 
        //! This value must contain `{}` placeholders for the argument name prefix, the argument
        //! name, the value description, the aliases, the description, and the default value of the
        //! argument.
        //! 
        //! The placeholder for the aliases is set to a blank string if the argument has no
        //! aliases, or if include_aliases_in_description is `false`.
        //! 
        //! The placeholder for the default value is set to a blank string if the argument has no
        //! default value, or if include_default_value_in_description is `false`.
        string_type argument_description_format{defaults::argument_description_format.data()};

        //! \brief The format to use for the default value of an argument.
        //! 
        //! This value must contain a `{}` placeholder for the default value.
        string_type default_value_format{defaults::default_value_format.data()};

        //! \brief The format to use for the aliases of an argument
        //! 
        //! This value must contain a `{}` placeholder for the aliases.
        string_type alias_format{defaults::alias_format.data()};

        //! \brief The separator to use if the argument has more than one alias.
        string_type alias_separator{defaults::alias_separator.data()};

        //! \brief The level of indentation to use when writing the usage syntax.
        //!
        //! Note that the first line of the syntax is not indented.
        //!
        //! This value has no effect if the output stream is not using a line_wrapping_streambuf.
        size_t syntax_indent{defaults::syntax_indent};

        //! \brief The level of indentation to use when writing argument descriptions.
        //!
        //! Note that the first line of each argument is not indented.
        //!
        //! This value has no effect if the output stream is not using a line_wrapping_streambuf.
        size_t argument_description_indent{defaults::argument_description_indent};

        //! \brief Indicates whether to use white space as the argument name separator in the usage
        //!        syntax.
        //!
        //! If `false`, the separator specified in basic_parser_builder::argument_name_separator() is
        //! used instead. The default value is `true`.
        //! 
        //! If basic_parser_builder::allow_white_space_separator() is set to `false`, this value has
        //! no effect and the basic_parser_builder::argument_name_separator() is always used.
        bool use_white_space_value_separator{true};

        //! \brief Indicates whether to include the default value of arguments in the description.
        //! 
        //! The default value is `true`.
        bool include_default_value_in_description{true};

        //! \brief Indicates whether to include the aliases of arguments in the description.
        //! 
        //! The default value is `true`.
        bool include_aliases_in_description{true};
    };

    //! \brief Typedef for basic_usage_options using `char` as the character type.
    using usage_options = basic_usage_options<char>;
    //! \brief Typedef for basic_usage_options using `wchar_t` as the character type.
    using wusage_options = basic_usage_options<wchar_t>;
}

#endif