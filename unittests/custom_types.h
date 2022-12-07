#ifndef OOKII_CUSTOM_TYPES_H_
#define OOKII_CUSTOM_TYPES_H_

#pragma once

#include <iostream>
#include <string>
#include <optional>
#include <ookii/string_helper.h>
#include "unicode.h"

// Custom argument type that uses a stream extractor for conversion.
struct point
{
    int x;
    int y;
};

inline ookii::tistream &operator>>(ookii::tistream &stream, point &value)
{
    point result;
    ookii::tchar_t separator;
    stream >> result.x;
    stream >> separator;
    stream >> result.y;
    if (stream)
    {
        if (separator == ',')
            value = result;
        else
            stream.setstate(std::ios_base::failbit);
    }

    return stream;
}

template<>
struct OOKII_FMT_NS formatter<point, ookii::tchar_t> : public OOKII_FMT_NS formatter<int, ookii::tchar_t>
{
    template<typename FormatContext>
    auto format(point value, FormatContext &ctx)
    {
        return OOKII_FMT_NS format_to(ctx.out(), TEXT("{},{}"), value.x, value.y);
    }
};

inline std::basic_ostream<ookii::tchar_t> &operator<<(std::basic_ostream<ookii::tchar_t> &stream, point value)
{
    stream << value.x << TEXT(",") << value.y;
    return stream;
}

// Custom argument type that specializes lexical_convert.
enum class animal
{
    dog,
    cat,
    hamster
};

constexpr const ookii::tstring_view animal_strings[] = {
    TEXT("dog"),
    TEXT("cat"),
    TEXT("hamster")
};

template<>
struct ookii::lexical_convert<animal, ookii::tchar_t>
{
    static std::optional<animal> from_string(ookii::tstring_view value, const std::locale &loc)
    {
        auto it = std::find_if(std::begin(animal_strings), std::end(animal_strings), [&](auto item)
            {
                return string_equal_case_insensitive(item, value, loc);
            });

        if (it == std::end(animal_strings))
            return {};

        return static_cast<animal>(std::distance(std::begin(animal_strings), it));
    }
};

template<>
struct OOKII_FMT_NS formatter<animal, ookii::tchar_t> : public OOKII_FMT_NS formatter<ookii::tstring_view, ookii::tchar_t>
{
    template<typename FormatContext>
    auto format(animal value, FormatContext &ctx)
    {
        ookii::tstring_view string_value;
        if (static_cast<int>(value) < 0 || static_cast<int>(value) >= static_cast<int>(std::size(animal_strings)))
        {
            string_value = TEXT("invalid");
        }
        else
        {
            string_value = animal_strings[static_cast<size_t>(value)];
        }

        return OOKII_FMT_NS formatter<ookii::tstring_view, ookii::tchar_t>::format(string_value, ctx);
    }
};

inline std::basic_ostream<ookii::tchar_t> &operator<<(std::basic_ostream<ookii::tchar_t> &stream, animal value)
{
    ookii::tstring_view string_value;
    if (static_cast<int>(value) < 0 || static_cast<int>(value) >= static_cast<int>(std::size(animal_strings)))
    {
        string_value = TEXT("invalid");
    }
    else
    {
        string_value = animal_strings[static_cast<size_t>(value)];
    }

    stream << string_value;
    return stream;
}

#endif