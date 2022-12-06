//! \file basic_usage_options.h
//! \brief Provides the ookii::basic_usage_options class.
#ifndef OOKII_USAGE_WRITER_H_
#define OOKII_USAGE_WRITER_H_

#pragma once

#include "string_helper.h"
#include "line_wrapping_stream.h"

namespace ookii
{
    template<typename CharType, typename Traits, typename Alloc>
    class basic_command_line_parser;

    enum class usage_help_request
    {
        full,
        syntax_only,
        none
    };

    template<typename CharType, typename Traits = std::char_traits<CharType>, typename Alloc = std::allocator<CharType>>
    class basic_usage_writer
    {
    public:
        //! \brief Provides default values for the fields of basic_usage_writer.
        struct defaults
        {
            //! \brief Default value for basic_usage_options::syntax_indent.
            static constexpr size_t syntax_indent = 3;
            //! \brief Default value for basic_usage_options::argument_description_indent.
            static constexpr size_t argument_description_indent = 8;
        };

        using line_wrapping_stream_type = basic_line_wrapping_ostream<CharType, Traits>;
        //! \brief The concrete string type used.
        using string_type = std::basic_string<CharType, Traits, Alloc>;
        //! \brief The concrete string_view type used.
        using string_view_type = std::basic_string_view<CharType, Traits>;
        //! \brief The concrete stream type used.
        using stream_type = std::basic_ostream<CharType, Traits>;
        using parser_type = basic_command_line_parser<CharType, Traits, Alloc>;
        using argument_type = command_line_argument_base<CharType, Traits, Alloc>;

        //! \brief Initializes a new instance of the basic_usage_writer class.
        //!
        //! This instance will write to a line_wrapping_ostream for the standard output stream
        //! and the standard input stream.
        basic_usage_writer()
            : _owned_output{line_wrapping_stream_type::for_cout()},
              _owned_error{line_wrapping_stream_type::for_cerr()},
              output{*_owned_output},
              error{*_owned_error}
        {

        }

        //! \brief Initializes a new instance of the basic_usage_writer class with the specified stream.
        //!
        //! This instance will write both errors and usage to the same stream.
        //!
        //! \param output The stream used for usage help and errors.
        basic_usage_writer(stream_type &output)
            : output{output},
              error{output}
        {
        }

        //! \brief Initializes a new instance of the basic_usage_writer class with the specified output
        //!        and error streams.
        //!
        //! \param output The stream used for usage help.
        //! \param error The stream used for errors.
        basic_usage_writer(stream_type &output, stream_type &error)
            : output{output},
              error{error}
        {
        }

        //! \brief The stream used to write usage help to.
        stream_type &output;

        //! \brief The stream used to write errors to.
        stream_type &error;

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

        bool include_application_description{true};

        //! \brief Indicates whether to include the default value of arguments in the description.
        //! 
        //! The default value is `true`.
        bool include_default_value_in_description{true};

        //! \brief Indicates whether to include the aliases of arguments in the description.
        //! 
        //! The default value is `true`.
        bool include_aliases_in_description{true};

        bool use_abbreviated_syntax{};

        bool blank_line_after_syntax{true};

        virtual void write_parser_usage(const parser_type &parser)
        {
            _parser = &parser;
            write_parser_usage_core();
        }


    protected:
        virtual void write_parser_usage_core()
        {
            if (include_application_description && !parser().description().empty())
            {
                write_application_description(parser().description());
            }

            write_parser_usage_syntax();
        }

        virtual void write_application_description(string_view_type description)
        {
            output << description << std::endl << std::endl;
        }

        virtual void write_parser_usage_syntax()
        {
            output << reset_indent << set_indent(syntax_indent);
            write_usage_syntax_prefix();
            parser().for_each_argument_in_usage_order([this](const auto &arg)
                {
                    output << ' ';
                    if (use_abbreviated_syntax && !arg.position())
                    {
                        write_abbreviated_remaining_arguments();
                        return false;
                    }

                    if (arg.is_required())
                    {
                        write_argument_syntax(arg);
                    }
                    else
                    {
                        write_optional_argument_syntax(arg);
                    }

                    return true;
                });

            output << std::endl;
            if (blank_line_after_syntax)
            {
                output << std::endl;
            }
        }

        virtual void write_usage_syntax_prefix()
        {
            output << "Usage: " << parser().command_name();
        }

        virtual void write_abbreviated_remaining_arguments()
        {
            output << "[arguments]";
        }

        virtual void write_optional_argument_syntax(const argument_type &arg)
        {
            output << c_optionalStart;
            write_argument_syntax(arg);
            output << c_optionalEnd;
        }

        virtual void write_argument_syntax(const argument_type &arg)
        {
            const auto &prefix = parser().prefixes()[0];
            std::optional<CharType> separator = parser().allow_white_space_separator() && use_white_space_value_separator
                ? std::nullopt
                : std::optional<CharType>(parser().argument_value_separator());

            if (arg.position())
            {
                write_positional_argument_name(arg.name(), prefix, separator);
            }
            else
            {
                write_argument_name(arg.name(), prefix);
            }

            if (!arg.is_switch())
            {
                // Otherwise, the separator was included in the argument name.
                if (!arg.position() || !separator)
                {
                    if (separator)
                    {
                        output << *separator;
                    }
                    else
                    {
                        output << ' ';
                    }
                }

                write_value_description(arg.value_description());
            }

            if (arg.is_multi_value())
            {
                write_multi_value_suffix();
            }
        }

        virtual void write_argument_name(string_view_type name, string_view_type prefix)
        {
            output << prefix << name;
        }

        virtual void write_positional_argument_name(string_view_type name, string_view_type prefix, std::optional<CharType> separator)
        {
            output << c_optionalStart;
            write_argument_name(name, prefix);
            if (separator)
            {
                output << *separator;
            }

            output << c_optionalEnd;
        }

        virtual void write_value_description(string_view_type value_description)
        {
            output << '<';
            output << value_description;
            output << '>';
        }

        virtual void write_multi_value_suffix()
        {
            output << "...";
        }

        const parser_type &parser()
        {
            return *_parser;
        }

    private:
        std::optional<line_wrapping_stream_type> _owned_output;
        std::optional<line_wrapping_stream_type> _owned_error;
        const parser_type *_parser{};

        static constexpr char c_optionalStart = '[';
        static constexpr char c_optionalEnd = ']';
    };

    //! \brief Typedef for basic_usage_options using `char` as the character type.
    using usage_writer = basic_usage_writer<char>;
    //! \brief Typedef for basic_usage_options using `wchar_t` as the character type.
    using wusage_writer = basic_usage_writer<wchar_t>;

}

#endif