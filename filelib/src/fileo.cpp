#include "../include/fileo.hpp"

#include <fstream>

bool file::write_to(const std::filesystem::path& path, std::stringstream&& buffer, const bool& append)
{
	if (std::ofstream ofs(path, (append ? std::ios_base::app : std::ios_base::trunc)); ofs.is_open())
		return static_cast<bool>(ofs << std::move(buffer.rdbuf()));
	return false;
}
