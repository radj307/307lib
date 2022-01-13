/**
 * @file	getch.hpp
 * @author	radj307
 * @brief	Provides similar functionality to Microsoft's _getch() function located in the deprecated <conio.h> header.
 *\n		It can be used to retrieve user key presses without blocking until the enter key is pressed.
 */
#pragma once
#include <sysarch.h>

#include <cstdio>

#ifdef OS_WIN
// Windows headers
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
// VIRTUAL KEY CODE DOCS: https://docs.microsoft.com/en-us/windows/win32/inputdev/virtual-key-codes

namespace nstd {

    /// @brief  Checks if any virtual keys are pressed.
    inline int vk_kbhit() noexcept
    {
        PBYTE state{ nullptr };
        if (GetKeyboardState(state) && state != nullptr)
            return static_cast<int>(*state);
        return 0;
    }
    /// @brief  Checks if the specified virtual key is pressed.
    inline bool vk_pressed(const int& vk_code) noexcept
    {
        return GetAsyncKeyState(vk_code);
    }

    inline int kbhit() noexcept
    {
        return (WaitForSingleObject(GetStdHandle(STD_INPUT_HANDLE), 0) == WAIT_OBJECT_0);
    }

    inline int getch(const bool& wait_for_key_up = true) noexcept
    {
        INPUT_RECORD irec;
        HANDLE hInput{ GetStdHandle(STD_INPUT_HANDLE) };

        while (WaitForSingleObject(hInput, 0) == WAIT_OBJECT_0) {
            DWORD dwRead;
            ReadConsoleInput(hInput, &irec, 1, &dwRead);
            if (irec.EventType != KEY_EVENT)
                continue;
            else if (wait_for_key_up && irec.Event.KeyEvent.bKeyDown)
                continue;
            else {
                #ifdef UNICODE
                return static_cast<int>(irec.Event.KeyEvent.uChar.UnicodeChar);
                #else
                return static_cast<int>(irec.Event.KeyEvent.uChar.AsciiChar);
                #endif
            }
        }

        return EOF;
    }
}
#else
#include <cstdlib>
#include <unistd.h>
#include <sys/select.h>
#include <termios.h>
namespace nstd {
    inline void set_terminal_mode()
    {
        tcsetattr(0, TCSANOW, &orig_termios);
    }

    inline int kbhit()
    {

    }
    inline int getch()
    {

    }
}
#endif
