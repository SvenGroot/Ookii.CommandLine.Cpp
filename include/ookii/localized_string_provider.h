#ifndef OOKII_LOCALIZED_STRING_PROVIDER_H_
#define OOKII_LOCALIZED_STRING_PROVIDER_H_

#pragma once

#include "format_helper.h"

namespace ookii
{
    //! \brief Provides custom localized strings.
    //!
    //! You can derive from this class and override its members to provide customized or localized
    //! strings. A custom string provider can be provided to the
    //! command_line_builder::command_line_builder() constructor.
    //!
    //! Two typedefs for common character types are provided:
    //! 
    //! Type                                | Definition
    //! ----------------------------------- | -------------------------------------
    //! `ookii::localized_string_provider`  | `ookii::basic_localized_string_provider<char>`
    //! `ookii::wlocalized_string_provider` | `ookii::basic_localized_string_provider<wchar_t>`
    //! 
    //! \tparam CharType The character type used for strings.
    //! \tparam Traits The character traits to use for strings. Defaults to `std::char_traits<CharType>`.
    //! \tparam Alloc The allocator to use for strings. Defaults to `std::allocator<CharType>`.
    template<typename CharType, typename Traits = std::char_traits<CharType>, typename Alloc = std::allocator<CharType>>
    class basic_localized_string_provider
    {
    public:
        //! \brief The concrete string type used.
        using string_type = std::basic_string<CharType, Traits, Alloc>;
        //! \brief The concrete string_view type used.
        using string_view_type = std::basic_string_view<CharType, Traits>;

        //! \brief Gets a default instance of the basic_localized_string_provider.
        static basic_localized_string_provider &get_default() noexcept
        {
            static basic_localized_string_provider default_string_provider;
            return default_string_provider;
        }

        //! \brief Gets the error message for parse_error::invalid_value.
        //! \param argument_name The name of the argument.
        virtual string_type invalid_value(string_view_type argument_name) const
        {
            return OOKII_FMT_NS format(defaults::invalid_value_format.data(), argument_name);
        }

        //! \brief Gets the error message for parse_error::unknown_argument.
        //! \param argument_name The name of the argument.
        virtual string_type unknown_argument(string_view_type argument_name) const
        {
            return OOKII_FMT_NS format(defaults::unknown_argument_format.data(), argument_name);
        }

        //! \brief Gets the error message for parse_error::missing_value.
        //! \param argument_name The name of the argument.
        virtual string_type missing_value(string_view_type argument_name) const
        {
            return OOKII_FMT_NS format(defaults::missing_value_format.data(), argument_name);
        }

        //! \brief Gets the error message for parse_error::duplicate_argument.
        //! \param argument_name The name of the argument.
        virtual string_type duplicate_argument(string_view_type argument_name) const
        {
            return OOKII_FMT_NS format(defaults::duplicate_argument_format.data(), argument_name);
        }

        //! \brief Gets the error message for parse_error::too_many_arguments.
        virtual string_type too_many_arguments() const
        {
            return defaults::too_many_arguments.data();
        }

        //! \brief Gets the error message for parse_error::missing_required_argument.
        //! \param argument_name The name of the argument.
        virtual string_type missing_required_argument(string_view_type argument_name) const
        {
            return OOKII_FMT_NS format(defaults::missing_required_argument_format.data(), argument_name);
        }

        //! \brief Gets the error message for parse_error::combined_short_name_non_switch.
        //! \param argument_name The name of the argument.
        virtual string_type combined_short_name_non_switch(string_view_type argument_name) const
        {
            return OOKII_FMT_NS format(defaults::combined_short_name_non_switch.data(), argument_name);
        }

        //! \brief Gets the error message for parse_error::unknown.
        virtual string_type unknown_error() const
        {
            return defaults::unknown.data();
        }

        //! \brief Gets the name of the help argument created if
        //! basic_parser_builder::automatic_help_argument() was not set to `false`.
        virtual string_type automatic_help_name() const
        {
            return defaults::automatic_help_name.data();
        }

        //! \brief Gets the short name of the help argument created if
        //! basic_parser_builder::automatic_help_argument() was not set to `false`.
        //!
        //! In addition to the short name, the help argument will automatically have a short alias
        //! that is the lower case first character of the value returned by automatic_help_name().
        //! If the short name and short alias are equal, then no alias is added.
        //!
        //! If you are not using parsing_mode::long_short, the short name and short alias will be
        //! used as regular aliases instead.
        virtual CharType automatic_help_short_name() const
        {
            return defaults::automatic_help_short_name;
        }

        //! \brief Gets the description of the help argument created if
        //! basic_parser_builder::automatic_help_argument() was not set to `false`.
        virtual string_type automatic_help_description() const
        {
            return defaults::automatic_help_description.data();
        }

        //! \brief Gets the name of the version argument created by
        //! basic_parser_builder::add_version_argument().
        virtual string_type automatic_version_name() const
        {
            return defaults::automatic_version_name.data();
        }

        //! \brief Gets the description of the version argument created by
        //! basic_parser_builder::add_version_argument().
        virtual string_type automatic_version_description() const
        {
            return defaults::automatic_version_description.data();
        }

    private:
        struct defaults
        {
            static constexpr auto invalid_value_format = literal_cast<CharType>("The value provided for the argument '{}' was invalid.");
            static constexpr auto unknown_argument_format = literal_cast<CharType>("Unknown argument name '{}'.");
            static constexpr auto missing_value_format = literal_cast<CharType>("No value was supplied for the argument '{}'.");
            static constexpr auto duplicate_argument_format = literal_cast<CharType>("The argument '{}' was supplied more than once.");
            static constexpr auto too_many_arguments = literal_cast<CharType>("Too many arguments were supplied.");
            static constexpr auto missing_required_argument_format = literal_cast<CharType>("The required argument '{}' was not supplied.");
            static constexpr auto combined_short_name_non_switch = literal_cast<CharType>("The combined short argument '{}' contains an argument that is not a switch.");
            static constexpr auto unknown = literal_cast<CharType>("An unknown error has occurred.");
            static constexpr auto automatic_help_name = literal_cast<CharType>("Help");
            static constexpr CharType automatic_help_short_name = '?';
            static constexpr auto automatic_help_description = literal_cast<CharType>("Displays this help message.");
            static constexpr auto automatic_version_name = literal_cast<CharType>("Version");
            static constexpr auto automatic_version_description = literal_cast<CharType>("Displays version information.");
        };
    };

    //! \brief Typedef for basic_localized_string_provider using `char` as the character type.
    using localized_string_provider = basic_localized_string_provider<char>;
    //! \brief Typedef for basic_localized_string_provider using `wchar_t` as the character type.
    using wlocalized_string_provider = basic_localized_string_provider<wchar_t>;

}

#endif