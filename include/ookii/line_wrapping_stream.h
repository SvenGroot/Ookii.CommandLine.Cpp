//! \file line_wrapping_stream.h
//! \brief Provides an output stream buffer, and associated stream, that allow for white-space
//!        wrapping text at the specified width.
#pragma once

#include <cassert>
#include <vector>
#include "console_helper.h"
#include "vt_helper.h"

namespace ookii
{

    //! \brief Indicates that the basic_line_wrapping_streambuf should use the console width as
    //!        the line length.
    constexpr size_t use_console_width = std::numeric_limits<size_t>::max();

    //! \brief Stream buffer that wraps lines on white-space characters at the specified line
    //!        length, and with support for indentation.
    //!
    //! This stream buffer writes its output to another stream buffer, which could belong to any
    //! stream (like a file stream, or string stream).
    //! 
    //! \warning This class assumes that the target stream buffer is at the start of a line
    //!          when writing starts, and that no other users are writing to the same stream
    //!          buffer. Otherwise, output will not be correctly wrapped or indented.
    //! 
    //! \warning Syncing this buffer will not sync the contents of the last unfinished line.
    //! 
    //! Several typedefs for common character types are provided:
    //! 
    //! Type                              | Definition
    //! --------------------------------- | -------------------------------------
    //! `ookii::line_wrapping_streambuf`  | `ookii::basic_line_wrapping_streambuf<char>`
    //! `ookii::wline_wrapping_streambuf` | `ookii::basic_line_wrapping_streambuf<wchar_t>`
    //! 
    //! \tparam CharType The type of characters used by the target stream buffer.
    //! \tparam Traits The character traits used by the target stream buffer.
    template<typename CharType, typename Traits = std::char_traits<CharType>>
    class basic_line_wrapping_streambuf : public std::basic_streambuf<CharType, Traits>
    {
    public:
        //! \brief The concrete type that this class derives from.
        using base_type = std::basic_streambuf<CharType, Traits>;
        //! \brief Integer type used by the base type.
        using int_type = typename base_type::int_type;
        //! \brief Character type used by the base type.
        using char_type = typename base_type::char_type;
        //! \brief Traits type used by the base type.
        using traits_type = typename base_type::traits_type;

        //! \brief Initializes a new instance of the basic_line_wrapping_streambuf class.
        //! 
        //! This instance cannot be used until init() is called.
        basic_line_wrapping_streambuf() noexcept = default;

        //! \brief Initializes a new instance of the basic_line_wrapping_streambuf class with the
        //!        specified underlying stream buffer and maximum line length.
        //!
        //! \param streambuf The stream buffer to write output to.
        //! \param max_line_length The maximum line length, or a value of 0 or larger than 65536
        //!        to specify no limit. Use the use_console_width constant to use the console width
        //!        as the maximum.
        basic_line_wrapping_streambuf(base_type *streambuf, size_t max_line_length)
        {
            init(streambuf, max_line_length);
        }

        //! \brief Move constructor.
        basic_line_wrapping_streambuf(basic_line_wrapping_streambuf &&other) noexcept
        {
            swap(other);
        }

        //! \brief Move assignment operator.
        basic_line_wrapping_streambuf &operator=(basic_line_wrapping_streambuf &&other) noexcept
        {
            swap(other);
            return *this;
        }

        basic_line_wrapping_streambuf(basic_line_wrapping_streambuf &) = delete;
        basic_line_wrapping_streambuf &operator=(basic_line_wrapping_streambuf &) = delete;

        //! \brief Initializes this basic_line_wrapping_streambuf instance with the specified
        //!        underlying stream buffer and maximum line length.
        //! 
        //! \param streambuf The stream buffer to write output to.
        //! \param max_line_length The maximum line length, or a value of 0 or larger than 65536
        //!        to specify no limit. Use the use_console_width constant to use the console width
        //!        as the maximum.
        //! \param count_formatting Include virtual terminal sequences when calculating the length
        //!        of a line.
        void init(base_type *streambuf, size_t max_line_length, bool count_formatting = false) noexcept
        {
            if (_base_streambuf != nullptr)
            {
                // TODO: Flush full.
                flush_buffer();
            }

            _base_streambuf = streambuf;
            _count_formatting = count_formatting;

            // Check if the caller wants to use the console width.
            if (max_line_length == use_console_width)
            {
                _max_line_length = get_console_width();
                if (_max_line_length != 0)
                {
                    // Subtract one because it looks nicer.
                    --_max_line_length;
                }
            }
            else
            {
                _max_line_length = max_line_length;
            }

            // Don't allow super long lines, to avoid allocating large buffers.
            if (_max_line_length > c_max_allowed_line_length)
            {
                _max_line_length = c_max_allowed_line_length;
            }

            // Allocate a buffer only if line wrapping is used.
            if (_max_line_length > 0)
            {
                // Make the buffer twice the length of the line to simplify the line wrapping code.
                _buffer.resize(_max_line_length * 2);
                reset_put_area();
            }
            else
            {
                // Clear out the buffer if there was one.
                _buffer = {};
                this->setp(nullptr, nullptr);
            }
        }

