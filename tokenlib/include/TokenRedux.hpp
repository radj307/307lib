/**
 * @file	TokenRedux.hpp
 * @author	radj307
 * @brief	New tokenization & parsing library providing superior abstraction capabilities and a much less confusing usage experience.
 *			For more information on tokenization, see: https://en.wikipedia.org/wiki/Lexical_analysis#Tokenization
 * @page	TokenRedux
 *
 *			# TokenRedux
 *
 *			This is a universal tokenization & parsing library that uses templated objects and stringstreams to parse data into
 *			 any implementation-defined object type.
 *
 *			## Packages
 *
 *			Packages are collections of objects that perform a similar, specific function.
 *
 *			### Framework Package
 *
 *			The base virtual objects that should be inherited from to implement the tokenizer/parser pipeline.
 *			This also provides all of the base functions available to the Tokenizer and the Parser.
 *			In almost all cases, the framework provided by <TokenRedux.hpp> is adequate.
 *
 *			### Definition Packages
 *
 *			Provides lexeme and token types that can be reused between any number of file formats.
 *
 *			#### Types
 *
 *			##### Objects
 *			- __Lexeme Dictionary__
 *            Provides bindings between characters and their associated Lexeme types that are usable by the Tokenizer.
 *            Must be derived from, or implement the signature of, the `LexemeDictBase` object.
 *          - __Token__
 *            Container object that holds a character sequence and a single type provided by the __Token Type__ enum.
 *            These are read in a list by the parser, which looks for specific sequences of tokens to both validate and parse data.
 *            Must be derived from, or implement the signature of, the `TokenBase` object.
 *
 *			##### Enums
 *			- __Lexeme Type__
 *			  Defines possible types associated with a single character, such as a digit, quotation mark, or letter.
 *			- __Token Type__
 *			  Defines possible types associated with sections of the input data, such as words, numbers, or lines.
 *
 *			### Format Packages
 *
 *			These provide the actual tokenizer and parser implementations used for a single file format.
 *          Format Packages are dependent on a single _Definition Package_, and cannot be transferred without modifying code.
 *			This acts like an interface for retrieving usable data from tokenized data.
 *
 *			#### Types
 *
 *			##### Objects
 *			- __Tokenizer__
 *			  Takes a stringstream as input, and divides it into a vector of Tokens using the provided Definition Package.
 *            Must derive from the `TokenizerBase` object.
 *			- __TokenParser__
 *			  Takes a vector of Tokens as input, and parses it into a usable object type, defined by the implementation.
 *            Must derive from the `TokenParserBase` object.
 *
 *			# Getting Started
 * 
 *			## Planning
 * 
 *			## Building a Definitions Package
 * 
 *			## Building a Format Package
 * 
 *			 1. Start by creating a new `Tokenizer`-derived type, filling in template arguments with the corresponding  
                object provided by the _Definitions Package_ you're using, and override the `getNext()` function.
 *			```cpp
 *			class Tokenizer : token::base::TokenizerBase<Lexeme, LexemeDictionary, Token, Token> {
 *				Token getNext() override
 *				{
 *					// TODO: Read in characters
 *				}
 *			};
 *			```
 *           2. Next, decide how you want to implement the Parser phase:
 *				- No parsing phase, use Token vector directly in-place or with another parser.  
 *                1. __If you choose to use another parser, or not to use a parser, you can skip this step.__
 *              - Use the TokenRedux parser base object.
 *                1. Choose the object/type you want the parser to return. This is usually a list of some sort, or a custom object.
 *                2. 
 */
#pragma once
#include <sysarch.h>
#include <make_exception.hpp>
#include <var.hpp>
#include <str.hpp>

#include <sstream>
#include <istream>
#include <ostream>
#include <vector>
#include <string>
#include <iterator>
#include <concepts>

namespace token::base {
	/**
	 * @struct			LexemeDictBase
	 * @brief			Used to convert characters to a templated Lexeme object, which represents a single character's lexical "type".
	 *\n				A Lexeme can be thought of as a character's type, and as such multiple characters can be associated with a single Lexeme, but a single Lexeme always represents a single character length.
	 * @tparam LexemeT	Lexeme type. This is defined by a "definitions package".
	 */
	template<typename LexemeType>
	struct LexemeDictBase {
		using LexemeT = LexemeType;

