/**
 * @file TermAPIWin.hpp
 * @author radj307
 * @brief Provides windows-only functionality to the TermAPI.hpp header-only-library, such as enabling ANSI escape sequences.
 */
#pragma once
#include <sysarch.h>
#include <memory>
#include <optional>
#include <functional>

#define TERMAPIWIN_HPP
#ifndef _WINDOWS_
#include <Windows.hpp> // for windows-specific functions
#endif

namespace sys::term {
#ifdef WINDOWS_HPP
	using namespace win;
#else
	namespace win {
		inline constexpr const DWORD _STD_INPUT_HANDLE{ STD_INPUT_HANDLE }, _STD_OUTPUT_HANDLE{ STD_OUTPUT_HANDLE };
	}
#endif
#ifdef OS_WIN
	struct TargetHandle {
		const unsigned _target;
		constexpr TargetHandle(unsigned target) : _target{ std::move(target) } {}
		constexpr operator unsigned() const { return _target; }

		unsigned operator&(const unsigned& o) const { return _target & o; }
		unsigned operator|(const unsigned& o) const { return _target | o; }

		bool has_flag(const unsigned& flag_value) const
		{
			return this->operator&(flag_value) != 0u;
		}

		static const TargetHandle NONE, STDIN, STDOUT;
	};

	inline const TargetHandle TargetHandle::NONE{ 0u }, TargetHandle::STDIN{ 1u }, TargetHandle::STDOUT{ 2u };
	static struct {
	private:
		std::unique_ptr<HANDLE> _STDIN{
			std::make_unique<HANDLE>(GetStdHandle(win::_STD_INPUT_HANDLE))
		}, _STDOUT{
			std::make_unique<HANDLE>(GetStdHandle(win::_STD_OUTPUT_HANDLE))
		};

		using Mode = std::optional<DWORD>;

		/**
		 * @brief Get the current STDIN console mode.
		 * @returns Mode
		 */
		Mode getInputMode() const noexcept
		{
			try {
				if (DWORD mode; GetConsoleMode(*_STDIN.get(), &mode))
					return mode;
			} catch (...) {}
			return std::nullopt;
		}
		/**
		 * @brief Set the console mode for the STDIN handle.
		 * @param dwMode	- Mode parameter to set.
		 * @returns bool
		 */
		bool setInputMode(const DWORD& dwMode) const
		{
			return SetConsoleMode(_STDIN.get(), dwMode);
		}
		/**
		 * @brief Modify the console mode for the STDOUT handle by adding the given mode.
		 * @param dwMode	- Mode parameter to add.
		 * @returns bool
		*/
		bool modInputMode(const DWORD& dwMode) const
		{
			return setInputMode(getInputMode().value_or(0ul) | dwMode);
		}

		/**
		 * @brief Get the current STDOUT console mode.
		 * @returns Mode
		 */
		Mode getOutputMode() const noexcept
		{
			try {
				if (DWORD mode; GetConsoleMode(*_STDOUT.get(), &mode))
					return mode;
			} catch (...) {}
			return std::nullopt;
		}
		/**
		 * @brief Set the console mode for the STDOUT handle.
		 * @param dwMode	- Mode parameter to set.
		 * @returns bool
		 */
		bool setOutputMode(const DWORD& dwMode) const
		{
			return SetConsoleMode(_STDOUT.get(), dwMode);
		}
		/**
		 * @brief Modify the console mode for the STDOUT handle by adding the given mode.
		 * @param dwMode	- Mode parameter to add.
		 * @returns bool
		*/
		bool modOutputMode(const DWORD& dwMode) const
		{
			return setOutputMode(getOutputMode().value_or(0ul) | dwMode);
		}

	public:

		bool VirtualSequences(const TargetHandle& target, const bool enable)
		{
			if (target.has_flag(TargetHandle::STDIN))
				return modInputMode(ENABLE_VIRTUAL_TERMINAL_INPUT);
			else if (target.has_flag(TargetHandle::STDOUT))
				return modOutputMode(ENABLE_VIRTUAL_TERMINAL_PROCESSING | DISABLE_NEWLINE_AUTO_RETURN);
			return false;
		}
	} ConsoleModeSetter;
#endif
}
#ifndef TERMAPI_HPP
#include <TermAPI.hpp>
#endif