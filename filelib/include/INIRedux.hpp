/**
 * @file	INIRedux.hpp
 * @author	radj307
 * @brief	INI Tokenizer, Parser, and Container/Manipulator.
 */
#pragma once
#include "ini/INI_Tokenizer.hpp"

#include <TokenRedux.hpp>				// For base tokenization framework
#include <TokenReduxDefaultDefs.hpp>	// For tokenizer definitions package
#include <fileio.hpp>					// For file I/O

#include <string>
#include <variant>
#include <concepts>

 /**
  * @namespace	ini
  * @brief		Contains everything used for tokenizing, parsing, storing, and interacting with INI configuration files.
  */
namespace file::ini {
	using String = std::string;	///< @brief Represents an ini variable of type TokenType::STRING
	using Float = long double;	///< @brief Represents an ini variable of type TokenType::NUMBER
	using Integer = long long;	///< @brief Represents an ini variable of type TokenType::NUMBER_INT
	using Boolean = bool;		///< @brief Represents an ini variable of type TokenType::BOOLEAN
	using VariableT = std::variant<std::monostate, String, Float, Integer, Boolean>;

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

	/**
	 * @brief Convert a VariableT to a std::string
	 * @param var					- Input variable
	 * @param use_quotes_for_string	- When true, variables with String type are returned with enclosing double quotation marks for file/human-readable output.
	 * @returns std::string
	 * @throws bad_variant_access when the given VariableT instance is null/std::monostate.
	 */
	std::string to_string(const VariableT& var, const bool& use_quotes_for_string = false)
	{
		if (std::holds_alternative<String>(var)) { // String
			const auto str{ std::get<String>(var) };
			return (use_quotes_for_string ? "\"" + str + "\"" : str);
		}
		else if (std::holds_alternative<Float>(var)) // Float
			return std::to_string(std::get<Float>(var));
		else if (std::holds_alternative<Integer>(var)) // Integer
			return std::to_string(std::get<Integer>(var));
		else if (std::holds_alternative<Boolean>(var)) // Boolean
			return str::bool_to_string(std::get<Boolean>(var));
		throw std::bad_variant_access();
	}

	/**
	 * @class	INIContainer
	 * @brief	Base container object for the INI object. Contains all of the functions from the INI format that are not related to file i/o.
	 */
	class INIContainer {
	public:
		/// @brief Contains the keys and associated values located in a header, where pairs = { key, value }.
		using SectionContent = std::unordered_map<std::string, VariableT>;
		/// @brief Contains all headers and associated SectionContent containers, where pairs = { header, SectionContent }
		using Map = std::unordered_map<std::string, SectionContent>;
		/// @brief Alternative input type for some functions that allows operator-type-deduction, where pair = { key, value }
		using KeyPair = std::pair<std::string, VariableT>;
		/// @brief Alternative input type for some functions that allows operator-type-deduction, where pair = { header, key }
		using HeaderKeyPair = std::pair<std::string, std::string>;

	protected:
		Map _cont; ///< @brief Local container
		AlignType _align_output;

	public:
		/// @brief	Default Constructor
		INIContainer(const AlignType& align_output = AlignType::SECTION) : _align_output{ align_output } {}
		/**
		 * @brief		Move-Constructor
		 * @param map	rvalue reference of a pre-constructed Map instance.
		 */
		INIContainer(Map&& map, const AlignType& align_output = AlignType::SECTION) : _cont{ std::move(map) }, _align_output{ align_output } {}

		/// @brief	Returns local container.
		explicit operator Map() const { return _cont; }

		/// @brief	Returns the begin const_iterator for the local container.
		[[nodiscard]] auto begin() const { return _cont.begin(); }
		/// @brief	Returns the end const_iterator for the local container.
		[[nodiscard]] auto end() const { return _cont.end(); }
		/// @brief	Check if the local container is empty. Checks for keys, not sections, so if headers exist in the file but not keys, this function will still return true.
		[[nodiscard]] auto empty() const { if (_cont.empty()) return true; for (const auto& [header, keys] : _cont) if (!keys.empty()) return false; return true; }
		/// @brief	Returns the number of section in the container.
		[[nodiscard]] auto size() const { return _cont.size(); }

