#pragma once
#include "INI_Tokenizer.hpp"

#include <fileio.hpp>
#include <var.hpp>
#include <map>

namespace file::ini {
	/**
	 * @brief			Basic inner container for key-value pairs.
	 * @tparam Key		Key type.
	 * @tparam Value	Value type.
	 */
	template<typename Key, typename Value>
	struct basic_container : public std::map<Key, Value> {
		using base = std::map<Key, Value>;
		using base::base;

		basic_container() = default;

		/**
		 * @brief		Get the value of the specified key.
		 * @param key	The key to retrieve.
		 * @returns		std::optional<Value>
		 */
		[[nodiscard]] std::optional<Value> get(const Key& key) const
		{
			if (const auto& it{ this->find(key) }; it != this->end())
				return it->second;
			return std::nullopt;
		}
		/**
		 * @brief		Get the value of the specified key, or another value if that wasn't found.
		 * @param key	The key to retrieve.
		 * @param def	The default value to return if the key wasn't found.
		 * @returns		Value
		 */
		[[nodiscard]] Value get_or(const Key& key, const Value& def) const { return get(key).value_or(def); }

		/**
		 * @brief				Get the value of the specified key, casted to a given type.
		 * @tparam ReturnT		Any type to cast the value to before returning it.
		 * @param key			The key to retrieve.
		 * @param converter		A conversion function capable of converting from std::string -> ReturnT
		 * @returns				std::optional<ReturnT>
		 */
		template<typename ReturnT>
		[[nodiscard]] std::optional<ReturnT> getcast(const Key& key, const std::function<ReturnT(Value)>& converter) const
		{
			if (const auto& v{ get(key) }; v.has_value())
				return converter(v.value());
			return std::nullopt;
		}
		/**
		 * @brief				Get the value of the specified key, casted to a given type.
		 *\n					This overload is only usable when the return type directly accepts std::string.
		 * @tparam ReturnT		Any type to cast the value to before returning it.
		 * @param key			The key to retrieve.
		 * @returns				std::optional<ReturnT>
		 */
		template<typename ReturnT> requires std::constructible_from<ReturnT, Value>
		[[nodiscard]] std::optional<ReturnT> getcast(const Key& key) const
		{
			if (const auto& v{ get(key) }; v.has_value())
				return static_cast<ReturnT>(v.value());
			return std::nullopt;
		}

		/**
		 * @brief				Get the value of the specified key, casted to a given type.
		 * @tparam ReturnT		Any type to cast the value to before returning it.
		 * @param key			The key to retrieve.
		 * @param converter		A conversion function capable of converting from std::string -> ReturnT
		 * @param def			The default value to return if the key wasn't found.
		 * @returns				std::optional<ReturnT>
		 */
		template<typename ReturnT>
		[[nodiscard]] ReturnT getcast_or(const Key& key, const std::function<ReturnT(Value)>& converter, const ReturnT& def) const
		{
			return getcast<ReturnT>(key, converter).value_or(def);
		}
		/**
		 * @brief				Get the value of the specified key, casted to a given type.
		 *\n					This overload is only usable when the return type directly accepts std::string.
		 * @tparam ReturnT		Any type to cast the value to before returning it.
		 * @param key			The key to retrieve.
		 * @param def			The default value to return if the key wasn't found.
		 * @returns				std::optional<ReturnT>
		 */
		template<typename ReturnT> requires std::constructible_from<ReturnT, Value>
		[[nodiscard]] ReturnT getcast_or(const Key& key, const ReturnT& def) const
		{
			return getcast<ReturnT>(key).value_or(def);
		}

		/**
		 * @brief		Writes all key-value pairs in the container to the given output stream.
		 * @param os	The output stream to write to.
		 * @param cont	A container instance to write.
		 * @returns		std::ostream&
		 */
		friend std::ostream& operator<<(std::ostream& os, const basic_container<Key, Value>& cont)
		{
			for (const auto& [key, val] : cont)
				os << key << " = " << val << '\n';
			return os;
		}
	};
	/**
	 * @brief			Basic outer container for key-section pairs.
	 * @tparam Key		Key type.
	 * @tparam Section	Section type.
	 */
	template<typename Header, typename Section>
	struct basic_outer_container : public std::map<Header, Section> {
		using base = std::map<Header, Section>;
		using base::base;

