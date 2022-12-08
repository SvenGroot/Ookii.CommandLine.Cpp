//! \file command_line_parser.h
//!
//! \brief Provides the ookii::basic_command_line_parser class.
//! 
//! This file contains the core functionality of the argument parsing library.
#ifndef OOKII_COMMAND_LINE_PARSER_H_
#define OOKII_COMMAND_LINE_PARSER_H_

#pragma once

#include <iterator>
#include <map>
#include "command_line_argument.h"
#include "usage_writer.h"
#include "parse_result.h"
#include "owned_or_borrowed_ptr.h"
#include "range_helper.h"

//! \brief Namespace containing the core Ookii.CommandLine.Cpp types.
//!
//! This namespace defines all the types needed to use Ookii.CommandLine.Cpp, including
//! basic_command_line_parser and basic_parser_builder. It also defines utility types such as
//! basic_line_wrapping_ostream.
namespace ookii
{
    namespace details
    {
#ifdef _UNICODE
        using default_char_type = wchar_t;
#else
        using default_char_type = char;
#endif

        template<typename CharType, typename Traits, typename Alloc>
        struct parser_storage
        {
            using string_type = std::basic_string<CharType, Traits, Alloc>;
            using string_provider_type = basic_localized_string_provider<CharType, Traits, Alloc>;

            parser_storage(string_type command_name, string_provider_type *string_provider)
                : command_name{command_name},
                  string_provider{string_provider}
            {
            }

            string_type command_name;
            string_type description;
            std::vector<string_type> prefixes;
            std::locale locale;
            CharType argument_value_separator{':'};
            bool allow_white_space_separator{true};
            bool allow_duplicate_arguments{false};
            string_provider_type *string_provider;
        };
    }

    //! \brief Value to be returned from the callback passed to the basic_command_line_parser::on_parsed()
    //!        method.
    enum class on_parsed_action
    {
        //! \brief Don't take any special action.
        none,
        //! \brief Cancel parsing immediately, disregarding the rest of the command line. Parsing
        //!        will return with parse_error::parsing_cancelled.
        cancel_parsing,
        //! \brief Continue parsing even if command_line_argument::cancel_parsing() returns `true`.
        always_continue
    };

    //! \brief Parses command line arguments into strongly-typed values.
    //!
    //! The basic_command_line_parser class can parse a set of command line arguments in string form
    //! into a set of typed values. Which arguments are accepted and where their values will be
    //! stored is defined by the basic_parser_builder that created the basic_command_line_parser instance.
    //! 
    //! In addition, this class can generate detailed usage help for the defined arguments, which
    //! can be shown to the user in case an argument parsing error happens or help is requested.
    //! 
    //! To parse arguments, call one of the overloads of the parse() method. Those overloads taking
    //! a basic_usage_writer will handle any errors and print the error as well as usage information
    //! to the console. Most commonly, the overload you'll want to use is parse(int argc, const CharType *const argv[], usage_writer_type *usage).
    //! 
    //! Two typedefs for common character types are provided:
    //! 
    //! Type                          | Definition
    //! ----------------------------- | -------------------------------------
    //! `ookii::command_line_parser`  | `ookii::basic_command_line_parser<char>`
    //! `ookii::wcommand_line_parser` | `ookii::basic_command_line_parser<wchar_t>`
    //! 
    //! \tparam CharType The character type used for arguments and other strings. Only `char` and
    //!         `wchar_t` are supported. Defaults to `wchar_t` if `_UNICODE` is defined, otherwise
    //!         to `char`.
    //! \tparam Traits The character traits to use for strings. Defaults to `std::char_traits<CharType>`.
    //! \tparam Alloc The allocator to use for strings. Defaults to `std::allocator<CharType>`.
    template<typename CharType = details::default_char_type, typename Traits = std::char_traits<CharType>, typename Alloc = std::allocator<CharType>>
    class basic_command_line_parser
    {
    public:
        //! \brief The specialized type of command_line_argument_base used.
        using argument_base_type = command_line_argument_base<CharType, Traits, Alloc>;
        //! \brief The specialized type of `std::basic_string` used.
        using string_type = typename argument_base_type::string_type;
        //! \brief The specialized type of `std::basic_string_view` used.
        using string_view_type = std::basic_string_view<CharType, Traits>;
        //! \brief The specialized type of parse_result used.
        using result_type = parse_result<CharType, Traits, Alloc>;
        //! \brief The specialized type of basic_usage_writer used.
        using usage_writer_type = basic_usage_writer<CharType, Traits, Alloc>;
        //! \brief The specialized type of parser parameter storage used. For internal use.
        using storage_type = details::parser_storage<CharType, Traits, Alloc>;
        //! \brief The callback function type for on_parsed().
        using on_parsed_callback = std::function<on_parsed_action(argument_base_type &, string_view_type value)>;
        using string_provider_type = basic_localized_string_provider<CharType, Traits, Alloc>;

