/**
 * @file	clogger.hpp
 * @author	radj307
 * @brief
 */
#pragma once
#include <var.hpp>

#include <iostream>
#include <sstream>
#include <concepts>

namespace xlog {
	using std::clog;

	/**
	 * @brief				Change the read buffer of an output stream to a different one, and return the previous buffer.
	 * @tparam Char			Stream Char Type
	 * @tparam CharTraits	Stream Char Traits Type
	 * @param os			The output stream type to swap the buffer of.
	 * @param buf			The buffer to use instead of os' current buffer.
	 * @returns				std::basic_streambuf<Char, CharTraits>*
	 */
	template<typename Char, class CharTraits = std::char_traits<Char>>
	inline std::basic_streambuf<Char, CharTraits>* swap_buffer(std::basic_ostream<Char, CharTraits>& os, std::basic_streambuf<Char, CharTraits>* buf) { return os.rdbuf(buf); }



	/**
	 * @struct	ManipulatorBase
	 * @brief	Stream manipulator base object.
	 *\n		This object acts as a placeholder that does nothing.
	 */
	struct ManipulatorBase {
		ManipulatorBase() {}
		friend std::ostream& operator<<(std::ostream& os, const ManipulatorBase& manipbase) { return os; }
		friend std::istream& operator>>(std::istream& is, const ManipulatorBase& manipbase) { return is; }
	};

	/**
	 * @concept	manipulator
	 * @brief	Concept that allows any xlog stream manipulators derived from ManipulatorBase.
	 */
	template<class T> concept manipulator = std::derived_from<T, ManipulatorBase>;


	/**
	 * @brief				Base clogger object that inherits from the specified stream type to provide most of its I/O functionality.
	 * @tparam Char			Char type to use.
	 * @tparam CharTraits	Character traits.
	 */
	template<typename Char = char, class CharTraits = std::char_traits<Char>, std::derived_from<std::basic_ostream<Char, CharTraits>> StreamType = std::ostream>
	class basic_clogger : public StreamType {
		using this_t = basic_clogger<Char, CharTraits, StreamType>;
		using streambuf = std::basic_streambuf<Char, CharTraits>;
		using ostream = std::basic_ostream<Char, CharTraits>;
		streambuf* _originalBuffer{ nullptr };
		ostream* _autoOut{ nullptr };

		/**
		 * @brief	Pure virtual function that must be overloaded in order to use the operator<< or operator>> functions.
		 * @returns	std::basic_streambuf<Char, CharTraits>*
		 */
		virtual ostream* getAltBuf() const = 0;

	public:
		explicit basic_clogger(streambuf* sb) : StreamType(sb), _originalBuffer{ swap_buffer(clog, sb) } {}
		~basic_clogger()
		{
			swap_buffer(clog, _originalBuffer);
		}

		ostream* getAutoOut() const { return _autoOut; }
		ostream* setAutoOut(ostream* target)
		{
			auto copy{ _autoOut };
			_autoOut = target;
			return copy;
		}

		friend std::ostream& operator<<(std::ostream& os, const basic_clogger<Char, CharTraits, StreamType>& clog)
		{
			auto altbuf{ clog.getAltBuf() };
			return os << altbuf;
		}
		friend std::istream& operator>>(std::istream& is, const basic_clogger<Char, CharTraits, StreamType>& clog)
		{
			auto altbuf{ clog.getAltBuf() };
			return is >> altbuf;
		}

		template<typename T>
		friend this_t& operator<<(this_t& clog, T&& input)
		{
			if constexpr (manipulator<T>) {
				// do something?
			}
			auto os{ clog.getAltBuf() };
			if (os != nullptr)
				*os << std::forward<T>(input);
			else throw make_exception("getAltBuf() returned nullptr! clogger instance is invalid.");
			if (clog._autoOut != nullptr)
				*clog._autoOut << std::forward<T>(input);
			return clog;
		}
	};

	/**
	 * @class	clogger
	 * @brief	Logging object that automatically redirects std::clog to itself to allow capturing the standard log stream.
	 *\n		This is a buffered object, and does not have any output sources by default; To retrieve data from the log,
	 *\n		 use the (<<|>>) operators or assign an automatic output source with the setAutoOut() function.
	 */
	class clogger : public basic_clogger<char, std::char_traits<char>, std::ostream> {
		std::stringstream ss;

		std::ostream* getAltBuf() const override { return (std::ostream*)&ss; }

	public:
		clogger() : basic_clogger<char, std::char_traits<char>, std::ostream>(ss.rdbuf()) {}
	};
}

