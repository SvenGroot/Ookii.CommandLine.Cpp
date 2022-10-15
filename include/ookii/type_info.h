//! \file type_info.h
//! \brief Provides helpers to obtain the name of a type.
#ifndef OOKII_TYPE_INFO_H_
#define OOKII_TYPE_INFO_H_

#pragma once

#include <typeinfo>
#include <memory>
#include "string_helper.h"

#ifdef __GNUC__
#include <cxxabi.h>
#endif

namespace ookii
{
    namespace details
    {
#ifdef __GNUC__

        template<typename T>
        struct malloc_deleter
        {
            void operator()(T *p)
            {
                free(p);
            }
        };

        // Unique pointer to free the value returned by __cxa_demangle, which is allocated with
        // malloc.
        using name_ptr = std::unique_ptr<char, malloc_deleter<char>>;

#endif

        inline static std::string_view remove_prefix(std::string_view type_name, std::string_view prefix)
        {
            if (type_name.starts_with(prefix))
                return type_name.substr(prefix.length());
            else
                return type_name;
        }

        inline std::string_view make_short(std::string_view type_name)
        {
            // First strip namespaces. These may occur inside template arguments, so check for
            // those first.
            auto index = type_name.find_last_of(':', type_name.find_first_of('<'));
            if (index == std::string_view::npos)
            {
                type_name = remove_prefix(type_name, "class ");
                type_name = remove_prefix(type_name, "struct ");
            }
            else
            {
                type_name = type_name.substr(index + 1);
            }

            return type_name;
        }

        template<typename CharType, typename Traits, typename Alloc>
        inline auto get_type_name(const std::type_info &type, bool short_name)
        {
            std::string_view type_name = type.name();
#ifdef __GNUC__
            int status;
            name_ptr demangled_name{abi::__cxa_demangle(type_name.data(), nullptr, nullptr, &status)};
            if (demangled_name)
                type_name = demangled_name.get();
#endif
            if (short_name)
                type_name = make_short(type_name);

            return string_convert<CharType, Traits, Alloc>::from_bytes(type_name);
        }

    }

    //! \brief Gets the name of a type.
    //!
    //! On g++, this demangles the type name before returning it.
    //! 
    //! \tparam CharType The character type to use for the returned string.
    //! \tparam Traits The character traits to use.
    //! \tparam Alloc The allocator to use.
    //! \param type The type_info for the type.
    template<typename CharType = char, typename Traits = std::char_traits<CharType>, typename Alloc = std::allocator<CharType>>
    inline auto get_type_name(const std::type_info &type)
    {
        return details::get_type_name<CharType, Traits, Alloc>(type, false);
    }

    //! \brief Gets the name of a type excluding the namespace name.
    //!
    //! On g++, this demangles the type name before returning it. On VC++, it also strips the
    //! "class" or "struct" prefix.
    //! 
    //! \tparam CharType The character type to use for the returned string.
    //! \tparam Traits The character traits to use.
    //! \tparam Alloc The allocator to use.
    //! \param type The type_info for the type.
    template<typename CharType = char, typename Traits = std::char_traits<CharType>, typename Alloc = std::allocator<CharType>>
    inline auto get_short_type_name(const std::type_info &type)
    {
        return details::get_type_name<CharType, Traits, Alloc>(type, true);
    }

    //! \brief Gets the name of a type.
    //!
    //! On g++, this demangles the type name before returning it.
    //! 
    //! \tparam T The type whose name to return.
    //! \tparam CharType The character type to use for the returned string.
    //! \tparam Traits The character traits to use.
    //! \tparam Alloc The allocator to use.
    template<typename T, typename CharType = char, typename Traits = std::char_traits<CharType>, typename Alloc = std::allocator<CharType>>
    inline auto get_type_name()
    {
        return get_type_name<CharType, Traits, Alloc>(typeid(T));
    }

    //! \brief Gets the name of a type excluding the namespace name.
    //!
    //! On g++, this demangles the type name before returning it. On VC++, it also strips the
    //! "class" or "struct" prefix.
    //! 
    //! \tparam T The type whose name to return.
    //! \tparam CharType The character type to use for the returned string.
    //! \tparam Traits The character traits to use.
    //! \tparam Alloc The allocator to use.
    template<typename T, typename CharType = char, typename Traits = std::char_traits<CharType>, typename Alloc = std::allocator<CharType>>
    inline auto get_short_type_name()
    {
        return get_short_type_name<CharType, Traits, Alloc>(typeid(T));
    }
}

#endif // !defined(OOKII_TYPE_INFO_H_)
