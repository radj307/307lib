#pragma once
#include <sysarch.h>
#include <utility>
#include <fstream>
#include <sstream>

namespace file {
#ifdef read
#undef read
#endif
#ifdef write
#undef write
#endif
#pragma region READ
	template<class RT> static std::enable_if_t<std::is_same_v<RT, std::ifstream>, std::ifstream>
	read(const std::string& path)
	{
		return std::move(std::ifstream(path));
	}
	template<class RT> static std::enable_if_t<std::is_same_v<RT, std::stringstream>, std::stringstream>
	read(const std::string& path)
	{
		auto ifs{ read<std::ifstream>(path) };
	#pragma warning(disable: 26800) // suppress "Use of a moved from object (Lifetime.1)" warning as no move operations are performed here
		std::stringstream buff;
		if (ifs.is_open())
			ifs >> buff.rdbuf();
		else
			buff.setstate(std::ios::failbit);
		return std::move(buff);
	#pragma warning(default: 26800)
	}
	template<class RT> static std::enable_if_t<std::is_same_v<RT, std::string>, std::string>
	read(const std::string& path)
	{
		auto ifs{ read<std::ifstream>(std::forward<decltype(path)>(path)) };
		std::string buffer;
		if (ifs.is_open())
			ifs >> buffer;
		return std::move(buffer);
	}
	inline std::stringstream read(const std::string& path)
	{
		return std::move(read<std::stringstream>(path));
	}
#pragma endregion READ
#pragma region WRITE
	/**
	 * @brief		Write data to the given output filestream.
	 * @param ofs	An open filestream.
	 * @param data	Data to write to file. Must have a std::ostream& operator<<
	 * @returns		bool
	 */
	template<var::Streamable T>
	inline bool write(std::ofstream& ofs, const T& data)
	{
		return ofs.is_open() && ofs << data;
	}
	/**
	 * @brief		Write a stringstream to the given output filestream.
	 * @param ofs	An open filestream.
	 * @param ss	A stringstream to write.
	 * @returns		bool
	*/
	inline bool write(std::ofstream& ofs, std::stringstream&& ss)
	{
		return ofs.is_open() && ofs << ss.rdbuf();
	}
	inline bool write(std::ofstream& ofs, const std::stringbuf* rdbuf)
	{
		return ofs.is_open() && ofs << rdbuf;
	}

	/**
	 * @brief Write data to a file.
	 * @param path		- Target filepath.
	 * @param data		- rvalue ref of the data to write to file.
	 * @param append	- When true, appends the given data to the end of the file if it exists, rather than overwriting it.
	 * @returns bool
	 */
	template<var::Streamable T>
	inline bool write(const std::string& path, T&& data, const bool append = true)
	{
		std::ofstream ofs{ path, append ? std::ios_base::app : std::ios_base::out };
		return file::write(ofs, std::forward<T>(data));
	}
	inline bool write(const std::string& path, std::stringstream& ss, const bool append = true)
	{
		std::ofstream ofs{ path,append ? std::ios_base::app : std::ios_base::out };
		return file::write(ofs, ss.rdbuf());
	}
	inline bool write(const std::string& path, const std::stringbuf* rdbuf, const bool append = true)
	{
		std::ofstream ofs{ path, append ? std::ios_base::app : std::ios_base::out };
		return write(ofs, rdbuf);
	}
#pragma endregion WRITE
}