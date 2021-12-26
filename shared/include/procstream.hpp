/**
 * @file	procstream.hpp
 * @author	radj307
 * @brief	Wrapper object for the popen function that exposes a stream interface.
 */
#pragma once
#include <sysarch.h>
#include <make_exception.hpp>

#include <utility>
#include <cstdio>

#ifdef OS_WIN
#define POPEN _popen
#define PCLOSE _pclose
#else
#define POPEN popen
#define PCLOSE pclose
#endif

template<size_t BUFFER_SIZE = 2048>
struct procstream {
	using BufferT = std::array<char, BUFFER_SIZE>;
private:
	mutable std::unique_ptr<FILE> _proc;
	int* _rc_ptr;

public:
	procstream(int* return_code_ptr, const std::string& command, const std::string& mode = "r") : _proc{ POPEN((command).c_str(), mode.c_str()) }, _rc_ptr{ return_code_ptr }
	{
		if (!_proc.get()) {// process pipe is null
			const auto err{ errno };
			throw make_exception("procstream command \"", command, "\" failed with error code ", err, ": ", std::strerror(err));
		}
	}
	~procstream(void) noexcept
	{
		if (_proc.get()) {
			if (_rc_ptr != nullptr)
				*_rc_ptr = PCLOSE(_proc.get());
			else PCLOSE(_proc.get());
			(void)_proc.release();
		}
	}

	friend std::ostream& operator<<(std::ostream& os, const procstream& pipe)
	{
		if (pipe._proc.get())
			for (BufferT buffer{}; ::fread(&buffer, sizeof(char), BUFFER_SIZE, pipe._proc.get()); )
				for (auto& ch : buffer)
					os << ch;
		return os;
	}
};

// cleanup preprocessor
#undef POPEN
#undef PCLOSE