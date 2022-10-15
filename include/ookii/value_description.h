//! \file value_description.h
//! \brief Provides a template used to determine the default value description for a type.
#ifndef OOKII_VALUE_DESCRIPTION_H_
#define OOKII_VALUE_DESCRIPTION_H_

#pragma once

#include "type_info.h"

namespace ookii
{
    //! \brief Template used to specify the default value description for a type.
    //! 
    //! The value description is a short, often one-word description that indicates the types of
    //! values an argument accepts. It is not the same as the long description that described an
    //! argument's function.
    //! 
    //! This base template defaults to using the short type name (the type name stripped of any
    //! namespace prefixes). The exact result may depend on your compiler, and may not look good
    //! for template types.
    //! 
    //! Specialize this template to provide a different default value description for a type.
    //! 
    //! \tparam T The type whose value description to provide.
    //! \tparam CharType The character type to use.
    //! \tparam Traits The character traits to use.
    //! \tparam Alloc The allocator to use for.
    template<typename T, typename CharType = char, typename Traits = std::char_traits<char>, typename Alloc = std::allocator<char>>
    struct value_description
    {
        //! \brief Gets the value description for the type.
        static std::basic_string<CharType, Traits, Alloc> get()
        {
            return get_short_type_name<T, CharType, Traits, Alloc>();
        }
    };
    
    //! \brief Specialization of value_description for strings.
    //! 
    //! This template sets the value description of any string type to "string" instead of the
    //! template name.
    //! 
    //! \tparam CharType The character type to use for the value description.
    //! \tparam Traits The character traits to use for the value description.
    //! \tparam Alloc The allocator to use for for the value description.
    //! \tparam CharType2 The character type to use for the target string type.
    //! \tparam Traits2 The character traits to use for the target string type.
    //! \tparam Alloc2 The allocator to use for for the target string type.
    template<typename CharType, typename Traits, typename Alloc, typename CharType2, typename Traits2, typename Alloc2>
    struct value_description<std::basic_string<CharType2, Traits2, Alloc2>, CharType, Traits, Alloc>
    {
        //! \copydoc value_description::get()
        static std::basic_string<CharType, Traits, Alloc> get()
        {
            constexpr auto result = literal_cast<CharType>("string");
            return {result.data()};
        }
    };

    //! \brief Specialization of value_description for `std::optional<T>`.
    //! 
    //! The value description of `std::optional<T>` is the same as for T.
    //! 
    //! \tparam T The type contained in the `std::optional<T>`.
    //! \tparam CharType The character type to use.
    //! \tparam Traits The character traits to use.
    //! \tparam Alloc The allocator to use for.
    template<typename T, typename CharType, typename Traits, typename Alloc>
    struct value_description<std::optional<T>, CharType, Traits, Alloc>
    {
        //! \copydoc value_description::get()
        static std::basic_string<CharType, Traits, Alloc> get()
        {
            return value_description<T, CharType, Traits, Alloc>::get();
        }
    };

}

#endif // !defined(OOKII_VALUE_DESCRIPTION_H_)