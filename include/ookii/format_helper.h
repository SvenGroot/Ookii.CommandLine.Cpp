//! \file format_helper.h
//! \brief Allows selection between the C++20 \<format> library and libfmt.
//! 
//! This file makes it possible for Ookii.CommandLine.Cpp to work with either the standard \<format>
//! header provided in C++20, or with libfmt on compilers that don't provide \<format>.
//! 
//! To force the use of libfmt even if \<format> is available, you can define OOKII_FORCE_LIBFMT.
//! 
//! In addition, this header provides several helper types and functions used for formatting
//! strings.
#ifndef OOKII_FORMAT_HELPER_H_
#define OOKII_FORMAT_HELPER_H_

#include "string_helper.h"

#if __has_include(<format>) && !defined(OOKII_FORCE_LIBFMT)

#include <format>

//! \brief The namespace which contains the formatting library.
//!
//! This evaluates to `std::` if \<format> is available, otherwise to `fmt::`.
#define OOKII_FMT_NS std::

#else

#include <fmt/core.h>
#include <fmt/ranges.h>
#include <fmt/xchar.h>

//! \brief The namespace which contains the formatting library.
//!
//! This evaluates to `std::` if \<format> is available, otherwise to `fmt::`.
#define OOKII_FMT_NS fmt::

//! \brief Indicates libfmt is in use.
//!
//! This is only defined if \<format> is not available or OOKII_FORCE_LIBFMT is used.
#define OOKII_USING_LIBFMT

#endif

//! \brief Namespace containing helper functions for formatting.
namespace ookii::format
{
    //! \brief Helper to format using a non-const format string without needing to explicitly
    //!        construct a format_args.
    //! 
    //! \tparam Args The types of the format arguments.
    //! \param loc The locale to use for formatting.
    //! \param format The format string.
    //! \param args The arguments
    //! \return The formatted string.
    template<typename... Args>
    std::string ncformat(const std::locale &loc, std::string_view format, Args&... args)
    {
        return OOKII_FMT_NS vformat(loc, format, OOKII_FMT_NS make_format_args(args...));
    }

    //! \brief Helper to format using a non-const format wide string without needing to explicitly
    //!        construct a wformat_args.
    //! 
    //! \tparam Args The types of the format arguments.
    //! \param loc The locale to use for formatting.
    //! \param format The format string.
    //! \param args The arguments
    //! \return The formatted string.
    template<typename... Args>
    std::wstring ncformat(const std::locale &loc, std::wstring_view format, Args&... args)
    {
        return OOKII_FMT_NS vformat(loc, format, OOKII_FMT_NS make_format_args<OOKII_FMT_NS wformat_context>(std::forward<Args>(args)...));
    }
}

#endif