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