//! \file command_line_builder.h
//! \brief Provides the ookii::basic_parser_builder class.
//! 
//! This file contains types used to construct a basic_command_line_parser.
#ifndef OOKII_COMMAND_LINE_BUILDER_H_
#define OOKII_COMMAND_LINE_BUILDER_H_

#pragma once

#include "command_line_core.h"

namespace ookii
{
    //! \brief Provides functionality to specify options and arguments to create a
    //!        new basic_command_line_parser.
    //! 
    //! To create a basic_command_line_parser, you will first create an instance of the basic_parser_builder,
    //! calling various methods to set options that control parsing behavior. Then, use the
    //! add_argument() and add_multi_value_argument() methods to add arguments and set their
    //! options.
    //! 
    //! Several typedefs for common character types are provided:
    //! 
    //! Type                     | Definition
    //! ------------------------ | -------------------------------------
    //! `ookii::parser_builder`  | `ookii::basic_parser_builder<char>`
    //! `ookii::wparser_builder` | `ookii::basic_parser_builder<wchar_t>`
    //! 
    //! \tparam CharType The character type used for arguments and other strings. Only `char` and
    //!         `wchar_t` are supported. Defaults to `wchar_t` if `_UNICODE` is defined, otherwise
    //!         to `char`.
    //! \tparam Traits The character traits to use for strings. Defaults to `std::char_traits<CharType>`.
    //! \tparam Alloc The allocator to use for strings. Defaults to `std::allocator<CharType>`.
    template<typename CharType = details::default_char_type, typename Traits = std::char_traits<CharType>, typename Alloc = std::allocator<CharType>>
    class basic_parser_builder
    {
        using parser_storage_type = details::parser_storage<CharType, Traits, Alloc>;

    public:
        //! \brief The specialized type of basic_command_line_parser that is being built.
        using parser_type = basic_command_line_parser<CharType, Traits, Alloc>;
        //! \copydoc parser_type::string_type
        using string_type = typename parser_type::string_type;
        //! \copydoc parser_type::argument_base_type
        using argument_base_type = typename parser_type::argument_base_type;

        //! \brief The specialized type of the command_line_argument instances that will be built.
        //! \tparam T The type of the argument's value.
        template<typename T>
        using typed_argument_type = command_line_argument<T, CharType, Traits, Alloc>;

        class argument_builder_base;

        template<typename ArgumentType, typename BaseType>
        class argument_builder;

        template<typename T>
        class multi_value_argument_builder;

        //! \brief Abstract base class with common functionality for the argument builders.
        //!
        //! This class contains functionality that doesn't depend on the argument's type.
        class argument_builder_base
        {
            using storage_type = typename argument_base_type::storage_type;

        public:
            //! \brief Default destructor.
            virtual ~argument_builder_base() = default;

            argument_builder_base(argument_builder_base &) = delete;
            argument_builder_base &operator=(argument_builder_base &) = delete;

            //! \copydoc basic_parser_builder::add_argument()
            template<typename T>
            argument_builder<typed_argument_type<T>, argument_builder_base> &add_argument(T &value, string_type name)
            {
                return _parser_builder.add_argument(value, name);
            }

            //! \copydoc basic_parser_builder::add_multi_value_argument()
            template<typename T>
            typename multi_value_argument_builder<T>::builder_type &add_multi_value_argument(T &value, string_type name)
            {
                return _parser_builder.add_multi_value_argument(value, name);
            }

            //! \copydoc basic_parser_builder::build()
            parser_type build()
            {
                return _parser_builder.build();
            }

            //! \brief Returns the name of the argument.
            const string_type &name() const
            {
                return this->_storage.name;
            }

            //! \brief Converts the argument_builder_base into a command_line_argument_base that
            //!        can be used by the basic_command_line_parser.
            virtual owned_or_borrowed_ptr<argument_base_type> to_argument() = 0;

        protected:
            //! \brief Initializes a new instance of the argument_builder_base class.
            //! \param basic_parser_builder A reference to the basic_parser_builder used to build this argument.
            //! \param name The name of the argument.
            argument_builder_base(basic_parser_builder &basic_parser_builder, string_type name)
                : _storage{name},
                  _parser_builder{basic_parser_builder}
            {
            }

