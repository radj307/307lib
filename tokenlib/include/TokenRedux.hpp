/**
 * @file	TokenRedux.hpp
 * @author	radj307
 * @brief	New tokenization & parsing library providing superior abstraction capabilities and a much less confusing usage experience.  
 *			For more information on tokenization, see: https://en.wikipedia.org/wiki/Lexical_analysis#Tokenization  
 *	
 *			## Terminology  
 * 
 *			| Term    | Description                                                                                         |  
 *			|---------|-----------------------------------------------------------------------------------------------------|  
 *          | Format  | A data format, usually a file format, such as JSON, YAML, INI, etc.                                 |  
 *			| Package | Contains the actual implementations of some portion of the TokenRedux pipeline.                     |  
 *			| Lexeme  | A single character's type, such as a lowercase letter, digit, or semicolon.                         |  
 *			| TokenBase   | Composed of 0 or more characters with a similar lexical type, such as a string, boolean, or number. |  
 *
 * 
 *			## TokenRedux Components  
 *			
 *			| Component / Layer  | Functionality                                                                          |  
 *			|--------------------|----------------------------------------------------------------------------------------|  
 *			| Virtualized Base   | Provides the basic tokenization framework, as well as helper methods.                  |  
 *			| Definition Package | Provides generic Lexeme definitions and TokenBase types that apply to one or more formats. |  
 *			| Format Package     | Provides the Tokenization and parsing implementations specific to one format.          |  
 *	
 *
 *			## Definition Packages  
 * 
 *			A definitions package is usually composed of a LexemeDictBase override, and 2 enums:  
 *			- __Lexeme Type__  
 *			  Defines possible types associated with a single character, such as a digit, quotation mark, or letter.  
 *			- __Token Type__  
 *			  Defines possible types associated with sections of the input data, such as words, numbers, or lines.  
 * 
 *
 *			## Format Packages  
 *
 *			A format package is composed of 2 derived objects, to implement the tokenization & parsing stages of reading a single serialized data format.  
 *			- __Tokenizer__  
 *			  Takes a stringstream as input, and divides it into a vector of Tokens using the provided Definition Package.  
 *			- __Parser__  
 *			  Takes a vector of Tokens as input, and parses it into a usable type, defined by the implementation.  
 */
#pragma once
#include <sysarch.h>
#include <var.hpp>
#include <str.hpp>