        //! \brief Gets the current number of spaces that each line is indented with.
        size_t indent() const
        {
            return _indent_count;
        }

        //! \brief Sets the number of spaces that each line is indented with.
        //! \param indent The new indentation size.
        void indent(size_t indent) noexcept
        {
            if (_max_line_length > 0 && indent >= _max_line_length)
                _indent_count = 0;
            else
                _indent_count = indent;
        }

        //! \brief Disables indentation for the next line.
        //!
        //! \warning If the last character written to the buffer was not a new line, one is inserted.
        //!
        //! \return `false` if a new line character was needed but could not be written; otherwise,
        //!         `true`.
        bool reset_indent()
        {
            if (_base_streambuf == nullptr)
            {
                return false;
            }

            if (this->pubsync() != 0)
            {
                return false;
            }

            if (this->pptr() != this->pbase())
            {
                if (is_eof(this->sputc(_new_line)))
                {
                    return false;
                }

                if (this->pubsync() != 0)
                {
                    return false;
                }

                assert(this->pptr() == this->pbase());
            }

            _need_indent = false;
            return true;
        }

        //! \brief Swaps the contents basic_line_wrapping_streambuf instance with another.
        //! \param other The basic_line_wrapping_streambuf to swap with.
        void swap(basic_line_wrapping_streambuf &other) noexcept
        {
            if (this != std::addressof(other))
            {
                base_type::swap(other);
                std::swap(_base_streambuf, other._base_streambuf);
                std::swap(_max_line_length, other._max_line_length);
                std::swap(_buffer, other._buffer);
                std::swap(_new_line, other._new_line);
                std::swap(_space, other._space);
                std::swap(_indent_count, other._indent_count);
                std::swap(_need_indent, other._need_indent);
                std::swap(_count_formatting, other._count_formatting);
            }
        }

    protected:
        //! \brief Ensure there is space to write at least one character to the buffer.
        //! 
        //! Called when there is no more space in the buffer, or when sync() is called.
        //!
        //! \attention This function flushes the buffer to the underlying stream buffer, wrapping
        //!            lines and adding indent as necessary. The passed character, if not eof, will
        //!            be added to the buffer afterwards.
        //! 
        //! \param ch The character to put in the buffer.
        //! \return A value not equal to `Traits::eof()` on success, and `Traits::eof()` on failure.
        virtual int_type overflow(int_type ch = traits_type::eof()) override
        {
            if (_base_streambuf == nullptr)
            {
                return traits_type::eof();
            }

            // If there is no maximum line length set, just forward the characters to the base stream.
            if (_buffer.size() == 0)
            {
                // Nothing to do if this is eof.
                if (is_eof(ch))
                {
                    return traits_type::not_eof(ch);
                }

                // If a line break, indicate the next line needs indentation, unless the previous
                // line was blank.
                if (is_new_line(ch))
                {
                    _need_indent = !_blank_line;
                    _blank_line = true;
                }
                else
                {
                    // Write indentation if needed.
                    // N.B. Don't indent blank lines.
                    if (_need_indent && !write_indent())
                    {
                       return traits_type::eof();
                    }

                    _blank_line = false;
                }

                // Write the character to the base stream.
                return _base_streambuf->sputc(traits_type::to_char_type(ch));
            }

            // If the buffer is not empty full, try to flush it.
            if (this->pptr() > this->pbase() && !flush_buffer())
            {
                return traits_type::eof();
            }

            if (!is_eof(ch))
            {
                // If the buffer is still full, try to grow it.
                if (this->pptr() == this->epptr() && !grow_buffer())
                {
                    return traits_type::eof();
                }

                // The buffer must now have space for at least one character.
                assert(this->pptr() != this->epptr());
                this->sputc(traits_type::to_char_type(ch));
            }

            // Return a character other than eof to indicate success.
            return traits_type::not_eof(ch);
        }

