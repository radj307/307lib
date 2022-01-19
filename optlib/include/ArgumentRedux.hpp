#pragma once
#include <sysarch.h>
#include <TokenRedux.hpp>
#include <ArgContainer.hpp>

#include <string>
#include <ostream>
#include <utility>
#include <algorithm>

namespace opt {
	namespace token {
		#pragma region DEFINITIONS_PACKAGE
		//using namespace ::token;
		/// @brief	This is a character's lexical type.
		enum class LEXEME : char {
			NULL_TYPE,
			_EOF,
			END,
			WHITESPACE,
			DASH,
			SLASH,
			BACKSLASH,
			COLON,
			EQUALS,
			QUOTE_SINGLE,
			QUOTE_DOUBLE,
			DIGIT,
			LETTER,
			CHARACTER,
		};

		/**
		 * @struct	LexemeDictionary
		 * @brief	Lexeme dictionary override for ArgumentRedux
		 */
		struct LexemeDictionary final : public ::token::base::LexemeDictBase<LEXEME> {
			/**
			 * @brief		Retrieve the lexeme associated with the given character.
			 * @param ch	Input Character
			 * @returnS		LexemeT
			 */
			LexemeT char_to_lexeme(const char& ch) const noexcept override
			{
				if (isdigit(ch))
					return LEXEME::DIGIT;
				else if (isalpha(ch))
					return LEXEME::LETTER;
				else if (isspace(ch))
					return LEXEME::WHITESPACE;
				switch (ch) {
				case '\377':	return LEXEME::_EOF;
				case '\03':		return LEXEME::END; // ETX char 3 (end of text), used as a delimiter to represents the boundaries of argv[]
				case '-':		return LEXEME::DASH;
				case '/':		return LEXEME::SLASH;
				case '\\':		return LEXEME::BACKSLASH;
				case ':':		return LEXEME::COLON;
				case '=':		return LEXEME::EQUALS;
				case '\'':		return LEXEME::QUOTE_SINGLE;
				case '\"':		return LEXEME::QUOTE_DOUBLE;
				default:		return LEXEME::CHARACTER;
				}
			}
		};

		/// @brief	This is a Token's type.
		enum class Type {
			NULL_TYPE,
			_EOF,
			END,
			PARAMETER,
			OPTION,
			FLAG,
			SETTER,
			STRING,
		};

		/**
		 * @brief	Print a token type to an output stream.
		 */
		inline std::ostream& operator<<(std::ostream& os, const Type& t)
		{
			switch (t) {
			case Type::NULL_TYPE:
				return os << "[NULL]";
			case Type::_EOF:
				return os << "[EOF]";
			case Type::END:
				return os << "[END]";
			case Type::PARAMETER:
				return os << "Parameter";
			case Type::OPTION:
				return os << "Option";
			case Type::FLAG:
				return os << "Flag";
			case Type::SETTER:
				return os << "Setter";
			case Type::STRING:
				return os << "String";
			default:
				return os;
			}
		}

		/// @brief	ArgumentRedux Token.
		using Token = ::token::base::TokenBase<Type>;

		#pragma endregion DEFINITIONS_PACKAGE
		#pragma region FORMAT_PACKAGE

		/**
		 * @class	Tokenizer
		 * @brief	Tokenizes streams containing arguments.
		 */
		class Tokenizer final : public ::token::base::TokenizerBase<LEXEME, LexemeDictionary, Type, Token> {
			bool _allowSlash;

		public:
			Tokenizer(std::stringstream&& ss, const bool& allow_slash_prefix = false) : TokenizerBase<LEXEME, LexemeDictionary, Type, Token>(std::forward<std::stringstream>(ss), LEXEME::WHITESPACE), _allowSlash{ allow_slash_prefix } {}

			/**
			 * @brief			Enable of disable using the slash character as an additional type prefix to dashes.
			 * @param enable	When true, slashes can be prefixes.
			 * @returns			bool
			 *\n				Previous value.
			 */
			bool setSlashEnabled(const bool& enable)
			{
				const auto copy{ _allowSlash };
				_allowSlash = enable;
				return copy;
			}
			/**
			 * @brief	Check if slashes may be considered additional type prefixes to dashes.
			 * @returns	bool
			 *\n		Current value.
			 */
			bool getSlashEnabled() const { return _allowSlash; }

