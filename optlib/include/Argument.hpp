/**
 * @file	Argument.hpp
 * @author	radj307
 * @brief	The base unit of the ParamsAPI3 interface.
 */
#include <sysarch.h>
#include <string>
#include <variant>
#include <concepts>

namespace opt {
	class Argument : public std::string_view {
		std::optional<std::string_view> _cap;
	public:
		Argument(const std::string& arg) : std::string_view(arg), _cap{ std::nullopt } {}
		Argument(const std::string& arg, const std::string& cap) : std::string_view(arg), _cap{ cap } {}
		Argument(char arg, const std::string& cap) : std::string_view(&arg), _cap{ cap } {}
	};
}