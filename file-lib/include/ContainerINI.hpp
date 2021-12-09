/**
 * @file ContainerINI.hpp
 * @author radj307
 * @brief Contains the ContainerINI struct, an INI Data Storage Container used to interact with previously parsed data.
 *\n TERMINOLOGY
 *\n	Header		- Refers to the name given to a section using the syntax: [header-name]
 *\n	Section		- Refers to all of the keys found under a header tag definition, using the syntax: keyname = value
 *\n	Key			- Refers to the name of a setting, located within a section.
 *\n	Value		- Refers to the value of a key. All values are stored in type std::string, any conversions must be done externally.
 */
#pragma once
#ifndef INI_USE_EXPERIMENTAL
#include <unordered_map>
#include <vector>
#include <var.hpp>

namespace file::ini {
	/**
	 * @struct ContainerINI
	 * @brief Data storage used for INI files. It cannot parse files itself, and does not have any knowledge of the INI parser found in INI.hpp
	 */
	struct ContainerINI {
		using SectionKeyMap = std::unordered_map<std::string, std::string>; ///< @brief Represents the key-value pairs in a section.
		using SectionMap = std::unordered_map<std::string, SectionKeyMap>; ///< @brief Represents all sections (denoted by the header name) in the file, and the contained key-value pairs.
		SectionMap _cont; ///< @brief Contains the INI data.

		using StringPair = std::pair<std::string, std::string>;

		auto begin() const { return _cont.begin(); }
		auto end() const { return _cont.end(); }
		auto empty() const { return _cont.empty(); }
		auto at(const std::string& header) const { return _cont.at(header); }
		auto at(const std::string& header, const std::string& key) const { return _cont.at(header).at(key); }
		auto at(const std::pair<std::string, std::string>& header_key_pair) const { return _cont.at(header_key_pair.first).at(header_key_pair.second); }

		/**
		 * @brief Null Constructor
		 */
		ContainerINI() = default;
		/**
		 * @brief Map Constructor
		 * @param map	- rvalue reference of a SectionMap instance to initialize with.
		 */
		ContainerINI(SectionMap&& map) : _cont{ std::move(map) } {}

		/**
		 * @brief Insert a new key-value pair into a specified section.
		 * @param header		 - The section to insert the given key-value pair into.
		 * @param key_value_pair - The key-value pair to insert.
		 * @returns pair<iterator, bool>
		 *\n		iterator	 - This points to the pre-existing key, or the key that was just inserted.
		 *\n		bool		 - This is true if the key-value pair was inserted successfully, or false if the key already existed.
		 */
		auto insert(const std::string& header, StringPair key_value_pair)
		{
			return _cont[header].insert(std::move(key_value_pair));
		}
		auto insert(const StringPair& header_key_pair, std::string value)
		{
			return insert(header_key_pair.first, { header_key_pair.second, std::move(value) });
		}

		/**
		 * @brief Set the value of a specified key to a new value, or insert the specified key if it didn't exist.
		 * @param header	- The section containing the target key. If it doesn't exist, it will be created.
		 * @param key		- The name of the target key. If it doesn't exist, it will be created.
		 * @param value		- The value to set the target key to.
		 */
		void set(const std::string& header, const std::string& key, std::string value)
		{
			_cont[header][key] = value;
		}
		void set(const StringPair& header_key_pair, std::string value)
		{
			set(header_key_pair.first, header_key_pair.second, std::move(value));
		}

		/**
		 * @brief Move-Merge a map from another location into this ContainerINI instance.
		 * @param other					- Other SectionMap rvalue
		 * @param overwrite_existing	- When true, overwrites existing keys with given map.
		 */
		void merge_container(SectionMap&& other, const bool overwrite_existing = true)
		{
			for (auto& it : other) // iterate through other container
				if (const auto [pos, success] { _cont.insert(it) }; !success) // attempt to insert each section as a whole
					for (auto& [key, val] : it.second) // if the section already exists, iterate through each key-value pair in the incoming section
						if (const auto [ipos, isuccess] { it.second.insert({ key, val }) }; !isuccess && overwrite_existing) // attempt to insert each key-value pair
							ipos->second = std::move(val); // if the key already exists, replace its value with the incoming one.
		}

