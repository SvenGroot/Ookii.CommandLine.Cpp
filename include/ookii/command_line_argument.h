//! \file command_line_argument.h
//! \brief Provides the ookii::command_line_argument class.
//! 
//! This file contains types that provide information about the command line arguments that a
//! basic_command_line_parser accepts.
#ifndef OOKII_COMMAND_LINE_ARGUMENT_H_
#define OOKII_COMMAND_LINE_ARGUMENT_H_

#pragma once

#include <functional>
#include "command_line_switch.h"

namespace ookii
{
    namespace details
    {
        template<typename CharType, typename Traits, typename Alloc>
        struct argument_storage
        {
            using string_type = std::basic_string<CharType, Traits, Alloc>;

            argument_storage(string_type name)
                : name(name)
            {
            }

            string_type name;
            string_type value_description;
            string_type description;
            std::optional<size_t> position;
            std::vector<string_type> aliases;
            bool is_required{};
            bool cancel_parsing{};
            CharType multi_value_separator{};
        };

        template<class T, typename Element, typename CharType, typename Traits>
        struct typed_argument_storage
        {
            using converter_type = std::function<std::optional<Element>(std::basic_string_view<CharType, Traits>, const std::locale &)>;

            typed_argument_storage(T &value)
                : value{value}
            {
            }

            T &value;
            std::optional<Element> default_value;
            converter_type converter;
        };

        template<typename T>
        struct element_type
        {
            using type = T;
        };

        template<typename T>
        struct element_type<std::optional<T>>
        {
            using type = T;
        };
    }

    //! \brief Abstract base class for regular and multi-value arguments.
    //! \tparam CharType The character type used for arguments and other strings.
    //! \tparam Traits The character traits to use for strings. Defaults to `std::char_traits<CharType>`.
    //! \tparam Alloc The allocator to use for strings. Defaults to `std::allocator<CharType>`.
    //!
    //! This class provides the common functionality for both regular and multi-value arguments.
    template<typename CharType, typename Traits = std::char_traits<CharType>, typename Alloc = std::allocator<CharType>>
    class command_line_argument_base
    {
    public:
        //! \brief The concrete type of argument information storage used. For internal use.
        using storage_type = details::argument_storage<CharType, Traits, Alloc>;
        //! \brief The concrete type of `std::basic_string` used.
        using string_type = typename storage_type::string_type;
        //! \brief The concrete type of `std::basic_string_view` used.
        using string_view_type = std::basic_string_view<CharType, Traits>;
        using stream_type = std::basic_ostream<CharType, Traits>;

        command_line_argument_base(const command_line_argument_base &) = delete;

        virtual ~command_line_argument_base() = default;

        command_line_argument_base &operator=(const command_line_argument_base &) = delete;

        //! \brief Gets the name of the argument.
        //!
        //! The argument's name is set using the basic_parser_builder::add_argument() method.
        const string_type &name() const noexcept
        {
            return _storage.name;
        }

        //! \brief Gets a list of aliases that can be used instead of the argument's name.
        //!
        //! An argument may not have any aliases.
        //! 
        //! Aliases can be added using the basic_parser_builder::argument_builder::alias() method.
        const std::vector<string_type> &aliases() const noexcept
        {
            return _storage.aliases;
        }

        //! \brief Gets the value description for the argument.
        //!
        //! The value description is a brief, often one-word description of the type of values that
        //! the argument accepts. It's used as part of the usage help's syntax.
        //! 
        //! This value defaults to the short type name (excluding namespace names) of the argument's type. The exact
        //! value may depend on your compiler.
        //! 
        //! This value can be changed using the basic_parser_builder::argument_builder::value_description() method.
        const string_type &value_description() const noexcept
        {
            return _storage.value_description;
        }

        //! \brief Gets the long description of the argument.
        //! 
        //! The description is used when generating usage help, and is included in the list of
        //! descriptions after the usage syntax.
        //! 
        //! An argument without a description will not be included in the list of descriptions
        //! (but will still be included in the syntax).
        //! 
        //! This value can be changed using the basic_parser_builder::argument_builder::description() method.
        const string_type &description() const noexcept
        {
            return _storage.description;
        }

