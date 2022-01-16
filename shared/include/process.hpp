/**
 * @file	process.hpp
 * @author	radj307
 * @brief	Contains the process namespace, which acts as an abstraction layer between the `popen` function's process streams and standard iostreams.
 */
#pragma once
#include <sysarch.h>
#include <var.hpp>
#include <make_exception.hpp>

#include <array>
#include <sstream>
#include <utility>
#include <cstdio>
#include <cstring>

#ifdef OS_WIN
#define POPEN _popen
#define PCLOSE _pclose
#else
#define POPEN popen
#define PCLOSE pclose
#endif

namespace process {
	template<typename T, size_t SIZE>
	std::array<T, SIZE> make_array(const int& val = 0)
	{
		std::array<T, SIZE> arr;
		memset(&arr, val, (sizeof(T) * SIZE));
		return arr;
	}

	/**
	 * @struct	Mode
	 * @brief	Simple abstraction layer for popen's mode parameter.
	 *\n		Input is accepted by std::string, or by any of the predefined member flags.
	 *
	 *			| Mode   | Character | Value | Description                                           |
	 *			|--------|:---------:|:-----:|:------------------------------------------------------|
	 *          | NONE   |           | 0     | Null mode. If a mode is equal to this, it is invalid. |
	 *			| READ   | 'r'       | 1     | Allows reading from the process pipe.                 |
	 *          | WRITE  | 'w'       | 2     | Allows writing to the process pipe.                   |
	 *			| BINARY | 'b'       | 4     | Opens the pipe in binary mode.                        |
	 *			| TEXT   | 't'       | 8     | Opens the pipe in text mode.                          |
	 */
	struct Mode final {
	private:
		char _v;
	public:
		/**
		 * @brief	Default Constructor
		 * @param v	Mode Value. This should be a bitwise expression using the static member modes.
		 */
		CONSTEXPR Mode(const char& v) : _v{ v } {}
		/**
		 * @brief		String Constructor
		 * @param str	A string containing at least one of the following:
		 */
		WINCONSTEXPR Mode(const std::string& str) : _v{ NONE }
		{
			for (auto& ch : str) { // iterate through string
				switch (ch) {
				case 'r': // check for read flag
					_v |= READ;
					break;
				case 'w': // check for write flag
					_v |= WRITE;
					break;
				case 'b': // check for binary flag
					_v |= BINARY;
					break;
				case 't': // check for text flag
					_v |= TEXT;
					break;
				default: // invalid mode character
					throw make_exception("Invalid mode character: \'", ch, "\' in string \"", str, "\"! Valid mode characters: 'r', 'w', 'b', 't'");
				}
			}
		}
		[[nodiscard]] CONSTEXPR operator char() const { return _v; }
		/**
		 * @brief	Check if this mode contains a given mode flag.
		 * @param o	Other Mode instance.
		 * @returns	bool
		 */
		[[nodiscard]] CONSTEXPR bool contains(const Mode& o) const
		{
			return (_v & o._v) != 0;
		}
		/**
		 * @brief	Retrieve the mode as a string for popen.
		 * @returns	std::string
		 */
		[[nodiscard]] std::string str() const
		{
			std::string s;
			if (contains(READ))
				s += 'r';
			if (contains(WRITE))
				s += 'w';
			if (contains(BINARY))
				s += 'b';
			if (contains(TEXT))
				s += 't';
			return s;
		}
		static const Mode NONE, READ, WRITE, BINARY, TEXT;
	};
	/// @brief	No modes. This is an invalid parameter.
	INLINE CONSTEXPR const Mode Mode::NONE{ 0 };
	/// @brief	Read mode. Allows reading from the process pipe.
	INLINE CONSTEXPR const Mode Mode::READ{ 1 };
	/// @brief	Write mode. Allows writing to the process pipe.
	INLINE CONSTEXPR const Mode Mode::WRITE{ 2 };
	/// @brief	Binary mode. All process pipe operations run in binary mode.
	INLINE CONSTEXPR const Mode Mode::BINARY{ 4 };
	/// @brief	Text mode. All process pipe operations run in text mode.
	INLINE CONSTEXPR const Mode Mode::TEXT{ 8 };

	/// @brief	When appended to a shell command, this will redirect STDERR to STDOUT.
	INLINE CONSTEXPR const auto SEQ_SHELL_OUTPUT_REDIRECT{ " 2>&1" };