        //! \brief The specialized type of command_line_argument used.
        //!
        //! \tparam T The type of the argument.
        template<typename T>
        using argument_type = command_line_argument<T, CharType, Traits, Alloc>;

        //! \brief The specialized type of the custom command line converter function used.
        //!
        //! \tparam T The type of the argument.
        template<typename T>
        using converter_type = typename argument_type<T>::typed_storage_type::converter_type;

        //! \brief Gets the default prefixes accepted by the parser.
        //!
        //! By default, the parser accepts '/' and '-' on Windows, and only '-' on other systems.
        //! 
        //! Which prefixes are accepted can be changed using basic_parser_builder::prefixes().
        static constexpr std::vector<string_type> get_default_prefixes()
        {
            std::vector<string_type> result;

#ifdef _WIN32
            result.reserve(2);
#else
            result.reserve(1);
#endif

            constexpr auto prefix1 = literal_cast<CharType>("-");
            result.push_back(prefix1.data());

#ifdef _WIN32
            constexpr auto prefix2 = literal_cast<CharType>("/");
            result.push_back(prefix2.data());
#endif

            return result;
        }

        //! \brief Creates a new instance of the basic_command_line_parser class.
        //!
        //! \warning This constructor should not be used directly; instead, use the basic_parser_builder
        //! class to create instances.
        //! 
        //! \tparam Range The type of a range containing basic_parser_builder::argument_builder
        //!         instances.
        //! \param arguments A range containing basic_parser_builder::argument_builder instances.
        //! \param storage Parameters for the basic_command_line_parser.
        //! \param case_sensitive Indicates whether command line argument names are case sensitive.
        template<typename Range>
        basic_command_line_parser(const Range &arguments, storage_type &&storage, bool case_sensitive)
            : _storage{std::move(storage)},
              _arguments{string_less{case_sensitive, storage.locale}}
        {
            if (_storage.string_provider == nullptr)
            {
                _storage.string_provider = &_default_string_provider;
            }

            for (const auto &argument : arguments)
            {
                auto actual_arg = argument->to_argument();
                auto name = actual_arg->name();
                const auto [it, success] = _arguments.insert(std::pair{name, std::move(actual_arg)});
                if (!success)
                    throw std::logic_error("Duplicate argument name.");

                if (it->second->position())
                {
                    // Usually, positional arguments should be in order in the builder, but it's
                    // technically possible for the caller to have stored references to the argument
                    // builders and called positional() on them in a different order than they were
                    // created, so make sure the resulting list is ordered correctly regardless.
                    const auto pos = std::lower_bound(_positional_arguments.begin(),
                        _positional_arguments.end(),
                        it->second,
                        [](const auto &left, const auto &right)
                        {
                            return *left->position() < *right->position();
                        });

                    _positional_arguments.insert(pos, it->second.get());
                }

                for (const auto &alias : it->second->aliases())
                {
                    // Add the aliases to the arguments list using borrowed pointers.
                    const auto [alias_it, alias_success] = _arguments.insert(std::pair{alias, it->second.as_borrowed()});
                    if (!alias_success)
                        throw std::logic_error("Duplicate argument name.");
                }
            }
        }

