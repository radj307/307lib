#include "../include/fileio.hpp"

std::stringstream file::read(const std::filesystem::path& path) noexcept
{
	std::stringstream buffer;
	if (std::ifstream ifs(path); ifs.is_open())
		buffer << ifs.rdbuf();
	return std::move(buffer);
}

bool file::write(const std::filesystem::path& path, std::stringstream&& buffer, const bool& append)
{
	if (std::ofstream ofs(path, (append ? std::ios_base::app : std::ios_base::trunc)); ofs.is_open())
		return static_cast<bool>(ofs << std::move(buffer.rdbuf()));
	return false;
}
