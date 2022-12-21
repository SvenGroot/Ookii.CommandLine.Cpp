#ifndef OOKII_LINUX_HELPER_H_
#define OOKII_LINUX_HELPER_H_

#pragma once.

#if !defined(OOKII_NO_PLATFORM_HEADERS) && (!defined(OOKII_CONSOLE_NOT_INLINE) || defined(OOKII_CONSOLE_DEFINITION))

#include <unistd.h>
#include <sys/ioctl.h>

#endif

#include <optional>

namespace ookii::details
{

    OOKII_PLATFORM_FUNC(std::optional<short> get_console_width())
#ifdef OOKII_CONSOLE_FUNC_HAS_BODY
    {
#if defined(TIOCGWINSZ)

        struct winsize ws;

        if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &ws) == 0)
        {
            return static_cast<short>(ws.ws_col);
        }

#endif

        return {};
    }
#endif

}

#endif
