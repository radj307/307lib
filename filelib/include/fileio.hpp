#pragma once
#include <sysarch.h>
#include <var.hpp>

#include <utility>
#include <fstream>
#include <sstream>
#include <type_traits>
#include <var.hpp>

#if LANG_CPP >= 17
#include <filesystem>
#endif

namespace file {
	template<typename T> concept valid_path_type = (var::same_or_derived<std::remove_cvref_t<T>, std::filesystem::path> || var::valid_string<T> || var::valid_string_view<T>);

	// IFSTREAM
	/**
	 * @brief			Read from a specified file.
	 * @tparam ReturnT	SFINAE overloaded
	 * @param path		Perfectly-forwarded path argument to std::ifstream::open
	 * @param mode		Any ios_base::openmode flags that should be passed in addition to `std::ios_base::in` to the file stream.
	 * @returns			ReturnT
	 */
	template<std::same_as<std::ifstream> ReturnT>
	inline static ReturnT read(auto&& path, const std::ios_base::openmode& mode = 0)
	{
		return std::move(ReturnT(std::forward<decltype(path)>(path), (std::ios_base::in | mode)));
	}
	// STRINGSTREAM
	template<var::same_or_derived<std::stringstream> ReturnT>
	inline static ReturnT read(auto&& path, const std::ios_base::openmode& mode = 0)
	{
		std::ifstream ifs{ std::forward<decltype(path)>(path), (std::ios_base::in | mode) };
		std::stringstream ss;
		ss.setstate(ifs.rdstate()); // copy state flags
		if (ifs.is_open()) {
			if (ifs.good()) // rdbuf is valid, file was opened
				ss << ifs.rdbuf(); // insert file's read buffer into the stringstream
		}
		return std::move(ss);
	}
	// STRING_VIEW
	template<var::valid_string_view ReturnT>
	inline static ReturnT read(auto&& path, const std::ios_base::openmode& mode = 0)
	{
		ReturnT view{ read<std::stringstream>().str() };
		return std::move(ReturnT{ read<std::stringstream>(std::forward<decltype(path)>(path), (std::ios_base::in | mode)).str() });
	}

	// DEFAULT
	inline std::stringstream read(auto&& path, const std::ios_base::openmode& mode = 0)
	{
		return std::move(read<std::stringstream>(std::forward<decltype(path)>(path), mode));
	}

	template<var::Streamable T, std::integral... Modes>
	inline static bool write(auto&& path, T&& data, const std::ios_base::openmode& mode = std::ios_base::app)
	{
		std::ofstream ofs{ std::forward<decltype(path)>(path), std::ios_base::out | mode };
		return (ofs.is_open() && (ofs << std::forward<T>(data)));
	}
	// Legacy compatibility override that accepts a boolean with whether to append the data to the target file, if it exists, instead of the raw mode.
	template<var::Streamable T>
	inline static bool write(auto&& path, T&& data, const bool& append = true)
	{
		return write<T>(std::forward<decltype(path)>(path), std::forward<T>(data), (append ? std::ios_base::app : std::ios_base::trunc));
	}
}

namespace file::legacy {
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
		if (ifs.is_open() && ifs.good())
			buff << ifs.rdbuf();
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
		if (ifs.is_open() && ifs.good())
			ifs >> buffer;
		return std::move(buffer);
	}
	inline std::stringstream read(const std::string& path)
	{
		return read<std::stringstream>(path);
	}
	#if LANG_CPP >= 17 // include std::filesystem::path support
	template<class RT> static std::enable_if_t<std::is_same_v<RT, std::ifstream>, std::ifstream>
	read(const std::filesystem::path& path)
	{
		return std::move(std::ifstream(path));
	}
	template<class RT> static std::enable_if_t<std::is_same_v<RT, std::stringstream>, std::stringstream>
	read(const std::filesystem::path& path)
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
	read(const std::filesystem::path& path)
	{
		auto ifs{ read<std::ifstream>(std::forward<decltype(path)>(path)) };
		std::string buffer;
		if (ifs.is_open())
			ifs >> buffer;
		return std::move(buffer);
	}
	inline std::stringstream read(const std::filesystem::path& path)
	{
		return std::move(read<std::stringstream>(path));
	}
	#endif
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
		return write(ofs, std::forward<T>(data));
	}
	inline bool write(const std::string& path, std::stringstream& ss, const bool append = true)
	{
		std::ofstream ofs{ path,append ? std::ios_base::app : std::ios_base::out };
		return write(ofs, ss.rdbuf());
	}
	inline bool write(const std::string& path, std::stringbuf* rdbuf, const bool append = true)
	{
		std::ofstream ofs{ path, append ? std::ios_base::app : std::ios_base::out };
		return write(ofs, rdbuf);
	}
	#if LANG_CPP >= 17
	template<typename T>
	inline bool write(const std::filesystem::path& path, T&& data, const bool append = true)
	{
		return write(path.generic_string(), std::forward<T>(data), append);
	}
	#endif
	#pragma endregion WRITE
}