		basic_outer_container() {}

		/**
		 * @brief			Get the section associated with the given header.
		 * @param header	The name of the header to retrieve.
		 * @returns			std::optional<Section>
		 */
		[[nodiscard]] std::optional<Section> get(const Header& header) const
		{
			if (const auto& it{ this->find(header) }; it != this->end())
				return it->second;
			return std::nullopt;
		}
		/**
		 * @brief			Get the section associated with the given header, or the specified default if it wasn't found.
		 * @param header	The header to retrieve.
		 * @param def		The default section to return if the header wasn't found.
		 * @returns			Section
		 */
		[[nodiscard]] Section get_or(const Header& header, const Section& def) const { return get(header).value_or(def); }

		/**
		 * @brief		Writes all header-section pairs in the container to the given output stream.
		 * @param os	The output stream to write to.
		 * @param cont	A container instance to write.
		 * @returns		std::ostream&
		 */
		friend std::ostream& operator<<(std::ostream& os, const basic_outer_container<Header, Section>& cont)
		{
			for (const auto& [header, section] : cont)
				os << '[' << header << ']' << '\n' << section;
			return os;
		}
	};

	using std::string;
	using inner_container = basic_container<string, string>;
	using outer_container = basic_outer_container<string, inner_container>;

#	if LANG_CPP >= 17
	using path = std::filesystem::path;
#	else
	using path = std::string;
#	endif

	namespace tokenizer {
		struct MINIParser : public token::base::TokenParserBase<outer_container, Token> {
			using base = token::base::TokenParserBase<outer_container, Token>;
			using base::OutputT;
			bool allowBlankValue{ true };
			const std::string filename{};

			MINIParser(std::vector<Token>&& tokens, const path& filename = {}) : base(std::move(tokens)), filename{ filename.generic_string() } {}

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
				std::optional<std::string> value;
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
					if (!map.contains(header))
						map.insert(std::make_pair(header, inner_container{}));
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
					case TokenType::BOOLEAN: [[fallthrough]];
					case TokenType::NUMBER: [[fallthrough]]; // set the temp value to a long double
					case TokenType::NUMBER_INT: [[fallthrough]];
					case TokenType::STRING: // set the temp value to a string
						if (!setter)
							throwEx(ln, "Missing Setter");
						if (value.has_value())
							value = value.value() + str;
						else
							value = str;
						break;
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
					case TokenType::NULL_TYPE:break;
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
	 * @class	MINI
	 * @brief	Contains a map of the headers, keys, and values of an INI file, or multiple INI files.
	 *\n		This INI variant uses monotype values, and uses the upgraded container types.
	 */
	class MINI : public outer_container {
	public:
		using base = outer_container;
		using base::base;

		using Header = string;
		using Key = string;
		using Value = string;

		/**
		 * @brief			Retrieve the specified key from the specified header as the specified type.
		 * @tparam TReturn	Return type that is implicitly convertible from std::string.
		 * @param header	Target Header
		 * @param key		Target Key
		 * @returns			std::optional<TReturn>
		 */
		template<var::convertible_from<Value> TReturn>
		std::optional<TReturn> castgetv(const Header& header, const Key& key) const&
		{
			if (const auto& v{ getv(header, key) }; v.has_value())
				return static_cast<TReturn>(v.value());
			return std::nullopt;
		}
		/**
		 * @brief			Retrieve the specified key from the specified header as the specified type.
		 * @tparam TReturn	Return type that is implicitly convertible from std::string.
		 * @param header	Target Header
		 * @param key		Target Key
		 * @param converter	A function that converts from (std::string) to (TReturn).
		 *\n				This is only required for types that are not directly castable from std::string.
		 * @returns			std::optional<TReturn>
		 */
		template<typename TReturn>
		std::optional<TReturn> castgetv(const Header& header, const Key& key, const std::function<TReturn(Value)>& converter) const&
		{
			if (const auto& v{ getv(header, key) }; v.has_value())
				return converter(v.value());
			return std::nullopt;
		}

