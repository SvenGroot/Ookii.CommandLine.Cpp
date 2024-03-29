//! \file usage_writer.h
//! \brief Provides the ookii::basic_usage_writer class.
#ifndef OOKII_USAGE_WRITER_H_
#define OOKII_USAGE_WRITER_H_

#pragma once

#include "string_helper.h"
#include "line_wrapping_stream.h"
#include "scope_helper.h"

namespace ookii
{
    template<typename CharType, typename Traits, typename Alloc>
    class basic_command_line_parser;

    template<typename CharType, typename Traits, typename Alloc>
    class basic_command_manager;

    template<typename CharType, typename Traits, typename Alloc>
    class command_info;


    //! \brief Indicates if and how usage is shown if an error occurred parsing the command line.
    enum class usage_help_request
    {
        //! \brief Full usage help is shown, including the argument descriptions.
        full,
        //! \brief Only the usage syntax is shown; the argument descriptions are not. In addition,
        //! the basic_usage_writer::write_more_info_message() message is shown.
        syntax_only,
        //! \brief No usage help is shown. Instead, the
        //! basic_usage_writer::write_more_info_message() message is shown.
        none
    };

    //! \brief Indicates which arguments should be included in the description list when printing usage.
    enum class description_list_filter_mode
    {
        //! \brief Include arguments that have any information that is not included in the syntax,
        //! such as aliases, a default value, or a description.
        information,
        //! \brief Include only arguments that have a description.
        descripion,
        //! \brief Include all arguments.
        all,
        //! \brief Omit the description list entirely.
        none,
    };

    //! \brief Indicates how the arguments in the description list should be sorted.
    enum class description_list_sort_mode
    {
        //! \brief The descriptions are listed in the same order as the usage syntax: first the
        //! positional arguments, then the required named arguments sorted by name, then the
        //! remaining arguments sorted by name.
        usage_order,
        //! \brief The descriptions are listed in alphabetical order by argument name. If the
        //! parsing mode is parsing_mode::long_short, this uses the long name of the argument,
        //! unless the argument has no long name, in which case the short name is used.
        alphabetical,
        //! \brief The same as alphabetical, but in reverse order.
        alphabetical_descending,
        //! \brief The descriptions are listed in alphabetical order by the short argument name. If
        //! the argument has no short name, the long name is used. If the parsing mode is not
        //! parsing_mode::long_short, this has the same effect as alphabetical.
        alphabetical_short_name,
        //! \brief The same as alphabetical_short_name, but in reverse order.
        alphabetical_short_name_descending
    };

    //! \brief Creates usage help for the basic_command_line_parser and basic_command_manager
    //! classes.
    //!
    //! You can derive from this class to override the formatting of various aspects of the usage
    //! help. Pass it to the basic_command_line_parser::parse() method or the methods of the
    //! basic_command_manager class to specify a custom instance.
    //!
    //! Depending on what methods you override, you can change small parts of the formatting, or
    //! completely change how usage looks. Certain methods may not be called if you override the
    //! methods that call them.
    //!
    //! This class has a number of properties that customize the usage help for the base
    //! implementation of this class. It is not guaranteed that a derived class will respect
    //! these properties.
    //!
    //! Two typedefs for common character types are provided:
    //! 
    //! Type                   | Definition
    //! ---------------------- | -------------------------------------
    //! `ookii::usage_writer`  | `ookii::basic_usage_writer<char>`
    //! `ookii::wusage_writer` | `ookii::basic_usage_writer<wchar_t>`
    //! 
    //! \tparam CharType The character type used for strings.
    //! \tparam Traits The character traits to use for strings. Defaults to `std::char_traits<CharType>`.
    //! \tparam Alloc The allocator to use for strings. Defaults to `std::allocator<CharType>`.
    template<typename CharType, typename Traits = std::char_traits<CharType>, typename Alloc = std::allocator<CharType>>
    class basic_usage_writer
    {
        using line_wrapping_stream_type = basic_line_wrapping_ostream<CharType, Traits>;

        std::optional<line_wrapping_stream_type> _owned_output;
        std::optional<line_wrapping_stream_type> _owned_error;

    public:
        //! \brief Provides default values for the fields of basic_usage_writer.
        struct defaults
        {
            //! \brief Default value for basic_usage_writer::name_separator.
            static constexpr auto name_separator = literal_cast<CharType>(", ");
            //! \brief Default value for basic_usage_writer::application_description_indent.
            static constexpr size_t application_description_indent = 0;
            //! \brief Default value for basic_usage_writer::syntax_indent.
            static constexpr size_t syntax_indent = 3;
            //! \brief Default value for basic_usage_writer::argument_description_indent.
            static constexpr size_t argument_description_indent = 8;
            //! \brief Default value for basic_usage_writer::command_description_indent.
            static constexpr size_t command_description_indent = 8;
        };