		virtual ~LexemeDictBase() = default;

		/**
		 * @brief		Override this function to select a lexeme type given a single character.
		 * @param ch	Input Character
		 * @returns		LexemeT
		 */
		[[nodiscard]] virtual LexemeT char_to_lexeme(const char&) const noexcept = 0;

		/**
		 * @brief		Convert a char to a LexemeT
		 * @param ch	Input Character
		 * @returns		LexemeT
		 */
		[[nodiscard]] LexemeT operator()(const char& ch) const noexcept
		{
			return char_to_lexeme(ch);
		}
	};

	/**
	 * @struct			TokenBase
	 * @brief			Base token object. It represents zero or more characters of data, and is the most basic unit recognized by the parsing step.
	 * @tparam TokenType	The TokenBase Type instance to use. This is defined by a "definitions package".
	 */
	template<typename TokenType>
	struct TokenBase {
		using Type = TokenType;

		/// @brief	This token's type identifier
		Type type;
		/// @brief	Contains this token's string value.
		std::string str;

		[[nodiscard]] WINCONSTEXPR operator Type() const { return type; }
		[[nodiscard]] operator std::string() const { return str; }

		[[nodiscard]] WINCONSTEXPR bool operator==(const TokenBase<Type>& o) const { return type == o.type && str == o.str; }
		[[nodiscard]] WINCONSTEXPR bool operator!=(auto&& o) const { return !operator==(std::forward<decltype(o)>(o)); }

		friend std::ostream& operator<<(std::ostream& os, const TokenBase<Type>& token) { return os << token.str; }

		/**
		 * @brief		TokenBase Constructor.
		 * @param type	This token's type.
		 * @param str	This token's string.
		 */
		WINCONSTEXPR TokenBase(const Type& type, const std::string& str = {}) : type{ type }, str{ str } {}
		/**
		 * @brief		TokenBase Constructor.
		 * @param type	This token's type.
		 * @param ch	This token's character.
		 */
		WINCONSTEXPR TokenBase(const Type& type, const char& ch) : type{ type }, str{ 1ull, ch } {}
		/**
		 * @brief		TokenBase Constructor.
		 * @param str	This token's string.
		 * @param type	This token's type.
		 */
		WINCONSTEXPR TokenBase(const std::string& str, const Type& type) : type{ type }, str{ str } {}
		/**
		 * @brief		TokenBase Constructor.
		 * @param ch	This token's character.
		 * @param type	This token's type.
		 */
		WINCONSTEXPR TokenBase(const char& ch, const Type& type) : type{ type }, str{ 1ull, ch } {}

		/// @brief	Virtual Destructor
		virtual ~TokenBase() noexcept = default;

		static const TokenBase NullToken;
	};
	template<class TokenType>
	inline const TokenBase<TokenType> TokenBase<TokenType>::NullToken{ static_cast<TokenType>(0), '\0' };

	/**
	 * @class				TokenizerBase
	 * @brief				Base TokenizerBase object that accepts a stringstream and lexeme dictionary, then tokenizes the data.
	 *\n					Can be used inline by calling `tokenize()`.
	 * @tparam LexemeT		The Lexeme Type to use. This is defined by a "definitions package".
	 * @tparam Dictionary	The Lexeme Dictionary Type to use. This is defined by a "definitions package".
	 * @tparam TokenType	The TokenBase::Type to use. This is defined by a "definitions package".
	 * @tparam Token		The TokenBase type to use. This is defined by a "definitions package".
	 */
	template<typename LexemeT, std::derived_from<LexemeDictBase<LexemeT>> Dictionary, typename TokenType, std::derived_from<TokenBase<TokenType>> Token = TokenBase<TokenType>>
	class TokenizerBase {
	public:
		using TokenT = Token;
	protected:
		/// @brief	The stringstream containing the untokenized data.
		std::stringstream ss;
		/// @brief	The stream reader position prior to the most recent call to a base reader method such as `getch`, `getsimilar`, `getUntil`, etc.
		std::streamoff lastPos{ 0ll };
		/// @brief	Lexeme dictionary used to retrieve the lexeme associated with a given character.
		Dictionary get_lexeme;
		/// @brief	Lexeme associated with skippable whitespace.
		LexemeT lexeme_whitespace;