        //! \brief Returns the command name used when generating usage help.
        //!
        //! This value is set by the basic_parser_builder::basic_parser_builder() constructor.
        const string_type &command_name() const noexcept
        {
            return _storage.command_name;
        }

        //! \brief Returns the description used when generating usage help.
        //!
        //! This value is set by basic_parser_builder::description().
        const string_type &description() const noexcept
        {
            return _storage.description;
        }

        //! \brief Indicates whether argument names and values can be separated by white space.
        //!
        //! This value is set by basic_parser_builder::allow_white_space_separator()
        bool allow_white_space_separator() const noexcept
        {
            return _storage.allow_white_space_separator;
        }

        //! \brief Indicates whether duplicate arguments are allowed.
        //!
        //! This value is set by basic_parser_builder::allow_duplicate_arguments()
        bool allow_duplicate_arguments() const noexcept
        {
            return _storage.allow_duplicate_arguments;
        }

        //! Indicates the non-whitespace separator used to separaet argument names and values.
        //!
        //! This value is set by basic_parser_builder::argument_value_separator()
        CharType argument_value_separator() const noexcept
        {
            return _storage.argument_value_separator;
        }

        //! \brief Gets a list of all the argument name prefixes accepted by the parser.
        //!
        //! By default, the parser accepts '/' and '-' on Windows, and only '-' on other systems.
        //! 
        //! Which prefixes are accepted can be changed using basic_parser_builder::prefixes().
        const std::vector<string_type> &prefixes() const noexcept
        {
            return _storage.prefixes;
        }

        //! \brief Gets the locale used to parse argument values and to format strings.
        //!
        //! This value is set by basic_parser_builder::locale()
        const std::locale &locale() const noexcept
        {
            return _storage.locale;
        }

        //! \brief Gets a view of all the arguments defined by the parser.
        //!
        //! \warning On some versions of Clang, calling this function may cause compiler errors.
        //! 
        //! The arguments will be returned in alphabetical order.
        auto arguments() const
        {
            return details::range_filter<const argument_base_type&, typename std::map<string_type, owned_or_borrowed_ptr<argument_base_type>, string_less>::const_iterator>{_arguments.begin(), _arguments.end(),
                [](const auto &a) -> auto&
                {
                    return *a.second;
                },
                [](const auto &a)
                {
                    // Borrowed pointers are aliases and shouldn't be returned here.
                    return a.second.is_owned();
                }
            };
        }

        //! \brief Gets the number of positional arguments.
        //!
        //! Positional arguments are created using basic_parser_builder::argument_builder::positional().
        size_t positional_argument_count() const
        {
            return _positional_arguments.size();
        }

        //! \brief Gets an argument by position.
        //! 
        //! \param pos The argument's position.
        //!
        //! Only positional arguments can be retrieved using this method.
        //! 
        //! Positional arguments are created using basic_parser_builder::argument_builder::positional().
        //! 
        //! \exception std::out_of_range There is no argument at the specified position.
        const argument_base_type &get_argument(size_t pos) const
        {
            return *_positional_arguments.at(pos);
        }

        //! \brief Gets an argument by name.
        //! 
        //! \param name The argument's name or alias.
        //!
        //! Both the argument's main name and any of its aliases can be used.
        //! 
        //! \exception std::out_of_range There is no argument with the specified name or alias.
        const argument_base_type &get_argument(const string_type &name) const
        {
            return *_arguments.at(name);
        }