        //! \brief Flushes the buffer to the underlying stream buffer.
        //! \return The result of calling `pubsync()` on the underlying stream buffer.
        virtual int sync() override
        {
            // Attempt to overflow the buffer, and if that succeeds also flush the underlying stream
            // buffer.
            if (_base_streambuf != nullptr && !is_eof(overflow()))
            {
                return _base_streambuf->pubsync();
            }

            return -1;
        }

        //! \brief Change the locale of the stream buffer.
        //! \param loc The new locale.
        virtual void imbue(const std::locale &loc) override
        {
            if (_base_streambuf != nullptr)
            {
                // Change the locale of the underlying buffer.
                _base_streambuf->pubimbue(loc);
                update_locale_characters(loc);
            }
        }

    private:
        using vt_helper_type = vt::details::vt_helper<CharType, Traits>;

        // Write the contents of the buffer to the underlying stream buffer, wrapping lines and adding
        // indentation as necessary.
        bool flush_buffer()
        {
            if (_base_streambuf == nullptr)
            {
                return false;
            }

            auto start = this->pbase();
            auto end = this->pptr();

            // Only called when there is a buffer.
            assert(start != nullptr && end != nullptr);

            std::streamsize count;
            char_type *potential_line_break{};
            char_type *new_start{};
            auto locale = this->getloc();
            size_t line_length{};
            if (_need_indent)
            {
                line_length = _indent_count;
            }

            // Loop over the contents of the buffer.
            for (auto current = start; current < end; ++current)
            {
                // Check if this character can be used as a line break.
                if (std::isspace(*current, locale))
                {
                    potential_line_break = current;
                }

                if (!_count_formatting && traits_type::eq(*current, vt_helper_type::c_escape))
                {
                    auto vt_end = vt_helper_type::find_sequence_end(current + 1, end, locale);
                    if (vt_end == nullptr)
                    {
                        // Incomplete VT sequence, so we can't flush until the end is found.
                        break;
                    }

                    // Continue with the next character after the sequence.
                    current = vt_end;
                    continue;
                }

                // Check if we need to output a line.
                if (line_length >= _max_line_length || traits_type::eq(*current, _new_line))
                {
                    // Before writing the line, add indentation if necessary.
                    // N.B. Don't indent empty lines.
                    if (_need_indent)
                    {
                        line_length -= _indent_count;
                        if (line_length > 0 && !write_indent())
                        {
                            return false;
                        }
                    }

                    // If no place was found to break the line, break it here.
                    if (potential_line_break == nullptr)
                    {
                        potential_line_break = current;
                        new_start = current;
                    }
                    else
                    {
                        new_start = potential_line_break + 1;
                    }

                    // Write the line up to the break spot.
                    count = potential_line_break - start;
                    if (_base_streambuf->sputn(start, count) < count)
                    {
                        return false;
                    }

                    // Write the line break.
                    _base_streambuf->sputc(_new_line);

                    // Update the state for the new line.
                    start = new_start;
                    potential_line_break = nullptr;
                    _need_indent = line_length > 0;
                    line_length = ((current + 1) - start);
                    if (_need_indent)
                    {
                        line_length += _indent_count;
                    }
                }
                else
                {
                    ++line_length;
                }
            }

            // If we flushed any characters, move the remaining characters to the front for the next
            // flush.
            // N.B. line_length can't be used for this because it includes indent character count.
            if (start > this->pbase())
            {
                count = 0;
                if (end > start)
                {
                    count = end - start;
                    traits_type::move(this->pbase(), start, static_cast<size_t>(count));
                }

                // Reset the put area, and indicate the number of characters moved to the front.
                reset_put_area(static_cast<int>(count));
            }

            return true;
        }

        bool grow_buffer()
        {
            assert(this->epptr() == this->pptr());
            auto old_size = static_cast<int>(_buffer.size());
            auto new_size = old_size * 2;
            // Cap consumed memory at 2GB if overflow happened.
            if (new_size < old_size)
            {
                new_size = std::numeric_limits<std::int32_t>::max();
            }

            if (new_size == old_size)
            {
                return false;
            }

            _buffer.resize(new_size);
            reset_put_area(old_size);
            return true;
        }

        // Reset the put area pointers.
        void reset_put_area(int valid_data = 0)
        {
            this->setp(_buffer.data(), _buffer.data() + _buffer.size());
            if (valid_data > 0)
            {
                this->pbump(valid_data);
            }
        }

