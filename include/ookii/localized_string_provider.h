#ifndef OOKII_LOCALIZED_STRING_PROVIDER_H_
#define OOKII_LOCALIZED_STRING_PROVIDER_H_

#pragma once

#include "format_helper.h"

namespace ookii
{
    template<typename CharType, typename Traits = std::char_traits<CharType>, typename Alloc = std::allocator<CharType>>
    class basic_localized_string_provider
    {
    public:
        //! \brief The concrete string type used.
        using string_type = std::basic_string<CharType, Traits, Alloc>;
        //! \brief The concrete string_view type used.
        using string_view_type = std::basic_string_view<CharType, Traits>;

        virtual string_type invalid_value(string_view_type argument_name) const
        {
            return OOKII_FMT_NS format(defaults::invalid_value_format.data(), argument_name);
        }

        virtual string_type unknown_argument(string_view_type argument_name) const
        {
            return OOKII_FMT_NS format(defaults::unknown_argument_format.data(), argument_name);
        }

        virtual string_type missing_value(string_view_type argument_name) const
        {
            return OOKII_FMT_NS format(defaults::missing_value_format.data(), argument_name);
        }

        virtual string_type duplicate_argument(string_view_type argument_name) const
        {
            return OOKII_FMT_NS format(defaults::duplicate_argument_format.data(), argument_name);
        }

        virtual string_type too_many_arguments() const
        {
            return defaults::too_many_arguments.data();
        }

        virtual string_type missing_required_argument(string_view_type argument_name) const
        {
            return OOKII_FMT_NS format(defaults::missing_required_argument_format.data(), argument_name);
        }

        virtual string_type unknown_error() const
        {
            return defaults::unknown.data();
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
            static constexpr auto unknown = literal_cast<CharType>("An unknown error has occurred.");
        };
    };

    //! \brief Typedef for basic_usage_writer using `char` as the character type.
    using localized_string_provider = basic_localized_string_provider<char>;
    //! \brief Typedef for basic_usage_writer using `wchar_t` as the character type.
    using wlocalized_string_provider = basic_localized_string_provider<wchar_t>;

}

#endif