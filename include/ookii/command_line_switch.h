//! \file command_line_switch.h
//! \brief Provides functionality for switch arguments.
#ifndef OOKII_COMMAND_LINE_SWITCH_H_
#define OOKII_COMMAND_LINE_SWITCH_H_

#pragma once

#include <ostream>
#include "format_helper.h"
#include "value_description.h"

namespace ookii
{
    //! \brief Specialization of the lexical_convert template for conversion from string to bool.
    //! 
    //! Whether or not stream extraction works for the string values "true" and "false" when
    //! `std::boolalpha` is used varies by compiler, so this specialization makes sure those
    //! conversions always work.
    //! 
    //! \tparam CharType The character type used for arguments and other strings.
    //! \tparam Traits The character traits to use for strings.
    //! \tparam Alloc The allocator to use for strings.
    template<typename CharType, typename Traits, typename Alloc>
    struct lexical_convert<bool, CharType, Traits, Alloc>
    {
        //! \brief Convert a string to the specified type.
        //! \param value The string value to convert.
        //! \return The converted value, or `std::nullopt` if conversion failed.
        static std::optional<bool> from_string(std::basic_string_view<CharType, Traits> value, const std::locale & = {})
        {
            constexpr auto true_value = literal_cast<CharType>("true");
            constexpr auto true_alt_value = literal_cast<CharType>("1");
            constexpr auto false_value = literal_cast<CharType>("false");
            constexpr auto false_alt_value = literal_cast<CharType>("0");

            if (string_equal_case_insensitive<CharType, Traits>(value, true_value.data()) ||
                value == true_alt_value.data())
            {
                return {true};
            }
            else if (string_equal_case_insensitive<CharType, Traits>(value, false_value.data()) ||
                value == false_alt_value.data())
            {
                return {false};
            }
            else
            {
                return {};
            }
        }
    };

    namespace details
    {
        template<typename T>
        struct is_switch : public std::false_type {};

        template<>
        struct is_switch<bool> : public std::true_type {};

        template<>
        struct is_switch<std::optional<bool>> : public std::true_type {};
    }
}

#endif