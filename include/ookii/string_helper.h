//! \file string_helper.h
//! \brief Provides helper types and functions for working with strings.
#ifndef OOKII_STRING_HELPER_H_
#define OOKII_STRING_HELPER_H_

#pragma once

#include <locale>
#include <sstream>
#include <optional>
#include <array>
#include <string>
#include <string_view>
#include <algorithm>

namespace ookii
{

    //! \brief A version of the std::less predicate for strings that supports case insensitive
    //!        comparison.
    struct string_less
    {
    public:
        //! \brief Indicates that this function accepts any type and uses perfect forwarding.
        using is_transparent = int;

        //! \brief Initializes a new instance of the StringLess class.
        //! \param case_sensitive `true` to perform case sensitive comparisons; `false` to perform
        //!        case insensitive comparisons.
        //! \param loc The locale to use for case insensitive comparisons. This is not used for
        //!        case sensitive comparisons.
        string_less(bool case_sensitive = true, std::locale loc = {})
            : _case_sensitive{case_sensitive},
              _locale{loc}
        {
        }

        //! \brief Compares two strings.
        //! \tparam Range1 The type of the first string.
        //! \tparam Range2 The type of the second string.
        //! \param left The first string.
        //! \param right The second string.
        //! \return `true` if left is less than right; otherwise, `false`.
        template<typename Range1, typename Range2>
        bool operator()(Range1 &&left, Range2 &&right) const
        {
            // Using enables ADL.
            using std::begin;
            using std::end;
            if (_case_sensitive)
            {
                return std::lexicographical_compare(begin(left), end(left), begin(right), end(right), std::less<>{});
            }
            else
            {
                return std::lexicographical_compare(begin(left), end(left), begin(right), end(right), [this](auto left, auto right) 
                    {
                        return std::toupper(left, _locale) < std::toupper(right, _locale); 
                    });
            }
        }

    private:
        bool _case_sensitive;
        std::locale _locale;
    };

    //! \brief Compares two strings, ignoring their case.
    //! \tparam CharType The character type used.
    //! \tparam Traits The character traits used.
    //! \param string1 The first string.
    //! \param string2 The second string.
    //! \param locale The locale to use for case conversion.
    //! \return `true` if the strings are equal, ignoring case; otherwise, `false`.
    template<typename CharType, typename Traits>
    bool string_equal_case_insensitive(std::basic_string_view<CharType, Traits> string1, std::basic_string_view<CharType, Traits> string2, const std::locale &locale = {})
    {
        return std::equal(string1.begin(), string1.end(), string2.begin(), string2.end(), [&locale](auto left, auto right) 
            {
                return std::toupper(left, locale) == std::toupper(right, locale); 
            });
    }

    //! \brief Converts a simple ASCII string literal to the specified character type at compile time.
    //!
    //! This function is intended to be used on simple literals, to allow template functions to use
    //! string literals where the character type depends on the template parameters.
    //!
    //! \warning This function simply copies the source array to the target array, and will not
    //!          work correctly if the source array contains multi-byte characters.
    //!
    //! \tparam CharType The target character type.
    //! \tparam Length The length of the string.
    //! \param value The character sequence to convert.
    //! \return A `std::array` holding the converted character sequence.
    template<typename CharType, size_t Length>
    constexpr const std::array<CharType, Length> literal_cast(const char (&value)[Length])
    {
        std::array<CharType, Length> result{};
        std::copy(std::begin(value), std::end(value), result.begin());
        return result;
    }

    //! \brief Performs a simple conversion of a narrow character string to a specified character
    //!        type.
    //!
    //! \warning This method is marginally safer than literal_cast, because it uses the specified
    //!          locale for the conversion. However, it still works on a per-character basis and
    //!          will not work correctly on strings containing multi-byte characters.
    //! 
    //! \tparam CharType The character type used.
    //! \tparam Traits The character traits to use. Defaults to `std::char_traits<CharType>`.
    //! \tparam Alloc The allocator to use. Defaults to `std::allocator<CharType>`.
    template<typename CharType, typename Traits = std::char_traits<CharType>, typename Alloc = std::allocator<CharType>>
    struct string_convert
    {
        //! \brief Converts a string to the specified character type.
        //! \param value The string to convert.
        //! \param loc The locale to use to widen the characters.
        //! \return The converted string.
        static std::basic_string<CharType, Traits, Alloc> from_bytes(std::string_view value, const std::locale &loc = {})
        {
            std::basic_string<CharType, Traits, Alloc> result;
            auto &facet = std::use_facet<std::ctype<CharType>>(loc);
            result.reserve(value.length());
            std::transform(value.begin(), value.end(), std::back_inserter(result), [&facet](auto c)
                {
                    return facet.widen(c);
                });

            return result;
        }
    };

    //! \brief Specialization of string_convert where the target type is also char.
    //! \tparam Traits The character traits to use.
    //! \tparam Alloc The allocator to use.
    template<typename Traits, typename Alloc>
    struct string_convert<char, Traits, Alloc>
    {
        //! \brief Converts a string to the specified character type.
        //! \param value The string to convert.
        //! \return The same string as the `value` parameter.
        static std::basic_string<char, Traits, Alloc> from_bytes(std::string_view value, const std::locale & = {})
        {
            return std::basic_string<char, Traits, Alloc>{value};
        }
    };