            //! \brief Provides access to the argument's options storage.
            storage_type &storage()
            {
                return _storage;
            }

            //! \brief Gets the next position for a positional argument.
            size_t get_next_position()
            {
                return _parser_builder.get_next_position();
            }

        private:
            storage_type _storage;
            basic_parser_builder &_parser_builder;
        };

        //! \brief Specifies options for an argument under construction.
        //!
        //! This class inherits from argument_builder_base for arguments that are not multi-value,
        //! and inherits from multi_value_argument_builder for arguments that are.
        template<typename ArgumentType, typename BaseType>
        class argument_builder : public BaseType
        {
            using typed_storage_type = typename ArgumentType::typed_storage_type;
            using value_type = typename ArgumentType::value_type;
            using element_type = typename ArgumentType::element_type;
            using converter_type = typename typed_storage_type::converter_type;

        public:
            //! \brief Initializes a new instance of the argument_builder class.
            //! \param basic_parser_builder A reference to the basic_parser_builder used to build this argument.
            //! \param name The name of the argument.
            //! \param value A reference where the argument's value will be stored.
            argument_builder(basic_parser_builder &basic_parser_builder, string_type name, value_type &value)
                : BaseType{basic_parser_builder, name},
                  _typed_storage{value}
            {
            }

            //! \brief Sets the value description for the argument.
            //! 
            //! \param value_description The new value description.
            //! 
            //! \return The argument builder.
            //!
            //! The value description is a brief, often one-word description of the type of values
            //! that the argument accepts. It's used as part of the usage help's syntax. An
            //! example of a value description could be "number", "string", "name", or someting
            //! similarly simple.
            //! 
            //! For longer descriptions of the argument's purpose, use the description() method
            //! instead.
            //! 
            //! If not specified explicitly, the value description defaults to the short type name
            //! (excluding namespace names) of the argument's type. The exact value may depend on
            //! your compiler.
            //! 
            //! The default value description for a type can also be overridden by specializing the
            //! value_description template.
            argument_builder &value_description(string_type value_description)
            {
                this->storage().value_description = value_description;
                return *this;
            }

            //! \brief Sets the long description for the argument.
            //! 
            //! \param description The new description.
            //!
            //! The description is used when generating usage help, and is included in the list of
            //! descriptions after the usage syntax.
            //! 
            //! An argument without a description will not be included in the list of descriptions
            //! (but will still be included in the syntax).
            argument_builder &description(string_type description)
            {
                this->storage().description = description;
                return *this;
            }

            //! \brief Indicates that the argument can be specified by position.
            //!
            //! Calling the positional() method will make the argument positional, assigning it the
            //! next available position. Therefore, you should define positional arguments in the
            //! order you want them to be specified.
            //! 
            //! A positional argument can still be specified by name, as well as by position.
            //! 
            //! If the argument is already positional, this method has no effect.
            argument_builder &positional()
            {
                if (!this->storage().position)
                {
                    this->storage().position = this->get_next_position();
                }

                return *this;
            }

            //! \brief Indicates that the argument is required.
            //!
            //! A required argument must be provided, otherwise parsing is not successful. Typically,
            //! required arguments should also be positional, but this is not mandatory.
            //! 
            //! If the argument is already required, this method has no effect.
            argument_builder &required()
            {
                this->storage().is_required = true;
                return *this;
            }

            //! \brief Sets a default value for the argument, which will be used if the argument
            //!        is not supplied..
            //! \param default_value The default value to use.
            //!
            //! The default value will be applied after parsing to all non-required arguments that
            //! weren't specified on the command line.
            //! 
            //! If no default value is set, basic_command_line_parser will not change the value of the
            //! variable holding the argument's value at all, so you can also initialize the
            //! variable to the desired value. The advantage of using default_value is that the
            //! value can be included in the usage help.
            //! 
            //! Setting a default value for a required argument is allowed, but the value will
            //! never be used.
            argument_builder &default_value(element_type default_value)
            {
                this->_typed_storage.default_value = default_value;
                return *this;
            }