        //! \brief Parses the arguments in the range specified by the iterators.
        //!
        //! \warning The range indicated by begin, end should *not* include the application name.
        //! 
        //! \tparam Iterator The type of the iterator. This type must meet the requirements of a
        //!         forward_iterator.
        //! \param begin An iterator pointing to the first argument.
        //! \param end An iterator pointing to directly after the last argument.
        //! 
        //! \return A parse_result that indicates whether the operation was successful.
        template<typename Iterator>
        [[nodiscard]] result_type parse(Iterator begin, Iterator end)
        {
            for (auto &arg : _arguments)
                arg.second->reset();

            size_t position = 0;
            for (auto current = begin; current != end; ++current)
            {
                auto arg = *current;
                auto prefix_length = check_prefix(arg);
                if (prefix_length > 0)
                {
                    // Current is updated if parsing used the next argument for a value.
                    auto result = parse_named_argument(arg, current, end, prefix_length);
                    if (!result)
                        return result;
                }
                else
                {
                    // If this is a multi-value argument then we're on the last argument, otherwise search for the next argument without a value
                    // Skip positional arguments that have already been specified by name
                    while (position < _positional_arguments.size() && 
                        !_positional_arguments[position]->is_multi_value() &&
                        _positional_arguments[position]->has_value())
                    {
                        ++position;
                    }

                    if (position >= _positional_arguments.size())
                        return {*_storage.string_provider, parse_error::too_many_arguments};

                    auto result = set_argument_value(*_positional_arguments[position], arg);
                    if (!result)
                        return result;
                }
            }

            result_type result{*_storage.string_provider};
            for_each_argument_in_usage_order([this, &result](auto &arg)
                {
                    if (arg.is_required())
                    {
                        if (!arg.has_value())
                        {
                            result = {*_storage.string_provider, parse_error::missing_required_argument, arg.name()};
                            return false;
                        }
                    }
                    else
                    {
                        arg.apply_default_value();
                    }

                    return true;
                });

            return result;
        }

        //! \brief Parses the arguments in the range specified by the iterators, and writes error
        //!        and usage information to the console if a parsing error occurs.
        //!
        //! \warning The range indicated by begin, end should *not* include the application name.
        //! 
        //! \tparam Iterator The type of the iterator. This type must meet the requirements of a
        //!         forward_iterator.
        //! \param begin An iterator pointing to the first argument.
        //! \param end An iterator pointing to directly after the last argument.
        //! \param usage The basic_usage_writer used for writing errors and usage help in the
        //!        case of an error. Use `{}` to specify a default usage writer.
        //! 
        //! \return A parse_result that indicates whether the operation was successful.
        template<typename Iterator>
        result_type parse(Iterator begin, Iterator end, usage_writer_type *usage)
        {
            auto result = parse(begin, end);
            handle_error(result, usage);
            return result;
        }

        //! \brief Parses the arguments in the specified range.
        //!
        //! \warning The range should *not* include the application name.
        //! 
        //! \tparam Range The type of the range. This type must define global begin() and end()
        //!         functions.
        //! \param range A range containing the arguments.
        //! 
        //! \return A parse_result that indicates whether the operation was successful.
        template<typename Range>
        result_type parse(const Range &range)
        {
            // Using allows ADL.
            using std::begin;
            using std::end;
            return parse(begin(range), end(range));
        }

        //! \brief Parses the arguments in the specified range, and writes error and usage
        //!        information to the console if a parsing error occurs.
        //!
        //! \warning The range should *not* include the application name.
        //! 
        //! \tparam Range The type of the range. This type must define global begin() and end()
        //!         functions.
        //! \param range A range containing the arguments.
        //! \param usage The basic_usage_writer used for writing errors and usage help in the
        //!        case of an error. Use `{}` to specify a default usage writer.
        //! 
        //! \return A parse_result that indicates whether the operation was successful.
        template<typename Range>
        result_type parse(Range range, usage_writer_type *usage)
        {
            auto result = parse(range);
            handle_error(result, usage);
            return result;
        }

        //! \brief Parses the arguments in the specified initializer list.
        //!
        //! \warning The list should *not* include the application name.
        //! 
        //! \tparam T The type of the elements in the initializer list. This must be a string type
        //!         that can be converted to std::basic_string<CharType, Traits, Alloc>.
        //! \param args The arguments.
        //! 
        //! \return A parse_result that indicates whether the operation was successful.
        template<typename T>
        result_type parse(std::initializer_list<T> args)
        {
            return parse(args.begin(), args.end());
        }

