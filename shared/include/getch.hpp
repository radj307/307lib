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
#include <WindowsNotToday.h>
#include <Windows.h>

namespace nstd {
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

    /**
     * @brief		Check if the given read file descriptor has pending data.
     * @param fd	The target file descriptor. (STDIN = 0)
     * @param s		Timeout in seconds.
     * @param us	Additional timeout in nanoseconds.
     * @returns		bool
     *\n			true	There is pending input.
     *\n			false	There is no pending input.
     */
    inline bool hasPendingInput(const int& fd)
    {
        struct timespec timeout { 0L, 0L };
        fd_set fds; // create a file descriptor set
        FD_ZERO(&fds);
        FD_SET(fd, &fds); // stdin file descriptor is 0
        fflush(NULL); // flush input buffer
        return pselect(fd + 1, &fds, nullptr, nullptr, &timeout, nullptr) == 1;
    }

    /**
     * @brief	Check if STDIN has pending input.
     * @returns	bool
     */
    inline int kbhit()
    {
        return !!hasPendingInput(0);
    }
}
#endif