	/**
	 * @struct				Proc
	 * @brief				Execute a process, capture STDOUT & STDERR, and retrieve the return code.
	 *\n					Note that to retrieve the return code, you must call close() once the process has completed.
	 * @tparam BUFFER_SIZE	The size of the character array buffer used to read/write from the process pipe. Defaults to 2048.
	 */
	template<size_t BUFFER_SIZE = 2048>
	struct Proc {
	private:
		mutable FILE* _pipe{ nullptr };
		CONSTEXPR static const int RETURN_ERROR_CODE{ -1 };
		int* _rc_ptr;
	public:
		/**
		 * @brief					Execute a command in the shell through a process pipe, and set the given pointer to the return code.
		 * @param return_code_ptr	Optional pointer to an integer to allow receiving the return code from inline functions.
		 *\n						If null, or if the process doesn't successfully return, this is left unmodified.
		 * @param command			The shell command to execute.
		 * @param mode				The Mode to pass to popen.
		 *
		 *							| Mode   | Description                            |
		 *							|:------:|:---------------------------------------|
		 *							| READ   | Enables reading from STDOUT & STDERR.  |
		 *							| WRITE  | Enables writing to STDIN.              |
		 *							| BINARY | Opens the process pipe in binary mode. |
		 *							| TEXT   | Opens the process pipe in text mode.   |
		 */
		Proc(int& out_return_code, const std::string& command, const Mode& mode = (Mode::READ | Mode::TEXT)) noexcept(false) : _rc_ptr{ &out_return_code }
		{
			fflush(NULL); // flush all streams
			_pipe = POPEN((command + SEQ_SHELL_OUTPUT_REDIRECT).c_str(), mode.str().c_str()); // open the pipe
			if (!_pipe) // if the pipe isn't open, throw an exception
				throw make_exception("Failed to open a pipe with command: \"", command, "\" and mode: \"", mode.str(), '\"');
		}
		/**
		 * @brief			Execute a command in the shell through a process pipe.
		 * @param command	The shell command to execute.
		 * @param mode		The Mode to pass to popen.
		 *
		 *					| Mode   | Description                            |
		 *					|:------:|:---------------------------------------|
		 *					| READ   | Enables reading from STDOUT & STDERR.  |
		 *					| WRITE  | Enables writing to STDIN.              |
		 *					| BINARY | Opens the process pipe in binary mode. |
		 *					| TEXT   | Opens the process pipe in text mode.   |
		 */
		Proc(const std::string& command, const Mode& mode = (Mode::READ | Mode::BINARY)) noexcept(false) : _rc_ptr{ nullptr }
		{
			fflush(NULL); // flush all streams
			_pipe = POPEN((command + SEQ_SHELL_OUTPUT_REDIRECT).c_str(), mode.str().c_str()); // open the pipe
			if (!_pipe) // if the pipe isn't open, throw an exception
				throw make_exception("Failed to open a pipe with command: \"", command, "\" and mode: \"", mode.str(), '\"');
		}
		/**
		 * @brief					Execute a command in the shell through a process pipe, and set the given pointer to the return code.
		 * @param return_code_ptr	Optional pointer to an integer to allow receiving the return code from inline functions.
		 *\n						If null, or if the process doesn't successfully return, this is left unmodified.
		 * @param command			The shell command to execute.
		 * @param mode				The mode to pass to popen. This string can contain 'r', 'w', 'b', and/or 't'.
		 *
		 *							| Char   | Description                            |
		 *							|:------:|:---------------------------------------|
		 *							| 'r'    | Enables reading from STDOUT & STDERR.  |
		 *							| 'w'    | Enables writing to STDIN.              |
		 *							| 'b'    | Opens the process pipe in binary mode. |
		 *							| 't'    | Opens the process pipe in text mode.   |
		 */
		Proc(int& out_return_code, const std::string& command, const std::string& mode) noexcept(false) : Proc(&out_return_code, command, std::move(Mode(mode))) {}
		/**
		 * @brief			Execute a command in the shell through a process pipe.
		 * @param command	The shell command to execute.
		 * @param mode		The mode to pass to popen. This string can contain 'r', 'w', 'b', and/or 't'.
		 *
		 *					| Char   | Description                            |
		 *					|:------:|:---------------------------------------|
		 *					| 'r'    | Enables reading from STDOUT & STDERR.  |
		 *					| 'w'    | Enables writing to STDIN.              |
		 *					| 'b'    | Opens the process pipe in binary mode. |
		 *					| 't'    | Opens the process pipe in text mode.   |
		 */
		Proc(const std::string& command, const std::string& mode) noexcept(false) : Proc(command, std::move(Mode(mode))) {}