		/**
		 * @brief			Retrieve the specified key from the specified header as the specified type, or a default value if it doesn't exist.
		 * @tparam TReturn	Return type that is implicitly convertible from std::string.
		 * @param header	Target Header
		 * @param key		Target Key
		 * @param def		A value to return in the event that the specified key doesn't exist.
		 * @returns			std::optional<TReturn>
		 */
		template<var::convertible_from<Value> TReturn>
		TReturn castgetv_or(const Header& header, const Key& key, const TReturn& def) const&
		{
			if (const auto& v{ getv(header, key) }; v.has_value())
				return static_cast<TReturn>(v.value());
			return def;
		}
		/**
		 * @brief			Retrieve the specified key from the specified header as the specified type, or a default value if it doesn't exist.
		 * @tparam TReturn	Return type that is implicitly convertible from std::string.
		 * @param header	Target Header
		 * @param key		Target Key
		 * @param def		A value to return in the event that the specified key doesn't exist.
		 * @returns			std::optional<TReturn>
		 */
		template<var::convertible_from<Value> TReturn>
		TReturn castgetv_or(const Header& header, const Key& key, const Value& def) const&
		{
			if (const auto& v{ getv(header, key) }; v.has_value())
				return static_cast<TReturn>(v.value());
			return static_cast<TReturn>(def);
		}
		/**
		 * @brief			Retrieve the specified key from the specified header as the specified type, or a default value if it doesn't exist.
		 * @tparam TReturn	Return type that is implicitly convertible from std::string.
		 * @param header	Target Header
		 * @param key		Target Key
		 * @param def		A value to return in the event that the specified key doesn't exist.
		 * @param converter	A function that converts from (std::string) to (TReturn).
		 *\n				This is only required for types that are not directly castable from std::string.
		 * @returns			std::optional<TReturn>
		 */
		template<typename TReturn>
		TReturn castgetv_or(const Header& header, const Key& key, const TReturn& def, const std::function<TReturn(Value)>& converter) const&
		{
			if (const auto& v{ getv(header, key) }; v.has_value())
				return converter(v.value());
			return def;
		}
		/**
		 * @brief			Retrieve the specified key from the specified header as the specified type, or a default value if it doesn't exist.
		 * @tparam TReturn	Return type that is implicitly convertible from std::string.
		 * @param header	Target Header
		 * @param key		Target Key
		 * @param def		A value to return in the event that the specified key doesn't exist.
		 * @param converter	A function that converts from (std::string) to (TReturn).
		 *\n				This is only required for types that are not directly castable from std::string.
		 * @returns			std::optional<TReturn>
		 */
		template<typename TReturn>
		TReturn castgetv_or(const Header& header, const Key& key, const Value& def, const std::function<TReturn(Value)>& converter) const&
		{
			if (const auto& v{ getv(header, key) }; v.has_value())
				return converter(v.value());
			return converter(def);
		}