        //! \brief Parses the arguments in the specified initializer list, and writes error
        //!        and usage information to the console if a parsing error occurs.
        //!
        //! \warning The list should *not* include the application name.
        //! 
        //! \tparam T The type of the elements in the initializer list. This must be a string type
        //!         that can be converted to std::basic_string<CharType, Traits, Alloc>.
        //! \param args The arguments.
        //! \param usage The basic_usage_writer used for writing errors and usage help in the
        //!        case of an error. Use `{}` to specify a default usage writer.
        //! 
        //! \return A parse_result that indicates whether the operation was successful.
        template<typename T>
        result_type parse(std::initializer_list<T> args, usage_writer_type *usage)
        {
            auto result = parse(args);
            handle_error(result, usage);
            return result;
        }

        //! \brief Parses the provided arguments.
        //!
        //! \warning It's assumed that `argv[0]` contains the application name, so this value is
        //!          skipped when parsing.
        //! 
        //! \param argc The number of arguments.
        //! \param argv An array containing the arguments.
        //! 
        //! \return A parse_result that indicates whether the operation was successful.
        result_type parse(int argc, const CharType *const argv[])
        {
            if (argc < 1)
                return parse<const CharType *>({});
            else
                return parse(argv + 1, argv + argc);
        }

        //! \brief Parses the provided arguments, and writes error and usage information to the
        //!        console if a parsing error occurs.
        //!
        //! \warning It's assumed that `argv[0]` contains the application name, so this value is
        //!          skipped when parsing.
        //! 
        //! \param argc The number of arguments.
        //! \param argv An array containing the arguments.
        //! \param usage The basic_usage_writer used for writing errors and usage help in the
        //!        case of an error. Use `{}` to specify a default usage writer.
        //! 
        //! \return A parse_result that indicates whether the operation was successful.
        result_type parse(int argc, const CharType *const argv[], usage_writer_type *usage)
        {
            auto result = parse(argc, argv);
            handle_error(result, usage);
            return result;
        }

        //! \brief Writes usage help for this parser's arguments.
        //!
        //! \param usage The basic_usage_writer used for creating the usage help.
        //! 
        //! Usage will be written to basic_usage_writer::output. In the default basic_usage_writer,
        //! this is a basic_line_wrapping_ostream for the standard output stream.
        void write_usage(usage_writer_type *usage = nullptr)
        {
            if (usage == nullptr)
            {
                usage_writer_type{}.write_parser_usage(*this);
            }
            else
            {
                usage->write_parser_usage(*this);
            }
        }

        //! \brief Invokes the specified function on each argument in the order they are shown in
        //!        in the usage help.
        //! 
        //! \tparam Func The type of the function.
        //! \param f The function to call.
        //! 
        //! Usage help order is as follows:
        //! 
        //! 1. The positional arguments, in their specified order.
        //! 2. Any required non-positional arguments, in alphabetical order.
        //! 3. All remaining arguments, in alphabetical order.
        template<typename Func>
        bool for_each_argument_in_usage_order(Func f) const
        {
            // First the positional arguments
            for (auto &arg : _positional_arguments)
            {
                auto result = f(*arg);
                if (!result)
                    return result;
            }

            // Then required non-positional arguments
            for (auto &arg : _arguments)
            {
                if (arg.second.is_owned() && !arg.second->position() && arg.second->is_required())
                {
                    auto result = f(*arg.second);
                    if (!result)
                        return result;
                }
            }

            // Then the optional non-positional arguments
            for (auto &arg : _arguments)
            {
                if (arg.second.is_owned() && !arg.second->position() && !arg.second->is_required())
                {
                    auto result = f(*arg.second);
                    if (!result)
                        return result;
                }
            }

            return true;
        }

        //! \brief Sets a callback that will be invoked every time an argument is parsed.
        //! \param callback The callback to be invoked.
        //! 
        //! The callback must have the signature `on_parsed_action(argument_base_type &arg, string_view_type value)`.
        //! 
        //! The callback will be invoked after the argument's value has been set. If the argument's
        //! value was invalid, it will not be invoked. The value may be an empty string if this
        //! is a switch argument.
        //! 
        //! The callback can control whether parsing continues with the returned on_parsed_action
        //! value.
        //! 
        //! Only one callback can be registered. Calling this function again will replace the
        //! previous callback.
        void on_parsed(on_parsed_callback callback)
        {
            _on_parsed_callback = callback;
        }

