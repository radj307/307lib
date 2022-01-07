#include "../include/fileio.hpp"

/**
 * @brief		Read the contents of a file to a stringstream.
 * @param path	Target Filepath
 * @returns		std::stringstream&&
 */
std::stringstream file::read(const std::filesystem::path& path) noexcept
{
	std::stringstream buffer;
	if (std::ifstream ifs(path); ifs.is_open())
		buffer << ifs.rdbuf();
	return std::move(buffer);
}

/**
 * @brief			Write the contents of a stringstream buffer to a file.
 * @param path		Target filepath
 * @param buffer	Stringstream rvalue reference.
 * @param append	When true, the buffer is appended to the end of the file rather than overwriting the previous contents.
 * @returns			bool
 *\n				true	Successfully wrote all data to file without error.
 *\n				false	Failed to write all data to file because of an error.
 */
bool file::write(const std::filesystem::path& path, std::stringstream&& buffer, const bool& append)
{
	if (std::ofstream ofs(path, (append ? std::ios_base::app : std::ios_base::trunc)); ofs.is_open())
		return static_cast<bool>(ofs << std::move(buffer.rdbuf()));
	return false;
}
