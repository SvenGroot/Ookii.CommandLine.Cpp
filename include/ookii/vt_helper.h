#ifndef OOKII_VT_HELPER_H_
#define OOKII_VT_HELPER_H_

#pragma once

#include "console_helper.h"
#include "scope_helper.h"
#include <string_view>
#include <locale>

//! \brief Namespace containing functions and types for working with virtual terminal sequences.
namespace ookii::vt
{
    namespace details
    {
        template<typename CharType, typename Traits>
        struct vt_helper
        {
            static constexpr CharType c_escape = '\x1b';

            static CharType *find_csi_end(CharType *begin, CharType *end, const std::locale &loc)
            {
                for (auto current = begin; current < end; ++current)
                {
                    auto ch = *current;
                    if (!std::isdigit(ch, loc) && !Traits::eq(ch, ';') && !Traits::eq(ch, ' '))
                    {
                        return current;
                    }
                }

                return nullptr;
            }

            static CharType *find_osc_end(CharType *begin, CharType *end)
            {
                bool has_escape = false;
                for (auto current = begin; current < end; ++current)
                {
                    auto ch = *current;
                    if (Traits::eq(ch, 0x7))
                    {
                        return current;
                    }

                    if (has_escape)
                    {
                        if (Traits::eq(ch, '\\'))
                        {
                            return current;
                        }

                        // Invalid next character, just pretend the escape character was the end.
                        return current - 1;
                    }

                    if (Traits::eq(ch, c_escape))
                    {
                        has_escape = true;
                    }
                }

                return nullptr;
            }

            static CharType *find_sequence_end(CharType *begin, CharType *end, const std::locale &loc)
            {
                if (begin == end)
                {
                    return begin;
                }

                switch (*begin)
                {
                case '[':
                    return find_csi_end(begin + 1, end, loc);

                case ']':
                    return find_osc_end(begin + 1, end);

                case '(':
                    ++begin;
                    if (begin != end)
                    {
                        return begin;
                    }
                    else
                    {
                        return nullptr;
                    }

                default:
                    return begin;
                }
            }
        };
    }