        // Update the new line and space characters based on the specified locale.
        void update_locale_characters(const std::locale &loc)
        {
            auto &ctype = std::use_facet<std::ctype<char_type>>(loc);
            _new_line = ctype.widen('\n');
            _space = ctype.widen(' ');
        }

        // Write indentation to the underlying stream buffer.
        bool write_indent()
        {
            if (_base_streambuf == nullptr)
            {
                return false;
            }

            assert(_need_indent);
            for (size_t i = 0; i < _indent_count; ++i)
            {
                if (is_eof(_base_streambuf->sputc(_space)))
                {
                    return false;
                }
            }

            _need_indent = false;
            return true;
        }

        // Checks if the character (in integer representation) equals a new line.
        bool is_new_line(int_type ch) const
        {
            return traits_type::eq_int_type(ch, traits_type::to_int_type(_new_line));
        }

        // Checks if the character (in integer representation) is an end-of-file character.
        static bool is_eof(int_type ch)
        {
            return traits_type::eq_int_type(ch, traits_type::eof());
        }

        static constexpr size_t c_max_allowed_line_length = 65536;

        base_type *_base_streambuf{};
        size_t _max_line_length{};
        std::vector<char_type> _buffer;
        char_type _new_line{};
        char_type _space{};
        size_t _indent_count{};
        bool _need_indent{};
        bool _blank_line{true};
        bool _count_formatting{};
    };

    //! \brief A line wrapping stream buffer for use with the `char` type.
    using line_wrapping_streambuf = basic_line_wrapping_streambuf<char>;
    //! \brief A line wrapping stream buffer for use with the `wchar_t` type.
    using wline_wrapping_streambuf = basic_line_wrapping_streambuf<wchar_t>;

    namespace details
    {

        // Returns a stream's buffer, or NULL if the stream doesn't use a basic_line_wrapping_streambuf.
        //
        // N.B. This may require RTTI.
        template<typename CharType, typename Traits>
        auto get_line_wrapping_streambuf(std::basic_ostream<CharType, Traits> &stream)
        {
            return dynamic_cast<basic_line_wrapping_streambuf<CharType, Traits>*>(stream.rdbuf());
        }

        // Stream manipulator to change the indent.
        struct set_indent_helper
        {
            size_t indent;
        };

        // Changes the indent size of the stream.
        //
        // N.B. Has no effect if the stream does not use a basic_line_wrapping_streambuf.
        template<typename CharType, typename Traits>
        std::basic_ostream<CharType, Traits> &operator<<(std::basic_ostream<CharType, Traits> &stream, const set_indent_helper &helper)
        {
            auto buffer = get_line_wrapping_streambuf(stream);
            if (buffer != nullptr)
            {
                buffer->indent(helper.indent);
            }

            return stream;
        }

    }

    //! \brief IO manipulator that changes the number of spaces that each line is indented with
    //!        for a line wrapping stream.
    //!
    //! \attention This has no effect if the stream does not use a basic_line_wrapping_streambuf.
    //! \warning This IO manipulator uses `dynamic_cast` and may require RTTI.
    //! 
    //! Sample:
    //! 
    //! ```
    //! stream << set_indent(4);
    //! ```
    //! 
    //! \param indent The new indentation size.
    inline details::set_indent_helper set_indent(size_t indent)
    {
        return details::set_indent_helper{indent};
    }

    //! \brief IO manipulator that lets the next line start at the beginning of the line, without
    //!        indenting it.
    //!
    //! \attention If the last character written to the buffer was not a new line, one is inserted.
    //! \attention This has no effect if the stream does not use a basic_line_wrapping_streambuf.
    //! \warning This IO manipulator uses `dynamic_cast` and may require RTTI.
    //! 
    //! Sample:
    //! 
    //! ```
    //! stream << reset_indent;
    //! ```
    //! 
    //! \tparam CharType The character type of the stream.
    //! \tparam Traits The character traits type of the stream.
    //! \param stream The stream to write to.
    template<typename CharType, typename Traits>
    std::basic_ostream<CharType, Traits> &reset_indent(std::basic_ostream<CharType, Traits> &stream)
    {
        auto buffer = details::get_line_wrapping_streambuf(stream);
        if (buffer != nullptr)
        {
            if (!buffer->reset_indent())
            {
                stream.setstate(std::ios::failbit);
            }
        }

        return stream;
    }