	public:
		/**
		 * @brief				Default Constructor.
		 * @param ss			Stringstream rvalue reference containing the target data.
		 * @param whitespace	The lexeme associated with whitespace characters. This is used by the getch() method to skip whitespace.
		 */
		TokenizerBase(std::stringstream&& ss, LexemeT whitespace) : ss{ std::move(ss) }, lastPos{ 0ull }, lexeme_whitespace{ whitespace } {}
	protected:
		/// @brief	Virtual Destructor
		virtual ~TokenizerBase() noexcept = default;

		/**
		 * @brief String operator, returns the result local stream's str() function.
		 * @returns std::string
		 */
		[[nodiscard]] virtual explicit operator const std::string() const { return ss.str(); }

		/// @brief Stream extraction operator. Inserts the istream's read buffer into the local stream.
		template<class T> requires std::derived_from<T, TokenizerBase>
		friend std::istream& operator>>(std::istream& is, T& tokenizer)
		{
			is >> tokenizer.ss.rdbuf();
			return is;
		}
		/// @brief Stream insertion operator. Inserts the local stream's read buffer in the given ostream.
		template<class T> requires std::derived_from<T, TokenizerBase>
		friend std::ostream& operator<<(std::ostream& os, const T& tokenizer)
		{
			os << tokenizer.ss.rdbuf();
			return os;
		}

		/// @brief Retrieve the current goodbit state of the local stream
		[[nodiscard]] bool good() const { return ss.good(); }
		/// @brief Retrieve the current badbit state of the local stream
		[[nodiscard]] bool bad() const { return ss.bad(); }
		/// @brief Retrieve the current failbit state of the local stream.
		[[nodiscard]] bool fail() const { return ss.fail(); }
		/// @brief Retrieve the current eofbit state of the local stream. Use the hasMore() function to check whether the stream is at the end or not.
		[[nodiscard]] bool eof() const { return ss.eof(); }

		/**
		 * @brief Check if there are more valid characters ahead of the current stream getter position. Use this when looping until the end of stream.
		 * @returns bool
		 */
		[[nodiscard]] virtual bool hasMore() const
		{
			if (ss)
				return true;
			return false;
		}

		/**
		 * @brief					Get the next character from the stream, and move the getter position forward by one.
		 *\n						This function sets lastPos to the current position before incrementing the getter.
		 * @param allow_whitespace	When true, allows any character to be returned, including whitespace characters.
		 * @returns					char
		 */
		[[nodiscard]] char getch(const bool allow_whitespace = false)
		{
			setLastPosHere();
			if (allow_whitespace) // allow whitespace, return next character
				return static_cast<char>(ss.get());
			char c; // don't allow whitespace, read ahead until next non-whitespace character and return that
			for (c = static_cast<char>(ss.get()); get_lexeme(c) == lexeme_whitespace; c = static_cast<char>(ss.get())) { setLastPosHere(); }
			return c;
		}

		/**
		 * @brief	Retrieve the next character from the stream, but don't advance the read pointer.
		 * @returns char
		 */
		[[nodiscard]] char peek()
		{
			if (!hasMore())
				return '\0';
			return static_cast<char>(ss.peek());
		}

		/**
		 * @brief			Retrieve the next character from the stream, but don't advance the read pointer.
		 * @throws except	There are no more tokens to peek at in the buffer.
		 * @returns			LexemeT
		 */
		[[nodiscard]] LexemeT peeklex() noexcept(false)
		{
			if (!hasMore())
				throw make_exception("peeklex() failed:  There are no remaining tokens!");
			return get_lexeme(static_cast<char>(ss.peek()));
		}

		/**
		 * @brief					Retrieve the next character from the stream, but don't advance the read pointer.
		 * @param noTokensDefault	If there are no more tokens to peek at in the buffer, return this value instead of throwing like the parameterless variant of peeklex().
		 * @returns					LexemeT
		 */
		[[nodiscard]] LexemeT peeklex(const LexemeT& noTokensDefault) noexcept
		{
			if (!hasMore())
				return noTokensDefault;
			return get_lexeme(static_cast<char>(ss.peek()));
		}