			Token getNext() override
			{
				const char ch{ getch() };
				switch (get_lexeme(ch)) {
				case LEXEME::END:
					return{ Type::END, ch };
				case LEXEME::SLASH:
					if (!_allowSlash) goto HANDLE_PARAMETER; //////////> GOTO
					else [[fallthrough]];
				case LEXEME::DASH:
				{
					auto next{ peeklex(LEXEME::NULL_TYPE) };
					if (next == LEXEME::NULL_TYPE)
						return{ Type::_EOF };

					if (next == LEXEME::DASH) { // is option
						eatnext(); // eat the dash
						return{ Type::OPTION, getnotsimilar(LEXEME::END, LEXEME::WHITESPACE, LEXEME::EQUALS) };
					}

					else if (next == LEXEME::DIGIT) // negative number
						goto HANDLE_PARAMETER; //////////> GOTO

					// else is a flag
					else return{ Type::FLAG, getnotsimilar(LEXEME::END, LEXEME::WHITESPACE, LEXEME::EQUALS) };
				}
				case LEXEME::BACKSLASH: [[fallthrough]];
				case LEXEME::CHARACTER: [[fallthrough]];
				case LEXEME::COLON: [[fallthrough]];
				case LEXEME::DIGIT: [[fallthrough]];
				case LEXEME::LETTER:
				HANDLE_PARAMETER: /////////< GOTO TARGET
					rollback();
					return{ Type::PARAMETER, getnotsimilar(LEXEME::END, LEXEME::WHITESPACE) };
				case LEXEME::EQUALS:
					return{ Type::SETTER, ch };
				case LEXEME::QUOTE_DOUBLE:
					return{ Type::STRING, getuntil_unescaped('\"') };
				case LEXEME::QUOTE_SINGLE:
					return{ Type::STRING, getuntil_unescaped('\'') };
				case LEXEME::_EOF:
					return{ Type::_EOF };
				default:
					return Token{ Type::NULL_TYPE, ch };
				}
			}
		};

		/**
		 * @class	Parser
		 * @brief	Parses tokenized argument vectors.
		 */
		class Parser final : public ::token::base::TokenParserBase<ArgContainerType, Token> {
		public:
			struct Input final {
				using variant = std::variant<std::string, char>;
				using invec = std::vector<variant>;
				using outvec = std::vector<std::string>;
				outvec operator()(invec&& variant_vec) const
				{
					strvec vec;
					vec.reserve(variant_vec.size());
					for (auto& it : variant_vec) {
						std::string s{};
						if (std::holds_alternative<char>(it))
							s = std::string{ 1ull, std::move(std::get<char>(std::move(it))) };
						else
							s = std::move(std::get<std::string>(std::move(it)));
						vec.emplace_back(std::move(str::strip_preceeding(std::move(s), '-')));
					}
					vec.shrink_to_fit();
					return vec;
				}
				CONSTEXPR outvec&& operator()(outvec&& vec) const { return std::forward<outvec>(vec); }
				outvec operator()(const invec& variant_vec) const
				{
					strvec vec;
					vec.reserve(variant_vec.size());
					for (auto& it : variant_vec) {
						std::string s{};
						if (std::holds_alternative<char>(it))
							s = std::string{ 1ull, std::get<char>(it) };
						else
							s = std::get<std::string>(it);
						vec.emplace_back(str::strip_preceeding(s, '-'));
					}
					vec.shrink_to_fit();
					return vec;
				}
				outvec operator()(const outvec& vec) const { return vec; }
			};

			using capture_variant = std::variant<std::string, char>;
			using invec = std::vector<capture_variant>;
			using strvec = std::vector<std::string>;
		private:
			strvec capture_list;

			/// @brief	This function accepts a vector of string/char variants, and returns a vector of strings.
			static strvec expand_capture_vec(invec&& capturelist)
			{
				strvec vec;
				vec.reserve(capturelist.size());
				for (auto& it : capturelist) {
					std::string s{};
					if (std::holds_alternative<char>(it))
						s = std::string{ 1ull, std::get<char>(it) };
					else
						s = std::get<std::string>(it);
					vec.emplace_back(str::strip_preceeding(s, '-'));
				}
				vec.shrink_to_fit();
				return vec;
			}