            //! \brief Supplies a custom function to convert strings to the argument's type.
            //! \param converter The function used for conversions.
            //! 
            //! A conversion function must have the signature `std::optional<T>(std::basic_string_view<CharType> value, const std::locale &loc)`.
            //! It should return std::nullopt if the conversion failed.
            //! 
            //! Even if a custom converter is used, conversion using the default method (stream
            //! extraction or a specialized lexical_convert template) must still be possible in
            //! order to avoid compiler errors. Use this method if a default conversion exists,
            //! but you wish to deviate from it for a specific argument.
            argument_builder &converter(converter_type converter)
            {
                this->_typed_storage.converter = converter;
                return *this;
            }

            //! \brief Adds an alias to the argument.
            //! \param alias The alias to add.
            //! 
            //! An alias is an alternate, often shorter, name for an argument that can be used to
            //! specify it on the command line. For example, an argument `-Verbose` might have the
            //! alias `-v`.
            //! 
            //! An argument can have multiple aliases, which can be specified by invoking this
            //! method multiple times.
            argument_builder &alias(string_type alias)
            {
                this->storage().aliases.push_back(alias);
                return *this;
            }

            //! \brief Indicates that supplying this argument will cancel parsing.
            //!
            //! If set, if this argument is supplied, argument parsing will immediately return with
            //! parse_error::parsing_cancelled. Arguments after this one will not be evaluated.
            //! 
            //! This can be used, for example, to implement a `-Help` argument where usage help
            //! should be shown when supplied even if the rest of the command line is valid.
            //! 
            //! Note that parsing will be cancelled regardless of the supplied value. So, e.g. for
            //! a switch argument, `-Help:false` will still cancel parsing and display help. If
            //! you want to conditionally cancel parsing, you can do so by using a callback with
            //! the basic_command_line_parser::on_parsed() method.
            argument_builder &cancel_parsing()
            {
                this->storage().cancel_parsing = true;
                return *this;
            }

        private:
            virtual owned_or_borrowed_ptr<argument_base_type> to_argument() override
            {
                if (this->storage().value_description.empty())
                    this->storage().value_description = ::ookii::value_description<element_type, CharType, Traits, Alloc>::get();

                return make_owned_ptr<ArgumentType>(std::move(this->storage()), std::move(_typed_storage));
            }

            typed_storage_type _typed_storage;
        };

        //! \brief Base class for argument_builder for multi-value arguments.
        //! \tparam T The type of the argument's container.
        template<typename T>
        class multi_value_argument_builder : public argument_builder_base
        {
        public:
            //! \brief The type of the argument_builder that derives from multi_value_argument_builder.
            using builder_type = argument_builder<multi_value_command_line_argument<T, CharType, Traits, Alloc>, multi_value_argument_builder>;

            //! \brief Specifies a separator that separates multiple values in a single argument
            //!        value.
            //! \param separator The separator to use, or '\0' to not use a separator.
            //! 
            //! Normally, multi-value arguments must be supplied multiple times in order to supply
            //! multiple values (e.g. `-Value 1 -Value 2`). Using a separator allows multiple values
            //! to be supplied at once (e.g. `-Value 1;2`).
            //! 
            //! \warning The separator character *cannot* be used in argument values; there is no
            //!          way to escape it. Be particularly careful with types whose parsing might
            //!          be locale-dependent.
            //! 
            //! By default, no separator is used.
            builder_type &separator(CharType separator)
            {
                this->storage().multi_value_separator = separator;
                return *static_cast<builder_type*>(this);
            }

        protected:
            //! \brief Initializes a new instance of the multi_value_argument_builder class.
            //! \param basic_parser_builder A reference to the basic_parser_builder used to build this argument.
            //! \param name The name of the argument.
            multi_value_argument_builder(basic_parser_builder &basic_parser_builder, string_type name)
                : argument_builder_base{basic_parser_builder, name}
            {
            }
        };

        //! \brief Initializes a new instance of the basic_parser_builder class.
        //! \param command_name The name of the command that the arguments belong to. Often, this
        //!        is the executable name of the application.
        basic_parser_builder(string_type command_name)
            : _storage{command_name}
        {
        }

