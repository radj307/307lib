#pragma once
#include <sysarch.h>
#include <Segments.h>
#include <Sequence.hpp>
#include <LineCharacter.hpp>

#ifdef OS_WIN
#include <winEnableANSI.hpp>
#endif

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