//! \file console_helper.h
//! \brief Provides helpers for using the console.
#ifndef OOKII_CONSOLE_HELPER_H_
#define OOKII_CONSOLE_HELPER_H_

#pragma once

#include "platform_helper.h"
#include <iostream>

namespace ookii
{
    //! \brief Determines the width of the console.
    //! 
    //! This function returns the width of the console attached to stdout. If stdout is redirected
    //! to a file, it's platform dependent whether the width is still returned.
    //! 
    //! \param default_width The width to assume if the actual width can't be determined.
    inline short get_console_width(short default_width = 80) noexcept
    {
        auto width = details::get_console_width();
        if (width)
        {
            return *width;
        }

        return default_width;
    }

    //! \brief Represents one of the standard console streams.
    enum class standard_stream
    {
        //! \brief The standard input stream.
        input,
        //! \brief The standard output stream.
        output,
        //! \brief The standard error stream.
        error
    };

    //! \brief Template to determine the correct console streams based on the character type.
    //! 
    //! For `char` and `wchar_t` only, this template defines methods that provide access to the
    //! correct stdout, stderr and stdin streams.
    //! 
    //! \tparam CharType The character type.
    template<typename CharType>
    struct console_stream
    {
    };

    //! \brief Specialization of console_stream for `char`.
    template<>
    struct console_stream<char>
    {
        //! \brief Provides access to cout.
        static inline std::ostream &cout()
        {
            return std::cout;
        };

        //! \brief Provides access to cerr.
        static inline std::ostream &cerr()
        {
            return std::cerr;
        }

        //! \brief Provides access to cin.
        static inline std::istream &cin()
        {
            return std::cin;
        }
    };

    //! \brief Specialization of console_stream for `wchar_t`.
    template<>
    struct console_stream<wchar_t>
    {
        //! \brief Provides access to wcout.
        static inline std::wostream &cout()
        {
            return std::wcout;
        };

        //! \brief Provides access to wcerr.
        static inline std::wostream &cerr()
        {
            return std::wcerr;
        }

        //! \brief Provides access to wcin.
        static inline std::wistream &cin()
        {
            return std::wcin;
        }
    };

    namespace details
    {
        inline FILE *get_stream_file(standard_stream stream)
        {
            switch (stream)
            {
            case standard_stream::input:
                return stdin;

            case standard_stream::error:
                return stderr;

            default:
                return stdout;
            }
        }

#ifdef _WIN32
#ifdef OOKII_PLATFORM_FUNC_HAS_BODY
        OOKII_PLATFORM_FUNC(HANDLE get_stream_handle(standard_stream stream))
        {
            switch (stream)
            {
            case standard_stream::input:
                return GetStdHandle(STD_INPUT_HANDLE);

            case standard_stream::error:
                return GetStdHandle(STD_ERROR_HANDLE);

            default:
                return GetStdHandle(STD_OUTPUT_HANDLE);
            }
        }
#endif

        inline int get_stream_fd(standard_stream stream)
        {
            return _fileno(get_stream_file(stream));
        }

        OOKII_PLATFORM_FUNC(bool is_console(standard_stream stream))
#ifdef OOKII_PLATFORM_FUNC_HAS_BODY
        {
            return _isatty(get_stream_fd(stream));
        }
#endif

#else
        inline int get_stream_fd(standard_stream stream)
        {
            return fileno(get_stream_file(stream));
        }

        OOKII_PLATFORM_FUNC(bool is_console(standard_stream stream))
#ifdef OOKII_PLATFORM_FUNC_HAS_BODY
        {
            return isatty(get_stream_fd(stream));
        }
#endif
#endif
    }

    //! \brief Indicates the result of the set_console_vt_support operation.
    enum class vt_result
    {
        //! \brief Virtual terminal sequences could not be enabled.
        failed,
        //! \brief No action was taken, because the value already matched the requested setting.
        no_change,
        //! \brief Virtual terminal sequences were successfully enabled.
        success
    };

    //! \brief Enables or disables console support for virtual terminal sequences.
    //!
    //! \attention You may want to use ookii::vt::enable_virtual_terminal_sequences() or
    //! ookii::vt::enable_color() instead.
    //!
    //! On Windows, this enables virtual terminal support using `SetConsoleMode()`. If this function
    //! returns vt_result::failed, use `GetLastError()` to get more information.
    //!
    //! On other platforms, this function does nothing, and always returns `vt_result::no_change`.
    //!
    //! \param stream The standard stream to enable/disable VT support for.
    //! \param enable `true` to enable VT support, otherwise, `false`.
    //! \return One of the values of the vt_result enumeration.
#ifdef _WIN32
    OOKII_PLATFORM_FUNC(vt_result set_console_vt_support(standard_stream stream, bool enable) noexcept)
#ifdef OOKII_PLATFORM_FUNC_HAS_BODY
    {
        auto handle = details::get_stream_handle(stream);
        DWORD mode;
        if (!GetConsoleMode(handle, &mode))
        {
            return vt_result::failed;
        }

        auto flag = stream == standard_stream::input
            ? ENABLE_VIRTUAL_TERMINAL_INPUT
            : ENABLE_VIRTUAL_TERMINAL_PROCESSING;

        auto old_mode = mode;
        if (enable)
        {
            mode |= flag;
        }
        else
        {
            mode &= ~flag;
        }

        if (mode == old_mode)
        {
            return vt_result::no_change;
        }

        if (!SetConsoleMode(handle, mode))
        {
            return vt_result::failed;
        }

        return vt_result::success;
    }
#endif
#else
    inline vt_result set_console_vt_support([[maybe_unused]] standard_stream stream, [[maybe_unused]] bool enable) noexcept
    {
        return vt_result::no_change;
    }
#endif

}

#endif