		/**
		 * @brief			Retrieve the value(s) of the first matching key in all headers as the specified type.
		 * @tparam TReturn	Return type that is implicitly convertible from std::string.
		 * @param key		Target Key.
		 * @returns			std::optional<TReturn>
		 */
		template<var::convertible_from<Value> TReturn>
		std::optional<TReturn> castgetv_any(const Key& key) const&
		{
			if (const auto& v{ getv_any(key) }; v.has_value())
				return static_cast<TReturn>(v.value());
			return std::nullopt;
		}
		/**
		 * @brief			Retrieve the value(s) of the first matching key in all headers as the specified type.
		 * @tparam TReturn	Return type that is implicitly convertible from std::string.
		 * @param key		Target Key.
		 * @param converter	A function that converts from (std::string) to (TReturn).
		 *\n				This is only required for types that are not directly castable from std::string.
		 * @returns			std::optional<TReturn>
		 */
		template<typename TReturn>
		std::optional<TReturn> castgetv_any(const Key& key, const std::function<TReturn(Value)>& converter) const&
		{
			if (const auto& v{ getv_any(key) }; v.has_value())
				return converter(v.value());
			return std::nullopt;
		}
		/**
		 * @brief			Retrieve the value(s) of the first matching key in all headers as the specified type.
		 * @tparam TReturn	Return type that is implicitly convertible from std::string.
		 * @param key		Target Key.
		 * @param pred		Predicate function.
		 * @returns			std::optional<TReturn>
		 */
		template<var::convertible_from<Value> TReturn>
		std::optional<TReturn> castgetv_any(const Key& key, const std::function<bool(Value)>& pred) const&
		{
			if (const auto& v{ getv_any(key, pred) }; v.has_value())
				return static_cast<TReturn>(v.value());
			return std::nullopt;
		}		/**
		 * @brief			Retrieve the value(s) of the first matching key in all headers.
		 * @tparam TReturn	Return type that is implicitly convertible from std::string.
		 * @param key		Target Key.
		 * @param pred		Predicate function.
		 * @returns			std::optional<TReturn>
		 */
		 /**
		  * @brief			Retrieve the value(s) of the first matching key in all headers as the specified type.
		  * @tparam TReturn	Return type that is implicitly convertible from std::string.
		  * @param key		Target Key.
		  * @param pred		Predicate function.
		  * @param converter	A function that converts from (std::string) to (TReturn).
		  *\n				This is only required for types that are not directly castable from std::string.
		  * @returns			std::optional<TReturn>
		  */
		template<typename TReturn>
		std::optional<TReturn> castgetv_any(const Key& key, const std::function<bool(Value)>& pred, const std::function<TReturn(Value)>& converter) const&
		{
			if (const auto& v{ getv_any(key, pred) }; v.has_value())
				return converter(v.value());
			return std::nullopt;
		}
		/**
		 * @brief			Retrieve the value(s) of the first matching key in all headers as the specified type.
		 * @tparam TReturn	Return type that is implicitly convertible from std::string.
		 * @param key		Target Key.
		 * @param pred		Predicate function that accepts both a value, and the header that it belongs to.
		 * @returns			std::optional<TReturn>
		 */
		template<var::convertible_from<Value> TReturn>
		std::optional<TReturn> castgetv_any(const Key& key, const std::function<bool(Header, Value)>& pred) const&
		{
			if (const auto& v{ getv_any(key, pred) }; v.has_value())
				return static_cast<TReturn>(v.value());
			return std::nullopt;
		}
		/**
		 * @brief			Retrieve the value(s) of the first matching key in all headers as the specified type.
		 * @tparam TReturn	Return type that is implicitly convertible from std::string.
		 * @param key		Target Key.
		 * @param pred		Predicate function that accepts both a value, and the header that it belongs to.
		 * @param converter	A function that converts from (std::string) to (TReturn).
		 *\n				This is only required for types that are not directly castable from std::string.
		 * @returns			std::optional<TReturn>
		 */
		template<typename TReturn>
		std::optional<TReturn> castgetv_any(const Key& key, const std::function<bool(Header, Value)>& pred, const std::function<TReturn(Value)>& converter) const&
		{
			if (const auto& v{ getv_any(key, pred) }; v.has_value())
				return converter(v.value());
			return std::nullopt;
		}

