#include "../include/fileo.hpp"

#include <fstream>

bool file::write_to(const std::filesystem::path& path, std::stringstream&& buffer, const openmode& mode)
{
	if (std::ofstream ofs(path, static_cast<std::ios_base::openmode>(mode)); ofs.is_open())
		return static_cast<bool>(ofs << std::move(buffer.rdbuf()));
	return false;
}
bool file::write_to(const std::filesystem::path& path, std::stringstream&& buffer, const bool& append)
{
	if (std::ofstream ofs(path, static_cast<std::ios_base::openmode>(append ? openmode::app : openmode::trunc)); ofs.is_open())
		return static_cast<bool>(ofs << std::move(buffer.rdbuf()));
	return false;
}
