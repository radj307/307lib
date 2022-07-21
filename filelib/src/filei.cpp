#include "../include/filei.hpp"

#include <fstream>

std::stringstream file::read(const std::filesystem::path& path, const openmode& mode) noexcept
{
	std::stringstream buffer;
	if (std::ifstream ifs(path, static_cast<std::ios_base::openmode>(mode)); ifs.is_open())
		buffer << ifs.rdbuf();
	return std::move(buffer);
}

std::stringstream file::read(const std::filesystem::path& path, const std::ios_base::openmode& mode) noexcept
{
	std::stringstream buffer;
	if (std::ifstream ifs(path, static_cast<std::ios_base::openmode>(mode)); ifs.is_open())
		buffer << ifs.rdbuf();
	return std::move(buffer);
}