        //! \brief The concrete string type used.
        using string_type = std::basic_string<CharType, Traits, Alloc>;
        //! \brief The concrete string_view type used.
        using string_view_type = std::basic_string_view<CharType, Traits>;
        //! \brief The concrete stream type used.
        using stream_type = std::basic_ostream<CharType, Traits>;
        //! \brief The concrete basic_command_line_parser type used.
        using parser_type = basic_command_line_parser<CharType, Traits, Alloc>;
        //! \brief The concrete command_line_argument_base type used.
        using argument_type = command_line_argument_base<CharType, Traits, Alloc>;
        //! \brief The concrete basic_command_manager type used.
        using command_manager_type = basic_command_manager<CharType, Traits, Alloc>;
        //! \brief The concrete command_info type used.
        using command_info_type = command_info<CharType, Traits, Alloc>;

        //! \brief Initializes a new instance of the basic_usage_writer class.
        //!
        //! This instance will write to a line_wrapping_ostream for the standard output stream
        //! and the standard input stream.
        //!
        //! \param use_color `true` to enable color output using virtual terminal sequences, `false`
        //!        to disable it, and `std::nullopt` to automatically enable it if supported using
        //!        the ookii::vt::enable_color() method.
        basic_usage_writer(std::optional<bool> use_color = {})
            : _owned_output{line_wrapping_stream_type::for_cout()},
              _owned_error{line_wrapping_stream_type::for_cerr()},
              output{*_owned_output},
              error{*_owned_error},
              _use_color{use_color}
        {
        }

        //! \brief Initializes a new instance of the basic_usage_writer class with the specified stream.
        //!
        //! This instance will write both errors and usage to the same stream.
        //!
        //! \param output The stream used for usage help and errors.
        //! \param use_color `true` to enable color output using virtual terminal sequences, or
        //!        `false` to disable it.
        basic_usage_writer(stream_type &output, bool use_color = false)
            : output{output},
              error{output},
              _use_color{use_color}
        {
        }

        //! \brief Initializes a new instance of the basic_usage_writer class with the specified output
        //!        and error streams.
        //!
        //! \param output The stream used for usage help.
        //! \param error The stream used for errors.
        //! \param use_color `true` to enable color output using virtual terminal sequences, or
        //!        `false` to disable it.
        basic_usage_writer(stream_type &output, stream_type &error, bool use_color = false)
            : output{output},
              error{error},
              _use_color{use_color}
        {
        }

        virtual ~basic_usage_writer()
        {
        }

        //! \brief The stream used to write usage help to.
        stream_type &output;

        //! \brief The stream used to write errors to.
        stream_type &error;

        //! \brief The level of indentation to use when writing the application description.
        //!
        //! Note that the first line of the description is not indented.
        //!
        //! This value has no effect if the output stream is not using a line_wrapping_streambuf.
        size_t application_description_indent{defaults::application_description_indent};

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

        //! \brief A value that indicates which arguments should be included in the list of argument
        //! descriptions.
        //!
        //! The default value is description_list_filter_mode::information.
        description_list_filter_mode argument_description_list_filter{};

        //! \brief A value that indicates the order of the arguments in the list of argument
        //! descriptions.
        //!
        //! The default value is description_list_sort_mode::usage_order.
        description_list_sort_mode argument_description_list_order{};

        //! \brief Indicates whether to use white space as the argument name separator in the usage
        //!        syntax.
        //!
        //! If `false`, the separator specified in basic_parser_builder::argument_name_separator()
        //! is used instead. The default value is `true`.
        //! 
        //! If basic_parser_builder::allow_white_space_separator() is set to `false`, this value has
        //! no effect and the basic_parser_builder::argument_name_separator() is always used.
        bool use_white_space_value_separator{true};

        //! \brief Indicates whether to include the application description in the usage help.
        //! 
        //! The default value is `true`.
        bool include_application_description{true};

        //! \brief Indicates whether to include the default value of arguments in the description.
        //! 
        //! The default value is `true`.
        bool include_default_value_in_description{true};

        //! \brief Indicates whether to include the aliases of arguments in the description.
        //! 
        //! The default value is `true`.
        bool include_aliases_in_description{true};

        //! \brief Indicates whether to list only positional arguments in the usage syntax.
        //! 
        //! The default value is `false`.
        //!
        //! Abbreviated usage syntax only lists the positional arguments explicitly. After that, if
        //! there are any more arguments, it will just call the
        //! write_abbreviated_remaining_arguments() method. The user will have to refer to the
        //! description list to see the remaining possible arguments.
        bool use_abbreviated_syntax{};

        //! \brief Indicates whether to add a blank line after the usage syntax.
        //! 
        //! The default value is `true`.
        bool blank_line_after_syntax{true};

        //! \brief When using parsing_mode::long_short, use short names in the usage syntax.
        //!
        //! This has no effect when not using parsing_mode::long_short.
        bool use_short_names_for_syntax{};

        //! \brief Indicates whether to add a blank line after each argument's description.
        //! 
        //! The default value is `true`.
        bool blank_line_after_description{true};

        //! \brief The separator to use between names of arguments and commands.
        //! 
        //! The default value is ", ".
        string_type name_separator{defaults::name_separator.data()};

