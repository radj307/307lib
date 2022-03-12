#include "term.hpp"

#ifdef OS_WIN
#include <Windows.h>

[[nodiscard]] std::pair<size_t, size_t> term::getScreenBufferSize()
{
	CONSOLE_SCREEN_BUFFER_INFO csbi;
	if (!GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi)) {
		// put together a more useful error message, then throw an exception.
		constexpr const unsigned BUFFER_SIZE{ 256u };
		char msg[BUFFER_SIZE];
		const DWORD err{ GetLastError() };
		FormatMessage(
			FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
			0,
			err,
			0,
			msg,
			BUFFER_SIZE,
			0
		);
		throw make_exception("term::getScreenBufferSize() failed:  Win32 API Error!\n",
			indent(10), "Function:       \'GetConsoleScreenBufferInfo()\'\n",
			indent(10), "Error Code:     \'", err, "\'\n",
			indent(10), "Error Message:  \"", msg, "\"\n"
		);
	}
	const auto& srWindow{ csbi.srWindow };
	return{ static_cast<size_t>(srWindow.Top + srWindow.Bottom), static_cast<size_t>(srWindow.Left + srWindow.Right) };
}

#else // POSIX
#include <sys/ioctl.h>
#include <stdio.h>
#include <unistd.h>

[[nodiscard]] std::pair<size_t, size_t> term::getScreenBufferSize()
{
	winsize ts;
	ioctl(STDIN_FILENO, TIOCGWINSZ, &ts);
	return{ static_cast<size_t>(ts.ws_col), static_cast<size_t>(ts.ws_row) };
}


#endif