        //! \brief Gets the position of the argument.
        //! 
        //! \return The position, or `std::nullopt` if the argument is not positional.
        //! 
        //! A positional argument can be specified without suppling `-Name` before the value.
        //! Note that a positional argument can still be specified by name, as well as by position.
        //! 
        //! An argument can be made positional using the basic_parser_builder::argument_builder::positional() method.
        std::optional<size_t> position() const noexcept
        {
            return _storage.position;
        }

        //! \brief Gets a value that indicates whether the argument is required.
        //!
        //! A required argument must be provided, otherwise parsing is not successful.
        //! 
        //! An argument can be made required using the basic_parser_builder::argument_builder::required() method.
        bool is_required() const noexcept
        {
            return _storage.is_required;
        }

        //! \brief Gets a value that indicates whether supplying this argument will cancel parsing.
        //!
        //! If this returns `true`, if this argument is supplied, argument parsing will immediately
        //! return with parse_error::parsing_cancelled. Arguments after this one will not be
        //! evaluated.
        //! 
        //! This can be used, for example, to implement a `-Help` argument where usage help should
        //! be shown when supplied even if the rest of the command line is valid.
        //! 
        //! This value can be changed using the basic_parser_builder::argument_builder::cancel_parsing()
        //! method.
        bool cancel_parsing() const noexcept
        {
            return _storage.cancel_parsing;
        }

        //! \brief Gets a value that indicates whether the argument was specified on the last
        //!        invocation of basic_command_line_parser::parse().
        //! 
        //! This allows you to determine if an argument was explicitly specified with a value that
        //! matches the default value.
        bool has_value() const noexcept
        {
            return _has_value;
        }

        //! \brief Gets a value that indicates whether the argument is a switch, which means it
        //!        can be supplied without a value.
        //! 
        //! An argument is a switch if its type is `bool` or `std::optional<bool>`.
        virtual bool is_switch() const noexcept = 0;

        //! \brief Gets a value that indicates whether the argument can be provided more than
        //!        once, collecting all the specified values.
        //! 
        //! An argument is multi-value if it was created using basic_parser_builder::add_multi_value_argument().
        virtual bool is_multi_value() const noexcept
        {
            return false;
        }

        //! \brief Resets the argument to indicate it hasn't been set.
        //!
        //! The reset() method is called on all arguments before parsing. After the call, the
        //! has_value() method will return `false`. This method does not affect the value of the
        //! variable holding the argument's value, except for multi-value arguments, where the
        //! container holding the values is cleared before parsing.
        virtual void reset()
        {
            _has_value = false;
        }

        //! \brief Sets the argument to the specified value.
        //! \param value The string value of the argument.
        //! \param loc The locale to use to parse the argument value.
        //! \return `true` if the value was successfully set, or `false` if conversion to the
        //!         argument's type failed.
        //! 
        //! When the value is set, this method converts to the string value to the actual type of
        //! the argument using either the custom converter function (if any), or the default
        //! converter, which is the lexical_convert template.
        //! 
        //! For multi-value arguments, the new value will be added to the list of values.
        virtual bool set_value(string_view_type value, std::locale loc = {}) = 0;

        //! \brief Sets the variable holding the argument's value to the default value.
        //!
        //! This method has no effect if the argument was supplied on the command line (has_value()
        //! returns `true`) or no default value was specified.
        //! 
        //! For a multi-value argument, the default value will be added as the only value in the
        //! container if no values were supplied on the command line.
        //! 
        //! The default value for an argument can be specified using basic_parser_builder::argument_builder::default_value().
        virtual void apply_default_value() = 0;

        //! \brief Applies the implicit value for a switch argument.
        //! \return `true` if the argument was a switch argument; otherwise, `false`.
        //! 
        //! If the argument is a switch argument (is_switch() returns `true`), the variable holding
        //! its variable will be set to `true`. Otherwise, the value is not changed and `false` is
        //! returned.
        //! 
        //! For a multi-value argument, this adds a value of `true` to the container.
        virtual bool set_switch_value() = 0;

        //! \brief Convert the default value of the argument to a string using the specified usage
        //!        options.
        //! \param options The usage options specifying the default value format string.
        //! \param loc The locale used to format the value.
        //! \return The formatted default value, or an empty string if the argument doesn't have
        //!         a default value or the options indicate to exclude it.
        //! 
        //! With the default options, this will return a string like "Default value: value".
        //! 
        //! The default value for an argument can be specified using basic_parser_builder::argument_builder::default_value().
        virtual stream_type &write_default_value(stream_type &stream) const = 0;

        virtual bool has_default_value() const noexcept = 0;

