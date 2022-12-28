//! \file platform_helper.h
//! \brief Provides platform-specific functionality depending on the target platform for compilation.
//!
//! This header needs to include a number of platform headers in order to be able to determine the
//! properties of the console. For example, windows.h needs to be included on Windows, and unistd.h
//! on other platforms.
//! 
//! Functionality included from windows.h is minimized using a number of defines, but if you
//! already included it using different settings, you can define OOKII_NO_PLATFORM_HEADERS to
//! avoid creating the defines and including the headers again. Note that on Windows you must
//! define NOMINMAX or the build will fail.
//! 
//! If you want to avoid polluting the global namespace with items from these platform headers,
//! you can define OOKII_PLATFORM_NOT_INLINE for your project to avoid defining the functions in the
//! header. Then, in one file only, you must define OOKII_PLATFORM_DEFINITION and then include
//! console_helper.h, to provide the symbols to the linker. This way, the platform headers are
//! limited to only that one file.
//! 
//! Check the unittests project for an example of how this can be done.
#ifndef OOKII_PLATFORM_HELPER_H_
#define OOKII_PLATFORM_HELPER_H_

#pragma once

#ifndef OOKII_PLATFORM_NOT_INLINE
#define OOKII_PLATFORM_FUNC(decl) inline decl
#define OOKII_PLATFORM_FUNC_HAS_BODY
#elif defined(OOKII_PLATFORM_DEFINITION)
#define OOKII_PLATFORM_FUNC(decl) decl
#define OOKII_PLATFORM_FUNC_HAS_BODY
#else
#define OOKII_PLATFORM_FUNC(decl) decl;
#endif

#ifdef _WIN32
#include "win32_helper.h"
#else
#include "linux_helper.h"
#endif

#endif