/**
 * @file INIContainerMonoType.hpp
 * @author radj307
 * @brief Contains the INIContainerMonoType object and its single templated type.
 */
#pragma once
#include <TokenizerINI.hpp>
#include <variant>
#include <concepts>

#define INI_CONT_MONO

namespace token::parse {
#ifndef INI_CONT
	using String = std::string;	///< @brief Represents an ini variable of type TokenType::STRING
	using Float = long double;	///< @brief Represents an ini variable of type TokenType::NUMBER
	using Integer = long long;	///< @brief Represents an ini variable of type TokenType::NUMBER_INT
	using Boolean = bool;		///< @brief Represents an ini variable of type TokenType::BOOLEAN

	/**
	 * @brief Concept that tests whether a given template type is a valid String, Float, Integer, or Boolean.
	 * @tparam T	- Templated input type
	 */
	template<class T> concept ValidValueT = std::same_as<T, String> || std::same_as<T, Float> || std::same_as<T, Integer> || std::same_as<T, Boolean>;

	/**
	 * @enum AlignType
	 * @brief Controls the alignment of = signs in INIContainer's operator<< output.
	 */
	enum class AlignType {
		NONE,	///< @brief Inserts " = " between all keys and values.
		SECTION,///< @brief Determines the longest key name in a section, and inserts "  = " after that length, for all keys in the section.
		ALL,	///< @brief Determines the longest key name in the whole file, and inserts "  = " after that length, for all keys.
	};
#endif

	/**
	 * @brief Convert a T to a std::string
	 * @param var					- Input variable
	 * @param use_quotes_for_string	- When true, variables with String type are returned with enclosing double quotation marks for file/human-readable output.
	 * @returns std::string
	 * @throws bad_variant_access when the given T instance is null/std::monostate.
	 */
	template<ValidValueT T>
	std::string to_string(const T& var, const bool use_quotes_for_string = true)
	{
		if constexpr (std::same_as<T, String>) // String
			return (use_quotes_for_string ? "\"" + var + "\"" : var);
		else if constexpr (std::same_as<T, Boolean>) // Boolean
			return str::bool_to_string(var);
		return std::to_string(var); // other
	}

	/**
	 * @class INIContainer
	 * @brief Storage container for ini variables, stored in unordered_maps that reflect file structure.
	 */
	template<ValidValueT T>
	class INIContainerMonoType {
	public:
		/// @brief Contains the keys and associated values located in a header, where pairs = { key, value }.
		using SectionContent = std::unordered_map<std::string, T>;
		/// @brief Contains all headers and associated SectionContent containers, where pairs = { header, SectionContent }
		using Map = std::unordered_map<std::string, SectionContent>;
		/// @brief Alternative input type for some functions that allows operator-type-deduction, where pair = { key, value }
		using KeyPair = std::pair<std::string, T>;
		/// @brief Alternative input type for some functions that allows operator-type-deduction, where pair = { header, key }
		using HeaderKeyPair = std::pair<std::string, std::string>;

	protected:
		Map _cont; ///< @brief Local container
		AlignType _align_output;

	public:
		/// @brief Default Constructor
		INIContainerMonoType(const AlignType& align_output = AlignType::SECTION) : _align_output{ align_output } {}
		/**
		 * @brief Move-Constructor
		 * @param map	- rvalue reference of a pre-constructed Map instance.
		 */
		INIContainerMonoType(Map&& map, const AlignType& align_output = AlignType::SECTION) : _cont{ std::move(map) }, _align_output{ align_output } {}

		/// @brief Returns local container.
		explicit operator Map() const { return _cont; }

		/// @brief Returns the begin const_iterator for the local container.
		[[nodiscard]] auto begin() const { return _cont.begin(); }
		/// @brief Returns the end const_iterator for the local container.
		[[nodiscard]] auto end() const { return _cont.end(); }
		/// @brief Check if the local container is empty. Checks for keys, not sections, so if headers exist in the file but not keys, this function will still return true.
		[[nodiscard]] auto empty() const { if (_cont.empty()) return true; for (const auto& [header, keys] : _cont) if (!keys.empty()) return false; return true; }

		/// @brief Retrieve the SectionContent of a given header name. @throws out_of_range exception if the header does not exist.
		[[nodiscard]] auto at(const std::string& header) const { return _cont.at(header); }
		/// @brief Retrieve the value of a given key name. @throws out_of_range exception if the header/key doesn't exist.
		[[nodiscard]] auto at(const std::string& header, const std::string& key) const { return _cont.at(header).at(key); }
		/// @brief Retrieve the value of a given key name, but accepts a Header-Key pair as input. @throws out_of_range exception if the header/key doesn't exist.
		[[nodiscard]] auto at(const HeaderKeyPair& hkpr) const { return at(hkpr.first, hkpr.second); }

		/// @brief Equality-comparison operator, checks if the local container matches the container of another INIContainer instance.
		bool operator==(const INIContainerMonoType& o) const { return _cont == o._cont; }
		/// @brief Inverse Equality-comparison operator.
		bool operator!=(auto&& o) const { return !operator==(std::forward<decltype(o)>(o)); }

