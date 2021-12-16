/**
 * @file TokenizerBase.hpp
 * @author radj307
 * @brief Contains the TokenizerBase class; a generic virtual tokenizer.
 *\n	## Description #
 *\n	The tokenization process is split into 2 distinct phases:
 *\n		- __Scanner Stage__ _char -> LEXEME_
 *\n		- __Actual Tokenization__ _LEXEME -> TokenType_
 *\n	The TokenizerBase class should be specialized by inheritence.
 *\n	Derived objects should override the virtual __getNext__ function that handles the __Actual Tokenization__ _LEXEME -> TokenType_ phase.
 *\n	Other functions may also be overridden, but most file formats shouldn't require it.
 *\n	See the TokenizerINI class for an example.
 *\n
 *\n	## Standards & Requirements #
 *\n	- All token containers must have a token with _TokenType::END_ at the end, to represent EOF.
 *\n		_This is handled by the __tokenize__ function, which appends a blank token with TokenType::END._
 *\n	- The virtual __getNext__ function must be self-contained, and not require the end user to call other functions.
 *\n		_Excluding the __hasMore__ function,_
 *\n	- The virtual __getNext__ function may use recursion.
 *\n	- The virtual __getNext__ function must return a fully-constructed __Token__ object, and contain the string used to detect it.
 *\n	- It is recommended that the tokenization process does not verify whether a file is correctly formatted, only that tokens are correctly formatted: For example, quotation marks & brackets are closed properly. The format verification process should occur during the parsing process.
 *\n	- Derived objects _should_ be members of the token namespace, or a sub-namespace.
 */
#pragma once
#include <token/LEXEME.h>	// LEXEME, TokenType, & Token
#include <token/Token.hpp>	// TokenType, & Token

#include <string>		// std::string
#include <sstream>		// std::stringstream

namespace token {
	template<typename TokenT>
	struct TokenizerBaseFunctions {
	protected:
		std::stringstream ss;
		std::streamoff lastPos{ 0ll };

		TokenizerBaseFunctions() = default;
		TokenizerBaseFunctions(std::stringstream ss, std::streamoff lastPos) : ss{ std::move(ss) }, lastPos{ lastPos } {}
		virtual ~TokenizerBaseFunctions() = default;

		/**
		 * @brief String operator, returns the result local stream's str() function.
		 * @returns std::string
		 */
		[[nodiscard]] virtual explicit operator const std::string() const { return ss.str(); }

		/// @brief Stream extraction operator. Inserts the istream's read buffer into the local stream.
		template<class T> requires std::derived_from<T, TokenizerBaseFunctions>
		friend std::istream& operator>>(std::istream& is, T& tokenizer)
		{
			is >> tokenizer.ss.rdbuf();
			return is;
		}
		/// @brief Stream insertion operator. Inserts the local stream's read buffer in the given ostream.
		template<class T> requires std::derived_from<T, TokenizerBaseFunctions>
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
			for (c = ss.get(); get_lexeme(c) == LEXEME::WHITESPACE; c = ss.get()) { setLastPosHere(); }
			return c;
		}

