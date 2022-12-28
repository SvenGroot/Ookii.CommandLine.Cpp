// Helper functions used by several samples.
#ifndef OOKII_SAMPLE_COMMON_H_
#define OOKII_SAMPLE_COMMON_H_

#pragma once

// Stream formatter to print optional values.
template<typename T>
std::ostream &operator<<(std::ostream &stream, const std::optional<T> &value)
{
    if (value)
        stream << *value;
    else
        stream << "(null)";

    return stream;
}

// Stream formatter to print values in a vector.
template<typename T>
std::ostream &operator<<(std::ostream &stream, const std::vector<T> &value)
{
    bool first = true;
    for (const auto &item : value)
    {
        if (first)
            first = false;
        else
            stream << ", ";

        stream << item;
    }

    return stream;
}

#endif