        //! \brief Adds a new argument, and returns an argument_builder that can be used to
        //!        further customize it.
        //! \tparam T The type of the argument.
        //! \param value A reference to the storage for the argument's value. When the argument
        //!              is supplied, the converted value will be written to this reference.
        //! \param name The name of the argument, used to supply it on the command line.
        //! 
        //! Any type T can be used as an argument type, as long as it meets the following
        //! criteria:
        //! 
        //! - It must be possible to convert a string to type T, either using stream extraction
        //!   (`operator>>`), or by a specialization of the lexical_convert template.
        //! - It must be possible to convert type T to a string using `std::format` (or libfmt
        //!   if \<format> is not available).
        //! 
        //! For custom argument types, you must provide either a stream extractor (`operator>>`)
        //! or specialize lexical_convert, and you must provide a specialization of
        //! `std::formatter` (`fmt::formatter` if libfmt is being used).
        //! 
        //! Providing an `std::formatter` is required so default values can be displayed in
        //! the usage help. A formatter must be provided even if you don't plan to set a
        //! default value, to prevent compiler errors (in that case, you could of course
        //! provide a non-functional, empty formatter).
        //! 
        //! Of the argument is of type `std::optional<T>`, the requirements apply to the contained
        //! type, not the `std::optional<T>` itself.
        //! 
        //! If type T is either `bool` or `std::optional<bool>`, the resulting argument will
        //! be a switch argument.
        template<typename T>
        argument_builder<typed_argument_type<T>, argument_builder_base> &add_argument(T &value, string_type name)
        {
            _arguments.push_back(std::make_unique<argument_builder<typed_argument_type<T>, argument_builder_base>>(*this, name, value));
            return *static_cast<argument_builder<typed_argument_type<T>, argument_builder_base>*>(_arguments.back().get());
        }

        //! \brief Adds a new multi-value argument, and returns an argument_builder that can
        //!        be used to further customize it.
        //! \tparam T The type of the argument's container.
        //! \param value A reference to the container for the argument's values. When the
        //!              argument is supplied, the converted value will be added to this
        //!              container.
        //! \param name The name of the argument, used to supply it on the command line.
        //! 
        //! Any container type T can be used, provided it meets the following requirements.
        //! 
        //! - It defines a type T::value_type that meets the requirements outlined in the
        //!   documentation for add_argument().
        //! - It defines the methods T::push_back() and T::clear()
        template<typename T>
        typename multi_value_argument_builder<T>::builder_type &add_multi_value_argument(T &value, string_type name)
        {
            _arguments.push_back(std::make_unique<typename multi_value_argument_builder<T>::builder_type>(*this, name, value));
            return *static_cast<typename multi_value_argument_builder<T>::builder_type*>(_arguments.back().get());
        }

        //! \brief Creates a basic_command_line_parser using the current options and arguments.
        //! \return A basic_command_line_parser that can be used to parse the arguments defined by
        //!         the basic_parser_builder.
        //! 
        //! Since various values are moved out of the basic_parser_builder into the basic_command_line_parser,
        //! the basic_parser_builder should not be used after calling build().
        parser_type build()
        {
            if (_storage.prefixes.empty())
                _storage.prefixes = parser_type::get_default_prefixes();

            return parser_type{_arguments, std::move(_storage), _case_sensitive};
        }

        //! \brief Sets a value that indicates whether argument names are case sensitive.
        //! \param case_sensitive Indicates whether argument names are case sensitive.
        //! 
        //! If set to `true`, argument names must be supplied on the command line using their exact
        //! case, and it's possible to have multiple arguments whose names only differ by case. If
        //! `false`, argument names don't need to match case when supplied (e.g. `-foo` will match
        //! an argument named "Foo").
        //! 
        //! The default value is `false`.
        basic_parser_builder &case_sensitive(bool case_sensitive) noexcept
        {
            _case_sensitive = case_sensitive;
            return *this;
        }

        //! \brief Sets the locale to use when converting argument values and writing usage help.
        //! \param loc The locale to use.
        //! 
        //! The default value is a copy of the global locale when the basic_parser_builder was created.
        basic_parser_builder &locale(std::locale loc)
        {
            _storage.locale = loc;
            return *this;
        }