		/// @brief	Retrieve the SectionContent of a given header name. @throws out_of_range exception if the header does not exist.
		[[nodiscard]] auto at(const std::string& header) const { return _cont.at(header); }
		/// @brief	Retrieve the value of a given key name. @throws out_of_range exception if the header/key doesn't exist.
		[[nodiscard]] auto at(const std::string& header, const std::string& key) const { return _cont.at(header).at(key); }
		/// @brief	Retrieve the value of a given key name, but accepts a Header-Key pair as input. @throws out_of_range exception if the header/key doesn't exist.
		[[nodiscard]] auto at(const HeaderKeyPair& hkpr) const { return at(hkpr.first, hkpr.second); }
		/// @brief	Find an element with its header name.
		[[nodiscard]] auto find(auto&& header) const { return _cont.find(std::forward<decltype(header)>(header)); }
		/// @brief	Erase a section.
		bool erase(auto&& header) noexcept
		{
			return static_cast<bool>(_cont.erase(std::forward<decltype(header)>(header)));
		}
		/// @brief	Erase a key from a section
		bool erase(auto&& header, auto&& key) noexcept
		{
			if (auto& section{ _cont.find(std::forward<decltype(header)>(header)) }; section != _cont.end())
				return static_cast<bool>(section.erase(std::forward<decltype(key)>(key)));
			return false;
		}


		/**
		 * @brief	Retrieve the number of keys in the INI.
		 * @returns	size_t
		 */
		[[nodiscard]] size_t countKeys() const
		{
			size_t count{ 0ull };
			for (const auto& [_, section] : _cont)
				count += section.size();
			return count;
		}
		/**
		 * @brief	Retrieve the number of headers in the INI.
		 * @returns size_t
		 */
		[[nodiscard]] size_t countHeaders() const
		{
			return _cont.size();
		}

		/// @brief Equality-comparison operator, checks if the local container matches the container of another INIContainer instance.
		bool operator==(const INIContainer& o) const { return _cont == o._cont; }
		/// @brief Inverse Equality-comparison operator.
		bool operator!=(auto&& o) const { return !operator==(std::forward<decltype(o)>(o)); }

		/**
		 * @brief			Retrieve a given subsection if it exists.
		 * @param header	Header name of target section.
		 */
		[[nodiscard]] std::optional<SectionContent> operator[](const std::string& header) const
		{
			return check(header) ? get_section(header) : static_cast<std::optional<SectionContent>>(std::nullopt);
		}

		/**
		 * @brief		Stream insertion operator. Outputs in valid file format, with quotation-enclosed strings.
		 * @param os	(implicit) Target output stream.
		 * @param obj	(implicit) Target INIContainer instance.
		 * @returns		std::ostream&
		 */
		friend std::ostream& operator<<(std::ostream& os, const INIContainer& obj)
		{
			std::streamsize align_width{ 0ll };
			switch (obj._align_output) {
			case AlignType::SECTION:
				for (const auto& [header, section] : obj._cont) {
					if (!header.empty())
						os << '[' << header << ']' << '\n';
					const auto align_width{ [&section]() -> std::streamsize {
						size_t longest{ 0ll };
						for (auto& [keyname, _] : section)
							if (const auto sz{ keyname.size() }; sz > longest)
								longest = sz;
						return static_cast<std::streamsize>(longest);
					}() };
					for (auto& [key, val] : section)
						os << key << format::indent(align_width + 2ll, key.size()) << "= " << to_string(val) << '\n';
					os << '\n';
				}
				break;
			case AlignType::ALL:
				align_width = [&obj]() -> std::streamsize {
					size_t longest{ 0ll };
					for (auto& [_, section] : obj._cont)
						for (auto& [keyname, __] : section)
							if (const auto sz{ keyname.size() }; sz > longest)
								longest = sz;
					return static_cast<std::streamsize>(longest);
				}();
				[[fallthrough]];
			case AlignType::NONE:
				for (auto& [header, section] : obj._cont) {
					os << '[' << header << ']' << '\n';
					for (auto& [key, value] : section)
						os << key << format::indent(align_width + 2ll, key.size()) << "= " << to_string(value, true) << '\n';
					os << '\n';
				}
				break;
			}
			return os;
		}

#		pragma region Functions_Setters
		/// @brief	Insert a whole section into the INI container.
		auto insert(std::pair<std::string, SectionContent>&& pr)
		{
			return _cont.insert(std::forward<std::pair<std::string, SectionContent>>(pr));
		}
		/// @brief	Insert or assign a whole section of the INI.
		auto insert_or_assign(std::string&& header, SectionContent&& section)
		{
			return _cont.insert_or_assign(std::forward<std::string>(header), std::forward<SectionContent>(section));
		}
		/// @brief	Insert a whole section into the INI container.
		auto insert(const std::pair<std::string, SectionContent>& pr)
		{
			return _cont.insert(pr);
		}
		/// @brief	Insert or assign a whole section of the INI.
		auto insert_or_assign(const std::string& header, const SectionContent& section)
		{
			return _cont.insert_or_assign(header, section);
		}