			bool can_capture(const char& flag) const
			{
				return std::any_of(capture_list.begin(), capture_list.end(), [&flag](auto&& str) { return str.size() == 1ull && str.at(0ull) == flag; });
			}
			bool can_capture(const std::string& opt) const
			{
				return std::any_of(capture_list.begin(), capture_list.end(), [&opt](auto&& str) { return str == opt; });
			}

		public:
			Parser(std::vector<Token>&& tokens, Input::invec&& capture_list) : TokenParserBase<ArgContainerType, Token>(std::move(tokens)), capture_list{ Input()((std::move(capture_list))) } {}
			Parser(std::vector<Token>&& tokens, Input::outvec&& capture_list) : TokenParserBase<ArgContainerType, Token>(std::move(tokens)), capture_list{ Input()((std::move(capture_list))) } {}
			Parser(const std::vector<Token>& tokens, const Input::invec& capture_list) : TokenParserBase<ArgContainerType, Token>(tokens), capture_list{ Input()(capture_list) } {}
			Parser(const std::vector<Token>& tokens, const Input::outvec& capture_list) : TokenParserBase<ArgContainerType, Token>(tokens), capture_list{ Input()(capture_list) } {}

			OutputT parse() const
			{
				OutputT args{};

				if (tokens.empty())
					return args;

				const auto& peek{ [this](const std::vector<TokenT>::const_iterator& it) -> std::vector<TokenT>::const_iterator {
					if (std::distance(it, tokens.end()) <= 1ll)
						return tokens.end();
					return it + 1;
				} };

				for (auto it{ tokens.begin() }; it != tokens.end(); ++it) {
					switch (it->type) {
					case Type::FLAG: // parse each char
						for (auto ch{ it->str.begin() }; ch != it->str.end(); ++ch) {
							const auto remaining{ std::distance(ch, it->str.end()) - 1 };
							if (remaining == 0) {
								auto next{ peek(it) };
								if (next == tokens.end())
									break; // no more tokens, break early
								else if (next->type == Type::SETTER) {
									++it; // eat setter token
									if (can_capture(*ch)) {
										std::string captured{};
										++it; // move to next token
										for (; it != tokens.end() && it->type != Type::END; ++it) {
											captured += it->str;
											if (next = peek(it); next != tokens.end() && next->type != Type::END)
												captured += ' ';
										}
										if (!captured.empty())
											args.emplace_back(Flag{ *ch, std::move(captured) });
									}
									args.emplace_back(Flag{ *ch, std::nullopt });
								}
								else args.emplace_back(Flag{ *ch, std::nullopt });
								break;
							}
							else args.emplace_back(Flag{ *ch, std::nullopt });
						}
						break;
					case Type::OPTION:
					{
						const auto here{ it->str };
						if (can_capture(here)) {
							if (const auto next_it{ peek(it) }; next_it != tokens.end() && std::distance(it, tokens.end()) >= 2ll) {
								switch (next_it->type) {
								case Type::END: [[fallthrough]];
								case Type::SETTER:
									++it; // eat setter / end
									[[fallthrough]];
								case Type::PARAMETER:
									args.emplace_back(Option{ here, (++it)->str }); // option with arg
									break;
								default:break;
								}
								continue;
							}
						} // ^ fallthrough v
						args.emplace_back(Option{ here, std::nullopt }); // option without arg
						break;
					}
					case Type::PARAMETER:
						args.emplace_back(Parameter{ it->str });
						break;
					case Type::_EOF:
						return args;
						break;
					case Type::END: [[fallthrough]];
					case Type::SETTER: break; // eat any unused setters
					case Type::NULL_TYPE: [[fallthrough]];
					default:
						throw make_exception("Unexpected token type: ", it->type);
					}
				}

				return args;
			}
		};

		#pragma endregion FORMAT_PACKAGE
	}

	class ParamsAPI3 : public ArgContainer {

