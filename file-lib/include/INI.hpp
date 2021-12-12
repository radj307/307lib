#pragma once
#include <sysarch.h>
#include <fileio.hpp>
#include <fileutil.hpp>
#include <str.hpp>
#include <make_exception.hpp>
#ifndef INI_USE_EXPERIMENTAL
#include <container/ContainerINI.hpp>

#include <unordered_map>
#include <variant>

/**
 * @namespace file::ini
 * @brief Contains methods and objects related to parsing & interacting with .ini format files.
 */
namespace file::ini {
	/**
	 * @namespace _internal
	 * @brief Contains methods used internally by the INI struct.
	 */
	namespace _internal {
		/**
		 * @brief Splits a line by the first delimiter that occurs after a given position, then strips both strings of trailing/preceeding whitespace and returns them.
		 * @param line	- The string to split.
		 * @param delim	- Delimiter character to split the given line by. The delimiter is discarded.
		 * @param off	- Position in line to begin searching for delim.
		 * @returns optional<pair<string, string>>
		 */
		inline std::pair<std::string, std::string> split_and_strip_line(std::string line, const char delim, const size_t off = 0u)
		{
			const auto pos{ line.find(delim, off) };
			if (str::pos_valid(pos))
				return std::make_pair(str::strip_line(line.substr(0u, pos)), str::strip_line(line.substr(pos + 1u)));
			return{ line, {} };
		}

		/**
		 * @brief					Strips quotation marks from a string.
		 * @param str				Input String
		 * @param strip_all_quotes	When true, all quotation marks appearing in the string are removed rather than only enclosing quotes.
		 * @returns					std::string
		 */
		inline std::string strip_quotes(std::string str, const bool& strip_all_quotes = false)
		{
			if (!str.empty()) {
				if (strip_all_quotes) {
					str.erase(std::remove_if(str.begin(), str.end(), isquote), str.end());
					return str;
				}
				else
					return str::strip_line(str, "", "\"\'");
			}
			return str;
		}

		/**
		 * @brief Parse a stringstream containing raw INI text into an INI container.
		 * @param buffer	- stringstream to parse.
		 * @param lineDelim	- Delimiter used to detect line endings.
		 * @returns ContainerINI
		 */
		inline ContainerINI::SectionMap parseINI(std::stringstream buffer, const char lineDelim = '\n')
		{
			if (buffer.fail()) // if failbit is set, throw early
				throw make_exception("Cannot parse: Failed to read file!");

			ContainerINI::SectionMap cont; // init container
			std::string header{}; // init header string

			// lambda that checks a given line for a header def, and sets the header value if it finds one
			const auto getAndSetHeader{ [&header](const std::string& line) -> bool {
				const auto posOpen{ line.find_first_of("[") }, posClose{ line.find_last_of("]") };
				if (str::pos_valid(posOpen) && str::pos_valid(posClose)) {
					header = line.substr(posOpen + 1u, posClose - 1u);
					return true;
				}
				return false;
			} };

			// parse the stringstream
			for (std::string line{}; std::getline(buffer, line, lineDelim); ) { // iterate through file by line
				if (const auto ln{ str::strip_line(line, "#;") }; !ln.empty() && !getAndSetHeader(ln)) {// strip each line, check if empty, and set header if applicable
					if (const auto& [key, val] { _internal::split_and_strip_line(ln, '=') }; !val.empty())// split & strip line by '='
						cont[header].insert_or_assign(key, strip_quotes(val));
				}
			}
			return cont; // return container
		}
	}

	/**
	 * @struct KeyHelper
	 * @brief Represents one key located somewhere in the INI map.
	 *\n	To use similarly to an enum:
	 *\n	struct Setting {
	 *\n
	 *\n		static const Setting FOO, BAR;
	 *\n	};
	 *\n	inline const Setting Setting::FOO{ "my_header", "foo" }, Setting::BAR{ "my_header", "bar" };
	 *\n
	 *\n
	 */
	struct KeyHelper {
		const std::string
			header,	///< @brief The header name of the section that this setting belongs to.
			key;	///< @brief The key name of this setting

		/**
		 * @brief Default Constructor.
		 * @param header_name	- The header name of the section this key belongs to.
		 * @param key_name		- The name of the key associated with this setting.
		 */
		_CONSTEXPR KeyHelper(std::string header_name, std::string key_name) : header{ std::move(header_name) }, key{ std::move(key_name) } {}

