/**
 * @file	TokenRedux.hpp
 * @author	radj307
 * @brief	New tokenization & parsing library providing superior abstraction capabilities and a much less confusing usage experience.
 *			For more information on tokenization, see: https://en.wikipedia.org/wiki/Lexical_analysis#Tokenization
 */
/**
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
	 * @struct				TokenBase
	 * @brief				Base token object. It represents zero or more characters of data, and is the most basic unit recognized by the parsing step.
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
	template<typename TokenType>
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
		/// @brief	This is set to the type specified by the Token template parameter.
		using TokenT = Token;
	protected:
		/// @brief	The stringstream containing the untokenized data.
		std::stringstream ss;
		/// @brief	The stream reader position prior to the most recent call to a base reader method such as `getch`, `getsimilar`, `getUntil`, etc.
		std::streamoff lastPos{ 0ll };
		/// @brief	Lexeme dictionary used to retrieve the lexeme associated with a given character.
		Dictionary get_lexeme;
		/// @brief	Lexeme associated with skippable whitespace.
		LexemeT lexeme_whitespace, lexeme_eof;

	public:
		/**
		 * @brief				Default Constructor.
		 * @param ss			Stringstream rvalue reference containing the target data.
		 * @param whitespace	The lexeme associated with whitespace characters. This is used by the getch() method to skip whitespace.
		 */
		TokenizerBase(std::stringstream&& ss, LexemeT const& whitespace, LexemeT const& eof) : ss{ std::move(ss) }, lastPos{ 0ull }, lexeme_whitespace{ whitespace }, lexeme_eof{ eof } {}
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
		 * @brief			Set the underlying stream's format flags.
		 * @param format	The format flags to apply to the stream.
		 * @returns			std::ios_base::fmtflags	: The previous format control flags.
		 */
		auto setf(std::ios_base::fmtflags const& format)
		{
			return ss.setf(format);
		}
		auto unsetf(std::ios_base::fmtflags const& mask)
		{
			ss.unsetf(mask);
		}

		[[nodiscard]] virtual std::string str() const
		{
			return ss.str();
		}

		/**
		 * @protected
		 * @brief Check if there are more valid characters ahead of the current stream getter position. Use this when looping until the end of stream.
		 * @returns bool
		 */
		[[nodiscard]] virtual bool hasMore()
		{
			if (ss)
				return getCurrentPos() != static_cast<size_t>(EOF);
			return false;
		}

		/**
		 * @protected
		 * @brief					Get the next character from the stream, and move the getter position forward by one.
		 *\n						This function sets lastPos to the current position before incrementing the getter.
		 * @param allow_whitespace	When true, allows any character to be returned, including whitespace characters.
		 * @returns					char
		 */
		[[nodiscard]] virtual char getch(const bool allow_whitespace = false)
		{
			setLastPosHere();
			char c{ EOF };
			ss.get(c);
			if (allow_whitespace || get_lexeme(c) != lexeme_whitespace)
				return c;
			for (; get_lexeme(c) == lexeme_whitespace; ss.get(c))
				setLastPosHere();
			return c;
		}

		/**
		 * @protected
		 * @brief	Retrieve the next character from the stream, but don't advance the read pointer.
		 * @returns char
		 */
		[[nodiscard]] virtual char peek()
		{
			if (!hasMore())
				return '\0';
			return static_cast<char>(ss.peek());
		}

		/**
		 * @protected
		 * @brief			Retrieve the next character from the stream, but don't advance the read pointer.
		 * @throws except	There are no more tokens to peek at in the buffer.
		 * @returns			LexemeT
		 */
		[[nodiscard]] virtual LexemeT peeklex() noexcept(false)
		{
			if (!hasMore())
				throw make_exception("peeklex() failed:  There are no remaining tokens!");
			return get_lexeme(static_cast<char>(ss.peek()));
		}

		/**
		 * @protected
		 * @brief					Retrieve the next character from the stream, but don't advance the read pointer.
		 * @param noTokensDefault	If there are no more tokens to peek at in the buffer, return this value instead of throwing like the parameterless variant of peeklex().
		 * @returns					LexemeT
		 */
		[[nodiscard]] virtual LexemeT peeklex(const LexemeT& noTokensDefault) noexcept
		{
			if (!hasMore())
				return noTokensDefault;
			return get_lexeme(static_cast<char>(ss.peek()));
		}

		/**
		 * @protected
		 * @brief			Eat any number of characters by advancing the read position by the given number of characters.
		 * @param count		The number of characters to "eat".
		 * @returns			bool
		 *\n		true	There are more upcoming characters.
		 *\n		false	Reached EOF while eating characters.
		 */
		virtual bool eat(const size_t& count = 1ull) noexcept
		{
			char* c{ nullptr };
			if (ss.good())
				ss.get(c, count);
			return hasMore();
		}

		/**
		 * @protected
		 * @brief			Eat the next character by advancing the read position by one.
		 * @returns			bool
		 *\n		true	There are more upcoming characters.
		 *\n		false	Reached EOF while eating characters.
		 */
		virtual bool eatnext() noexcept { return eat(1ull); }

		/**
		 * @protected
		 * @brief				Recurse into the given string using a copy of this tokenizer instance.
		 * @tparam Tokenizer	Tokenizer type.
		 * @param str			Input String
		 * @param eof_tkn		EOF token type.
		 * @param reserve_sz	Number of elements to reserve at a time. This is passed to the tokenize() function.
		 * @returns				std::vector<TokenT>
		 */
		template<class Tokenizer = TokenizerBase<LexemeT, Dictionary, TokenType, Token>>
		[[nodiscard]] std::vector<TokenT> recurseInto(const std::string& str, const std::optional<TokenT>& eof_tkn = std::nullopt, const size_t& reserve_sz = 256ull)
		{
			return Tokenizer(std::move(std::stringstream{ str }), lexeme_whitespace).tokenize(eof_tkn, reserve_sz);
		}

		/**
		 * @protected
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
		 * @protected
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
		 * @protected
		 * @brief		Get a string containing a given number of characters from the current stream getter position.
		 * @param count	The number of characters to read ahead. If the stream reaches the end, it will return early.
		 * @returns		std::string
		 */
		[[nodiscard]] virtual std::string getline(const size_t& count)
		{
			std::string line{};
			char c;
			for (size_t i{ 0 }; hasMore() && i < count; ++i) {
				ss.get(c);
				line += c;
			}
			return line;
		}
		/**
		 * @protected
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
		 * @protected
		 * @brief		Variation of the getline(count) function that returns true if the result matches a given comparison string.
		 * @param count	The number of characters to read ahead.
		 * @param comp	Comparison string, if the final value of str matches this (case-insensitive), the function will return true.
		 * @param str	Reference of a string to use as output for getline.
		 * @returns		bool
		 */
		[[nodiscard]] virtual bool getmatch(const size_t& count, const std::string& comp, std::string& str)
		{
			str = getline(count);
			return str::tolower(str) == comp;
		}

		/**
		 * @protected
		 * @deprecated	This function uses an old naming convention that will be removed in a future release. Use the getmatch() method instead.
		 *
		 * @brief		Variation of the getline(count) function that returns true if the result matches a given comparison string.
		 * @param count	The number of characters to read ahead.
		 * @param comp	Comparison string, if the final value of str matches this (case-insensitive), the function will return true.
		 * @param str	Reference of a string to use as output for getline.
		 * @returns		bool
		 */
		[[nodiscard]] virtual bool getline_and_match(const size_t& count, const std::string& comp, std::string& str)
		{
			return getmatch(count, comp, str);
		}

		/**
		 * @protected
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
		 * @protected
		 * @brief				Retrieve all of the characters from the current read pos until the first unescaped delimiter
		 * @tparam PredicateT	Predicate Function Type.
		 * @param pred			A predicate function that accepts a char as input, and returns a boolean. getuntil() ends when this function returns true.
		 * @param no_rollback	When true, the character matched by the predicate will be eaten.
		 * @returns				std::string
		 */
		[[nodiscard]] virtual std::string getuntil_unescaped(const char& delim, const bool& no_rollback = true)
		{
			char prev{ EOF };
			return getuntil([&prev, &delim](auto&& ch) { return prev != '\\' && ch == delim; }, no_rollback);
		}

		/**
		 * @protected
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
		 * @protected
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

		/**
		 * @protected
		 * @brief				Continue reading ahead until the first character that isn't present in the given list of characters.
		 * @param ...character	Any number of characters to match.
		 * @returns				std::string
		 */
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
		 * @protected
		 * @brief				Continue reading ahead until the given predicate returns false.
		 * @tparam Pred			Predicate Function Type. Must accept a char and return a boolean / implicit boolean.
		 * @param predicate		Predicate Function.
		 * @returns				std::string
		 */
		template<class Pred>
		[[nodiscard]] std::string get_if(const Pred& predicate)
		{
			std::string str{};
			for (char c{ getch(true) }; predicate(c) && hasMore(); c = getch(true))
				str += c;
			ss.seekg(ss.tellg() - 1ll);
			return str;
		}

		/**
		 * @protected
		 * @brief Clears/Resets the internal stream buffer's _error state flags_.
		 */
		void clear()
		{
			ss.clear();
		}

		/**
		 * @protected
		 * @brief	Set the getter position to lastPos.
		 * @returns std::streamoff	The previous stream getter position.
		 */
		virtual std::streamoff rollback()
		{
			clear();
			const auto copyPos{ ss.tellg() };
			ss.seekg(lastPos);
			return copyPos;
		}

		/**
		 * @protected
		 * @brief	Set the getter position to the beginning of the stream.
		 * @returns std::streamoff	The previous stream getter position.
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
		 * @protected
		 * @brief	Retrieve the current stream read position. (character index)
		 * @returns	std::streamoff : Current Read Position.
		 */
		virtual std::streampos getCurrentPos()
		{
			return ss.tellg();
		}

		/**
		 * @protected
		 * @brief	Get the total number of characters in the stream.
		 * @returns	std::streamsize
		 */
		virtual std::streamsize getStreamSize()
		{
			const auto& currentPos{ getCurrentPos() };
			ss.seekp(std::ios::end);
			ss.seekg(std::ios::beg);
			const auto& begPos{ ss.tellg() };
			ss.seekg(currentPos);
			return ss.tellp() - begPos;
		}

		/**
		 * @protected
		 * @brief	Get the remaining number of characters in the stream.
		 * @returns	std::streamsize
		 */
		virtual std::streamsize getRemainingSize()
		{
			return getStreamSize() - getCurrentPos();
		}

		/**
		 * @protected
		 * @brief		Set (lastPos) to a specific read position.
		 * @param pos	Input Read Position
		 * @returns		std::streamoff
		 *				This is the previous value of (lastPos).
		 */
		virtual std::streamoff setLastPos(const std::streamoff& pos)
		{
			const auto copy{ lastPos };
			lastPos = pos;
			return copy;
		}

		/**
		 * @protected
		 * @brief	Set (lastPos) to the current read position.
		 * @returns	std::streamoff
		 */
		virtual std::streamoff setLastPosHere()
		{
			return setLastPos(getCurrentPos());
		}

		/**
		 * @protected
		 * @brief		Retrieve the next token from the stream.
		 *\n			By default, this method always returns (TokenT::NullToken). This is to support overriding either this function, OR the getNext() function.
		 *\n			This function should be overridden for each file format, and should be a self-contained token parser using recursion if necessary.
		 * @param ch	Automatically called by the BASE getNext() function. Depending on the getNext(bool) overload, this may or may not be whitespace.
		 * @returns		TokenT
		 */
		[[nodiscard]] virtual TokenT getNextToken(const char&)
		{
			return TokenT::NullToken;
		}

		/**
		 * @protected
		 * @brief					Pure virtual function that is used to retrieve the next-in-line TokenT from the local stream.
		 *\n						This function should be overridden for each file format, and should be a self-contained token parser using recursion if necessary.
		 *\n						By default, this function calls `return getNextToken(getch(allowWhitespace));`.
		 * @param allowWhitespace	When true, whitespace may be passed to the getNext(char) overload.
		 * @returns					TokenT
		 */
		[[nodiscard]] virtual TokenT getNext(const bool& allowWhitespace)
		{
			return getNextToken(getch(allowWhitespace));
		}

		/**
		 * @protected
		 * @deprecated Directly overloading this function is deprecated, but still supported. It is recommended to override the getNextToken(const char&) method instead.
		 *
		 * @overload
		 * @brief	Pure virtual function that is used to retrieve the next-in-line TokenT from the local stream.
		 *\n		This is implemented for backwards-compatibility, and is simply an alias for calling ` getNext(false); `.
		 *\n		For improved getNext() alternatives, see the getNextChar(bool) & getNextToken(char) overloads.
		 *\n		This function should be overridden for each file format, and should be a self-contained token parser using recursion if necessary.
		 * @returns	TokenT
		 */
		[[nodiscard]] virtual TokenT getNext()
		{
			return getNext(false);
		}

	private:
		/**
		 * @private
		 * @brief			Tokenizes the entire data stream.
		 *\n				This function is called by tokenize().
		 * @param reserve	The number of vector elements to reserve each time a reallocation is required.
		 *\n				Increasing this may improve performance by preventing reallocations each time a new element is inserted.
		 * @returns			std::vector<TokenT>
		 */
		[[nodiscard]] std::vector<TokenT> tokenize_internal(const size_t& reserve)
		{
			std::vector<TokenT> tokens;
			tokens.reserve(reserve);
			while (ss) { // tokenize the whole stream
				tokens.emplace_back(std::move(getNext()));
				// allocate more memory if we're approaching the capacity limit:
				if (tokens.size() >= (tokens.capacity() > 2ull ? tokens.capacity() - 2ull : 0ull))
					tokens.reserve(tokens.capacity() + reserve);
			}
			tokens.shrink_to_fit();
			return tokens;
		}

	public:
		/**
		 * @public
		 * @brief				Tokenize the whole local stream and return it as a vector.
		 * @param eof_tkn		An optional token to append to the end of the vector, if it doesn't already exist. If this is left blank, no EOF token is appended.
		 * @param reserve_sz	Expand the vector's capacity by this number of elements each time the capacity limit is reached.
		 * @returns				std::vector<TokenT>
		 */
		[[nodiscard]] std::vector<TokenT> tokenize(const std::optional<TokenT>& eof_tkn = std::nullopt, const size_t& reserve_sz = 64ull)
		{
			std::vector<TokenT> vec{ tokenize_internal(reserve_sz) };
			if (eof_tkn.has_value() && (vec.empty() || (!vec.empty() && vec.back().type != eof_tkn.value().type)))
				vec.emplace_back(eof_tkn.value());
			return vec;
		}
	};

	/**
	 * @namespace	gen1
	 * @brief		Contains the old "TokenParserBase" object for backwards-compatibility, and/or simplicity.
	 */
	namespace gen1 {
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

	/**
	 * @namespace	gen2
	 * @brief		Contains upgraded parsers with more features than the gen 1 "TokenParserBase" object.
	 */
	namespace gen2 {

		/**
		 * @class				BasicCoreParserBase
		 * @brief				Base object for all Gen.2 parsers. Does not specify an output type, allowing for implementation-defined flexibility.
		 * @tparam TokenType	The TokenType that Tokens use.
		 * @tparam Token		The type of Token object to use.
		 */
		template<typename TokenType, std::derived_from<TokenBase<TokenType>> Token = TokenBase<TokenType>>
		class BasicCoreParserBase {
		public:
			/// @brief	The internal TokenType enum type used by Tokens.
			using TokenTypeT = TokenType;
			/// @brief	The type of Token object being used by the Tokenizer & Parser
			using TokenT = Token;
			/// @brief	A std::vector of TokenT instances.
			using TokenCont = std::vector<TokenT>;
			/// @brief	Mutable TokenCont iterator type.
			using iterator = TokenCont::iterator;
			/// @brief	Immutable TokenCont iterator type.
			using const_iterator = TokenCont::const_iterator;

		protected:
			TokenCont tokens;

			/**
			 * @brief		Parser Move Constructor.
			 * @param tkns	A token container of type std::vector<Token> to move into the parser.
			 */
			BasicCoreParserBase(TokenCont&& tkns) : tokens{ std::move(tkns) } {}
			/**
			 * @brief		Parser Copy Constructor.
			 * @param tkns	A token container of type std::vector<Token> to copy into the parser.
			 */
			BasicCoreParserBase(const TokenCont& tkns) : tokens{ tkns } {}
			virtual ~BasicCoreParserBase() noexcept = default;

			/// @brief	Retrieve an iterator for the start of the token container.
			const_iterator begin() const { return tokens.begin(); }
			/// @brief	Retrieve an iterator for the end of the token container.
			const_iterator end() const { return tokens.end(); }

		public:
			/**
			 * @brief			Copy & remove all Tokens with types that match any of the specified TokenTypes.
			 * @tparam Ts...	Variadic input that accepts at least one TokenType.
			 * @param ...types	One of more TokenTypes to remove.
			 * @returns			TokenCont
			 *\n				This contains all of the removed tokens, in the order that they were removed. (forward iterated)
			 */
			template<std::same_as<TokenTypeT>... Ts> requires var::at_least_one<Ts...>
			TokenCont strip_tokens(Ts&&... types)
			{
				TokenCont vec;
				vec.reserve(tokens.size());

				tokens.erase(std::remove_if(tokens.begin(), tokens.end(), [&vec, &types...](auto&& tkn) {
					if (var::variadic_or(tkn.type == types...)) {
						vec.emplace_back(tkn);
						return true;
					}
					return false;
				}), tokens.end());

				vec.shrink_to_fit();
				return vec;
			}

			/**
			 * @brief			Copy all Tokens with types that match any of the specified types to a vector and return it.
			 * @tparam Ts...	Variadic input that accepts at least one TokenType.
			 * @param ...types	At least one TokenType to search for.
			 * @returns			TokenCont
			 */
			template<std::same_as<TokenTypeT>... Ts> requires var::at_least_one<Ts...>
			[[nodiscard]] TokenCont get_tokens(Ts&&... types) const
			{
				TokenCont vec;
				vec.reserve(tokens.size());

				for (const TokenT& tkn : tokens)
					if (var::variadic_or(tkn.type == types...))
						vec.emplace_back(tkn);

				vec.shrink_to_fit();
				return vec;
			}
		};

		/**
		 * @class				IteratingCoreParserBase
		 * @brief				Token Parser with an integrated read pointer, and helper functions similar to TokenizerBase.
		 * @tparam TokenType	The TokenType that Tokens use.
		 * @tparam Token		The type of Token object to use.
		 */
		template<typename TokenType, std::derived_from<TokenBase<TokenType>> Token = TokenBase<TokenType>>
		class IteratingCoreParserBase : public BasicCoreParserBase<TokenType, Token> {
		public:
			/// @brief	The internal TokenType enum type used by Tokens.
			using TokenTypeT = TokenType;
			using const_iterator = BasicCoreParserBase<TokenType, Token>::const_iterator;
			using TokenCont = BasicCoreParserBase<TokenType, Token>::TokenCont;

		protected:
			const_iterator readpos;

			IteratingCoreParserBase(TokenCont&& tkns) : BasicCoreParserBase<TokenType, Token>(std::forward<TokenCont>(tkns)), readpos{ this->tokens.begin() } {}
			IteratingCoreParserBase(const TokenCont& tkns) : BasicCoreParserBase<TokenType, Token>(tkns), readpos{ this->tokens.begin() } {}
			virtual ~IteratingCoreParserBase() noexcept = default;

			/// @brief	Check if the read position hasn't reached the end of the container.
			bool hasMore() const { return readpos < this->end(); }

			/**
			 * @brief		Advance the read position by the specified number of tokens.
			 * @param n		Number of tokens to move the read position by.
			 * @returns		const_iterator
			 */
			const_iterator getNext(const size_t& n = 1ull) noexcept
			{
				const auto& endit{ this->end() };
				if (std::distance(readpos, endit) > n)
					return readpos += n;
				return endit;
			}

			/**
			 * @brief			Keep advancing the read position until the first Token whose type matches one of the given TokenTypes.
			 * @param ...types	At least one TokenType to match.
			 * @returns			const_iterator
			 */
			template<std::same_as<TokenTypeT>... Ts> requires (var::at_least_one<Ts...>)
				const_iterator getNext(Ts&&... types) noexcept
			{
				const auto& endit{ this->end() };
				for (; readpos < endit; ++readpos)
					if (var::variadic_or(readpos->type == std::forward<Ts>(types)...))
						return readpos;
				return endit;
			}

			/**
			 * @brief		Move the read position back by the given number of tokens.
			 * @param n		Number of tokens to move the read position by.
			 * @returns		const_iterator
			 */
			const_iterator getLast(const size_t& n = 1ull) const noexcept
			{
				const auto& beg{ this->begin() };
				if (std::distance(beg, readpos) >= n)
					return readpos -= n;
				return beg;
			}

			/**
			 * @brief			Keep moving the read position back until the first Token whose type matches one of the given TokenTypes.
			 * @param ...types	At least one TokenType to match.
			 * @returns			const_iterator
			 */
			template<std::same_as<TokenTypeT>... Ts> requires var::at_least_one<Ts...>
			const_iterator getLast(Ts&&... types) noexcept
			{
				const auto& beg{ this->begin() };
				for (; readpos != beg; --readpos)
					if (var::variadic_or(readpos->type == std::forward<Ts>(types)...))
						return readpos;
				return beg;
			}


			/**
			 * @brief		Advance the read position by the specified number of tokens.
			 * @param n		Number of tokens to move the read position by.
			 * @returns		const_iterator
			 */
			const_iterator peekNext(const size_t& n = 1ull) const noexcept
			{
				const auto& endit{ this->end() };
				if (std::distance(readpos, endit) > n)
					return readpos + n;
				return endit - 1;
			}

			/**
			 * @brief			Keep advancing the read position until the first Token whose type matches one of the given TokenTypes.
			 * @param ...types	At least one TokenType to match.
			 * @returns			const_iterator
			 */
			template<std::same_as<TokenTypeT>... Ts> requires var::at_least_one<Ts...>
			const_iterator peekNext(Ts&&... types) const noexcept
			{
				const auto& endit{ this->end() };
				for (auto it{ readpos }; it < endit; ++it)
					if (var::variadic_or(it->type == std::forward<Ts>(types)...))
						return it;
				return endit - 1;
			}

			/**
			 * @brief		Move the read position back by the given number of tokens.
			 * @param n		Number of tokens to move the read position by.
			 * @returns		const_iterator
			 */
			const_iterator peekLast(const size_t& n = 1ull) const noexcept
			{
				const auto& beg{ this->begin() };
				if (std::distance(beg, readpos) >= n)
					return readpos - n;
				return beg;
			}

			/**
			 * @brief			Keep moving the read position back until the first Token whose type matches one of the given TokenTypes.
			 * @param ...types	At least one TokenType to match.
			 * @returns			const_iterator
			 */
			template<std::same_as<TokenTypeT>... Ts> requires var::at_least_one<Ts...>
			const_iterator peekLast(Ts&&... types) const noexcept
			{
				const auto& beg{ this->begin() };
				for (auto it{ readpos }; it != beg; --it)
					if (var::variadic_or(it->type == std::forward<Ts>(types)...))
						return it;
				return beg;
			}
		};

		template<typename Output, typename TokenType, std::derived_from<TokenBase<TokenType>> Token = TokenBase<TokenType>, template<typename, typename> class ParserBaseT = IteratingCoreParserBase>
		class ParserBase : public ParserBaseT<TokenType, Token> {
		public:
			/// @brief	The internal TokenType enum type used by Tokens.
			using TokenTypeT = TokenType;
			/// @brief	The type of Token object being used by the Tokenizer & Parser
			using TokenT = Token;
			/// @brief	A std::vector of TokenT instances.
			using TokenCont = std::vector<TokenT>;
			/// @brief	Mutable TokenCont iterator type.
			using iterator = TokenCont::iterator;
			/// @brief	Immutable TokenCont iterator type.
			using const_iterator = TokenCont::const_iterator;

			using OutputT = Output;

			ParserBase(TokenCont&& tkns) : ParserBaseT<TokenType, Token>(std::move(tkns)) {}
			ParserBase(const TokenCont& tkns) : ParserBaseT<TokenType, Token>(tkns) {}
			virtual ~ParserBase() noexcept = default;

			virtual OutputT parse() = 0;
		};

		template<typename Output, typename TokenType, std::derived_from<TokenBase<TokenType>> Token = TokenBase<TokenType>>
		using BasicParserBase = ParserBase<Output, TokenType, Token, BasicCoreParserBase>;
		template<typename Output, typename TokenType, std::derived_from<TokenBase<TokenType>> Token = TokenBase<TokenType>>
		using IteratingParserBase = ParserBase<Output, TokenType, Token, IteratingCoreParserBase>;
	}

	using gen1::TokenParserBase;
	using gen2::BasicParserBase;
	using gen2::IteratingParserBase;
}