		[[nodiscard]] auto peek()
		{
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
			if (no_rollback)
				return line;
			ss.seekg(ss.tellg() - 1ll);
			return line.substr(0u, line.size() - 1ull);
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
		template<std::same_as<LEXEME>... VT>
		[[nodiscard]] std::string getsimilar(const VT&... type)
		{
			std::string str{};
			for (char c{ getch(true) }; var::variadic_or((get_lexeme(c) == type)...) && hasMore(); c = getch(true))
				str += c;
			ss.seekg(ss.tellg() - 1ll); // rollback by 1 to exclude delimiter
			return str;
		}
		template<std::same_as<LEXEME>... VT>
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

		template<std::same_as<char>... vT>
		[[nodiscard]] std::string getsimilar_ch(const vT&... character)
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

		[[nodiscard]] std::vector<TokenT> tokenize_internal(const size_t& reserve)
		{
			std::vector<TokenT> tokens;
			tokens.reserve(reserve);
			while (ss)
				tokens.emplace_back(std::move(getNext()));
			if (!tokens.back().is_eof()) // append an EOF token if there isn't one already.
				tokens.emplace_back(TokenT{ TokenType::END });
			tokens.shrink_to_fit();
			return tokens;
		}

	public:
		using AccessibleTokenCont = std::vector<AccessibleToken>;

		/**
		 * @brief Tokenize the whole local stream and return it as a vector.
		 * @tparam RT			- Return Type _(AccessibleTokenCont)_
		 * @param reserve_size	- Number of elements to reserve before vector automatically reallocates for each insertion.
		 * @returns AccessibleTokenCont
		 */
		template<class RT = AccessibleTokenCont> requires std::is_same_v<RT, AccessibleTokenCont>
		[[nodiscard]] AccessibleTokenCont tokenize(const size_t& reserve_size = 0ull)
		{
			auto tokens{ tokenize_internal(reserve_size) };
			AccessibleTokenCont vec;
			vec.reserve(tokens.size());
			for (auto& tkn : tokens)
				vec.emplace_back(std::move(tkn.operator token::AccessibleToken()));
			vec.shrink_to_fit();
			return vec;
		}
		/**
		 * @brief Tokenize the whole local stream and return it as a vector.
		 * @tparam RT			- Return Type _(std::vector<TokenT>)_
		 * @param reserve_size	- Number of elements to reserve before vector automatically reallocates for each insertion.
		 * @returns AccessibleTokenCont
		 */
		template<class RT> requires std::is_same_v<RT, std::vector<TokenT>>
		[[nodiscard]] std::vector<TokenT> tokenize(const size_t& reserve_size = 0ull)
		{
			return tokenize_internal(reserve_size);
		}
	};

	class TokenizerBase : public TokenizerBaseFunctions<Token> {
	public:
		explicit TokenizerBase() = default;
		explicit TokenizerBase(std::stringstream&& buffer) : TokenizerBaseFunctions(std::move(buffer), 0ull) {}
		virtual ~TokenizerBase() = default;
	};

	/**
	 * @struct TokenizerBase
	 * @brief Base class for Tokenizer derivatives. Contains all of the virtual methods needed to interact with a token stream.
	 */
	 //class TokenizerBase {
	 //protected:
	 //	std::stringstream ss;
	 //	std::streamoff lastPos{ 0ll };

	 //	explicit TokenizerBase() = default;
	 //	explicit TokenizerBase(std::stringstream&& buffer) : ss{ std::move(buffer) }, lastPos{ ss.tellg() } {}
	 //	virtual ~TokenizerBase() = default;

	 //	/**
	 //	 * @brief String operator, returns the result local stream's str() function.
	 //	 * @returns std::string
	 //	 */
	 //	[[nodiscard]] virtual explicit operator const std::string() const { return ss.str(); }

	 //	/// @brief Stream extraction operator. Inserts the istream's read buffer into the local stream.
	 //	template<class T> requires std::derived_from<T, TokenizerBase>
	 //	friend std::istream& operator>>(std::istream& is, T& tokenizer)
	 //	{
	 //		is >> tokenizer.ss.rdbuf();
	 //		return is;
	 //	}
	 //	/// @brief Stream insertion operator. Inserts the local stream's read buffer in the given ostream.
	 //	template<class T> requires std::derived_from<T, TokenizerBase>
	 //	friend std::ostream& operator<<(std::ostream& os, const T& tokenizer)
	 //	{
	 //		os << tokenizer.ss.rdbuf();
	 //		return os;
	 //	}

	 //	/// @brief Retrieve the current goodbit state of the local stream
	 //	[[nodiscard]] bool good() const { return ss.good(); }
	 //	/// @brief Retrieve the current badbit state of the local stream
	 //	[[nodiscard]] bool bad() const { return ss.bad(); }
	 //	/// @brief Retrieve the current failbit state of the local stream.
	 //	[[nodiscard]] bool fail() const { return ss.fail(); }
	 //	/// @brief Retrieve the current eofbit state of the local stream. Use the hasMore() function to check whether the stream is at the end or not.
	 //	[[nodiscard]] bool eof() const { return ss.eof(); }