		/**
		 * @brief			Eat (advance the read pos past) upcoming characters.
		 * @param count		The number of characters the eat.
		 * @returns			bool
		 *\n		true	There are more upcoming characters.
		 *\n		false	Reached EOF while eating characters.
		 */
		bool eatnext(const size_t& count = 1ull) noexcept
		{
			for (size_t i{ 0ull }; i < count; ++i) {
				if (ss.good())
					(void)ss.get();
				else return false;
			}
			return hasMore();
		}

		template<class Tokenizer = decltype(this)>
		[[nodiscard]] std::vector<TokenT> recurseInto(const std::string& str, const std::optional<TokenT>& eof_tkn = std::nullopt, const size_t& reserve_sz = 256ull)
		{
			return Tokenizer(std::move(std::stringstream{ str }), lexeme_whitespace).tokenize(eof_tkn, reserve_sz);
		}

		/**
		 * @brief				Retrieve a string containing everything enclosed by the given bracket types, respecting bracket scope.
		 * @param bracketOpen	The lexeme type associated with opening brackets, used to detect scope.
		 * @param bracketClose	The lexeme type associated with closing brackets, used to detect scope & the ending delimiter.
		 *\n					The final closing bracket is eaten, and is not included in the returned string.
		 * @returns				std::string
		 */
		[[nodiscard]] virtual std::string getBrackets(const LexemeT& bracketOpen, const LexemeT& bracketClose, const bool& eatLast = true)
		{
			std::string s;
			size_t innerCount{ 0ull };
			for (char c{ getch(true) }; hasMore(); c = getch(true)) {
				const auto& lex{ get_lexeme(c) };
				if (lex == bracketOpen)
					++innerCount;
				else if (lex == bracketClose) {
					if (innerCount == 0ull) {
						if (!eatLast)
							s += c;
						break;
					}
					if (innerCount > 0)
						--innerCount;
				}
				s += c;
			}
			return s;
		}

