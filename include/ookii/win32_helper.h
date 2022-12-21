#ifndef OOKII_WIN32_HELPER_H_
#define OOKII_WIN32_HELPER_H_

#pragma once

#if !defined(OOKII_NO_PLATFORM_HEADERS) && (!defined(OOKII_PLATFORM_NOT_INLINE) || defined(OOKII_PLATFORM_DEFINITION))

#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#define NOSERVICE
#define NOMCX
#define NOIME
#define NOSOUND
#define NOCOMM
#define NOKANJI
#define NORPC
#define NOPROXYSTUB
#define NOIMAGE
#define NOTAPE
#include <Windows.h>
#include <io.h>

#endif

#include <optional>

namespace ookii::details
{
    OOKII_PLATFORM_FUNC(std::optional<short> get_console_width() noexcept)
#ifdef OOKII_PLATFORM_FUNC_HAS_BODY
    {
        HANDLE handle = GetStdHandle(STD_OUTPUT_HANDLE);
        CONSOLE_SCREEN_BUFFER_INFO info;
        if (GetConsoleScreenBufferInfo(handle, &info))
        {
            return static_cast<short>(info.srWindow.Right - info.srWindow.Left + 1);
        }

        return {};
    }
#endif

}

#endif