		/**
		 * @brief Retrieve a given subsection if it exists.
		 * @param header	- Header name of target section.
		 */
		[[nodiscard]] std::optional<SectionContent> operator[](const std::string& header) const
		{
			return check(header) ? get_section(header) : static_cast<std::optional<SectionContent>>(std::nullopt);
		}

		/**
		 * @brief Stream insertion operator. Outputs in valid file format, with quotation-enclosed strings.
		 * @param os	- (implicit) Target output stream.
		 * @param obj	- (implicit) Target INIContainer instance.
		 * @returns std::ostream&
		 */
		friend std::ostream& operator<<(std::ostream& os, const INIContainerMonoType& obj)
		{
			using enum AlignType;
			std::streamsize align_width{ 0ll };
			switch (obj._align_output) {
			case SECTION:
				for (const auto& [header, section] : obj._cont) {
					if (!header.empty())
						os << '[' << header << ']' << '\n';
					const auto align_width{ [&section]() -> std::streamsize {
						std::streamsize longest{ 0ll };
						for (auto& [keyname, _] : section)
							if (const auto sz{ keyname.size() }; sz > longest)
								longest = sz;
						return longest;
					}() };
					for (auto& [key, val] : section)
						os << key << str::VIndent(align_width + 2ll, key.size()) << "= " << to_string(val) << '\n';
					os << '\n';
				}
				break;
			case ALL:
				align_width = [&obj]() -> std::streamsize {
					std::streamsize longest{ 0ll };
					for (auto& [_, section] : obj._cont)
						for (auto& [keyname, __] : section)
							if (const auto sz{ keyname.size() }; sz > longest)
								longest = sz;
					return longest;
				}();
				[[fallthrough]];
			case NONE:
				for (auto& [header, section] : obj._cont) {
					os << '[' << header << ']' << '\n';
					for (auto& [key, value] : section)
						os << key << str::VIndent(align_width + 2ll, key.size()) << "= " << to_string(value, true) << '\n';
					os << '\n';
				}
				break;
			}
			return os;
		}

	#pragma region Functions_Setters
		/// @brief Insert a new Key-Value pair into the map.
		auto insert(const std::string& header, KeyPair kvpr)
		{
			return _cont[header].insert(std::move(kvpr));
		}
		/// @brief Insert a new Value into the map, at the specified key in the specified header.
		auto insert(const std::string& header, std::string key, T value)
		{
			return insert(header, std::move(std::make_pair(std::move(key), std::move(T{ std::move(value) }))));
		}
		/// @brief Insert a new value at the specified Header-Key pair.
		auto insert(const HeaderKeyPair& hkpr, T value)
		{
			return insert(hkpr.first, hkpr.second, std::move(value));
		}
		/// @brief Insert or assign an existing Key-Value pair.
		auto insert_or_assign(const std::string& header, KeyPair kvpr)
		{
			return _cont[header].insert_or_assign(std::move(kvpr.first), std::move(kvpr.second));
		}
		/// @brief Insert or assign an existing value at the specified key in the specified header.
		auto insert_or_assign(const std::string& header, std::string key, T value)
		{
			return _cont[header].insert_or_assign(std::move(key), std::move(T{ std::move(value) }));
		}
		/// @brief Insert or assign an existing value at the specified Header-Key pair.
		auto insert_or_assign(const HeaderKeyPair& hkpr, T value)
		{
			return insert_or_assign(hkpr.first, hkpr.second, std::move(value));
		}

		/// @brief Calls insert_or_assign with the given forwarded parameters.
		auto set(auto&& fst, auto&& snd)
		{
			return insert_or_assign(std::forward<decltype(fst)>(fst), std::forward<decltype(snd)>(snd));
		}
		/// @brief Calls insert_or_assign with the given forwarded parameters.
		auto set(auto&& header, auto&& key, auto&& value)
		{
			return insert_or_assign(std::forward<decltype(header)>(header), std::forward<decltype(key)>(key), std::forward<decltype(value)>(value));
		}

