#include "../include/filei.hpp"

#include <fstream>

std::stringstream file::read(const std::filesystem::path& path) noexcept
{
	std::stringstream buffer;
	if (std::ifstream ifs(path); ifs.is_open())
		buffer << ifs.rdbuf();
	return std::move(buffer);
}