		/**
		 * @brief			Retrieve the value(s) of every matching key in all headers as the specified type.
		 * @tparam TReturn	Return type that is implicitly convertible from std::string.
		 * @param key		Target Key.
		 * @returns			std::vector<TReturn>
		 */
		template<var::convertible_from<Value> TReturn>
		std::vector<TReturn> castgetv_all(const Key& key) const&
		{
			const auto& keys{ getv_all(key) };
			std::vector<TReturn> vec;
			if (keys.empty())
				return vec;
			vec.reserve(keys.size());
			for (const auto& it : keys)
				vec.emplace_back(static_cast<TReturn>(it));
			vec.shrink_to_fit();
			return vec;
		}
		/**
		 * @brief			Retrieve the value(s) of every matching key in all headers as the specified type.
		 * @tparam TReturn	Return type that is implicitly convertible from std::string.
		 * @param key		Target Key.
		 * @param converter	A function that converts from (std::string) to (TReturn).
		 *\n				This is only required for types that are not directly castable from std::string.
		 * @returns			std::vector<TReturn>
		 */
		template<typename TReturn>
		std::vector<TReturn> castgetv_all(const Key& key, const std::function<TReturn(Value)>& converter) const&
		{
			const auto& keys{ getv_all(key) };
			std::vector<TReturn> vec;
			if (keys.empty())
				return vec;
			vec.reserve(keys.size());
			for (const auto& it : keys)
				vec.emplace_back(converter(it));
			vec.shrink_to_fit();
			return vec;
		}
		/**
		 * @brief			Retrieve the value(s) of every matching key in all headers as the specified type.
		 * @tparam TReturn	Return type that is implicitly convertible from std::string.
		 * @param key		Target Key.
		 * @param pred		Predicate function.
		 * @returns			std::vector<TReturn>
		 */
		template<var::convertible_from<Value> TReturn>
		std::vector<TReturn> castgetv_all(const Key& key, const std::function<bool(Value)>& pred) const&
		{
			const auto& keys{ getv_all(key, pred) };
			std::vector<TReturn> vec;
			if (keys.empty())
				return vec;
			vec.reserve(keys.size());
			for (const auto& it : keys)
				vec.emplace_back(static_cast<TReturn>(it));
			vec.shrink_to_fit();
			return vec;
		}
		/**
		 * @brief			Retrieve the value(s) of every matching key in all headers as the specified type.
		 * @tparam TReturn	Return type that is implicitly convertible from std::string.
		 * @param key		Target Key.
		 * @param pred		Predicate function.
		 * @param converter	A function that converts from (std::string) to (TReturn).
		 *\n				This is only required for types that are not directly castable from std::string.
		 * @returns			std::vector<TReturn>
		 */
		template<typename TReturn>
		std::vector<TReturn> castgetv_all(const Key& key, const std::function<bool(Value)>& pred, const std::function<TReturn(Value)>& converter) const&
		{
			const auto& keys{ getv_all(key, pred) };
			std::vector<TReturn> vec;
			if (keys.empty())
				return vec;
			vec.reserve(keys.size());
			for (const auto& it : keys)
				vec.emplace_back(converter(it));
			vec.shrink_to_fit();
			return vec;
		}
		/**
		 * @brief			Retrieve the value(s) of every matching key in all headers.
		 * @tparam TReturn	Return type that is implicitly convertible from std::string.
		 * @param key		Target Key.
		 * @param pred		Predicate function that accepts both a value, and the header that it belongs to.
		 * @returns			std::vector<TReturn>
		 */
		template<var::convertible_from<Value> TReturn>
		std::vector<TReturn> castgetv_all(const Key& key, const std::function<bool(Header, Value)>& pred) const&
		{
			const auto& keys{ getv_all(key, pred) };
			std::vector<TReturn> vec;
			if (keys.empty())
				return vec;
			vec.reserve(keys.size());
			for (const auto& it : keys)
				vec.emplace_back(static_cast<TReturn>(it));
			vec.shrink_to_fit();
			return vec;
		}
		/**
		 * @brief			Retrieve the value(s) of every matching key in all headers.
		 * @tparam TReturn	Return type that is implicitly convertible from std::string.
		 * @param key		Target Key.
		 * @param pred		Predicate function that accepts both a value, and the header that it belongs to.
		 * @param converter	A function that converts from (std::string) to (TReturn).
		 *\n				This is only required for types that are not directly castable from std::string.
		 * @returns			std::vector<TReturn>
		 */
		template<typename TReturn>
		std::vector<TReturn> castgetv_all(const Key& key, const std::function<bool(Header, Value)>& pred, const std::function<TReturn(Value)>& converter) const&
		{
			const auto& keys{ getv_all(key, pred) };
			std::vector<TReturn> vec;
			if (keys.empty())
				return vec;
			vec.reserve(keys.size());
			for (const auto& it : keys)
				vec.emplace_back(converter(it));
			vec.shrink_to_fit();
			return vec;
		}