		/**
		 * @brief Return this setting as a header_key_pair, this is implicitly called when passed to ContainerINI functions.
		 * @returns std::pair<std::string, std::string>
		 */
		_CONSTEXPR operator std::pair<std::string, std::string>() const { return { header, key }; }
	};

	/**
	 * @struct INI
	 * @brief Simple wrapper/child of ContainerINI that introduces file parsing capability. This is simply a wrapper object for the _internal::parseINI() function that allows multiple files to be parsed & merged into a single configuration structure.
	 */
	struct INI final : ContainerINI {
		/**
		 * @brief Default Constructor.
		 */
		INI() = default;
		/**
		 * @brief Constructor that accepts an INI container with default values.
		 * @param cont	- INI Container.
		 */
		explicit INI(ContainerINI::SectionMap cont) : ContainerINI(std::move(cont)) {}
		/**
		 * @brief Constructor that accepts the name of a file, reads it, and initializes with the parsed values.
		 */
		explicit INI(const std::string& filename) : ContainerINI(std::move(_internal::parseINI(std::move(file::read(filename))))) {}
		/**
		 * @brief Constructor that accepts the contents of a file as a stringstream, and initializes with the parsed values.
		 */
		explicit INI(std::stringstream fileContent) : ContainerINI(std::move(_internal::parseINI(std::move(fileContent)))) {}

		/**
		 * @brief Read the given file, parse it, and merge it into the local setting map.
		 * @param filename				- Target INI config filename.
		 * @param overwrite_existing	- When true, overwrites the value of any settings that already exist.
		 */
		void read(std::string filename, const bool overwrite_existing = true)
		{
			merge_container(std::move(_internal::parseINI(std::move(file::read(std::move(filename))))), overwrite_existing);
		}

		/**
		 * @brief Merge a given INI container into the local settings map.
		 * @param cont	- rvalue reference of an INI container.
		 */
		void merge(ContainerINI::SectionMap&& cont, const bool overwrite_existing = true)
		{
			merge_container(std::move(cont), overwrite_existing);
		}

		/**
		 * @brief Parse, and merge the contents of an input stream into the local map.
		 * @param is	- (implicit) Target Input Stream ref.
		 * @param obj	- (implicit) INI instance ref.
		 * @returns std::istream&
		 */
		friend std::istream& operator>>(std::istream& is, INI& obj)
		{
			static_cast<ContainerINI::SectionMap>(obj._cont).merge(_internal::parseINI(std::move([&is, &obj]() {
				std::stringstream tmp;
				is >> tmp.rdbuf();
				return tmp;
				}())));
			return is;
		}
	};
}
#else
#include <parser/INIParser.hpp>

namespace file::ini {
	/**
	 * @class INI
	 * @brief Extends the (token::parse) INIContainer object with file I/O functionality.
	 */
	struct INI : token::parse::INIContainer {
		/**
		 * @brief Move-Constructor.
		 * @param map	- rvalue reference of a pre-constructed INIContainer::Map.
		 */
		INI(INIContainer::Map&& map) : INIContainer(std::move(map)) {}
		INI(const std::string& filename) : INI(std::move(token::parse::INIParser(filename, file::read(filename)).operator token::parse::INIContainer::Map())) {}
		INI() = default;

		/**
		 * @brief Read a specified file and merge its contents with the local INI map.
		 * @param filename				- Name and path to target INI file.
		 * @param overwrite_existing	- When true, any existing variables will have their values overwritten by the ones in the specified file.
		 */
		void read(const std::string& filename, const bool overwrite_existing = true)
		{
			merge_container(std::move(token::parse::INIParser(filename, file::read(filename))), overwrite_existing);
		}
		template<str::ConvertibleStringT... VT> requires (sizeof...(VT) > 1) void read(const VT&... filenames)
		{
			for (auto& file : var::variadic_accumulate<std::string>(std::string{ filenames }...))
				read(file);
		}
		bool write(const std::string& filename, const bool append = false) const
		{
			return file::write(filename, str::streamify(*this).rdbuf(), append);
		}
	};
}
namespace file { using ini::INI; }
#endif