		static std::stringstream to_stream(const int& argc, char** argv, const int& off = 1)
		{
			#pragma warning (disable:26800)
			std::stringstream ss;
			for (auto i{ off }; i < argc; ++i)
				ss << argv[i] << "\3 ";
			return std::move(ss);
			#pragma warning (default:26800)
		}
		static std::stringstream to_stream(const std::vector<std::string>& args)
		{
			#pragma warning (disable:26800)
			std::stringstream ss;
			for (const auto& it : args)
				ss << it << "\3 ";
			return std::move(ss);
			#pragma warning (default:26800)
		}

	public:
		/// @brief Default Constructor.
		ParamsAPI3() = default;
		ParamsAPI3(const ParamsAPI3&) = default;
		ParamsAPI3(ParamsAPI3&&) = default;
		/**
		 * @brief				Parsing Constructor.
		 * @param argc			Argument array size from main.
		 * @param argv			Argument array from main.
		 * @param ...captures	Argument names that should be able to capture. Do not include delimiter prefixes, they will be stripped.
		 */
		template<ValidInputType... Ts>
		ParamsAPI3(const int argc, char** argv, const Ts&... captures) : ArgContainer(token::Parser(token::Tokenizer(std::move(to_stream(argc, argv))).tokenize(), token::Parser::invec{ captures... }).parse(), argv[0]) {}
		/**
		 * @brief				Parsing Constructor.
		 * @param args			Argument vector.
		 * @param ...captures	Argument names that should be able to capture. Do not include delimiter prefixes, they will be stripped.
		 */
		template<ValidInputType... Ts>
		ParamsAPI3(const std::vector<std::string>& args, const Ts&... captures) : ArgContainer(token::Parser(token::Tokenizer(std::move(to_stream(args))).tokenize(), token::Parser::invec{ captures... }).parse()) {}
				/**
		 * @brief				Parsing Constructor.
		 * @param args			Argument vector.
		 * @param ...captures	Argument names that should be able to capture. Do not include delimiter prefixes, they will be stripped.
		 */
		template<ValidInputType... Ts>
		ParamsAPI3(std::vector<std::string>&& args, const Ts&... captures) : ArgContainer(token::Parser(token::Tokenizer(std::move(to_stream(std::move(args)))).tokenize(), token::Parser::invec{ captures... }).parse()) {}

		ParamsAPI3& operator=(const ParamsAPI3&) = default;
		ParamsAPI3& operator=(ParamsAPI3&&) = default;

		friend std::ostream& operator<<(std::ostream& os, const ParamsAPI3& obj)
		{
			for (auto arg{ obj.begin() }; arg != obj.end(); ++arg) {
				os << *arg;
				if (arg != obj.end() - 1ll)
					os << ' ';
			}
			return os;
		}

		/**
		 * @brief		Retrieve an argument of a specified type as its actual type; not as a variant.
		 * @tparam Type	Type to search for & return as.
		 * @tparam Name	Input argument name type.
		 * @param name	Argument name to search for.
		 * @param off	Position to start searching at. (inclusive)
		 * @param end	Position to stop searching at. (exclusive)
		 * @returns		std::optional<Type>
		 */
		template<ValidArg Type, ValidInputType Name>
		constexpr const std::optional<Type> typeget(const Name& name, const std::optional<ArgContainerIteratorType>& off = std::nullopt, const std::optional<ArgContainerIteratorType>& end = std::nullopt) const noexcept
		{
			const auto target{ get<Type>(name, off, end) };
			return (target.has_value() ? std::get<Type>(target.value()) : static_cast<std::optional<Type>>(std::nullopt));
		}
		/**
		 * @brief			Retrieve any arguments of a specified type as its actual type; not as a variant.
		 * @tparam Type		Type to search for & return as.
		 * @tparam Names	Input argument name types.
		 * @param names		Argument names to search for. If no names are included, any argument with the specified type will be returned.
		 * @returns			std::optional<Type>
		 */
		template<ValidArg Type, ValidInputType... Names>
		constexpr const std::optional<Type> typeget_any(const Names&... names) const noexcept
		{
			if (const auto t{ get_any<Type>(names...) }; t.has_value())
				return std::get<Type>(t.value());
			return std::nullopt;
		}
		/**
		 * @brief			Retrieve all arguments of a specified type, as their actual type; not as a variant.
		 * @tparam Type		Type to search for & return as.
		 * @tparam Names	Input argument name types.
		 * @param names		Argument name(s) to search for. If no names are included, all arguments of the specified type will be returned.
		 * @returns			std::vector<Type>
		 */
		template<ValidArg Type, ValidInputType... Names>
		constexpr const std::vector<Type> typeget_all(const Names&... names) const noexcept
		{
			if (const auto targets{ get_all<Type>(names...) }; !targets.empty()) {
				std::vector<Type> vec;
				vec.reserve(targets.size());
				for (auto& arg : targets)
					vec.emplace_back(std::get<Type>(arg));
				vec.shrink_to_fit();
				return vec;
			}
			return{};
		}