    //! \brief Template class used to convert strings to strongly typed argument values.
    //! 
    //! This base template uses a stringstream to do the conversion, relying on stream extraction
    //! `operator>>` for the provided type. The template can be specialized to provide conversion
    //! of custom types.
    //! 
    //! \tparam T The type to convert to.
    //! \tparam CharType The character type used.
    //! \tparam Traits The character traits to use. Defaults to `std::char_traits<CharType>`.
    //! \tparam Alloc The allocator to use. Defaults to `std::allocator<CharType>`.
    template<typename T, typename CharType, typename Traits = std::char_traits<CharType>, typename Alloc = std::allocator<CharType>>
    struct lexical_convert
    {
        //! \brief Convert a string to the specified type.
        //! \param value The string value to convert.
        //! \param loc The locale to use for conversion.
        //! \return The converted value, or `std::nullopt` if conversion failed.
        static std::optional<T> from_string(std::basic_string_view<CharType, Traits> value, const std::locale &loc = {})
        {
            std::basic_istringstream<CharType> stream{std::basic_string<CharType, Traits, Alloc>{value}};
            stream.imbue(loc);
            // Unsetting all base flags allows the stream to determine the base from the prefix
            // when converting a string to a number.
            stream.unsetf(std::ios::dec);
            stream.unsetf(std::ios::oct);
            stream.unsetf(std::ios::hex);
            T result{};
            stream >> result;
            if (!stream || !stream.eof())
                return {};

            return result;
        }
    };

    //! \brief Specialization of lexical_convert for strings.
    //!
    //! Since no conversion is required to convert a string to a string, this type just returns
    //! the value unmodified.
    //!
    //! \tparam CharType The character type used.
    //! \tparam Traits The character traits to use. Defaults to `std::char_traits<CharType>`.
    //! \tparam Alloc The allocator to use. Defaults to `std::allocator<CharType>`.
    template<typename CharType, typename Traits, typename Alloc>
    struct lexical_convert<std::basic_string<CharType, Traits, Alloc>, CharType, Traits, Alloc>
    {
        //! \brief Convert a string to the specified type.
        //! \param value The string value to convert.
        //! \return The converted value, or `std::nullopt` if conversion failed.
        static std::optional<std::basic_string<CharType, Traits, Alloc>> from_string(std::basic_string_view<CharType, Traits> value, const std::locale &)
        {
            return {std::basic_string<CharType, Traits, Alloc>{value}};
        }
    };

    //! \brief A pseudo-range for string tokenization.
    //!
    //! This type lets the user tokenize a string and iterate over the results. It's not quite a
    //! real range, so cannot be used with view adapters.
    //!
    //! \tparam CharType The character type of the string to tokenize.
    //! \tparam Traits The character traits of the string to tokenize. Defaults to `std::char_traits<CharType>`.
    template<typename CharType, typename Traits = std::char_traits<CharType>>
    class tokenize
    {
        class iterator
        {
        public:
            using iterator_concept = std::forward_iterator_tag;
            using value_type = std::basic_string_view<CharType, Traits>;
            using difference_type = std::ptrdiff_t;
            using pointer = value_type*;
            using reference = value_type&;

            iterator() noexcept = default;
    
            iterator(value_type value, CharType separator) noexcept
                : _remaining{value},
                  _separator{separator}
            {
                next_value();
            }

            iterator &operator++() noexcept
            {
                next_value();
                return *this;
            }

            iterator operator++(int) noexcept
            {
                iterator temp = *this;
                next_value();
                return temp;
            }

            value_type operator*() const noexcept
            {
                return _value;
            }

            bool operator==(const iterator &other) const noexcept
            {
                return _value == other._value && _remaining == other._remaining;
            }

        private:
            void next_value() noexcept
            {
                if (_remaining.length() != 0)
                {
                    auto index = _remaining.find_first_of(_separator);
                    _value = _remaining.substr(0, index);
                    if (index == std::string_view::npos)
                    {
                        _remaining = {};
                    }
                    else
                    {
                        _remaining = _remaining.substr(index + 1);
                    }
                }
                else
                {
                    _value = {};
                }
            }

            value_type _value;
            value_type _remaining;
            CharType _separator;
        };

    public:
        //! \brief Initializes a new instance of the tokenize class.
        //! \param value The string to tokenize.
        //! \param separator The separator that divides the tokens.
        tokenize(typename iterator::value_type value, CharType separator) noexcept
            : _value{value},
              _separator{separator}
        {
        }

        //! \brief Gets a forward iterator to the first token.
        iterator begin() const noexcept
        {
            return iterator{_value, _separator};
        }

        //! \brief Gets an iterator that will compare equal once the iterator returned by begin()
        //!        no longer has any tokens to return.
        iterator end() const noexcept
        {
            return iterator{};
        }

    private:
        typename iterator::value_type _value;
        CharType _separator;
    };

}

#endif