        //! \brief Sets the argument name prefixes accepted by the basic_command_line_parser.
        //! \tparam Range The type of a range containing the prefixes.
        //! \param prefixes A range containing the prefixes.
        //!
        //! Any value that starts with the specified prefixes is considered an argument name (with
        //! the exception of a dash followed by a number, which is always considered to be a
        //! negative number).
        //! 
        //! By default, the parser accepts '/' and '-' on Windows, and only '-' on other systems.
        //! 
        //! The prefixes are evaluated in order, so if for example you wish to use '--' and '-'
        //! you must provide the longer prefix first otherwise it will never be considered after
        //! the shorter one matches.
        template<typename Range>
        basic_parser_builder &prefixes(const Range &prefixes)
        {
            // Using enables ADL.
            using std::begin;
            using std::end;
            _storage.prefixes = std::vector<string_type>{ begin(prefixes), end(prefixes) };
            return *this;
        }

        //! \brief Sets the argument name prefixes accepted by the basic_command_line_parser.
        //! \tparam T The type of an element convertable to string_type.
        //! \param prefixes An initializer list containing the prefixes.
        //!
        //! Any value that starts with the specified prefixes is considered an argument name (with
        //! the exception of a dash followed by a number, which is always considered to be a
        //! negative number).
        //! 
        //! By default, the parser accepts '/' and '-' on Windows, and only '-' on other systems.
        //! 
        //! The prefixes are evaluated in order, so if for example you wish to use '--' and '-'
        //! you must provide the longer prefix first otherwise it will never be considered after
        //! the shorter one matches.
        template<typename T>
        basic_parser_builder &prefixes(std::initializer_list<T> prefixes)
        {
            return this->prefixes<std::initializer_list<T>>(prefixes);
        }

        //! \brief Sets a value that indicates whether argument names and values can be separated
        //!        by whitespace.
        //! \param allow A value that indicates whether argument names and values can be separated
        //!        by whitespace
        //! 
        //! If set to `true`, argument can be specified like `-Name value` as well as `-Name:value`
        //! (or the custom separator set using argument_value_separator()). If set to `false`, only
        //! the latter will be accepted.
        //! 
        //! The default value is `true`.
        basic_parser_builder &allow_whitespace_separator(bool allow) noexcept
        {
            _storage.allow_white_space_separator = allow;
            return *this;
        }

        //! \brief Sets a value that indicates whether arguments may be specified multiple times.
        //! \param allow A value that indicates whether duplicate arguments are allowed.
        //! 
        //! If set to `true`, it's an error to supply an argument whose value was already supplied
        //! previously. If `false`, no error occurs and the last value supplied will be used.
        //! 
        //! This setting has no effect on multi-value arguments, which can always be supplied
        //! multiple times.
        //! 
        //! The default value is `false`.
        basic_parser_builder &allow_duplicate_arguments(bool allow) noexcept
        {
            _storage.allow_duplicate_arguments = allow;
            return *this;
        }

        //! \brief Sets the character used to separate argument names and values.
        //! \param separator The haracter used to separate argument names and values.
        //! 
        //! Using this separator, arguments can be supplied like `-Name:value` (where `:` is the
        //! separator).
        //! 
        //! Setting the separator to a white-space character will only work if the name and the
        //! value are supplied as a single argument, usually by quoting them or by escaping the
        //! white-space character. Use allow_whitespace_separator() to control whether the name
        //! and the value can be supplied in separate arguments.
        basic_parser_builder &argument_value_separator(CharType separator) noexcept
        {
            _storage.argument_value_separator = separator;
            return *this;
        }

        //! \brief Sets a description for the application.
        //! \param description The description.
        //!
        //! This description will be added to the usage help generated by basic_command_line_parser::write_usage().
        basic_parser_builder &description(string_type description)
        {
            _storage.description = description;
            return *this;
        }

    private:
        size_t get_next_position() noexcept
        {
            return _next_position++;
        }

        std::vector<std::unique_ptr<argument_builder_base>> _arguments;
        size_t _next_position{};
        bool _case_sensitive{};
        
        parser_storage_type _storage;
    };

    //! \brief Typedef for basic_shell_command using `char` as the character type.
    using parser_builder = basic_parser_builder<char>;
    //! \brief Typedef for basic_shell_command using `wchar_t` as the character type.
    using wparser_builder = basic_parser_builder<wchar_t>;

}

#endif