    private:
        void handle_error(const result_type &result, usage_writer_type *usage)
        {
            if (!result)
            {
                // If parsing is cancelled that is not really an error, so we just show usage in
                // that case.
                if (result.error != parse_error::parsing_cancelled)
                {
                    if (usage == nullptr)
                    {
                        usage_writer_type{}.error << result.get_error_message() << std::endl << std::endl;
                    }
                    else
                    {
                        usage->error << result.get_error_message() << std::endl << std::endl;
                    }
                }

                write_usage(usage);
            }
        }

        size_t check_prefix(string_view_type argument)
        {
            // Even if the named argument switch is '-', we treat a '-' followed by a digit as a value, because it could denote a negative number.
            if (argument.length() >= 2 && argument[0] == '-' && std::isdigit(argument[1], _storage.locale))
                return 0;

            for (auto &prefix : _storage.prefixes)
            {
                if (argument.starts_with(prefix))
                    return prefix.length();
            }

            return 0;
        }

        template<typename Iterator>
        result_type parse_named_argument(string_view_type arg, Iterator &current, Iterator end, size_t prefix_length)
        {
            string_view_type name;
            string_view_type value;
            bool has_value = false;

            auto separator_index = arg.find(_storage.argument_value_separator);
            if (separator_index == string_view_type::npos)
            {
                name = arg.substr(prefix_length);
            }
            else
            {
                name = arg.substr(prefix_length, separator_index - prefix_length);
                value = arg.substr(separator_index + 1);
                has_value = true;
            }

            auto it = this->_arguments.find(name);
            if (it == this->_arguments.end())
                return {*_storage.string_provider, parse_error::unknown_argument, string_type{name}};

            if (!has_value)
            {
                if (it->second->set_switch_value())
                {
                    return post_process_argument(*it->second, {});
                }

                auto value_it = current;
                if (_storage.allow_white_space_separator && ++value_it != end)
                {
                    current = value_it;
                    value = *current;
                    // If the next argument looks like an argument name, don't use it as value.
                    has_value = check_prefix(value) == 0;
                }
            }

            if (has_value)
            {
                if (!_storage.allow_duplicate_arguments && !it->second->is_multi_value() && it->second->has_value())
                    return {*_storage.string_provider, parse_error::duplicate_argument, it->second->name()};

                return set_argument_value(*it->second, value);
            }
            else
            {
                return {*_storage.string_provider, parse_error::missing_value, it->second->name()};
            }
        }

        result_type set_argument_value(argument_base_type &arg, string_view_type value)
        {
            if (!arg.set_value(value, _storage.locale))
                return {*_storage.string_provider, parse_error::invalid_value, arg.name()};

            return post_process_argument(arg, value);
        }

        result_type post_process_argument(argument_base_type &arg, string_view_type value)
        {
            auto action = on_parsed_action::none;
            if (_on_parsed_callback)
                action = _on_parsed_callback(arg, value);

            if (action == on_parsed_action::cancel_parsing || 
                (arg.cancel_parsing() && action != on_parsed_action::always_continue))
            {
                return {*_storage.string_provider, parse_error::parsing_cancelled, arg.name()};
            }

            return {*_storage.string_provider, parse_error::none};
        }

        storage_type _storage;
        string_provider_type _default_string_provider;

        // _positional_arguments contains pointers to items owned by _arguments. Since the two
        // have the same lifetime, this is okay.
        std::map<string_type, owned_or_borrowed_ptr<argument_base_type>, string_less> _arguments;
        std::vector<argument_base_type *> _positional_arguments;
        on_parsed_callback _on_parsed_callback;
    };

    //! \brief Typedef for basic_command_line_parser using `char` as the character type.
    using command_line_parser = basic_command_line_parser<char>;
    //! \brief Typedef for basic_command_line_parser using `wchar_t` as the character type.
    using wcommand_line_parser = basic_command_line_parser<wchar_t>;

}

#endif