		/**
		 * @brief Move-Merge a map from another location into this ContainerINI instance.
		 * @param other					- Other SectionMap rvalue
		 * @param overwrite_existing	- When true, overwrites existing keys with given map.
		 */
		void merge_container(Map&& o, const bool overwrite_existing = true)
		{
			for (auto& [header, keymap] : o)
				for (auto& kvpr : keymap)
					overwrite_existing ? insert_or_assign(header, std::move(kvpr)) : insert(header, std::move(kvpr));
			return;
		}
	#pragma endregion Functions_Setters
	#pragma region Functions_Check
		/**
		 * @brief Check if a given header exists.
		 * @param header	- The section to check for.
		 * @returns bool
		 */
		[[nodiscard]] bool check_header(const std::string& header) const
		{
			return _cont.contains(header);
		}
		/**
		 * @brief Check if a given header or key in a blank header exists.
		 * @param header_or_key	- The header name, or key name in a blank header, to search for.
		 * @returns bool
		 */
		[[nodiscard]] bool check(const std::string& header_or_key) const
		{
			return _cont.contains(header_or_key) || _cont.contains("") && _cont.at("").contains(header_or_key);
		}
		/**
		 * @brief Check if a given key exists in a given header.
		 * @param header	- The section to search within.
		 * @param key		- The name of the target key.
		 * @returns bool
		 */
		[[nodiscard]] bool check(const std::string& header, const std::string& key) const
		{
			return check_header(header) && _cont.at(header).contains(key);
		}
		/// @brief Extends check function to accept Header-Key pairs.
		[[nodiscard]] bool check(const HeaderKeyPair& hkpr) const
		{
			return check(hkpr.first, hkpr.second);
		}
		/**
		 * @brief Check if any of an arbitrary number of keys exist in a given header.
		 * @tparam ...VT	- Variadic Template (std::string)
		 * @param header	- The header name to search within. If the target keys are not located under a header, use std::nullopt.
		 * @param ...keys	- More than 1 key names to check.
		 * @returns bool
		 */
		template<var::all_same<std::string>... VT>
		[[nodiscard]] bool check_any(const std::optional<std::string>& header, const VT&... keys) const
		{
			return var::variadic_or((check(header.value_or(""), keys))...);
		}
		/**
		 * @brief Check if any of an arbitrary number of keys exist in a given header.
		 * @tparam ...T		- Variadic templated argument, should be of type std::string.
		 * @param header	- The header name to search within. If the target keys are not located under a header, use std::nullopt.
		 * @param ...keys	- More than 1 key names to check.
		 * @returns bool
		 */
		template<var::all_same<std::string>... VT>
		[[nodiscard]] bool check_all(const std::optional<std::string>& header, const VT&... keys) const
		{
			return var::variadic_and((check(header.value_or(""), keys))...);
		}
		/**
		 * @brief Check the value of a key located in a given section.
		 * @param header	- The section name where the target key is located.
		 * @param key		- The name of the key to check.
		 * @param value		- The value to compare against the key value.
		 * @returns bool
		 */
		[[nodiscard]] bool checkv(const std::string& header, const std::string& key, const T& value) const
		{
			return check(header, key) && _cont.at(header).at(key) == value;
		}
		/**
		 * @brief Check the value of a key not located in a header.
		 * @param key	- The name of the key to check.
		 * @param value	- The value to compare against the key value.
		 * @returns bool
		 */
		[[nodiscard]] bool checkv(const std::string& key, const T& value) const
		{
			return check("", key) && _cont.at("").at(key) == value;
		}
		/// @brief Extends checkv function to allow Key-Value pairs as input.
		[[nodiscard]] bool checkv(const std::string& header, const KeyPair& kvpr) const { return checkv(header, kvpr.first, kvpr.second); }
		/// @brief Extends checkv function to allow Key-Value pairs as input.
		[[nodiscard]] bool checkv(const KeyPair& kvpr) const { return checkv(kvpr.first, kvpr.second); }
		/// @brief Extends checkv function to allow Header-Key pairs as input.
		[[nodiscard]] bool checkv(const HeaderKeyPair& hkpr, const T& value) const { return checkv(hkpr.first, hkpr.second, value); }
	#pragma endregion Functions_Check
	#pragma region Functions_Getters
		/**
		 * @brief Get the value of a specified key.
		 * @param header	- The section to search within.
		 * @param key		- The name of the target key.
		 * @returns std::optional<std::string>
		 */
		[[nodiscard]] std::optional<T> getv(const std::string& header, const std::string& key) const
		{
			return check(header, key)
				? _cont.at(header).at(key)
				: static_cast<std::optional<T>>(std::nullopt);
		}
		/**
		 * @brief Get the value of a specified key.
		 * @param header	- The section to search within.
		 * @param key		- The name of the target key.
		 * @returns std::optional<std::string>
		 */
		[[nodiscard]] std::optional<T> getv(const std::string& key) const
		{
			return check("", key)
				? _cont.at("").at(key)
				: static_cast<std::optional<T>>(std::nullopt);
		}

		/**
		 * @brief Get the value of a specified key.
		 * @param header_key_pair	- First element is the target header name, second element is the target key name.
		 * @returns std::optional<std::string>
		 */
		[[nodiscard]] std::optional<T> getv(const std::pair<std::string, std::string>& header_key_pair) const
		{
			return getv(header_key_pair.first, header_key_pair.second);
		}

		/**
		 * @brief Retrieve a copy of a specified section.
		 * @param header	- The section to copy.
		 * @returns unordered_map<string, string>
		 */
		[[nodiscard]] SectionContent get_section(const std::optional<std::string>& header) const
		{
			if (check(header.value_or("")))
				return _cont.at(header.value_or(""));
			return{};
		}

	#pragma endregion Functions_Getters
	};

}