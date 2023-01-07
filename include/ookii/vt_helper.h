//! \file vt_helper.h
//! \brief Provides types for using virtual terminal sequences with console output.
#ifndef OOKII_VT_HELPER_H_
#define OOKII_VT_HELPER_H_

#pragma once

#include "console_helper.h"
#include "scope_helper.h"
#include "format_helper.h"
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
        
        //! \brief Resets the text format to the settings before modification.
        constexpr const char *default_format = "\x1b[0m";
        
        //! \brief Applies the brightness/intensity flag to the foreground color.
        constexpr const char *bold_bright = "\x1b[1m";
        
        //! \brief Removes the brightness/intensity flag to the foreground color.
        constexpr const char *no_bold_bright = "\x1b[22m";
        
        //! \brief Adds underline.
        constexpr const char *underline = "\x1b[4m";
        
        //! \brief Removes underline.
        constexpr const char *no_underline = "\x1b[24m";
        
        //! \brief Swaps foreground and background colors.
        constexpr const char *negative = "\x1b[7m";
        
        //! \brief Returns foreground and background colors to normal.
        constexpr const char *positive = "\x1b[27m";
        
        //! \brief Sets the foreground color to black.
        constexpr const char *foreground_black = "\x1b[30m";
        
        //! \brief Sets the foreground color to red.
        constexpr const char *foreground_red = "\x1b[31m";
        
        //! \brief Sets the foreground color to green.
        constexpr const char *foreground_green = "\x1b[32m";
        
        //! \brief Sets the foreground color to yellow.
        constexpr const char *foreground_yellow = "\x1b[33m";
        
        //! \brief Sets the foreground color to blue.
        constexpr const char *foreground_blue = "\x1b[34m";
        
        //! \brief Sets the foreground color to magenta.
        constexpr const char *foreground_magenta = "\x1b[35m";
        
        //! \brief Sets the foreground color to cyan.
        constexpr const char *foreground_cyan = "\x1b[36m";
        
        //! \brief Sets the foreground color to white.
        constexpr const char *foreground_white = "\x1b[37m";
        
        //! \brief Sets the foreground color to default.
        constexpr const char *foreground_default = "\x1b[39m";
        
        //! \brief Sets the background color to black.
        constexpr const char *background_black = "\x1b[40m";
        
        //! \brief Sets the background color to red.
        constexpr const char *background_red = "\x1b[41m";
        
        //! \brief Sets the background color to green.
        constexpr const char *background_green = "\x1b[42m";
        
        //! \brief Sets the background color to yellow.
        constexpr const char *background_yellow = "\x1b[43m";
        
        //! \brief Sets the background color to blue.
        constexpr const char *background_blue = "\x1b[44m";
        
        //! \brief Sets the background color to magenta.
        constexpr const char *background_magenta = "\x1b[45m";
        
        //! \brief Sets the background color to cyan.
        constexpr const char *background_cyan = "\x1b[46m";
        
        //! \brief Sets the background color to white.
        constexpr const char *background_white = "\x1b[47m";
        
        //! \brief Sets the background color to default.
        constexpr const char *background_default = "\x1b[49m";
        
        //! \brief Sets the foreground color to bright black.
        constexpr const char *bright_foreground_black = "\x1b[90m";
        
        //! \brief Sets the foreground color to bright red.
        constexpr const char *bright_foreground_red = "\x1b[91m";
        
        //! \brief Sets the foreground color to bright green.
        constexpr const char *bright_foreground_green = "\x1b[92m";
        
        //! \brief Sets the foreground color to bright yellow.
        constexpr const char *bright_foreground_yellow = "\x1b[93m";
        
        //! \brief Sets the foreground color to bright blue.
        constexpr const char *bright_foreground_blue = "\x1b[94m";
        
        //! \brief Sets the foreground color to bright magenta.
        constexpr const char *bright_foreground_magenta = "\x1b[95m";
        
        //! \brief Sets the foreground color to bright cyan.
        constexpr const char *bright_foreground_cyan = "\x1b[96m";
        
        //! \brief Sets the foreground color to bright white.
        constexpr const char *bright_foreground_white = "\x1b[97m";
        
        //! \brief Sets the background color to bright black.
        constexpr const char *bright_background_black = "\x1b[100m";
        
        //! \brief Sets the background color to bright red.
        constexpr const char *bright_background_red = "\x1b[101m";
        
        //! \brief Sets the background color to bright green.
        constexpr const char *bright_background_green = "\x1b[102m";
        
        //! \brief Sets the background color to bright yellow.
        constexpr const char *bright_background_yellow = "\x1b[103m";
        
        //! \brief Sets the background color to bright blue.
        constexpr const char *bright_background_blue = "\x1b[104m";
        
        //! \brief Sets the background color to bright magenta.
        constexpr const char *bright_background_magenta = "\x1b[105m";
        
        //! \brief Sets the background color to bright cyan.
        constexpr const char *bright_background_cyan = "\x1b[106m";
        
        //! \brief Sets the background color to bright white.
        constexpr const char *bright_background_white = "\x1b[107m";

        
        //! \brief Returns the virtual terminal sequence to the foreground or background color to an RGB
        //! color.
        //! 
        //! \param red The red component of the color, between 0 and 255.
        //! \param green The green component of the color, between 0 and 255.
        //! \param blue The blue component of the color, between 0 and 255.
        //! \param foreground `true` to apply the color to the foreground; otherwise, it's applied
        //! to the background.
        //! \return A string with the virtual terminal sequence.
        template<typename CharType = ookii::details::default_char_type>
        inline std::basic_string<CharType> get_extended_color(int red, int green, int blue, bool foreground = true)
        {
            static constexpr auto format_string = literal_cast<CharType>("\x1b[{};2;{};{};{}m");
            return OOKII_FMT_NS format(format_string.data(), (foreground ? 38 : 48), red, green, blue);
        }
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

            bool has_term = false;
#ifdef _WIN32
            char term[10]{};
            size_t required;
            // If the buffer is too small, it can't be equal to "dumb", so we don't care.
            getenv_s(&required, term, "TERM");
            has_term = required > 0;
#else
            auto term = getenv("TERM");
            has_term = term != nullptr;
#endif

            // If "TERM=dumb" is set, assume no support.
            if (has_term && strcmp(term, "dumb") == 0)
            {
                return {};
            }

#ifndef _WIN32
            // Except on Windows, TERM not set is assumed to mean no support.
            if (!has_term)
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
            bool has_env;
#ifdef _WIN32
            size_t required;
            getenv_s(&required, nullptr, 0, "NO_COLOR");
            has_env = required > 0;
#else
            has_env = getenv("NO_COLOR") != nullptr;
#endif
            if (has_env)
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