		/**
		 * @brief Get the value of a specified key.
		 * @param header	- The section to search within.
		 * @param key		- The name of the target key.
		 * @returns std::optional<std::string>
		 */
		std::optional<std::string> getv(const std::string& header, const std::string& key) const
		{
			if (check(header, key))
				if (const auto val{ _cont.at(header).at(key) }; !val.empty())
					return val;
			return static_cast<std::optional<std::string>>(std::nullopt);
		}

		/**
		 * @brief Get the value of a specified key.
		 * @param header_key_pair	- First element is the target header name, second element is the target key name.
		 * @returns std::optional<std::string>
		 */
		std::optional<std::string> getv(const std::pair<std::string, std::string>& header_key_pair) const
		{
			return getv(header_key_pair.first, header_key_pair.second);
		}

		/**
		 * @brief Retrieve a copy of a specified section.
		 * @param header	- The section to copy.
		 * @returns unordered_map<string, string>
		 */
		std::unordered_map<std::string, std::string> get_section(const std::string& header) const
		{
			if (check(header))
				return _cont.at(header);
			return{};
		}

		/**
		 * @brief Check if a given header exists.
		 * @param header	- The section to check for.
		 * @returns bool
		 */
		bool check(const std::string& header) const
		{
			return _cont.contains(header);
		}
		/**
		 * @brief Check if a given key exists in a given header.
		 * @param header	- The section to search within.
		 * @param key		- The name of the target key.
		 * @returns bool
		 */
		bool check(const std::string& header, const std::string& key) const
		{
			return check(header) && _cont.at(header).contains(key);
		}
		bool check(const StringPair& header_key_pair) const
		{
			return check(header_key_pair.first, header_key_pair.second);
		}
		/**
		 * @brief Check if all of an arbitrary number of keys exist in a given header.
		 * @tparam ...T		- Variadic templated argument of type std::string.
		 * @param header	- The header name to search within.
		 * @param ...keys	- More than 1 key names to check.
		 * @returns bool
		 */
		template<class... T> std::enable_if_t<((sizeof...(T) > 1) && var::variadic_and(std::is_same_v<std::string, T>...)), bool> check_any(const std::string& header, const T&... keys) const
		{
			return var::variadic_or((check(header, keys))...);
		}
		/**
		 * @brief Check if any of an arbitrary number of keys exist in a given header.
		 * @tparam ...T		- Variadic templated argument, should be of type std::string.
		 * @param header	- The header name to search within.
		 * @param ...keys	- More than 1 key names to check.
		 * @returns bool
		 */
		template<class... T> std::enable_if_t<((sizeof...(T) > 1) && var::variadic_and(std::is_same_v<std::string, T>...)), bool> check_all(const std::string& header, const T&... keys) const
		{
			return var::variadic_and((check(header, keys))...);
		}
		/**
		 * @brief					Check the value of a key located in a given section.
		 * @param header			The section name where the target key is located.
		 * @param key				The name of the key to check.
		 * @param value				The value to compare against the key value.
		 * @param case_sensitive	When false, string comparison is case insensitive.
		 * @returns					bool
		 */
		bool checkv(const std::string& header, const std::string& key, const std::string& value, const bool& case_sensitive = true) const
		{
			return check(header, key) && (case_sensitive ? _cont.at(header).at(key) == value : str::tolower(_cont.at(header).at(key)) == str::tolower(value));
		}
		bool checkv(const StringPair& header_key_pair, const std::string& value, const bool& case_sensitive = true) const
		{
			return checkv(header_key_pair.first, header_key_pair.second, value, case_sensitive);
		}

		/**
		 * @brief Retrieve a given subsection if it exists.
		 * @param header	- Header name of target section.
		 */
		std::optional<SectionKeyMap> operator[](const std::string& header) const
		{
			return check(header) ? get_section(header) : static_cast<std::optional<SectionKeyMap>>(std::nullopt);
		}


		friend std::ostream& operator<<(std::ostream& os, const ContainerINI& obj)
		{
			for (auto& [header, section] : static_cast<ContainerINI::SectionMap>(obj._cont)) {
				os << '[' << header << ']' << '\n';
				for (auto& [key, value] : section)
					os << key << " = " << value << '\n';
				os << '\n';
			}
			return os;
		}

		explicit operator SectionMap()
		{
			return _cont;
		}
		explicit operator SectionMap() const
		{
			return _cont;
		}
	};
}
#endif