		/**
		 * @brief			Retrieve the specified key from the specified header.
		 * @param header	Target Header
		 * @param key		Target Key
		 * @returns			std::optional<Value>
		 */
		std::optional<Value> getv(const Header& header, const Key& key) const&
		{
			if (const auto& sect{ find(header) }; sect != end())
				if (const auto& it{ sect->second.get(key) }; it.has_value())
					return it.value();
			return std::nullopt;
		}
		/**
		 * @brief			Retrieve the specified key from the specified header.
		 * @param header	Target Header
		 * @param key		Target Key
		 * @param def		Value to return if the header or key doesn't exist.
		 * @returns			Value
		 */
		Value getv_or(const Header& header, const Key& key, const Value& def) const&
		{
			return getv(header, key).value_or(def);
		}

		/**
		 * @brief		Retrieve the value(s) of the first matching key in all headers.
		 * @param key	Target Key.
		 * @returns		std::optional<Value>
		 */
		std::optional<Value> getv_any(const Key& key) const&
		{
			for (const auto& [header, section] : *this)
				if (const auto& tgt{ section.get(key) }; tgt.has_value())
					return tgt.value();
			return std::nullopt;
		}
		/**
		 * @brief		Retrieve the value(s) of the first matching key in all headers.
		 * @param key	Target Key.
		 * @param pred	Predicate function.
		 * @returns		std::optional<Value>
		 */
		std::optional<Value> getv_any(const Key& key, const std::function<bool(Value)>& pred) const&
		{
			for (const auto& [header, section] : *this)
				if (const auto& tgt{ section.get(key) }; tgt.has_value() && pred(tgt.value()))
					return tgt.value();
			return std::nullopt;
		}
		/**
		 * @brief		Retrieve the value(s) of the first matching key in all headers.
		 * @param key	Target Key.
		 * @param pred	Predicate function that accepts both a value, and the header that it belongs to.
		 * @returns		std::optional<Value>
		 */
		std::optional<Value> getv_any(const Key& key, const std::function<bool(Header, Value)>& pred) const&
		{
			for (const auto& [header, section] : *this)
				if (const auto& tgt{ section.get(key) }; tgt.has_value() && pred(header, tgt.value()))
					return tgt.value();
			return std::nullopt;
		}

		/**
		 * @brief		Retrieve the value(s) of every matching key in all headers.
		 * @param key	Target Key.
		 * @returns		std::vector<Value>
		 */
		std::vector<Value> getv_all(const Key& key) const&
		{
			std::vector<Value> vec;
			vec.reserve(this->size());
			for (const auto& [header, section] : *this)
				if (const auto& tgt{ section.get(key) }; tgt.has_value())
					vec.emplace_back(tgt.value());
			vec.shrink_to_fit();
			return vec;
		}
		/**
		 * @brief		Retrieve the value(s) of every matching key in all headers.
		 * @param key	Target Key.
		 * @param pred	Predicate function.
		 * @returns		std::vector<Value>
		 */
		std::vector<Value> getv_all(const Key& key, const std::function<bool(Value)>& pred) const&
		{
			std::vector<Value> vec;
			vec.reserve(this->size());
			for (const auto& [header, section] : *this)
				if (const auto& tgt{ section.get(key) }; tgt.has_value() && pred(tgt.value()))
					vec.emplace_back(tgt.value());
			vec.shrink_to_fit();
			return vec;
		}
		/**
		 * @brief		Retrieve the value(s) of every matching key in all headers.
		 * @param key	Target Key.
		 * @param pred	Predicate function that accepts both a value, and the header that it belongs to.
		 * @returns		std::vector<Value>
		 */
		std::vector<Value> getv_all(const Key& key, const std::function<bool(Header, Value)>& pred) const&
		{
			std::vector<Value> vec;
			vec.reserve(this->size());
			for (const auto& [header, section] : *this)
				if (const auto& tgt{ section.get(key) }; tgt.has_value() && pred(header, tgt.value()))
					vec.emplace_back(tgt.value());
			vec.shrink_to_fit();
			return vec;
		}