		/**
		 * @brief				Get a string containing everything from the current stream getter position until a given delimiter.
		 * @param delim			Stop reading ahead when this character is encountered
		 * @param no_rollback	When true, the first delimiter reached will be eaten when returning. (You can still rollback manually.)
		 * @returns				std::string
		 */
		[[nodiscard]] virtual std::string getline(const char& delim = '\n', const bool& no_rollback = true)
		{
			std::string line{};
			str::getline(ss, line, delim);
			if (!no_rollback)
				ss.seekg(ss.tellg() - 1ll);
			return line;
		}
		/**
		 * @brief		Get a string containing a given number of characters from the current stream getter position.
		 * @param count	The number of characters to read ahead. If the stream reaches the end, it will return early.
		 * @returns		std::string
		 */
		[[nodiscard]] virtual std::string getline(const size_t& count)
		{
			std::string line{};
			for (size_t i{ 0 }; hasMore() && i < count; ++i)
				line += ss.get();
			return line;
		}
		/**
		 * @brief				Get a string containing all characters from the current read position to the specified occurrence of the specified lexeme.
		 * @param lex			The target lexeme type.
		 * @param no_rollback	When true, the first delimiter reached will be eaten when returning. (You can still rollback manually.)
		 * @param occurrence	Match after this many occurrences of the specified lexeme type.
		 * @returns				std::string
		 */
		[[nodiscard]] virtual std::string getline(const LexemeT& lex, const bool& no_rollback = true, const size_t& occurrence = 1ull)
		{
			if (occurrence == 0ull)
				throw make_exception("TokenizerBase::getline():  0 is not a valid occurrence count value.");
			std::string line{};
			size_t count{ 0ull };
			while (hasMore()) {
				char c{ getch(true) };
				if (get_lexeme(c) == lex && ++count >= occurrence) {
					if (!no_rollback)
						ss.seekg(ss.tellg() - 1ll);
					break;
				}
				line += c;
			}
			return line;
		}
		/**
		 * @brief				Continue getting characters until the given predicate function returns false.
		 * @tparam PredicateT	Predicate Function Type.
		 * @param pred			A predicate function that accepts a char as input, and returns a boolean. getuntil() ends when this function returns true.
		 * @param no_rollback	When true, the character matched by the predicate will be eaten.
		 * @returns				std::string
		 */
		template<class PredicateT>
		[[nodiscard]] std::string getuntil(const PredicateT& pred, const bool& no_rollback = true)
		{
			std::string s;
			s.reserve(50);
			for (char c{ getch(true) }; hasMore() && good() && !pred(c); c = getch(true))
				s += c;
			s.shrink_to_fit();
			if (!no_rollback)
				ss.seekg(ss.tellg() - 1ll); // rollback to eat predicate match
			return s;
		}
		/**
		 * @brief				Retrieve all of the characters from the current read pos until the first unescaped delimiter
		 * @tparam PredicateT	Predicate Function Type.
		 * @param pred			A predicate function that accepts a char as input, and returns a boolean. getuntil() ends when this function returns true.
		 * @param no_rollback	When true, the character matched by the predicate will be eaten.
		 * @returns				std::string
		 */
		[[nodiscard]] virtual std::string getuntil_unescaped(const char& delim, const bool& no_rollback = true)
		{
			char prev{ '\0' };
			return getuntil([&prev, &delim](auto&& ch) { return prev != '\\' && ch == delim; }, no_rollback);
		}
		/**
		 * @brief			Continue reading ahead until a character with an unspecified type is reached, and return it as a string. The delimiter is not consumed.
		 * @tparam ...Ts	Variadic Lexeme type(s).
		 * @param ...type	At least one lexeme type.
		 * @returns			std::string
		 */
		template<std::same_as<LexemeT>... Ts>
		[[nodiscard]] std::string getsimilar(const Ts&... type)
		{
			std::string str{};
			for (char c{ getch(true) }; var::variadic_or((get_lexeme(c) == type)...) && hasMore(); c = getch(true))
				str += c;
			ss.seekg(ss.tellg() - 1ll); // rollback by 1 to exclude delimiter
			return str;
		}
		/**
		 * @brief			Continue reading ahead until the first character whose type doesn't match at least one of the given lexeme types.
		 * @param ...type	At least one lexeme type.
		 * @returns			std::string
		*/
		template<std::same_as<LexemeT>... Ts>
		[[nodiscard]] std::string getnotsimilar(const Ts&... type)
		{
			std::string str{};
			for (char c{ getch(true) }; var::variadic_and(get_lexeme(c) != type...) && hasMore(); c = getch(true))
				str += c;
			ss.seekg(ss.tellg() - 1ll); // rollback by 1 to exclude delimiter
			return str;
		}

		template<class Pred>
		[[nodiscard]] std::string getsimilar(const Pred& predicate)
		{
			std::string str{};
			for (char c{ getch(true) }; predicate(c) && hasMore(); c = getch(true))
				str += c;
			ss.seekg(ss.tellg() - 1ll);
			return str;
		}

		template<std::same_as<char>... Ts>
		[[nodiscard]] std::string getsimilar_ch(const Ts&... character)
		{
			std::string str{};
			for (char c{ getch(true) }; var::variadic_or((c == character)...) && hasMore(); c = getch(true))
				str += c;
			ss.seekg(ss.tellg() - 1ll);
			return str;
		}

		/**
		 * @brief Clears/Resets the internal stream buffer's _error state flags_.
		 */
		void clear()
		{
			ss.clear();
		}

		/**
		 * @brief Set the getter position to lastPos.
		 * @returns std::streamoff	- The previous stream getter position.
		 */
		virtual std::streamoff rollback()
		{
			clear();
			const auto copyPos{ ss.tellg() };
			ss.seekg(lastPos);
			return copyPos;
		}

		/**
		 * @brief Set the getter position to the beginning of the stream.
		 * @returns std::streamoff	- The previous stream getter position.
		 */
		virtual std::streamoff rollback_reset()
		{
			clear();
			const auto copyPos{ ss.tellg() };
			ss.seekg(std::ios::beg);
			setLastPosHere();
			return copyPos;
		}