    protected:
        //! \brief Used to indicate that the argument has a value.
        //!
        //! After calling this method, `has_value()` will return `true`.
        void set_value() noexcept
        {
            _has_value = true;
        }

        //! \brief Provides access to the storage fields to derived classes.
        const storage_type &base_storage() const
        {
            return _storage;
        }

        //! \brief Move constructor for command_line_argument_base.
        command_line_argument_base(storage_type &&storage)
            : _storage{std::move(storage)}
        {
        }

    private:
        storage_type _storage;
        bool _has_value{};
    };

    //! \brief Class that provides information about arguments that are not multi-value arguments.
    //! \tparam T The type of the argument.
    //! \tparam Element The element type of the argument.
    //! \tparam CharType The character type used for arguments and other strings.
    //! \tparam Traits The character traits to use for strings. Defaults to `std::char_traits<CharType>`.
    //! \tparam Alloc The allocator to use for strings. Defaults to `std::allocator<CharType>`.
    //!
    //! This provides information for all arguments created using basic_parser_builder::add_argument().
    //! 
    //! For multi-value arguments, see multi_value_command_line_argument.
    template<class T, typename CharType, typename Traits = std::char_traits<CharType>, typename Alloc = std::allocator<CharType>>
    class command_line_argument final : public command_line_argument_base<CharType, Traits, Alloc>
    {
    public:
        //! \brief The type of the base class of this class.
        using base_type = command_line_argument_base<CharType, Traits, Alloc>;
        //! \brief The type of the argument's value, which equals `T`.
        using value_type = T;
        //! \brief The type of the argument's elements, which equals `T`.
        //!
        //! The element type will be differnt from value_type only for multi-value arguments and
        //! arguments that use `std::optional<T>`.
        using element_type = typename details::element_type<T>::type;
        //! \copydoc base_type::string_type
        using string_type = typename base_type::string_type;
        //! \copydoc base_type::string_view_type
        using string_view_type = typename base_type::string_view_type;
        //! \copydoc base_type::storage_type
        using typed_storage_type = details::typed_argument_storage<value_type, element_type, CharType, Traits>;

        //! \brief Initializes a new instance of the command_line_argument class.
        //! \param storage Storage containing the argument's information.
        //! \param typed_storage Storage containing information that depends on the argument's type.
        //! 
        //! You do not normally construct instances of this class manually. Instead, use the
        //! basic_parser_builder.
        command_line_argument(typename base_type::storage_type &&storage, typed_storage_type &&typed_storage)
            : base_type{std::move(storage)},
              _storage{std::move(typed_storage)}
        {
        }

        //! \copydoc base_type::is_switch()
        //! 
        //! For a multi-value argument, the element_type must be `bool` or `std::optional<bool>`.
        //! For example, an argument with the container type `std::vector<bool>` would be a
        //! multi-value switch argument.
        bool is_switch() const noexcept override
        {
            return details::is_switch<T>::value;
        }

        //! \copydoc base_type::set_value()
        bool set_value(string_view_type value, std::locale loc = {}) override
        {
            std::optional<element_type> converted;
            if (_storage.converter)
                converted = _storage.converter(value, loc);
            else
                converted = lexical_convert<element_type, CharType, Traits, Alloc>::from_string(value, loc);

            if (!converted)
                return false;

            _storage.value = std::move(*converted);
            base_type::set_value();
            return true;
        }

        //! \copydoc base_type::set_switch_value()
        bool set_switch_value() override
        {
            return set_switch_value_core();
        }

        //! \copydoc base_type::apply_default_value()
        void apply_default_value() override
        {
            if (!this->has_value() && _storage.default_value)
            {
                _storage.value = *_storage.default_value;
            }
        }

        //! \copydoc base_type::write_default_value()
        typename base_type::stream_type &write_default_value(typename base_type::stream_type &stream) const override
        {
            if (_storage.default_value)
            {
                stream << *_storage.default_value;
            }

            return stream;
        }

        //! \copydoc base_type::has_default_value()
        bool has_default_value() const noexcept override
        {
            return _storage.default_value.has_value();
        }

    private:
        template<typename T2 = T>
        std::enable_if_t<details::is_switch<T2>::value, bool> set_switch_value_core()
        {
            _storage.value = true;
            return true;
        }

        template<typename T2 = T>
        std::enable_if_t<!details::is_switch<T2>::value, bool> set_switch_value_core()
        {
            return false;
        }

