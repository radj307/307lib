#pragma once
#include <sysarch.h>
#include <Segments.h>
#include <Sequence.hpp>
#include <LineCharacter.hpp>

#ifdef OS_WIN
#ifndef TERMAPI_USE_LEGACY_ENABLE_ANSI
#include <WinTermAPI.hpp>		// Fast n' dirty method
#else
#include <winEnableANSI.hpp>	// Legacy method
#endif //#ifndef TERMAPI_USE_LEGACY_ENABLE_ANSI
#endif //#ifdef OS_WIN

// Include color library resources
#include <color-values.h>
#include <color-transform.hpp>
#include <color-format.hpp>
#include <setcolor.hpp>
#include <palette.hpp>

// Include term library resources
#include <CursorOrigin.h>
#include <Message.hpp>
#include <term.hpp>