        //! \brief The level of indentation to use when writing the command descriptions.
        //!
        //! Note that the first line of the syntax is not indented.
        //!
        //! This value has no effect if the output stream is not using a line_wrapping_streambuf.
        int command_description_indent{defaults::command_description_indent};

        //! \brief Indicates whether to add a blank line after each command's description.
        //! 
        //! The default value is `true`.
        bool blank_line_after_command_description{true};

        //! \brief The color applied by the base implementation of the write_usage_syntax_prefix
        //! method.
        //!
        //! This should be the virtual terminal sequence for a color. The default value is
        //! ookii::vt::text_format::foreground_cyan.
        //!
        //! The color will only be used if the use_color() method returns `true`.
        //!
        //! If the string contains anything other than virtual terminal sequences, those parts will
        //! be included in the output, but only when the use_color() method returns `true`.
        //!
        //! The portion of the string that has color will end with the value of color_reset.
        //!
        //! With the base implementation, only the "Usage:" portion of the string has color; the
        //! executable name does not.
        const char *usage_prefix_color{vt::text_format::foreground_cyan};

        //! \brief The color applied by the base implementation of the
        //! write_argument_description_header() method.
        //!
        //! This should be the virtual terminal sequence for a color. The default value is
        //! ookii::vt::text_format::foreground_green.
        //!
        //! The color will only be used if the use_color() method returns `true`.
        //!
        //! If the string contains anything other than virtual terminal sequences, those parts will
        //! be included in the output, but only when the use_color() method returns `true`.
        //!
        //! The portion of the string that has color will end with the value of color_reset.
        //!
        //! With the base implementation, only the description header has color.
        const char *argument_description_color{vt::text_format::foreground_green};

        //! \brief The color applied by the base implementation of the
        //! write_command_description_header() method.
        //!
        //! This should be the virtual terminal sequence for a color. The default value is
        //! ookii::vt::text_format::foreground_green.
        //!
        //! The color will only be used if the use_color() method returns `true`.
        //!
        //! If the string contains anything other than virtual terminal sequences, those parts will
        //! be included in the output, but only when the use_color() method returns `true`.
        //!
        //! The portion of the string that has color will end with the value of color_reset.
        //!
        //! With the base implementation, only the description header has color.
        const char *command_description_color{vt::text_format::foreground_green};

        //! \brief The color applied by the write_error() method.
        //!
        //! This should be the virtual terminal sequence for a color. The default value is
        //! ookii::vt::text_format::foreground_red.
        //!
        //! The color will only be used if the use_color() method returns `true`.
        //!
        //! If the string contains anything other than virtual terminal sequences, those parts will
        //! be included in the output, but only when the use_color() method returns `true`.
        //!
        //! The portion of the string that has color will end with the value of color_reset.
        const char *error_color{vt::text_format::foreground_red};

        //! \brief The sequence used to reset color applied a usage help element.
        //!
        //! This is the virtual terminal sequence used to reset color. The default value is
        //! ookii::vt::text_format::default_format.
        //!
        //! This value will only be used if the use_color() method returns `true`.
        //!
        //! If the string contains anything other than virtual terminal sequences, those parts will
        //! be included in the output, but only when the use_color() method returns `true`.
        const char *color_reset{vt::text_format::default_format};

        //! \brief Creates usage help for the specified parser.
        //!
        //! \param parser The basic_command_line_parser.
        //! \param request The parts of usage to write.
        virtual void write_parser_usage(const parser_type &parser, usage_help_request request = usage_help_request::full)
        {
            _parser = &parser;
            write_usage_internal(parser.locale(), request);
        }

        //! \brief Creates usage help for the specified command manager.
        //!
        //! \param manager The basic_command_manager.
        virtual void write_command_list_usage(const command_manager_type &manager)
        {
            _command_manager = &manager;
            write_usage_internal(manager.locale());
        }

        //! \brief Writes an error message to the error stream, using color if enabled.
        void write_error(string_view_type message)
        {
            auto support = enable_error_color();
            if (support)
            {
                error << error_color;
            }

            error << message;
            if (support)
            {
                error << color_reset;
            }

            error << std::endl << std::endl;
        }

    protected:
        //! \brief Creates the usage help for a basic_command_line_parser instance.
        //!
        //! This is the primary method used to generate usage help for the basic_command_line_parser
        //! class. It calls into the various other methods of this class, so overriding this method
        //! should not typically be necessary unless you wish to deviate from the order in which
        //! usage elements are written.
        //!
        //! The base implementation writes the application description, followed by the usage
        //! syntax, followed by the class validator help messages, followed by a list of argument
        //! descriptions.
        //! 
        //! \param request The parts of usage to write.
        virtual void write_parser_usage_core(usage_help_request request)
        {
            if (request == usage_help_request::none)
            {
                write_more_info_message();
                return;
            }

            if (request == usage_help_request::full && include_application_description && !parser().description().empty())
            {
                write_application_description(parser().description());
            }

            write_parser_usage_syntax();
            if (request == usage_help_request::full)
            {
                write_argument_descriptions();
            }
            else
            {
                output << reset_indent << set_indent(0);
                write_more_info_message();
            }
        }

