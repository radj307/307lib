/**
 * @file	Segments.h
 * @author	radj307
 * @brief	Contains constant ANSI sequence segment definitions.
 */
#pragma once
namespace ANSI {
	// https://en.wikipedia.org/wiki/ANSI_escape_code#Colors
	static const constexpr auto //## SEQUENCE TYPE
		ESC{ "\x1b" },			// ESC
		CSI{ "\x1b\x5b" },		// ESC[
		OSC{ "\x1b\x5d" };		// ESC]
	static const constexpr auto // ## COLOR SEQUENCE SEGMENTS
		COLOR{ ";5" },			// Indicates the following color sequence uses an SGR value.
		RGB_COLOR{ ";2" },		// Indicates the following color sequence uses RGB values.
		END{ "m" };				// Ends a color sequence.
	static const constexpr auto	// ## SGR CODES
		SGR_BOLD{ "1" },		// Bold/Bright
		SGR_NO_BOLD{ "22" },	// Resets Bold/Bright
		SGR_UNDERLINE{ "4" },	// Underline
		SGR_NO_UNDERLINE{ "24" },// Resets Underline
		SGR_NEGATIVE{ "7" },	// Invert
		SGR_POSITIVE{ "27" },	// Reset Invert
		SGR_DEFAULT_FORE{ "39" },	// Reset Color
		SGR_DEFAULT_BACK{ "49" },	// Reset Color
		SGR_RESET{ "0" };		// Reset all terminal settings
	static const constexpr auto	// ## ENABLE/DISABLE SEGMENTS
		ENABLE{ "h" },			// Used by some boolean options to enable them.
		DISABLE{ "l" };			// Used by some boolean options to disable them.
	static const constexpr auto // ## EXTRA CHARACTERS
		NULL_TERM{ "\0" };		// Null terminating character for strings. (this is just '\0')

}