		/// @brief	Insert a new Key-Value pair into the map.
		auto insert(const std::string& header, KeyPair kvpr)
		{
			return _cont[header].insert(std::move(kvpr));
		}
		/// @brief	Insert a new Value into the map, at the specified key in the specified header.
		template<ValidValueT T>
		auto insert(const std::string& header, std::string key, T value)
		{
			return insert(header, std::move(std::make_pair(std::move(key), std::move(VariableT{ std::move(value) }))));
		}
		/// @brief	Insert a new value at the specified Header-Key pair.
		template<ValidValueT T>
		auto insert(const HeaderKeyPair& hkpr, T value)
		{
			return insert(hkpr.first, hkpr.second, std::move(value));
		}
		/// @brief	Insert or assign an existing Key-Value pair.
		auto insert_or_assign(const std::string& header, KeyPair kvpr)
		{
			return _cont[header].insert_or_assign(std::move(kvpr.first), std::move(kvpr.second));
		}
		/// @brief	Insert or assign an existing value at the specified key in the specified header.
		template<ValidValueT T>
		auto insert_or_assign(const std::string& header, std::string key, T value)
		{
			return _cont[header].insert_or_assign(std::move(key), std::move(VariableT{ std::move(value) }));
		}
		/// @brief	Insert or assign an existing value at the specified Header-Key pair.
		template<ValidValueT T>
		auto insert_or_assign(const HeaderKeyPair& hkpr, T value)
		{
			return insert_or_assign(hkpr.first, hkpr.second, std::move(value));
		}

		/// @brief	Calls insert_or_assign with the given forwarded parameters.
		auto set(auto&& fst, auto&& snd)
		{
			return insert_or_assign(std::forward<decltype(fst)>(fst), std::forward<decltype(snd)>(snd));
		}
		/// @brief	Calls insert_or_assign with the given forwarded parameters.
		auto set(auto&& header, auto&& key, auto&& value)
		{
			return insert_or_assign(std::forward<decltype(header)>(header), std::forward<decltype(key)>(key), std::forward<decltype(value)>(value));
		}

		/**
		 * @brief						Move-Merge a map from another location into this ContainerINI instance.
		 * @param other					Other SectionMap rvalue
		 * @param overwrite_existing	When true, overwrites existing keys with given map.
		 */
		void merge_container(Map&& o, const bool overwrite_existing = true)
		{
			for (auto& [header, keymap] : o)
				for (auto& kvpr : keymap)
					overwrite_existing ? insert_or_assign(header, std::move(kvpr)) : insert(header, std::move(kvpr));
			return;
		}
#		pragma endregion Functions_Setters
#		pragma region Functions_Check
		/**
		 * @brief			Check if a given header exists.
		 * @param header	The section to check for.
		 * @returns bool
		 */
		[[nodiscard]] bool check_header(const std::string& header) const
		{
			return _cont.contains(header);
		}
		/**
		 * @brief					Check if a given header or key in a blank header exists.
		 * @param header_or_key		The header name, or key name in a blank header, to search for.
		 * @returns					bool
		 */
		[[nodiscard]] bool check(const std::string& header_or_key) const
		{
			return _cont.contains(header_or_key) || _cont.contains("") && _cont.at("").contains(header_or_key);
		}
		/**
		 * @brief			Check if a given key exists in a given header.
		 * @param header	The section to search within.
		 * @param key		The name of the target key.
		 * @returns			bool
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
		 * @brief			Check if any of an arbitrary number of keys exist in a given header.
		 * @tparam ...VT	Variadic Template (std::string)
		 * @param header	The header name to search within. If the target keys are not located under a header, use std::nullopt.
		 * @param ...keys	More than 1 key names to check.
		 * @returns			bool
		 */
		template<var::same_or_convertible<std::string>... Ts> requires var::at_least_one<Ts...>
		[[nodiscard]] bool check_any(const std::optional<std::string>& header, const Ts&... keys) const
		{
			return var::variadic_or((check(header.value_or(""), keys))...);
		}
		/**
		 * @brief			Check if any of an arbitrary number of keys exist in a given header.
		 * @tparam ...T		Variadic templated argument, should be of type std::string.
		 * @param header	The header name to search within. If the target keys are not located under a header, use std::nullopt.
		 * @param ...keys	More than 1 key names to check.
		 * @returns			bool
		 */
		template<class... VT> requires std::conjunction_v<std::is_same<VT, std::string>...> && (sizeof...(VT) > 1)
			[[nodiscard]] bool check_all(const std::optional<std::string>& header, const VT&... keys) const
		{
			return var::variadic_and((check(header.value_or(""), keys))...);
		}
		/**
		 * @brief			Check the value of a key located in a given section.
		 * @param header	The section name where the target key is located.
		 * @param key		The name of the key to check.
		 * @param value		The value to compare against the key value.
		 * @returns			bool
		 */
		[[nodiscard]] bool checkv(const std::string& header, const std::string& key, const std::string& value, const bool& case_sensitive = true) const
		{
			const auto comp{ [&case_sensitive](const auto& l, const auto& r) {return l == r || !case_sensitive && str::tolower(l) == str::tolower(r); } };
			return check(header, key) && comp(to_string(_cont.at(header).at(key)), value);
		}