    //! \brief Provides constants for various virtual terminal sequences that control text format.
    namespace text_format
    {
        /// <summary>
        /// Resets the text format to the settings before modification.
        /// </summary>
        static constexpr std::string_view default_format = "\x1b[0m";
        /// <summary>
        /// Applies the brightness/intensity flag to the foreground color.
        /// </summary>
        static constexpr std::string_view bold_bright = "\x1b[1m";
        /// <summary>
        /// Removes the brightness/intensity flag to the foreground color.
        /// </summary>
        static constexpr std::string_view no_bold_bright = "\x1b[22m";
        /// <summary>
        /// Adds underline.
        /// </summary>
        static constexpr std::string_view underline = "\x1b[4m";
        /// <summary>
        /// Removes underline.
        /// </summary>
        static constexpr std::string_view no_underline = "\x1b[24m";
        /// <summary>
        /// Swaps foreground and background colors.
        /// </summary>
        static constexpr std::string_view negative = "\x1b[7m";
        /// <summary>
        /// Returns foreground and background colors to normal.
        /// </summary>
        static constexpr std::string_view positive = "\x1b[27m";
        /// <summary>
        /// Sets the foreground color to Black.
        /// </summary>
        static constexpr std::string_view foreground_black = "\x1b[30m";
        /// <summary>
        /// Sets the foreground color to Red.
        /// </summary>
        static constexpr std::string_view foreground_red = "\x1b[31m";
        /// <summary>
        /// Sets the foreground color to Green.
        /// </summary>
        static constexpr std::string_view foreground_green = "\x1b[32m";
        /// <summary>
        /// Sets the foreground color to Yellow.
        /// </summary>
        static constexpr std::string_view foreground_yellow = "\x1b[33m";
        /// <summary>
        /// Sets the foreground color to Blue.
        /// </summary>
        static constexpr std::string_view foreground_blue = "\x1b[34m";
        /// <summary>
        /// Sets the foreground color to Magenta.
        /// </summary>
        static constexpr std::string_view foreground_magenta = "\x1b[35m";
        /// <summary>
        /// Sets the foreground color to Cyan.
        /// </summary>
        static constexpr std::string_view foreground_cyan = "\x1b[36m";
        /// <summary>
        /// Sets the foreground color to White.
        /// </summary>
        static constexpr std::string_view foreground_white = "\x1b[37m";
        /// <summary>
        /// Sets the foreground color to Default.
        /// </summary>
        static constexpr std::string_view foreground_default = "\x1b[39m";
        /// <summary>
        /// Sets the background color to Black.
        /// </summary>
        static constexpr std::string_view background_black = "\x1b[40m";
        /// <summary>
        /// Sets the background color to Red.
        /// </summary>
        static constexpr std::string_view background_red = "\x1b[41m";
        /// <summary>
        /// Sets the background color to Green.
        /// </summary>
        static constexpr std::string_view background_green = "\x1b[42m";
        /// <summary>
        /// Sets the background color to Yellow.
        /// </summary>
        static constexpr std::string_view background_yellow = "\x1b[43m";
        /// <summary>
        /// Sets the background color to Blue.
        /// </summary>
        static constexpr std::string_view background_blue = "\x1b[44m";
        /// <summary>
        /// Sets the background color to Magenta.
        /// </summary>
        static constexpr std::string_view background_magenta = "\x1b[45m";
        /// <summary>
        /// Sets the background color to Cyan.
        /// </summary>
        static constexpr std::string_view background_cyan = "\x1b[46m";
        /// <summary>
        /// Sets the background color to White.
        /// </summary>
        static constexpr std::string_view background_white = "\x1b[47m";
        /// <summary>
        /// Sets the background color to Default.
        /// </summary>
        static constexpr std::string_view background_default = "\x1b[49m";
        /// <summary>
        /// Sets the foreground color to bright Black.
        /// </summary>
        static constexpr std::string_view bright_foreground_black = "\x1b[90m";
        /// <summary>
        /// Sets the foreground color to bright Red.
        /// </summary>
        static constexpr std::string_view bright_foreground_red = "\x1b[91m";
        /// <summary>
        /// Sets the foreground color to bright Green.
        /// </summary>
        static constexpr std::string_view bright_foreground_green = "\x1b[92m";
        /// <summary>
        /// Sets the foreground color to bright Yellow.
        /// </summary>
        static constexpr std::string_view bright_foreground_yellow = "\x1b[93m";
        /// <summary>
        /// Sets the foreground color to bright Blue.
        /// </summary>
        static constexpr std::string_view bright_foreground_blue = "\x1b[94m";
        /// <summary>
        /// Sets the foreground color to bright Magenta.
        /// </summary>
        static constexpr std::string_view bright_foreground_magenta = "\x1b[95m";
        /// <summary>
        /// Sets the foreground color to bright Cyan.
        /// </summary>
        static constexpr std::string_view bright_foreground_cyan = "\x1b[96m";
        /// <summary>
        /// Sets the foreground color to bright White.
        /// </summary>
        static constexpr std::string_view bright_foreground_white = "\x1b[97m";
        /// <summary>
        /// Sets the background color to bright Black.
        /// </summary>
        static constexpr std::string_view bright_background_black = "\x1b[100m";
        /// <summary>
        /// Sets the background color to bright Red.
        /// </summary>
        static constexpr std::string_view bright_background_red = "\x1b[101m";
        /// <summary>
        /// Sets the background color to bright Green.
        /// </summary>
        static constexpr std::string_view bright_background_green = "\x1b[102m";
        /// <summary>
        /// Sets the background color to bright Yellow.
        /// </summary>
        static constexpr std::string_view bright_background_yellow = "\x1b[103m";
        /// <summary>
        /// Sets the background color to bright Blue.
        /// </summary>
        static constexpr std::string_view bright_background_blue = "\x1b[104m";
        /// <summary>
        /// Sets the background color to bright Magenta.
        /// </summary>
        static constexpr std::string_view bright_background_magenta = "\x1b[105m";
        /// <summary>
        /// Sets the background color to bright Cyan.
        /// </summary>
        static constexpr std::string_view bright_background_cyan = "\x1b[106m";
        /// <summary>
        /// Sets the background color to bright White.
        /// </summary>
        static constexpr std::string_view bright_background_white = "\x1b[107m";
    }