	 //	/**
	 //	 * @brief Check if there are more valid characters ahead of the current stream getter position. Use this when looping until the end of stream.
	 //	 * @returns bool
	 //	 */
	 //	[[nodiscard]] virtual bool hasMore() const
	 //	{
	 //		if (ss)
	 //			return true;
	 //		return false;
	 //	}

	 //	/**
	 //	 * @brief Get the next character from the stream, and move the getter position forward by one.
	 //	 * @param allow_whitespace	- When true, allows any character to be returned, including whitespace characters.
	 //	 * @returns char
	 //	 */
	 //	[[nodiscard]] char getch(const bool allow_whitespace = false)
	 //	{
	 //		setLastPosHere();
	 //		if (allow_whitespace) // allow whitespace, return next character
	 //			return static_cast<char>(ss.get());
	 //		char c; // don't allow whitespace, read ahead until next non-whitespace character and return that
	 //		for (c = ss.get(); get_lexeme(c) == LEXEME::WHITESPACE; c = ss.get()) { setLastPosHere(); }
	 //		return c;
	 //	}

	 //	/**
	 //	 * @brief Get a string containing everything from the current stream getter position until a given delimiter.
	 //	 * @param delim			- Stop reading ahead when this character is encountered
	 //	 * @param no_rollback	- When true, does not roll back the getter pos to 1 before the delimiter. This causes the delimiter character to be "eaten".
	 //	 * @returns std::string
	 //	 */
	 //	[[nodiscard]] virtual std::string getline(const char& delim = '\n', const bool no_rollback = true)
	 //	{
	 //		std::string line{};
	 //		str::getline(ss, line, delim);
	 //		if (no_rollback)
	 //			return line;
	 //		ss.seekg(ss.tellg() - 1ll);
	 //		return line.substr(0u, line.size() - 1ull);
	 //	}
	 //	/**
	 //	 * @brief Get a string containing a given number of characters from the current stream getter position.
	 //	 * @param count	- The number of characters to read ahead. If the stream reaches the end, it will return early.
	 //	 * @returns std::string
	 //	 */
	 //	[[nodiscard]] virtual std::string getline(const size_t& count)
	 //	{
	 //		std::string line{};
	 //		for (size_t i{ 0 }; i < count && hasMore(); ++i)
	 //			line += ss.get();
	 //		return line;
	 //	}
	 //	/**
	 //	 * @brief Continue reading ahead until a character with an unspecified type is reached, and return it as a string.
	 //	 * @tparam ...VT	- Variadic Template. (LEXEME) (n > 0)
	 //	 * @param ...type	- At least one character type.
	 //	 * @returns std::string
	 //	 */
	 //	template<class... VT> requires std::conjunction_v<std::is_same<VT, LEXEME>...> && (sizeof...(VT) > 0)
	 //		[[nodiscard]] std::string getsimilar(const VT&... type)
	 //	{
	 //		std::string str{};
	 //		for (char c{ getch(true) }; var::variadic_or((get_lexeme(c) == type)...) && hasMore(); c = getch(true))
	 //			str += c;
	 //		ss.seekg(ss.tellg() - 1ll); // rollback by 1 to exclude delimiter
	 //		return str;
	 //	}
	 //	template<class... VT> requires std::conjunction_v<std::is_same<VT, LEXEME>...> && (sizeof...(VT) > 0)
	 //		[[nodiscard]] std::string getnotsimilar(const VT&... type)
	 //	{
	 //		std::string str{};
	 //		for (char c{ getch(true) }; var::variadic_and(get_lexeme(c) != type...) && hasMore(); c = getch(true))
	 //			str += c;
	 //		ss.seekg(ss.tellg() - 1ll);
	 //		return str;
	 //	}
	 //	/**
	 //	 * @brief Clears/Resets the internal stream buffer's _error state flags_.
	 //	 */
	 //	void clear()
	 //	{
	 //		ss.clear();
	 //	}
	 //	/**
	 //	 * @brief Set the getter position to lastPos.
	 //	 * @returns std::streamoff	- The previous stream getter position.
	 //	 */
	 //	virtual std::streamoff rollback()
	 //	{
	 //		clear();
	 //		const auto copyPos{ ss.tellg() };
	 //		ss.seekg(lastPos);
	 //		return copyPos;
	 //	}
	 //	/**
	 //	 * @brief Set the getter position to the beginning of the stream.
	 //	 * @returns std::streamoff	- The previous stream getter position.
	 //	 */
	 //	virtual std::streamoff rollback_reset()
	 //	{
	 //		clear();
	 //		const auto copyPos{ ss.tellg() };
	 //		ss.seekg(std::ios::beg);
	 //		setLastPosHere();
	 //		return copyPos;
	 //	}
	 //	/**
	 //	 * @brief Set the value of lastPos directly.
	 //	 * @param pos				- Value to set as the last stream getter position.
	 //	 * @returns std::streamoff	- The previous stream getter position.
	 //	 */
	 //	virtual std::streamoff setLastPos(const std::streamoff& pos)
	 //	{
	 //		const auto copy{ lastPos };
	 //		lastPos = pos;
	 //		return copy;
	 //	}
	 //	/// @brief Set the last position to the current position.
	 //	virtual std::streamoff setLastPosHere()
	 //	{
	 //		return setLastPos(ss.tellg());
	 //	}
	 //	/**
	 //	 * @brief Variation of the getline(count) function that returns true if the result matches a given comparison string.
	 //	 * @param count	- The number of characters to read ahead.
	 //	 * @param comp	- Comparison string, if the final value of str matches this (case-insensitive), the function will return true.
	 //	 * @param str	- Reference of a string to use as output for getline.
	 //	 * @returns bool
	 //	 */
	 //	[[nodiscard]] virtual bool getline_and_match(const size_t& count, const std::string& comp, std::string& str)
	 //	{
	 //		str = getline(count);
	 //		return str::tolower(str) == comp;
	 //	}

