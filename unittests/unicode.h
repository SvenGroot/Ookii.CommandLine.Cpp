#ifndef OOKII_UNICODE_H_
#define OOKII_UNICODE_H_

#pragma once

#include <iostream>
#include <sstream>
#include <ookii/line_wrapping_stream.h>

#ifndef WIDEN
#define OOKII_WIDEN(t) L ## t
#define WIDEN(t) OOKII_WIDEN(t)
#endif

#ifdef _UNICODE

#ifndef TEXT
#define TEXT(t) WIDEN(t)
#endif

#define TMAIN wmain

#else

#ifndef TEXT
#define TEXT(t) t
#endif

#define TMAIN main

#endif

namespace ookii
{
#ifdef _UNICODE
    using tchar_t = wchar_t;

    namespace
    {
        std::wostream &tcout = std::wcout;
        std::wistream &tcin = std::wcin;
        std::wostream &tcerr = std::wcerr;
    }

#else
    using tchar_t = char;

    namespace
    {
        std::ostream &tcout = std::cout;
        std::istream &tcin = std::cin;
        std::ostream &tcerr = std::cerr;
    }

#endif

    using tstring = std::basic_string<tchar_t>;
    using tstring_view = std::basic_string_view<tchar_t>;
    using tostream = std::basic_ostream<tchar_t>;
    using tistream = std::basic_istream<tchar_t>;
    using tiostream = std::basic_iostream<tchar_t>;
    using tstringstream = std::basic_stringstream<tchar_t>;
    using tline_wrapping_stream = basic_line_wrapping_ostream<tchar_t>;

}

#endif