        typed_storage_type _storage;
    };

    //! \brief Class that provides information about arguments that are not multi-value arguments.
    //! \tparam T The type of the argument's container.
    //! \tparam CharType The character type used for arguments and other strings.
    //! \tparam Traits The character traits to use for strings. Defaults to `std::char_traits<CharType>`.
    //! \tparam Alloc The allocator to use for strings. Defaults to `std::allocator<CharType>`.
    //!
    //! This provides information for all arguments created using basic_parser_builder::add_multi_value_argument().
    //! 
    //! For arguments that are not multi-value, see command_line_argument.
    template<typename T, typename CharType, typename Traits = std::char_traits<CharType>, typename Alloc = std::allocator<CharType>>
    class multi_value_command_line_argument : public command_line_argument_base<CharType, Traits, Alloc>
    {
    public:
        //! \brief The type of the base class of this class.
        using base_type = command_line_argument_base<CharType, Traits, Alloc>;
        //! \brief The type of the argument's container, which equals `T`.
        using value_type = T;
        //! \brief The type of the argument's container's elements.
        using element_type = typename T::value_type;
        //! \copydoc base_type::string_type
        using string_type = typename base_type::string_type;
        //! \copydoc base_type::string_view_type
        using string_view_type = typename base_type::string_view_type;
        //! \copydoc base_type::storage_type
        using typed_storage_type = details::typed_argument_storage<T, element_type, CharType, Traits>;

        //! \brief Initializes a new instance of the multi_value_command_line_argument class.
        //! \param storage Storage containing the argument's information.
        //! \param typed_storage Storage containing information that depends on the argument's type.
        //! 
        //! You do not normally construct instances of this class manually. Instead, use the
        //! basic_parser_builder.
        multi_value_command_line_argument(typename base_type::storage_type &&storage, typed_storage_type &&typed_storage)
            : base_type{std::move(storage)},
              _storage{std::move(typed_storage)}
        {
        }

        //! \copydoc base_type::is_switch()
        bool is_switch() const noexcept override
        {
            return details::is_switch<element_type>::value;
        }

        //! \copydoc base_type::set_value()
        //!
        //! This method always returns `true`.
        bool is_multi_value() const noexcept override
        {
            return true;
        }

        //! \brief Gets the character used to separate multiple values in a single argument value,
        //!        or '\0' if no separator is used.
        //!
        //! The separator can be specified using basic_parser_builder::multi_value_argument_builder::separator().
        CharType separator() const noexcept
        {
            return this->base_storage().multi_value_separator;
        }

        //! \copydoc base_type::reset()
        void reset() override
        {
            base_type::reset();
            _storage.value.clear();
        }

        //! \copydoc base_type::set_value()
        bool set_value(string_view_type value, std::locale loc = {}) override
        {
            for (auto element : tokenize{value, separator()})
            {
                std::optional<element_type> converted;
                if (_storage.converter)
                    converted = _storage.converter(element, loc);
                else
                    converted = lexical_convert<element_type, CharType, Traits, Alloc>::from_string(element, loc);

                if (!converted)
                    return false;

                _storage.value.push_back(std::move(*converted));
            }

            base_type::set_value();
            return true;
        }

        //! \copydoc base_type::set_switch_value()
        bool set_switch_value() override
        {
            return set_switch_value_core();
        }

        //! \copydoc base_type::apply_default_value()
        void apply_default_value() override
        {
            if (!this->has_value() && _storage.default_value)
            {
                _storage.value.push_back(*_storage.default_value);
            }
        }

        //! \copydoc base_type::write_default_value()
        typename base_type::stream_type &write_default_value(typename base_type::stream_type &stream) const override
        {
            if (_storage.default_value)
            {
                stream << *_storage.default_value;
            }

            return stream;
        }

        //! \copydoc base_type::has_default_value()
        bool has_default_value() const noexcept override
        {
            return _storage.default_value.has_value();
        }

    private:
        template<typename T2 = element_type>
        std::enable_if_t<details::is_switch<T2>::value, bool> set_switch_value_core()
        {
            _storage.value.push_back(true);
            return true;
        }

        template<typename T2 = element_type>
        std::enable_if_t<!details::is_switch<T2>::value, bool> set_switch_value_core()
        {
            return false;
        }

        typed_storage_type _storage;
    };

}

#endif