		[[nodiscard]] bool checkv(const std::string& header, const std::string& key, const VariableT& value) const
		{
			return check(header, key) && _cont.at(header).at(key) == value;
		}
		/**
		 * @brief		Check the value of a key not located in a header.
		 * @param key	The name of the key to check.
		 * @param value	The value to compare against the key value.
		 * @returns		bool
		 */
		[[nodiscard]] bool checkv(const std::string& key, const VariableT& value) const
		{
			return check("", key) && _cont.at("").at(key) == value;
		}
		/// @brief Extends checkv function to allow Key-Value pairs as input.
		[[nodiscard]] bool checkv(const std::string& header, const KeyPair& kvpr) const { return checkv(header, kvpr.first, kvpr.second); }
		/// @brief Extends checkv function to allow Key-Value pairs as input.
		[[nodiscard]] bool checkv(const KeyPair& kvpr) const { return checkv(kvpr.first, kvpr.second); }
		/// @brief Extends checkv function to allow Header-Key pairs as input.
		[[nodiscard]] bool checkv(const HeaderKeyPair& hkpr, const VariableT& value) const { return checkv(hkpr.first, hkpr.second, value); }
#		pragma endregion Functions_Check

		/**
		 * @brief				Get the value of a key as a string or its type, cast it using a given function, and return it.
		 *\n					Returns std::nullopt if it wasn't found
		 * @tparam CastType:	The return type of the converter function.
		 * @tparam GetType:		This is the input type for the converter function. It may be a VariableT type or a std::string (default).
		 * @param header:		The name of the header where the target key is located. If the key isn't in a header, pass a blank string.
		 * @param key:			The name of the target key under the given header.
		 * @param converter:	Conversion function that accepts GetType and returns CastType.
		 * @returns				std::optional<CastType>
		 */
		template<typename CastType, var::any_same<VariableT, std::string> GetType = std::string>
		[[nodiscard]] std::optional<CastType> get(const std::string& header, const std::string& key, const std::function<CastType(GetType)>& converter) const
		{
			if (const auto& v{ getv(header, key) }; v.has_value()) {
				if constexpr (std::same_as<GetType, std::string>)
					return converter(to_string(v.value()));
				return converter(v.value());
			}
			return std::nullopt;
		}

#		pragma region Functions_Getters
		/**
		 * @brief			Get the value of a specified key.
		 * @param header	The section to search within.
		 * @param key		The name of the target key.
		 * @returns			std::optional<std::string>
		 */
		[[nodiscard]] std::optional<VariableT> getv(const std::string& header, const std::string& key) const
		{
			return check(header, key)
				? _cont.at(header).at(key)
				: static_cast<std::optional<VariableT>>(std::nullopt);
		}
		/**
		 * @brief Get the value of a specified key.
		 * @param header	- The section to search within.
		 * @param key		- The name of the target key.
		 * @returns std::optional<std::string>
		 */
		[[nodiscard]] std::optional<VariableT> getv(const std::string& key) const
		{
			return check("", key)
				? _cont.at("").at(key)
				: static_cast<std::optional<VariableT>>(std::nullopt);
		}