#include <sstream>
#include <istream>
#include <ostream>
#include <vector>
#include <string>
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
		[[nodiscard]] LexemeT operator()(const char& ch) const noexcept;
	};

	/**
	 * @struct			TokenBase
	 * @brief			Base token object. It represents zero or more characters of data, and is the most basic unit recognized by the parsing step.
	 * @tparam TknType	The TokenBase Type instance to use. This is defined by a "definitions package".
	 */
	template<typename TknType>
	struct TokenBase {
		using Type = TknType;

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
	};

	/**
	 * @class				TokenizerBase
	 * @brief				Base TokenizerBase object that accepts a stringstream and lexeme dictionary, then tokenizes the data.
	 *\n					Can be used inline by calling `tokenize()`.
	 * @tparam LexemeT		The Lexeme Type to use. This is defined by a "definitions package".
	 * @tparam Dictionary	The Lexeme Dictionary Type to use. This is defined by a "definitions package".
	 * @tparam TknType		The TokenBase::Type to use. This is defined by a "definitions package".
	 * @tparam TokenT		The TokenBase type to use. This is defined by a "definitions package".
	 */
	template<typename LexemeT, std::derived_from<LexemeDictBase<LexemeT>> Dictionary, typename TknType, std::derived_from<TokenBase<TknType>> TokenType = TokenBase<TknType>, typename SettingsType = void>
	class TokenizerBase {
	public:
		using TokenT = TokenType;
	protected:
		std::stringstream ss;
		std::streamoff lastPos{ 0ll };
		/// @brief	Lexeme dictionary used to retrieve the lexeme associated with a given character.
		Dictionary get_lexeme;
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
		 * @brief Get the next character from the stream, and move the getter position forward by one.
		 * @param allow_whitespace	- When true, allows any character to be returned, including whitespace characters.
		 * @returns char
		 */
		[[nodiscard]] char getch(const bool allow_whitespace = false)
		{
			setLastPosHere();
			if (allow_whitespace) // allow whitespace, return next character
				return static_cast<char>(ss.get());
			char c; // don't allow whitespace, read ahead until next non-whitespace character and return that
			for (c = ss.get(); get_lexeme(c) == lexeme_whitespace; c = ss.get()) { setLastPosHere(); }
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
		[[nodiscard]] LexemeT eatnext(const size_t& count = 1ull) noexcept
		{
			for (size_t i{ 0ull }; i < count; ++i) {
				if (ss.good())
					(void)ss.get();
				else return false;
			}
			return hasMore();
		}

		/**
		 * @brief Get a string containing everything from the current stream getter position until a given delimiter.
		 * @param delim			- Stop reading ahead when this character is encountered
		 * @param no_rollback	- When true, the first delimiter reached will be eaten when returning. (You can still rollback manually.)
		 * @returns std::string
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
		 * @brief Get a string containing a given number of characters from the current stream getter position.
		 * @param count	- The number of characters to read ahead. If the stream reaches the end, it will return early.
		 * @returns std::string
		 */
		[[nodiscard]] virtual std::string getline(const size_t& count)
		{
			std::string line{};
			for (size_t i{ 0 }; i < count && hasMore(); ++i)
				line += ss.get();
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
		[[nodiscard]] virtual std::string getuntil(const PredicateT& pred, const bool& no_rollback = true)
		{
			std::string s;
			s.reserve(50);
			for (char c{ ss.get() }; good() && !pred(c); c = ss.get())
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
			char prev{'\0'};
			return getuntil([&prev](auto&& ch) { return prev != '\\' && ch == delim; }, no_rollback);
		}
		/**
		 * @brief Continue reading ahead until a character with an unspecified type is reached, and return it as a string. The delimiter is not consumed.
		 * @tparam ...VT	- Variadic Template. (LEXEME) (n > 0)
		 * @param ...type	- At least one character type.
		 * @returns std::string
		 */
		template<std::same_as<LexemeT>... VT>
		[[nodiscard]] std::string getsimilar(const VT&... type)
		{
			std::string str{};
			for (char c{ getch(true) }; var::variadic_or((get_lexeme(c) == type)...) && hasMore(); c = getch(true))
				str += c;
			ss.seekg(ss.tellg() - 1ll); // rollback by 1 to exclude delimiter
			return str;
		}
		template<std::same_as<LexemeT>... VT>
		[[nodiscard]] std::string getnotsimilar(const VT&... type)
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
			while (ss) // tokenize the whole stream
				tokens.emplace_back(std::move(getNext()));
			tokens.shrink_to_fit();
			return tokens;
		}

	public:
		/**
		 * @brief			Tokenize the whole local stream and return it as a vector.
		 * @param eof_tkn	An optional token to append to the end of the vector, if it doesn't already exist. If this is left blank, no EOF token is appended.
		 * @returns			std::vector<TokenT>
		 */
		[[nodiscard]] std::vector<TokenT> tokenize(const std::optional<TokenT>& eof_tkn = std::nullopt, const size_t& reserve_sz = 256ull)
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
	template<class OutputType, typename TokenType>
	class TokenParserBase {
	protected:
		std::vector<TokenType> tokens;

		/**
		 * @brief			Constructor.
		 * @param tokens	TokenBase Vector Rvalue Reference
		 */
		TokenParserBase(std::vector<TokenType>&& tokens) : tokens{ std::move(tokens) } {}
		/**
		 * @brief			Constructor.
		 * @param tokens	TokenBase Vector
		 */
		TokenParserBase(const std::vector<TokenType>& tokens) : tokens{ tokens } {}

		/// @brief	Virtual Destructor
		virtual ~TokenParserBase() noexcept = default;

		template<typename... Ts>
		std::vector<TokenType> strip_types(const Ts&... types) const
		{
			std::vector<TokenType> copy{tokens};
			copy.erase(std::remove_if(copy.begin(), copy.end(), [&](auto&& tkn) { return var::variadic_or(tkn.type == types...); }), copy.end());
			return copy;
		}

	public:
		using OutputT = OutputType;
		using TokenT = TokenType;

		/**
		 * @brief	Parse the token container.
		 * @returns	OutputType
		 */
		virtual OutputT parse() const = 0;
		virtual operator OutputT() const { return parse(); }
	};
}