		template<CanHaveValueArgument Type, ValidInputType Name> requires std::same_as<Type, Option> || std::same_as<Type, Flag>
		constexpr const std::optional<std::string> typegetv(const Name & name, const std::optional<ArgContainerIteratorType>&off = std::nullopt, const std::optional<ArgContainerIteratorType>&end = std::nullopt) const noexcept(false)
		{
			if (const auto target{ find<Type>(name, off, end) }; target != _args.end())
				return std::get<Type>(*target).getv();
			return std::nullopt;
		}

		/**
		 * @brief			Return the capture value of the first matching argument.
		 * @tparam Types	Variadic Input Types (Option, Flag). If no types are included, any types may be considered a match.
		 * @tparam Names	Variadic Input Name Types.
		 * @param names...	Any number of option/flag names. If no names are included, any names may be considered a match.
		 */
		template<CanHaveValueArgument... Types, ValidInputType... Names>
		constexpr const std::optional<std::string> typegetv_any(const Names&... names) const
		{
			constexpr const bool match_opts{ std::disjunction_v<std::is_same<Option, Types>...> }, match_flags{ std::disjunction_v<std::is_same<Flag, Types>...> }, match_any_name{ var::none<Names...> };
			if (const auto target{ find_any<Types...>(names...) }; target != _args.end()) {
				if (match_any_name || var::variadic_or(names == InputWrapper(get_name(*target))...)) {
					if constexpr (match_opts) {
						if (is_type<Option>(*target))
							return std::get<Option>(*target).getv();
					}
					if constexpr (match_flags) {
						if (is_type<Flag>(*target))
							return std::get<Flag>(*target).getv();
					}
				}
			}
			return std::nullopt;
		}

		/**
		 * @brief			Get all matching parameters.
		 * @tparam Type		Input Type. (Parameter)
		 * @tparam Names	Variadic Input Name Types
		 * @param names...	Any number of parameter names. If no names are included, all parameters will be returned.
		 */
		template<var::all_same<Parameter> Type, ValidInputType... Names>
		constexpr const std::vector<std::string> typegetv_all(const Names&... names) const
		{
			std::vector<std::string> vec;
			vec.reserve(_args.size());
			for (auto& it : typeget_all<Type>(InputWrapper(names)...))
				vec.emplace_back(get_name(it));
			vec.shrink_to_fit();
			return vec;
		}
		template<CanHaveValueArgument... Types, ValidInputType... Names> requires var::not_same<Parameter, Types...>
		constexpr const std::vector<std::string> typegetv_all(const Names&... names) const
		{
			constexpr const bool match_any_type{ var::none<Types...> }, match_any_name{ var::none<Names...> };
			std::vector<std::string> vec;
			vec.reserve(_args.size());
			for (auto& it : get_all<Types...>(names...)) {
				if (const auto name{ get_name(it) };  match_any_name || var::variadic_or(InputWrapper(names) == name...)) {
					if constexpr (match_any_type || var::any_same<Option, Types... >) {
						if (is_type<Option>(it))
							if (const auto ty{ std::get<Option>(it) }; ty.hasv())
								vec.emplace_back(ty.getv().value());
					}
					if constexpr (match_any_type || var::any_same<Flag, Types...>) {
						if (is_type<Flag>(it))
							if (const auto ty{ std::get<Flag>(it) }; ty.hasv())
								vec.emplace_back(ty.getv().value());
					}
					if constexpr (match_any_type || var::any_same<Parameter, Types...>) {
						if (is_type<Parameter>(it))
							vec.emplace_back(name);
					}
				}
			}
			vec.shrink_to_fit();
			return vec;
		}
	};
}