	 //	/**
	 //	 * @brief Pure virtual function that is used to retrieve the next-in-line Token from the local stream.
	 //	 *\n	  This function should be overridden for each file format, and should be a self-contained token parser using recursion if necessary.
	 //	 * @returns Token
	 //	 */
	 //	[[nodiscard]] virtual Token getNext() = 0;

	 //	[[nodiscard]] std::vector<Token> tokenize_internal(const size_t& reserve)
	 //	{
	 //		std::vector<Token> tokens;
	 //		tokens.reserve(reserve);
	 //		while (ss)
	 //			tokens.emplace_back(std::move(getNext()));
	 //		if (!tokens.back().is_eof()) // append an EOF token if there isn't one already.
	 //			tokens.emplace_back(Token{ TokenType::END });
	 //		tokens.shrink_to_fit();
	 //		return tokens;
	 //	}

	 //public:
	 //	using AccessibleTokenCont = std::vector<AccessibleToken>;

	 //	/**
	 //	 * @brief Tokenize the whole local stream and return it as a vector.
	 //	 * @tparam RT			- Return Type _(AccessibleTokenCont)_
	 //	 * @param reserve_size	- Number of elements to reserve before vector automatically reallocates for each insertion.
	 //	 * @returns AccessibleTokenCont
	 //	 */
	 //	template<class RT = AccessibleTokenCont> requires std::is_same_v<RT, AccessibleTokenCont>
	 //	[[nodiscard]] AccessibleTokenCont tokenize(const size_t& reserve_size = 0ull)
	 //	{
	 //		auto tokens{ tokenize_internal(reserve_size) };
	 //		AccessibleTokenCont vec;
	 //		vec.reserve(tokens.size());
	 //		for (auto& tkn : tokens)
	 //			vec.emplace_back(std::move(tkn.operator token::AccessibleToken()));
	 //		vec.shrink_to_fit();
	 //		return vec;
	 //	}
	 //	/**
	 //	 * @brief Tokenize the whole local stream and return it as a vector.
	 //	 * @tparam RT			- Return Type _(std::vector<Token>)_
	 //	 * @param reserve_size	- Number of elements to reserve before vector automatically reallocates for each insertion.
	 //	 * @returns AccessibleTokenCont
	 //	 */
	 //	template<class RT> requires std::is_same_v<RT, std::vector<Token>>
	 //	[[nodiscard]] std::vector<Token> tokenize(const size_t& reserve_size = 0ull)
	 //	{
	 //		return tokenize_internal(reserve_size);
	 //	}
	 //};
}