		/**
		 * @brief Get the value of a specified key.
		 * @param header_key_pair	- First element is the target header name, second element is the target key name.
		 * @returns std::optional<std::string>
		 */
		[[nodiscard]] std::optional<VariableT> getv(const std::pair<std::string, std::string>& header_key_pair) const
		{
			return getv(header_key_pair.first, header_key_pair.second);
		}
		/**
		 * @brief Retrieve & cast a variable to the specified type. If the variable is not the specified type, std::nullopt is returned instead.
		 * @tparam RT		- Return Type, must be compliant with ValidValueT test.
		 * @param header	- The name of the header where the target key is located.
		 * @param key		- The name of the key to retrieve the value of.
		 * @returns std::optional<RT>
		 */
		template<ValidValueT RT>
		[[nodiscard]] std::optional<RT> getv(const std::string& header, const std::string& key) const
		{
			if (const auto target{ getv(header, key) }; target.has_value())
				if (auto v = std::get_if<RT>(&target.value()))
					return *v;
			return std::nullopt;
		}
		/**
		 * @brief Retrieve & cast a variable to the specified type. If the variable is not the specified type, std::nullopt is returned instead.
		 * @tparam RT		- Return Type, must be compliant with ValidValueT test.
		 * @param header	- The name of the header where the target key is located.
		 * @param key		- The name of the key to retrieve the value of.
		 * @returns std::optional<RT>
		 */
		template<ValidValueT RT>
		[[nodiscard]] std::optional<RT> getv(const std::string& key) const
		{
			return getv<RT>("", key);
		}
		/**
		 * @brief Retrieve & cast a variable to the specified type. If the variable is not the specified type, std::nullopt is returned instead.
		 * @tparam RT	- Return Type, must be compliant with ValidValueT test.
		 * @param hkpr	- Header-Key pair specifying the target value.
		 * @returns std::optional<RT>
		 */
		template<ValidValueT RT>
		[[nodiscard]] std::optional<RT> getv(const HeaderKeyPair& hkpr) const
		{
			return getv<RT>(hkpr.first, hkpr.second);
		}

		/**
		 * @brief				Retrieve a specified key's value, casted to a given type using a given function.
		 * @tparam CastTo:		A type to cast the value to using the given function, if the key exists.
		 * @param header:		The header to search within.
		 * @param key:			The name of the target key.
		 * @param castFunction:	A function or lambda capable of converting from (VariableT) => (CastTo)
		 * @returns				std::optional<CastTo>
		 */
		template<typename CastTo>
		[[nodiscard]] std::optional<CastTo> getv_cast(const std::string& header, const std::string& key, const std::function<CastTo(VariableT)>& castFunction) const
		{
			if (const auto& v{ getv(header, key) }; v.has_value())
				return castFunction(v.value());
			return std::nullopt;
		}
		/**
		 * @brief				Retrieve a specified key's value, casted to a given type using a given function.
		 * @tparam CastTo:		A type to cast the value to using the given function, if the key exists.
		 * @param hkpr:			A header-key pair.
		 * @param castFunction:	A function or lambda capable of converting from (VariableT) => (CastTo)
		 * @returns				std::optional<CastTo>
		 */
		template<typename CastTo>
		[[nodiscard]] std::optional<CastTo> getv_cast(const HeaderKeyPair& hkpr, const std::function<CastTo(VariableT)>& castFunction) const
		{
			return getv_cast<CastTo>(hkpr.first, hkpr.second, castFunction);
		}
		/**
		 * @brief				Retrieve a specified key's value as a string, then cast it to a given type using a function.
		 * @tparam CastTo:		A type to cast the value to using the given function, if the key exists.
		 * @param header:		The header to search within.
		 * @param key:			The name of the target key.
		 * @param castFunction:	A function or lambda capable of converting from (std::string) => (CastTo)
		 * @returns				std::optional<CastTo>
		 */
		template<typename CastTo>
		[[nodiscard]] std::optional<CastTo> getvs_cast(const std::string& header, const std::string& key, const std::function<CastTo(std::string)>& castFunction) const
		{
			if (const auto& v{ getvs(header, key) }; v.has_value())
				return castFunction(v.value());
			return std::nullopt;
		}
		/**
		 * @brief				Retrieve a specified key's value as a string, then cast it to a given type using a function.
		 * @tparam CastTo:		A type to cast the value to using the given function, if the key exists.
		 * @param hkpr:			A header-key pair.
		 * @param castFunction:	A function or lambda capable of converting from (std::string) => (CastTo)
		 * @returns				std::optional<CastTo>
		 */
		template<typename CastTo>
		[[nodiscard]] std::optional<CastTo> getvs_cast(const HeaderKeyPair& hkpr, const std::function<CastTo(std::string)>& castFunction) const
		{
			return getvs_cast<CastTo>(hkpr.first, hkpr.second, castFunction);
		}