		/**
		 * @brief			Check if the specified value is the same as a given value.
		 * @param header	Target Header.
		 * @param key		Target Key.
		 * @param value		Comparison value.
		 * @returns			bool
		 *					- true:		Value matches the given value.
		 *					- false:	Value doesn't match the given value.
		 */
		bool checkv(const Header& header, const Key& key, const Value& value) const
		{
			if (const auto& it{ getv(header, key) }; it.has_value())
				return it.value() == value;
			return false;
		}
		/**
		 * @brief			Use a given comparison function to compare the specified value to another given value.
		 * @param header	Target Header.
		 * @param key		Target Key.
		 * @param value		The value to pass to the comparison function as the _LEFT/FIRST_ value.
		 * @param compare	A comparison function.
		 * @returns			Comparison function return value.
		*/
		bool checkv(const Header& header, const Key& key, const Value& value, const std::function<bool(Value, Value)>& compare) const
		{
			if (const auto& it{ getv(header, key) }; it.has_value())
				return compare(value, it.value());
			return false;
		}
		/**
		 * @brief			Check all headers for the specified key with the specified value.
		 *\n				This function only stops when operator== returns true, or when the end of the container is reached.
		 * @param key		Target Key.
		 * @param value		Comparison value.
		 * @returns			bool
		 *					- true:		At least one header contains a key with the specified value.
		 *					- false:	No headers contain the specified key with the given value.
		 */
		bool checkv_any(const Key& key, const Value& value) const
		{
			for (const auto& [header, section] : *this)
				if (const auto& it{ section.get(key) }; it.has_value())
					if (it.value() == value)
						return true;
			return false;
		}
		/**
		 * @brief			Check all headers for the specified key with the specified value.
		 *\n				This function only stops when the predicate returns true, or when the end of the container is reached.
		 * @param key		Target Key.
		 * @param value		Comparison value.
		 * @param pred		Predicate/comparison function.
		 * @returns			Predicate return value or false.
		 */
		bool checkv_any(const Key& key, const Value& value, const std::function<bool(Value)>& pred) const
		{
			for (const auto& [header, section] : *this)
				if (const auto& it{ section.get(key) }; it.has_value())
					if (pred(it.value()))
						return true;
			return false;
		}

		/**
		 * @brief							Reads the specified file, merging the new keys into the current map.
		 * @param file						Target filepath.
		 * @param overwriteExistingKeys		When true, any existing values are overwritten by the incoming file's values.
		 */
		void read(path const& file, const bool& overwriteExistingKeys = true)
		{
			for (const auto& [header, section] : tokenizer::MINIParser(std::move(tokenizer::INITokenizer(std::move(file::read(file))).tokenize(token::DefaultDefs::TokenType::END))).parse()) {
				if (auto existing{ this->find(header) }; existing == this->end())
					this->insert_or_assign(header, section);
				else {
					auto& existingSection{ existing->second };
					for (const auto& [key, val] : section) {
						if (const bool exists{ existingSection.find(key) != existingSection.end() }; exists && overwriteExistingKeys)
							existingSection.insert_or_assign(key, val);
						else if (!exists) existingSection.insert(std::make_pair(key, val));
					}
				}
			}
		}
		/**
		 * @brief		Write all values present in the INI to the specified file.
		 * @param file	Target filepath.
		 * @returns		bool
		 *				- true:		The file was successfully written to.
		 *				- false:	File I/O failure.
		 */
		bool write(path const& file) const { return file::write(file, *this); }
		/**
		 * @brief				Write all matching headers present in the INI to the specified file.
		 * @param file			Target filepath.
		 * @param ...headers	List of headers to include in the output.
		 * @returns				bool
		 *						- true:		The file was successfully written to.
		 *						- false:	File I/O failure.
		 */
		template<var::same_or_convertible<string>... Headers>
		bool write_selective(path const& file, Headers... headers) const
		{
			std::stringstream buf;
			for (const auto& [header, section] : *this)
				if (var::variadic_or(header == headers...))
					buf << '[' << header << ']' << '\n' << section;
			return file::write_to(file, std::move(buf), false);
		}
	};
}

namespace file {
	using MINI = ini::MINI;
}