        //! \brief Writes the application description, or command description in case of a
        //! subcommand.
        //!
        //! This method is called by the base implementation of the write_parser_usage_core() method
        //! if the parser has a description and the include_application_description field is
        //! `true`.
        virtual void write_application_description(string_view_type description)
        {
            output << set_indent(application_description_indent);
            output << description << std::endl << std::endl;
        }

        //! \brief Writes the usage syntax for the application or subcommand.
        //!
        //! This method is called by the base implementation of the write_parser_usage_core() method.
        virtual void write_parser_usage_syntax()
        {
            output << reset_indent << set_indent(syntax_indent);
            write_usage_syntax_prefix(parser().command_name());
            for (const auto &arg : parser().arguments())
            {
                output << ' ';
                if (use_abbreviated_syntax && !arg.position())
                {
                    write_abbreviated_remaining_arguments();
                    break;
                }

                if (arg.is_required())
                {
                    write_argument_syntax(arg);
                }
                else
                {
                    write_optional_argument_syntax(arg);
                }
            }

            output << std::endl;
            if (blank_line_after_syntax)
            {
                output << std::endl;
            }
        }

        //! \brief Writes the prefix for the usage syntax, including the executable name and, for
        //! subcommands, the command name.
        //!
        //! The base implementation returns a string like "Usage: executable" or "Usage: executable
        //! command"
        //!
        //! If color is enabled, this uses the usage_prefix_color for the "Usage:" portion of the
        //! string.
        //!
        //! This method is called by the base implementation of the write_parser_usage_syntax()
        //! method.
        //!
        //! \param command_name The name of the executable or command.
        virtual void write_usage_syntax_prefix(string_view_type command_name)
        {
            set_color(usage_prefix_color);
            output << "Usage:";
            set_color(color_reset);
            output << " " << command_name;
        }

        //! \brief Writes the string used to indicate there are more arguments if the usage syntax
        //! was abbreviated.
        //!
        //! The base implementation returns a string like "[arguments]".
        //!
        //! This method is called by the base implementation of the write_parser_usage_syntax()
        //! method.
        virtual void write_abbreviated_remaining_arguments()
        {
            output << "[arguments]";
        }

        //! \brief Writes the string used to indicate there are more arguments if the usage syntax
        //! was abbreviated.
        //!
        //! The base implementation surrounds the result of the write_argument_syntax() method in
        //! square brackets.
        //!
        //! This method is called by the base implementation of the write_parser_usage_syntax()
        //! method.
        virtual void write_optional_argument_syntax(const argument_type &arg)
        {
            output << c_optionalStart;
            write_argument_syntax(arg);
            output << c_optionalEnd;
        }