    //! \brief Handles the lifetime of virtual terminal support.
    //!
    //! On Windows, this restores the terminal mode to its previous value when destructed. On other
    //! platforms, this does nothing.
    //!
    //! To create an instance of this class, you should use the enable_virtual_terminal_sequences()
    //! or enable_color() function.
    class [[nodiscard]] virtual_terminal_support
    {
    public:
        //! \brief Initializes a new instance of the virtual_terminal_support class.
        //!
        //! An instance created this way will indicate virtual terminal sequences are not supported
        //! and will not do anything when destructed.
        virtual_terminal_support() = default;

        virtual_terminal_support(const virtual_terminal_support &) = delete;
        virtual_terminal_support &operator=(const virtual_terminal_support &) = delete;

        //! \brief Move constructor for virtual_terminal_support.
        virtual_terminal_support(virtual_terminal_support &&) = default;
        //! \brief Move assignment operator for virtual_terminal_support.
        virtual_terminal_support &operator=(virtual_terminal_support &&) = default;

        ~virtual_terminal_support() noexcept
        {
            reset();
        }

        //! \brief Enables virtual terminal sequences for the console attached to the specified
        //! stream.
        //!
        //! Virtual terminal sequences are supported if the specified stream is a terminal according
        //! to `isatty()`, and the TERM environment variable is not set to "dumb". On Windows,
        //! enabling VT support has to succeed. On non-Windows platforms, VT support is assumed if
        //! the TERM environment variable is defined.
        //!
        //! \param stream The standard_stream to enable VT sequences for.
        //! \return An instance of the virtual_terminal_support class that will disable virtual terminal
        //! support when destructed, if the value was change. Use the
        //! virtual_terminal_support::is_supported() method to check if virtual terminal sequences are
        //! supported.
        static virtual_terminal_support enable(standard_stream stream)
        {
            if ((stream != standard_stream::output && stream != standard_stream::error) ||
                !ookii::details::is_console(stream))
            {
                return {};
            }

            // If "TERM=dumb" is set, assume no support.
            auto term = getenv("TERM");
            if (term != nullptr && strcmp(term, "dumb") == 0)
            {
                return {};
            }

#ifndef _WIN32
            // Except on Windows, TERM not set is assumed to mean no support.
            if (term == nullptr)
            {
                return {};
            }
#endif

            return {stream, set_console_vt_support(stream, true)};
        }

        //! \brief Enables color support using virtual terminal sequences for the console attached
        //! to the specified stream.
        //!
        //! If an environment variable named "NO_COLOR" exists, this function will not enable VT
        //! sequences. Otherwise, this function calls the enable_virtual_terminal_sequences()
        //! function and returns its result.
        //!
        //! \param stream The standard_stream to enable VT sequences for.
        //! \return An instance of the virtual_terminal_support class that will disable virtual
        //! terminal support when destructed, if the value was change. Use the
        //! virtual_terminal_support::is_supported() method to check if virtual terminal sequences
        //! are supported.
        static virtual_terminal_support enable_color(standard_stream stream)
        {
            if (getenv("NO_COLOR") != nullptr)
            {
                return {stream, vt_result::failed};
            }

            return enable(stream);
        }

        //! \brief Gets a value that indicates whether the stream supports virtual terminal
        //! sequences.
        bool is_supported() const noexcept
        {
            return _result != vt_result::failed;
        }

        //! \brief Gets a value that indicates whether the stream supports virtual terminal
        //! sequences.
        operator bool() const noexcept
        {
            return is_supported();
        }

        //! \brief Restores the console mode to its previous value, if it had been changed.
        //!
        //! On non-Windows platforms, this does nothing.
        void reset() noexcept
        {
            if (_result == vt_result::success)
            {
                set_console_vt_support(_stream, false);

                // Set to failed to reset() again won't do anything and is_supported will return
                // false.
                _result = vt_result::failed;
            }
        }

    private:
        virtual_terminal_support(standard_stream stream, vt_result result) noexcept
            : _stream{stream},
              _result{result}
        {
        }

        standard_stream _stream;
        vt_result _result;
    };
}

#endif