    //! \brief Output stream that wraps lines on white-space characters at the specified line
    //!        length, and with support for indentation.
    //! 
    //! This stream writes its output to the stream buffer of another stream, which could be any
    //! stream (like a file stream, or string stream).
    //! 
    //! \warning This class assumes that the target stream is at the start of a line
    //!          when writing starts, and that no other users are writing to the same stream.
    //!          Otherwise, output will not be correctly wrapped or indented.
    //! 
    //! \warning Flushing this stream will not flush the contents of the last unfinished line.
    //!          The only way to guarantee all content is flushed is to write a newline.
    //! 
    //! Several typedefs for common character types are provided:
    //! 
    //! Type                            | Definition
    //! ------------------------------- | -------------------------------------
    //! `ookii::line_wrapping_ostream`  | `ookii::basic_line_wrapping_ostream<char>`
    //! `ookii::wline_wrapping_ostream` | `ookii::basic_line_wrapping_ostream<wchar_t>`
    //! 
    //! \tparam CharType The type of characters used by the target stream.
    //! \tparam Traits The character traits used by the target stream.
    template<typename CharType, typename Traits = std::char_traits<CharType>>
    class basic_line_wrapping_ostream : public std::basic_ostream<CharType, Traits>
    {
    public:
        //! \brief The concrete type that this class derives from.
        using base_type = std::basic_ostream<CharType, Traits>;
        //! \brief The concrete base stream buffer type used by this stream.
        using streambuf_type = std::basic_streambuf<CharType, Traits>;

        //! \brief Initializes a new instance of the basic_line_wrapping_ostream class with the
        //!        specified underlying stream and maximum line length.
        //!
        //! \param base_stream The stream whose stream buffer to write output to.
        //! \param max_line_length The maximum line length, or a value of 0 or larger than 65536
        //!        to specify no limit. Use the use_console_width constant to use the console width
        //!        as the maximum.
        //! \param count_formatting Include virtual terminal sequences when calculating the length
        //!        of a line.
        basic_line_wrapping_ostream(base_type &base_stream, size_t max_line_length, bool count_formatting = false)
            : base_type{std::addressof(_buffer)}
        {
            _buffer.init(base_stream.rdbuf(), max_line_length, count_formatting);
            this->imbue(base_stream.rdbuf()->getloc());
        }

        //! \brief Move constructor.
        //! \param other The basic_line_wrapping_ostream to move from.
        basic_line_wrapping_ostream(basic_line_wrapping_ostream &&other) noexcept
            : base_type{std::addressof(_buffer)}
        {
            swap(other);
        }

        //! \brief Move assignment operator.
        //! \param other The basic_line_wrapping_ostream to move from.
        basic_line_wrapping_ostream &operator=(basic_line_wrapping_ostream &&other) noexcept
        {
            swap(other);
            return *this;
        }

        //! \brief Creates a basic_line_wrapping_ostream that writes to the standard output stream,
        //!        using the console width as the line width.
        //! 
        //! \param default_width The maximum line length to use if the console width cannot be
        //!        determined.
        static basic_line_wrapping_ostream for_cout(short default_width = 80)
        {
            return {console_stream<CharType>::cout(), static_cast<size_t>(get_console_width(default_width))};
        }

        //! \brief Creates a basic_line_wrapping_ostream that writes to the standard error stream,
        //!        using the console width as the line width.
        //! 
        //! \param default_width The maximum line length to use if the console width cannot be
        //!        determined.
        static basic_line_wrapping_ostream for_cerr(short default_width = 80)
        {
            return {console_stream<CharType>::cerr(), static_cast<size_t>(get_console_width(default_width))};
        }

        //! \brief Swaps this basic_line_wrapping_ostream instance with another.
        //! 
        //! \param other The instance to swap with.
        void swap(basic_line_wrapping_ostream &other) noexcept
        {
            if (this != std::addressof(other))
            {
                base_type::swap(other);
                _buffer.swap(other._buffer);
            }
        }

    private:
        basic_line_wrapping_streambuf<CharType, Traits> _buffer;
    };

    //! \brief A line wrapping stream for use with the `char` type.
    using line_wrapping_ostream = ookii::basic_line_wrapping_ostream<char>;
    //! \brief A line wrapping stream for use with the `wchar_t` type.
    using wline_wrapping_ostream = ookii::basic_line_wrapping_ostream<wchar_t>;

}