        //! \brief Writes the string used to indicate there are more arguments if the usage syntax
        //! was abbreviated.
        //!
        //! This method is called by the base implementation of the write_parser_usage_syntax() and
        //! write_optional_argument_syntax() methods.
        //!
        //! \param arg The argument.
        virtual void write_argument_syntax(const argument_type &arg)
        {
            string_view_type name;
            CharType short_name;
            if (arg.has_short_name() && use_short_names_for_syntax)
            {
                short_name = arg.short_name();
                name = string_view_type(&short_name, 1);
            }
            else
            {
                name = arg.name();
            }

            const auto &prefix = parser().mode() != parsing_mode::long_short || (arg.has_short_name() && (use_short_names_for_syntax || !arg.has_long_name()))
                ? parser().prefixes()[0]
                : parser().long_prefix();

            std::optional<CharType> separator = parser().allow_white_space_separator() && use_white_space_value_separator
                ? std::nullopt
                : std::optional<CharType>(parser().argument_value_separator());

            if (arg.position())
            {
                write_positional_argument_name(name, prefix, separator);
            }
            else
            {
                write_argument_name(name, prefix);
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

        //! \brief Writes the name of an argument.
        //!
        //! The default implementation returns the prefix followed by the name, e.g. "-Name".
        //!
        //! This method is called by the base implementation of the write_argument_syntax() and
        //! write_positional_argument_name() methods.
        //!
        //! \param name The name of the argument.
        //! \param prefix The prefix of the argument.
        virtual void write_argument_name(string_view_type name, string_view_type prefix)
        {
            output << prefix << name;
        }

        //! \brief Writes the name of a positional argument.
        //!
        //! The base implementation surrounds the result of the write_argument_name() method in
        //! square brackets.
        //!
        //! This method is called by the base implementation of the write_argument_syntax() method.
        //!
        //! \param name The name of the argument.
        //! \param prefix The prefix of the argument.
        //! \param separator The argument name/value separator, or `std::nullopt` if the
        //! use_white_space_value_separator field and the
        //! basic_command_line_parser::allow_white_space_separator() method are both `true`.
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

        //! \brief Writes the value description of an argument.
        //!
        //! The default implementation returns the value description surrounded by angle brackets,
        //! e.g. "<string>"
        //!
        //! This method is called by the base implementation of the write_argument_syntax() method.
        //!
        //! \param value_description The value description of the argument.
        virtual void write_value_description(string_view_type value_description)
        {
            output << '<' << value_description << '>';
        }

        //! \brief Writes a suffix that indicates an argument is a multi-value argument.
        //!
        //! The default implementation returns a string like "...".
        //!
        //! This method is called by the base implementation of the write_argument_syntax() method.
        virtual void write_multi_value_suffix()
        {
            output << "...";
        }

        //! \brief Writes the list of argument descriptions.
        //!
        //! The default implementation calls the write_argument_description() method on each
        //! argument.
        //!
        //! This method is called by the base implementation of the write_parser_usage_core()
        //! method.
        virtual void write_argument_descriptions()
        {
            auto indent = argument_description_indent;
            if (parser().mode() == parsing_mode::long_short)
            {
                // For long/short mode, increase the indentation by the size of the short argument.
                indent += parser().prefixes()[0].length() + name_separator.length() + 1;
            }

            output << set_indent(indent);
            bool first = true;
            for_each_argument_in_description_order([this, &first](const auto &arg)
                {
                    if (first)
                    {
                        write_argument_description_list_header();
                        first = false;
                    }

                    write_argument_description(arg);

                    return true;
                });
        }

        //! \brief Writes a header before the list of argument descriptions.
        //!
        //! The base implementation does not write anything, as a header is not used in the default
        //! format.
        //!
        //! This method is called by the base implementation of the write_argument_descriptions()
        //! method before the first argument.
        virtual void write_argument_description_list_header()
        {
            // Intentionally blank.
        }

        //! \brief Writes the description of a single argument.
        //!
        //! The base implementation calls the write_argument_description_header() method, the
        //! write_argument_description_body() method, and then adds an extra blank line if the
        //! blank_line_after_description field is `true`.
        //!
        //! This method is called by the base implementation of the write_argument_descriptions()
        //! method.
        //!
        //! \param arg The argument.
        virtual void write_argument_description(const argument_type &arg)
        {
            write_argument_description_header(arg);
            write_argument_description_body(arg);

            if (blank_line_after_description)
            {
                output << std::endl;
            }
        }

        //! \brief Writes the header of an argument's description, which is usually the name and
        //! value description.
        //!
        //! The base implementation writes the name(s), value description, and alias(es), ending
        //! with a new line. Which elements are included can be influenced using the fields of this
        //! class.
        //!
        //! If color is enabled, the base implementation will use the argument_description_color.
        //!
        //! This method is called by the base implementation of the write_argument_description()
        //! method.
        //!
        //! \param arg The argument.
        virtual void write_argument_description_header(const argument_type &arg)
        {
            output << reset_indent;
            write_spacing(argument_description_indent / 2);
            const auto &short_prefix = parser().prefixes()[0];
            const auto &prefix = parser().long_prefix().length() == 0
                ? short_prefix
                : parser().long_prefix();

            set_color(argument_description_color);
            if (parser().mode() == parsing_mode::long_short)
            {
                if (arg.has_short_name())
                {
                    auto short_name = arg.short_name();
                    write_argument_name_for_description(string_view_type{&short_name, 1}, short_prefix);
                    if (arg.has_long_name())
                    {
                        output << name_separator;
                    }
                }
                else
                {
                    write_spacing(short_prefix.length() + name_separator.length() + 1);
                }

                if (arg.has_long_name())
                {
                    write_argument_name_for_description(arg.name(), prefix);
                }
            }
            else
            {
                write_argument_name_for_description(arg.name(), prefix);
            }

            output << ' ';
            if (arg.is_switch())
            {
                write_switch_value_description(arg.value_description());
            }
            else
            {
                write_value_description_for_description(arg.value_description());
            }

            if (include_aliases_in_description)
            {
                write_aliases(arg.aliases(), arg.short_aliases(), prefix, short_prefix);
            }

            set_color(color_reset);
            output << std::endl;
        }

        //! \brief Writes the body of an argument description, which is usually the description
        //! itself with any supplemental information.
        //!
        //! The base implementation writes the description text and the default value, followed by
        //! two new lines. Which elements are included can be influenced using the fields of this
        //! class.
        //!
        //! This method is called by the base implementation of the write_argument_description()
        //! method.
        //!
        //! \param arg The argument.
        virtual void write_argument_description_body(const argument_type &arg)
        {
            if (!arg.description().empty())
            {
                write_argument_description(arg.description());
            }

            if (include_default_value_in_description && arg.has_default_value())
            {
                write_default_value(arg);
            }

            output << std::endl;
        }

        //! \brief Writes the name or alias of an argument for use in the argument description list.
        //!
        //! The default implementation returns the prefix followed by the name, e.g. "-Name".
        //!
        //! This method is called by the base implementation of the
        //! write_argument_description_header() method.
        //!
        //! \param name The name of the argument.
        //! \param prefix The prefix of the argument.
        virtual void write_argument_name_for_description(string_view_type name, string_view_type prefix)
        {
            output << prefix << name;
        }

        //! \brief Writes the value description of an argument for use in the argument description
        //! list.
        //!
        //! The default implementation returns the value description surrounded by angle brackets,
        //! e.g. "<string>"
        //!
        //! This method is called by the base implementation of the
        //! write_argument_description_header() and write_switch_value_description() methods.
        //!
        //! \param value_description The value description.
        virtual void write_value_description_for_description(string_view_type value_description)
        {
            output << '<' << value_description << '>';
        }

        //! \brief Writes the value description of a switch argument for use in the argument
        //! description list.
        //!
        //! The default implementation surrounds the value written by the
        //! write_value_description_for_description() method with angle brackets, to indicate that
        //! it is optional.
        //!
        //! This method is called by the base implementation of the
        //! write_argument_description_header() methods.
        //!
        //! \param value_description The value description.
        virtual void write_switch_value_description(string_view_type value_description)
        {
            output << c_optionalStart;
            write_value_description_for_description(value_description);
            output << c_optionalEnd;
        }

        //! \brief Writes the aliases of an argument for use in the argument description list.
        //!
        //! The base implementation writes a list of the aliases, surrounded by parentheses, and
        //! preceded by a single space. For example, " (-Alias1, -Alias2)".
        //!
        //! If there are no aliases at all, it writes nothing.
        //!
        //! This method is called by the base implementation of the
        //! write_argument_description_header() method.
        //!
        //! \param aliases A list of the aliases.
        //! \param short_aliases A list of the short aliases.
        //! \param prefix The argument name prefix to use.
        //! \param short_prefix The prefix to use for short argument names.
        virtual void write_aliases(const std::vector<string_type> &aliases, const std::vector<CharType> &short_aliases,
            string_view_type prefix, string_view_type short_prefix)
        {
            bool first = true;
            for (const auto &alias : short_aliases)
            {
                if (first)
                {
                    output << " (";
                    first = false;
                }
                else
                {
                    output << name_separator;
                }

                write_alias(string_view_type{&alias, 1}, short_prefix);
            }

            for (const auto &alias : aliases)
            {
                if (first)
                {
                    output << " (";
                    first = false;
                }
                else
                {
                    output << name_separator;
                }

                write_alias(alias, prefix);
            }

            if (!first)
            {
                output << ")";
            }
        }

        //! \brief Writes a single alias of an argument for use in the argument description list.
        //!
        //! The base implementation calls the write_argument_description() method.
        //!
        //! This method is called by the base implementation of the write_aliases() method.
        //!
        //! \param alias The name of the argument.
        //! \param prefix The prefix of the argument.
        virtual void write_alias(string_view_type alias, string_view_type prefix)
        {
            write_argument_name_for_description(alias, prefix);
        }

        //! \brief Writes the actual argument description text.
        //!
        //! The base implementation just writes the description text.
        //!
        //! This method is called by the base implementation of the
        //! write_argument_description_body() method.
        //!
        //! \param description The description of the argument.
        virtual void write_argument_description(string_view_type description)
        {
            output << description;
        }

        //! \brief Writes the default value of an argument.
        //!
        //! The base implementation writes a string like " Default value: value.", including the
        //! leading space.
        //!
        //! This method is called by the base implementation of the
        //! write_argument_description_body() method if the include_default_value_in_description
        //! field is true and the argument has a default value.
        //!
        //! \param arg The argument.
        virtual void write_default_value(const argument_type &arg)
        {
            output << " Default value: ";
            arg.write_default_value(output);
            output << '.';
        }

        //! \brief Writes a message telling to user how to get more detailed help.
        //! 
        //! The default implementation writes a message like "Run 'executable -Help' for more
        //! information." or "Run 'executable command -Help' for more information."
        //!
        //! If the basic_command_line_parser::get_help_argument() method returns `nullptr`, nothing
        //! is written.
        //! 
        //! This method is called by the base implementation of the write_parser_usage_core() method
        //! if the requested help is not `usage_help_request::full`.
        virtual void write_more_info_message()
        {
            auto arg = parser().get_help_argument();
            if (arg != nullptr)
            {
                output << "Run '" << parser().command_name() << " " << arg->name_with_prefix(parser()) << "' for more information."
                    << std::endl;
            }
        }

        //! \brief Creates the usage help for a basic_command_manager instance.
        //!
        //! This is the primary method used to generate usage help for the
        //! basic_command_manager class. It calls into the various other methods of this
        //! class, so overriding this method should not typically be necessary unless you wish to
        //! deviate from the order in which usage elements are written.
        //!
        //! The base implementation writes the application description, followed by the list of
        //! commands, followed by a message indicating how to get help on a command. Which elements
        //! are included exactly can be influenced by the properties of this class.
        virtual void write_command_list_usage_core()
        {
            if (!command_manager().description().empty())
            {
                write_application_description(command_manager().description());
            }

            output << reset_indent << set_indent(syntax_indent);
            write_command_list_usage_syntax();
            output << reset_indent << set_indent(0);
            write_available_commands_header();
            write_command_descriptions();
            output << reset_indent << set_indent(0);
            write_command_list_usage_footer();
        }

        //! \brief Writes the usage syntax for an application using subcommands.
        //!
        //! The base implementation calls write_usage_syntax_prefix(), and adds to it a string like
        //! " &lt;command&gt; [arguments]".
        //!
        //! This method is called by the base implementation of the write_command_list_usage_core()
        //! method.
        virtual void write_command_list_usage_syntax()
        {
            write_usage_syntax_prefix(command_manager().application_name());
            output << " <command> [arguments]" << std::endl;
            if (blank_line_after_syntax)
            {
                output << std::endl;
            }
        }

        //! \brief Writes the usage syntax for an application using subcommands.
        //!
        //! The base implementation writes a string like "The following commands are available:"
        //! followed by a blank line.
        //!
        //! This method is called by the base implementation of the write_command_list_usage_core()
        //! method.
        virtual void write_available_commands_header()
        {
            output << "The following commands are available:" << std::endl << std::endl;
        }

        //! \brief Writes a list of available commands.
        //!
        //! The base implementation calls write_command_description() for all commands.
        //!
        //! This method is called by the base implementation of the write_command_list_usage_core()
        //! method.
        virtual void write_command_descriptions()
        {
            output << set_indent(command_description_indent);
            for (const auto &command : command_manager().commands())
            {
                write_command_description(command);
            }
        }

        //! \brief Writes the description of a command.
        //!
        //! The base implementation calls the write_command_description_header() method, the
        //! write_command_description_body() method, and then adds an extra blank line if the
        //! blank_line_after_command_description field is `true`.
        //!
        //! This method is called by the base implementation of the write_command_descriptions()
        //! method.
        //!
        //! \param command The command.
        virtual void write_command_description(const command_info_type &command)
        {
            write_command_description_header(command);
            write_command_description_body(command);

            if (blank_line_after_command_description)
            {
                output << std::endl;
            }
        }

        //! \brief Writes the header of a command's description, which is typically the name of the
        //! command.
        //!
        //! The base implementation writes the command's name.
        //!
        //! If color is enabled, the base implementation will use the command_description_color.
        //!
        //! This method is called by the base implementation of the write_command_description()
        //! method.
        //!
        //! \param command The command.
        virtual void write_command_description_header(const command_info_type &command)
        {
            output << reset_indent;
            write_spacing(command_description_indent / 2);
            set_color(command_description_color);
            write_command_name(command.name());
            set_color(color_reset);
            output << std::endl;
        }

        //! \brief Writes the body of a command's description, which is typically the description
        /// of the command.
        //!
        //! The base implementation writes the command's description, followed by a newline.
        //!
        //! This method is called by the base implementation of the write_command_description()
        //! method.
        //!
        //! \param command The command.
        virtual void write_command_description_body(const command_info_type &command)
        {
            if (!command.description().empty())
            {
                write_command_description(command.description());
                output << std::endl;
            }
        }

        //! \brief Writes the name of a command.
        //!
        //! The base implementation just writes the name.
        //!
        //! This method is called by the base implementation of the
        //! write_command_description_header() method.
        //!
        //! \param name The name of the command.
        virtual void write_command_name(string_view_type name)
        {
            output << name;
        }

        //! \brief Writes the description of a command.
        //!
        //! The base implementation just writes the description.
        //!
        //! This method is called by the base implementation of the write_command_description_body()
        //! method if the command has a description.
        //!
        //! \param description The description of the command.
        virtual void write_command_description(string_view_type description)
        {
            output << description;
        }

        //! \brief Writes a footer after the list of commands.
        //! 
        //! The base implementation writes an instruction on how to get help for a command if the
        //! basic_command_manager::common_help_argument() method returns a non-empty string.
        virtual void write_command_list_usage_footer()
        {
            if (!command_manager().common_help_argument().empty())
            {
                // If there wasn't already a blank line after the last command, add one now.
                if (!blank_line_after_command_description)
                {
                    output << std::endl;
                }

                output << "Run '" << command_manager().application_name() << " <command> " <<
                    command_manager().common_help_argument() << "' for more information about a command." << std::endl;
            }
        }

        //! \brief Writes the specified amount of spaces.
        //! \param count The number of spaces.
        virtual void write_spacing(size_t count)
        {
            for (size_t i = 0; i < count; ++i)
            {
                output << ' ';
            }
        }

        //! \brief Gets the basic_command_line_parser that usage is being written for.
        //!
        //! This method is not safe to call except during an invocation of the write_parser_usage()
        //! method, and the methods it calls.
        const parser_type &parser() const
        {
            return *_parser;
        }

        //! \brief Gets the basic_command_manager that usage is being written for.
        //!
        //! This method is not safe to call except during an invocation of the
        //! write_command_list_usage() method, and the methods it calls.
        const command_manager_type &command_manager() const
        {
            return *_command_manager;
        }

        //! \brief Gets a value that indicates whether color virtual terminal sequences can be used
        //! in the output.
        //!
        //! \return `true` if color output is allowed; otherwise, `false`.
        bool use_color() const
        {
            return _use_color && *_use_color;
        }

        //! \brief Writes the specified color virtual terminal sequence, if color is enabled.
        //! \param color One or more virtual terminal sequences for colors.
        void set_color(const char *color)
        {
            if (use_color())
            {
                output << color;
            }
        }

        //! \brief Iterates over the arguments in the order set by the 
        //! argument_description_list_order field, including only the arguments selected by the
        //! argument_description_list_filter field.
        //! \param f A function to invoke for each argument.
        template<typename Func>
        void for_each_argument_in_description_order(Func f) const
        {
            if (argument_description_list_filter == description_list_filter_mode::none)
            {
                return;
            }

            if (argument_description_list_order == description_list_sort_mode::usage_order)
            {
                for (const auto &arg : parser().arguments())
                {
                    if (check_filter(arg))
                    {
                        f(arg);
                    }
                }

                return;
            }

            std::vector<const argument_type *> arguments;
            arguments.reserve(parser().argument_count());
            auto source = parser().arguments();
            std::transform(source.begin(), source.end(), std::back_inserter(arguments), [](const auto &arg) { return &arg; });
            std::sort(arguments.begin(), arguments.end(), get_sort_function());
            for (const auto &arg : arguments)
            {
                if (check_filter(*arg))
                {
                    f(*arg);
                }
            }
        }

    private:

        bool check_filter(const argument_type &arg) const
        {
            switch (argument_description_list_filter)
            {
            case description_list_filter_mode::information:
                return has_information(arg);

            case description_list_filter_mode::descripion:
                return !arg.description().empty();

            case description_list_filter_mode::all:
                return true;

            default:
                return false;
            }
        }

        bool has_information(const argument_type &arg) const
        {
            if (!arg.description().empty())
            {
                return true;
            }

            if (use_abbreviated_syntax && arg.position())
            {
                return true;
            }

            if (parser().mode() == parsing_mode::long_short && use_short_names_for_syntax)
            {
                if (arg.has_long_name())
                {
                    return true;
                }
            }
            else if (arg.has_short_name())
            {
                return true;
            }

            if (include_aliases_in_description && (!arg.aliases().empty() || !arg.short_aliases().empty()))
            {
                return true;
            }

            if (include_default_value_in_description && arg.has_default_value())
            {
                return true;
            }

            return false;
        }

        std::function<bool(const argument_type *, const argument_type *)> get_sort_function() const
        {
            auto comparer = parser().argument_comparer();
            switch (argument_description_list_order)
            {
            case description_list_sort_mode::alphabetical:
                return [comparer](const argument_type *left, const argument_type *right)
                {
                    return comparer(left->name(), right->name());
                };

            case description_list_sort_mode::alphabetical_descending:
                return [comparer](const argument_type *left, const argument_type *right)
                {
                    return comparer(right->name(), left->name());
                };

            case description_list_sort_mode::alphabetical_short_name:
                return [comparer](const argument_type *left, const argument_type *right)
                {
                    return comparer(left->short_or_long_name(), right->short_or_long_name());
                };

            case description_list_sort_mode::alphabetical_short_name_descending:
                return [comparer](const argument_type *left, const argument_type *right)
                {
                    return comparer(right->short_or_long_name(), left->short_or_long_name());
                };

            default:
                throw std::logic_error("Invalid sort mode.");
            }
        }

        void write_usage_internal(const std::locale &loc, usage_help_request request = usage_help_request::full)
        {
            auto old_output_loc = output.imbue(loc);
            std::optional<std::locale> old_error_loc;
            details::scope_exit revert{[this, &old_output_loc, &old_error_loc]()
                {
                    _parser = nullptr;
                    if (old_error_loc)
                    {
                        error.imbue(*old_error_loc);
                    }

                    output.imbue(old_output_loc);
                }
            };

            if (std::addressof(output) != std::addressof(error))
            {
                old_error_loc = error.imbue(loc);
            }

            auto color = enable_color();
            output << set_indent(0) << reset_indent;
            if (_parser != nullptr)
            {
                write_parser_usage_core(request);
            }
            else
            {
                write_command_list_usage_core();
            }
        }

        vt::virtual_terminal_support enable_color()
        {
            if (!_use_color)
            {
                auto support = vt::virtual_terminal_support::enable_color(standard_stream::output);
                _use_color = support.is_supported();
                return support;
            }

            return {};
        }

        vt::virtual_terminal_support enable_error_color()
        {
            if (!_use_color)
            {
                return vt::virtual_terminal_support::enable_color(standard_stream::error);
            }

            return {};
        }

        const parser_type *_parser{};
        const command_manager_type *_command_manager{};
        std::optional<bool> _use_color;

        static constexpr char c_optionalStart = '[';
        static constexpr char c_optionalEnd = ']';
    };

    //! \brief Typedef for basic_usage_writer using `char` as the character type.
    using usage_writer = basic_usage_writer<char>;
    //! \brief Typedef for basic_usage_writer using `wchar_t` as the character type.
    using wusage_writer = basic_usage_writer<wchar_t>;

}

#endif