		/**
		 * @brief Retrieve a copy of a specified section.
		 * @param header	- The section to copy.
		 * @returns unordered_map<string, string>
		 */
		[[nodiscard]] SectionContent get_section(const std::optional<std::string>& header) const&
		{
			if (check(header.value_or("")))
				return _cont.at(header.value_or(""));
			return{};
		}
#		pragma endregion Functions_Getters

#		pragma region Functions_StringVariants
		/**
		 * @brief		Retrieve a value from the map, and convert it to std::string. Uses perfect-forwarding.
		 * @param p1	First argument (Header)
		 * @param p2	Second argument (key)
		 * @returns		std::optional<std::string>
		 */
		[[nodiscard]] std::optional<std::string> getvs(const std::string& p1, const std::string& p2) const
		{
			const auto v{ getv(p1, p2) };

			if (v.has_value()) {
				return to_string(v.value());
			}
			else {
				return std::nullopt;
			}
		}
		/**
		 * @brief		Retrieve a value from the map, and convert it to std::string. Uses perfect-forwarding.
		 * @param p1	First argument (Header)
		 * @returns		std::optional<std::string>
		 */
		[[nodiscard]] std::optional<std::string> getvs(const std::string& p1) const
		{
			if (const auto v{ getv(p1) })
				return to_string(v.value());
			return std::nullopt;
		}
#		pragma endregion Functions_StringVariants
	};

	namespace tokenizer {
		/**
		 * @struct	INIParser
		 * @brief	Token parser for the INI file format.
		 */
		struct INIParser : token::base::TokenParserBase<INIContainer::Map, Token> {
			bool allowBlankValue{ true };
			const std::string filename{};

#			if LANG_CPP >= 17
			INIParser(std::vector<Token>&& tokens, const std::filesystem::path& filename = {}) : TokenParserBase(std::move(tokens)), filename{ filename.generic_string() } {}
#			else
			INIParser(std::vector<Token>&& tokens, const std::string_view& filename = {}) : TokenParserBase(std::move(tokens)), filename{ filename } {}
#			endif

		private:
			void throwEx(const size_t& line, const std::string& msg) const
			{
				throw make_exception("Syntax Error: ", msg, " at line ", line, " in file: \"", filename, '\"');
			}
		public:

			OutputT parse() const
			{
				// Init temporary variables:
				std::string header{ "" }; // ini standard allows empty headers
				std::optional<std::string> key;
				std::optional<VariableT> value;
				bool setter{ false };
				size_t ln{ 1ull };
				// Init map:
				OutputT map{};

				strip_types(TokenType::COMMENT); // remove all comments

				/// @brief Lambda for throwing exceptions with consistent naming conventions.
				/// @brief Lambda that handles variable verification & insertion into the map.
				const auto& insert{ [&]() {
					if (!key.has_value())
						throwEx(ln, "Missing Key Declaration");
					if (!value.has_value()) {
						if (allowBlankValue)
							value = std::string{};
						else
							throwEx(ln, "Missing Value Declaration");
					}
					map[header].insert_or_assign(key.value(), value.value());
				} };
				/// @brief Lambda that returns true if any temp variable is defined. This is used to detect whether a NEWLINE occurrance is valid or not
				const auto& any_defined{ [&key, &value, &setter]() { return key.has_value() || value.has_value() || setter; } };

				size_t i{ 0ull };
				for (const auto& [type, str] : tokens) {
					++i; // increment index by one (used to add _EOL)
					switch (type) {
					case TokenType::HEADER: // set the header
						header = str;
						break;
					case TokenType::SETTER: // set the temp setter variable to true if it isn't already
						if (!setter)
							setter = true;
						else throwEx(ln, "Duplicate Setters"); // throw duplicate exception if setter was already true
						break;
					case TokenType::KEY: // set the temp key
						if (!key.has_value()) {
							key = str;
							break;
						}
						else [[fallthrough]]; // if the key is already set, parse as a string value
					case TokenType::STRING: // set the temp value to a string
						if (!setter)
							throwEx(ln, "Missing Setter");
						if (value.has_value()) {
							if (std::holds_alternative<String>(value.value()))
								value = (std::get<String>(value.value()) + str); // allow fallthrough "keys" (additional values that were cut off by the tokenizer -- shouldn't happen but just in case) to be appended
							else throwEx(ln, "Duplicate Value");
						}
						else
							value = str;
						break;
					case TokenType::NUMBER: // set the temp value to a long double
						if (!setter)
							throwEx(ln, "Missing Setter");
						if (size_t decimal_count{ 0ull }; std::all_of(str.begin(), str.end(), [&decimal_count](auto&& ch) { if (ch == '.') ++decimal_count; return isdigit(ch) && decimal_count == 1; }))
							value = str::stold(str);
						else value = str;
						break;
					case TokenType::NUMBER_INT: // set the temp value to an integer
						if (!setter)
							throwEx(ln, "Missing Setter");
						if (std::all_of(str.begin(), str.end(), isdigit))
							value = str::stoll(str);
						else value = str;
						break;
					case TokenType::BOOLEAN:
					{ // set the temp value to a boolean
						if (!setter)
							throwEx(ln, "Missing Setter");
						auto b{ str::string_to_bool(str) };
						if (!b.has_value())
							value = str;
						else
							value = std::move(b);
						break;
					}
					case TokenType::NEWLINE: // insert & reset temp variables
						if (any_defined())
							insert();
						key = std::nullopt;
						value = std::nullopt;
						setter = false;
						++ln;
						break;
					case TokenType::END: // insert temp variables and return successfully.
						if (any_defined())
							insert();
						if (i == tokens.size()) // if this EOF is the last element in the tokens vec.
							return map;
						break;
					case TokenType::COMMENT:break;
					default: // throw unexpected token exception
						throwEx(ln, "Unexpected token type");
					}
				}
				// throw missing EOF exception
				throwEx(ln, "Missing Token: END / EOF");
				return{}; // this will never be reached, but it prevents compiler warning
			}
		};
	}

	/**
	 * @struct	INI
	 * @brief	Contains a map of the headers, keys, and values of an INI file, or multiple INI files.
	 *\n		Uses variadic templates
	*/
	struct INI : public INIContainer {
		INI() = default;
		INI(INIContainer::Map&& map) : INIContainer(std::move(map)) {}
#		if LANG_CPP >= 17
		INI(const std::filesystem::path& filepath) : INI(tokenizer::INIParser(std::move(tokenizer::INITokenizer(std::move(file::read(filepath))).tokenize(token::DefaultDefs::TokenType::END)), filepath).operator INIContainer::Map()) {}
#		else
		INI(const std::string_view& filepath) : INI(tokenizer::INIParser(std::move(tokenizer::INITokenizer(std::move(file::read(filepath))).tokenize(token::DefaultDefs::TokenType::END)), filepath).operator INIContainer::Map()) {}
#		endif

		/**
		 * @brief Read a specified file and merge its contents with the local INI map.
		 * @param filename				- Name and path to target INI file.
		 * @param overwrite_existing	- When true, any existing variables will have their values overwritten by the ones in the specified file.
		 */
		void read(const std::filesystem::path& filename, const bool overwrite_existing = true)
		{
			merge_container(tokenizer::INIParser(std::move(tokenizer::INITokenizer(std::move(file::read(filename))).tokenize()), filename), overwrite_existing);
		}
		template<str::ConvertibleStringT... VT> requires (sizeof...(VT) > 1) void read(const VT&... filenames)
		{
			for (auto& file : var::variadic_accumulate<std::string>(std::string{ filenames }...))
				read(file);
		}
		bool write(const std::filesystem::path& filename) const
		{
			return file::write(filename, *this);
		}
	};
}

namespace file {
	/// @brief	An INI file object that can read, write, and interact with configuration values. Uses variadic templates heavily.
	using INI = ini::INI;
}