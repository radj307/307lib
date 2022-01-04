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
 *			| Token   | Composed of 0 or more characters with a similar lexical type, such as a string, boolean, or number. |  
 *
 * 
 *			## TokenRedux Components  
 *			
 *			| Component / Layer  | Functionality                                                                          |  
 *			|--------------------|----------------------------------------------------------------------------------------|  
 *			| Virtualized Base   | Provides the basic tokenization framework, as well as helper methods.                  |  
 *			| Definition Package | Provides generic Lexeme definitions and Token types that apply to one or more formats. |  
 *			| Format Package     | Provides the Tokenization and parsing implementations specific to one format.          |  
 *	
 *
 *			## Definition Packages  
 * 
 *			A definitions package is usually composed of a LexemeDict override, and 2 enums:  
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

namespace file {
	/**
	 * @struct			LexemeDict
	 * @brief			Used to convert characters to a templated Lexeme object, which represents a single character's lexical "type".
	 *\n				A Lexeme can be thought of as a character's type, and as such multiple characters can be associated with a single Lexeme, but a single Lexeme always represents a single character length.
	 * @tparam LexemeT	Lexeme type. This is defined by a "definitions package".
	 */
	template<typename LexemeT>
	struct LexemeDict {
		/**
		 * @brief		Override this function to select a lexeme type given a single character.
		 * @param ch	Input Character
		 * @returns		LexemeT
		 */
		[[nodiscard]] virtual LexemeT char_to_lexeme(const char& ch) const noexcept = 0;

		/**
		 * @brief		Convert a char to a LexemeT
		 * @param ch	Input Character
		 * @returns		LexemeT
		 */
		[[nodiscard]] LexemeT operator()(const char& ch) const noexcept { return char_to_lexeme(ch); }
	};

	/**
	 * @struct			Token
	 * @brief			Base token object. It represents zero or more characters of data, and is the most basic unit recognized by the parsing step.
	 * @tparam TknType	The Token Type instance to use. This is defined by a "definitions package".
	 */
	template<typename TknType>
	struct Token {
		using Type = TknType;

		/// @brief	This token's type identifier
		Type type;
		/// @brief	Contains this token's string value.
		std::string str;

		[[nodiscard]] WINCONSTEXPR operator Type() const { return type; }
		[[nodiscard]] operator std::string() const { return str; }

		[[nodiscard]] WINCONSTEXPR bool operator==(const Token<Type>& o) const { return type == o.type && str == o.str; }
		[[nodiscard]] WINCONSTEXPR bool operator!=(auto&& o) const { return !operator==(std::forward<decltype(o)>(o)); }

		friend std::ostream& operator<<(std::ostream& os, const Token<Type>& token) { return os << token.str; }

	protected:
		/**
		 * @brief		Token Constructor.
		 * @param type	This token's type.
		 * @param str	This token's string.
		 */
		WINCONSTEXPR Token(const Type& type, const std::string& str = {}) : type{ type }, str{ str } {}
		/**
		 * @brief		Token Constructor.
		 * @param str	This token's string.
		 * @param type	This token's type.
		 */
		WINCONSTEXPR Token(const std::string& str, const Type& type) : type{ type }, str{ str } {}

		/// @brief	Virtual Destructor
		virtual ~Token() noexcept = default;
	};

	/**
	 * @class			Tokenizer
	 * @brief			Base Tokenizer object that accepts a stringstream and lexeme dictionary, then tokenizes the data.
	 *\n				Can be used inline by calling `tokenize()`.
	 * @tparam LexemeT	The Lexeme Type to use. This is defined by a "definitions package".
	 * @tparam TknType	The Token::Type to use. This is defined by a "definitions package".
	 * @tparam TokenT
	 */
	template<typename LexemeT, typename TknType, std::derived_from<Token<TknType>> TokenT>
	class Tokenizer {
	protected:
		std::stringstream ss;
		std::streamoff lastPos{ 0ll };
		/// @brief	Lexeme dictionary used to retrieve the lexeme associated with a given character.
		LexemeDict<LexemeT> get_lexeme;
		LexemeT lexeme_whitespace;

		/**
		 * @brief				Default Constructor.
		 * @param ss			Stringstream rvalue reference containing the target data.
		 * @param whitespace	The lexeme associated with whitespace characters. This is used by the getch() method to skip whitespace.
		 * @param end			The token type associated with the end of file (EOF).
		 */
		Tokenizer(std::stringstream&& ss, LexemeDict<LexemeT> lexemeDictionary, LexemeT whitespace) : ss{ std::move(ss) }, lastPos{ 0ull }, get_lexeme{ lexemeDictionary }, lexeme_whitespace{ whitespace } {}
		
		/// @brief	Virtual Destructor
		virtual ~Tokenizer() noexcept = default;

		/**
		 * @brief String operator, returns the result local stream's str() function.
		 * @returns std::string
		 */
		[[nodiscard]] virtual explicit operator const std::string() const { return ss.str(); }

		/// @brief Stream extraction operator. Inserts the istream's read buffer into the local stream.
		template<class T> requires std::derived_from<T, Tokenizer>
		friend std::istream& operator>>(std::istream& is, T& tokenizer)
		{
			is >> tokenizer.ss.rdbuf();
			return is;
		}
		/// @brief Stream insertion operator. Inserts the local stream's read buffer in the given ostream.
		template<class T> requires std::derived_from<T, Tokenizer>
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
			for (c = ss.get(); (c) == lexeme_whitespace; c = ss.get()) { setLastPosHere(); }
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
		 * @brief Get a string containing everything from the current stream getter position until a given delimiter.
		 * @param delim			- Stop reading ahead when this character is encountered
		 * @param no_rollback	- When true, does not roll back the getter pos to 1 before the delimiter. This causes the delimiter character to be "eaten".
		 * @returns std::string
		 */
		[[nodiscard]] virtual std::string getline(const char& delim = '\n', const bool no_rollback = true)
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
		 * @param eof_tkn	An optional token to append to the end of the vector, if it doesn't already exist.
		 * @returns			std::vector<TokenT>
		 */
		[[nodiscard]] std::vector<TokenT> tokenize(const std::optional<TokenT>& eof_tkn = std::nullopt)
		{
			auto vec{ tokenize_internal() };
			if (eof_tkn.has_value() && (vec.empty() || !vec.empty() && vec.back() != eof_tkn))
				vec.emplace_back(eof_tkn.value());
			return vec;
		}
	};

	/**
	 * @class				TokenParser
	 * @brief				Base parser object that accepts a vector of tokens, and returns an implementation-defined type.
	 * @tparam OutputType	User-defined Output Type.
	 * @tparam TokenT		Token Object Type.
	 */
	template<class OutputType, typename TokenT>
	class TokenParser {
	protected:
		std::vector<TokenT> tokens;

		/**
		 * @brief			Constructor.
		 * @param tokens	Token Vector Rvalue Reference
		 */
		TokenParser(std::vector<TokenT>&& tokens) : tokens{ std::move(tokens) } {}
		/**
		 * @brief			Constructor.
		 * @param tokens	Token Vector
		 */
		TokenParser(const std::vector<TokenT>& tokens) : tokens{ tokens } {}

		/// @brief	Virtual Destructor
		virtual ~TokenParser() noexcept = default;

	public:
		/**
		 * @brief	Parse the token container.
		 * @returns	OutputType
		 */
		virtual OutputType parse() const = 0;
		virtual operator OutputType() const { return parse(); }
	};
}