		/**
		 * @brief Set the value of lastPos directly.
		 * @param pos				- Value to set as the last stream getter position.
		 * @returns std::streamoff	- The previous stream getter position.
		 */
		virtual std::streamoff setLastPos(const std::streamoff& pos)
		{
			const auto copy{ lastPos };
			lastPos = pos;
			return copy;
		}

		/// @brief Set the last position to the current position.
		virtual std::streamoff setLastPosHere()
		{
			return setLastPos(ss.tellg());
		}

		/**
		 * @brief Variation of the getline(count) function that returns true if the result matches a given comparison string.
		 * @param count	- The number of characters to read ahead.
		 * @param comp	- Comparison string, if the final value of str matches this (case-insensitive), the function will return true.
		 * @param str	- Reference of a string to use as output for getline.
		 * @returns bool
		 */
		[[nodiscard]] virtual bool getline_and_match(const size_t& count, const std::string& comp, std::string& str)
		{
			str = getline(count);
			return str::tolower(str) == comp;
		}

		/**
		 * @brief Pure virtual function that is used to retrieve the next-in-line TokenT from the local stream.
		 *\n	  This function should be overridden for each file format, and should be a self-contained token parser using recursion if necessary.
		 * @returns TokenT
		 */
		[[nodiscard]] virtual TokenT getNext() = 0;

	private:
		[[nodiscard]] std::vector<TokenT> tokenize_internal(const size_t& reserve)
		{
			std::vector<TokenT> tokens;
			tokens.reserve(reserve);
			while (ss) { // tokenize the whole stream
				tokens.emplace_back(std::move(getNext()));
				if (tokens.size() >= tokens.capacity() - 1ull)
					tokens.reserve(tokens.size() + reserve);
			}
			tokens.shrink_to_fit();
			return tokens;
		}

	public:
		/**
		 * @brief				Tokenize the whole local stream and return it as a vector.
		 * @param eof_tkn		An optional token to append to the end of the vector, if it doesn't already exist. If this is left blank, no EOF token is appended.
		 * @param reserve_sz	Expand the vector's capacity by this number of elements each time the capacity limit is reached.
		 * @returns				std::vector<TokenT>
		 */
		[[nodiscard]] std::vector<TokenT> tokenize(const std::optional<TokenT>& eof_tkn = std::nullopt, const size_t& reserve_sz = 64ull)
		{
			auto vec{ tokenize_internal(reserve_sz) };
			if (eof_tkn.has_value() && (vec.empty() || !vec.empty() && vec.back() != eof_tkn))
				vec.emplace_back(eof_tkn.value());
			return vec;
		}
	};

	/**
	 * @class				TokenParserBase
	 * @brief				Base parser object that accepts a vector of tokens, and returns an implementation-defined type.
	 * @tparam OutputType	User-defined Output Type.
	 * @tparam TokenT		TokenBase Object Type.
	 */
	template<class OutputType, typename Token>
	class TokenParserBase {
	public:
		using OutputT = OutputType;
		using TokenT = Token;
	protected:
		std::vector<TokenT> tokens;

		/**
		 * @brief			Constructor.
		 * @param tokens	TokenBase Vector Rvalue Reference
		 */
		TokenParserBase(std::vector<TokenT>&& tokens) : tokens{ std::move(tokens) } {}
		/**
		 * @brief			Constructor.
		 * @param tokens	TokenBase Vector
		 */
		TokenParserBase(const std::vector<TokenT>& tokens) : tokens{ tokens } {}

		/// @brief	Virtual Destructor
		virtual ~TokenParserBase() noexcept = default;

		template<typename... Ts>
		std::vector<TokenT> strip_types(const Ts&... types) const
		{
			std::vector<TokenT> copy{ tokens };
			copy.erase(std::remove_if(copy.begin(), copy.end(), [&](auto&& tkn) { return var::variadic_or(tkn.type == types...); }), copy.end());
			return copy;
		}

	public:
		/**
		 * @brief	Parse the token container.
		 * @returns	OutputType
		 */
		virtual OutputT parse() const = 0;
		virtual operator OutputT() const { return parse(); }
	};
}