//! \file win32_helper.h
//! \brief Provides platform-specific functionality for Windows.
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

#ifdef _MSC_VER
#pragma comment(lib, "version.lib")
#endif

#endif

#include <optional>
#include <string>
#include <vector>
#include "format_helper.h"

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

#ifdef _UNICODE
    using tstring = std::wstring;
#else
    using tstring = std::string;
#endif

    struct version_info
    {
        tstring product_name;
        tstring version;
        tstring copyright;
    };

    OOKII_PLATFORM_FUNC(std::optional<tstring> get_module_file_name())
#ifdef OOKII_PLATFORM_FUNC_HAS_BODY
    {
        std::vector<TCHAR> buffer;
        DWORD count;
        do
        {
            buffer.resize(buffer.size() + MAX_PATH);
            count = GetModuleFileName(nullptr, buffer.data(), static_cast<DWORD>(buffer.size()));
        } while (count >= buffer.size());

        if (count == 0)
        {
            return {};
        }

        return tstring{buffer.begin(), buffer.begin() + count};
    }
#endif

    OOKII_PLATFORM_FUNC(std::optional<version_info> get_version_resource())
#ifdef OOKII_PLATFORM_FUNC_HAS_BODY
    {
        auto file_name = get_module_file_name();
        if (!file_name)
        {
            return {};
        }

        auto size = GetFileVersionInfoSize(file_name->data(), nullptr);
        if (size == 0)
        {
            return {};
        }

        std::vector<std::byte> buffer;
        buffer.resize(size);
        if (!GetFileVersionInfo(file_name->data(), 0, size, buffer.data()))
        {
            return {};
        }

        struct LANGANDCODEPAGE {
            WORD language;
            WORD codepage;
        } *translate;

        UINT len = 0;
        if (!VerQueryValue(buffer.data(), TEXT("\\VarFileInfo\\Translation"), reinterpret_cast<LPVOID *>(&translate), &len) ||
            len < sizeof(LANGANDCODEPAGE))
        {
            return {};
        }

        auto sub_block = OOKII_FMT_NS format(TEXT("\\StringFileInfo\\{:04x}{:04x}\\"), translate->language, translate->codepage);
        auto block = sub_block + TEXT("ProductName");
        LPTSTR value;
        if (!VerQueryValue(buffer.data(), block.data(), reinterpret_cast<LPVOID *>(&value), &len) || len == 0)
        {
            return {};
        }

        version_info result;
        result.product_name = tstring{value, value + len};
        block = sub_block + TEXT("ProductVersion");
        if (VerQueryValue(buffer.data(), block.data(), reinterpret_cast<LPVOID *>(&value), &len))
        {
            result.version = tstring{value, value + len};
        }

        block = sub_block + TEXT("LegalCopyright");
        if (VerQueryValue(buffer.data(), block.data(), reinterpret_cast<LPVOID *>(&value), &len))
        {
            result.copyright = tstring{value, value + len};
        }

        return result;
    }
#endif

}

#endif