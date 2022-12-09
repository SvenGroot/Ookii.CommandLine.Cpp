#ifndef OOKII_VT_HELPER_H_
#define OOKII_VT_HELPER_H_

#pragma once

#include <string_view>
#include <locale>

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
                    if (!std::isdigit(ch, loc) && Traits::eq(ch, ';') && Traits::eq(ch, ' '))
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
}

#endif