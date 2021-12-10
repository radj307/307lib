#include "fileio.hpp"

inline bool file::write(std::ofstream& ofs, auto&& data)
{
	return ofs.is_open() && ofs << std::forward<decltype(data)>(data);
}

inline bool file::write(std::ofstream& ofs, std::stringstream&& ss)
{
	return ofs.is_open() && ofs << std::forward<std::stringbuf*>(ss.rdbuf());
}

inline bool file::write(const std::string& path, auto&& data, const bool append)
{
	std::ofstream ofs{ path, append ? std::ios_base::app : std::ios_base::out };
	return file::write(ofs, std::forward<decltype(data)>(data));
}