		/**
		 * @brief	Default Destructor. Calls close if the pipe is open, and discards the return code.
		 */
		~Proc(void) noexcept
		{
			if (_pipe) {// if the pipe is open, close it:
				if (_rc_ptr)
					*_rc_ptr = this->close();
				else (void)this->close();
			}
		}

		/**
		 * @brief	Check if the process pipe is valid & open.
		 *\n		Note that this does ___not___ check whether there is pending data in the pipe, only if the pipe is open.
		 * @returns	bool
		 *\n		`true`	The process pipe is not null.
		 *\n		`false`	The process pipe is null. This can result from an error opening the pipe, or if the pipe was already closed with the close() function.
		 */
		[[nodiscard]] CONSTEXPR bool is_open() const noexcept
		{
			return(_pipe != nullptr);
		}

		/**
		 * @brief	Close the process pipe, and retrieve the return code.
		 * @returns	int
		 *			`RETURN_ERROR_CODE`	This means the pipe is already closed.
		 *			(other)				This is the return code of the command.
		 */
		[[nodiscard]] int close() const noexcept
		{
			int rc{ RETURN_ERROR_CODE };
			if (_pipe) {
				rc = PCLOSE(_pipe);
				_pipe = nullptr;
			}
			return rc;
		}

		/**
		 * @brief			Read BUFFER_SIZE characters from the process pipe, and insert them into the given character array.
		 * @param buffer	Reference of a character array with the correct size to use as the read buffer.
		 * @returns size_t	This is the number of characters that were read by `fread`.
		 */
		[[nodiscard]] size_t read(std::array<char, BUFFER_SIZE>& buffer) const { return ::fread(&buffer, sizeof(char), BUFFER_SIZE, _pipe); }
		/**
		 * @brief	Read BUFFER_SIZE characters from the process pipe.
		 * @returns	std::pair<size_t, std::array<char, BUFFER_SIZE>>
		 *\n		1) _size\_t_						The number of bytes that were successfully read.
		 *\n		2) _std::array<char, BUFFER_SIZE>_	The buffer character array.
		 */
		[[nodiscard]] std::pair<size_t, std::array<char, BUFFER_SIZE>> read() const
		{
			std::array<char, BUFFER_SIZE> buffer;
			return{ read(&buffer), buffer };
		}
		/**
		 * @brief			Write to the process pipe.
		 * @param buffer	Data is extracted from this buffer.
		 * @returns			size_t
		 */
		[[nodiscard]] size_t write(const std::array<char, BUFFER_SIZE>& buffer) const { return ::fwrite(&buffer, sizeof(char), BUFFER_SIZE, _pipe); }

		/**
		 * @brief		Receive input from the process, and insert it into an output stream.
		 * @param os	Output stream instance.
		 * @param p		Process pipe instance.
		 * @returns		std::ostream&
		 */
		friend std::ostream& operator<<(std::ostream& os, const Proc& p)
		{
			auto buff{ make_array<char, BUFFER_SIZE>() };
			while (p.read(buff)) { // while fread is receiving characters
				for (auto& ch : buff) {
					switch (ch) {
					case '\0': break;
					default:
						os << ch;
						break;
					}
				}
				buff = {};
			}
			return os;
		}
	};

	/**
	 * @brief			Execute a command in the default shell using `popen`, and return the result. STDOUT & STDERR output is discarded.
	 * @param command	The shell command to execute.
	 * @param mode		The pipe operation mode.
	 * @returns			int
	 */
	inline int exec(const std::string& command, const Mode& mode = (Mode::TEXT)) { return Proc{ command, mode }.close(); }

	/**
	 * @brief			Execute a command in the default shell using `popen`, and return the result. STDOUT & STDERR output is inserted into the given stringstream pointer.
	 * @param buffer	A pointer to a std::stringstream instance to insert the captured contents of STDOUT/STDERR.
	 *\n				Note that the buffer is only populated once the given command is finished executing. For live I/O, see the Proc struct.
	 * @param command	The shell command to execute.
	 * @param mode		The pipe operation mode.
	 * @returns			int
	 */
	inline int exec(std::stringstream* buffer, const std::string& command, const Mode& mode = (Mode::READ | Mode::TEXT))
	{
		if (buffer) { // if pointer is valid
			Proc p{ command, mode };
			*buffer << p;
			return p.close();
		}
		else return exec(command, mode); // if the pointer is invalid, discard STDOUT/STDERR.
	}
}

// cleanup preprocessor
#undef POPEN
#undef PCLOSE