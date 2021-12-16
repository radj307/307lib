/**
 * @file color-values.h
 * @author radj307
 * @brief Contains constant variables used to map hexadecimal color codes to recognizable words.
 */
#pragma once
namespace color {
	static constexpr const short
		red{ 0x01 },				///< @brief Red color.
		green{ 0x02 },				///< @brief Green color.
		blue{ 0x04 },				///< @brief Blue color.
		yellow{ 0x03 },				///< @brief Yellow color.
		magenta{ 0x05 },			///< @brief Magenta color.
		purple{ 0x5C },				///< @brief Purple color.
		cyan{ 0x06 },				///< @brief Cyan color.
		white{ 0x07 },				///< @brief White color.
		gray{ 0x08 },				///< @brief Gray color.
		light_gray{ 0xF6 },			///< @brief Light Gray color.
		black{ 0x16 },				///< @brief Black color.
		intense_red{ 0x09 },		///< @brief High Intensity Red color.
		intense_green{ 0x0A },		///< @brief High Intensity Green color.
		intense_blue{ 0x0C },		///< @brief High Intensity Blue color.
		intense_yellow{ 0x0B },		///< @brief High Intensity Yellow color.
		intense_magenta{ 0x0D },	///< @brief High Intensity Magenta color.
		intense_cyan{ 0x0E },		///< @brief High Intensity Cyan color.
		intense_white{ 0x0F },		///< @brief High Intensity White color.
		orange{ 208 },				///< @brief Orange color.
		dark_blue{ 18 },
		light_blue{ 26 },
		dark_red{ 88 },
		dark_green{ 22 },
		dark_cyan{ 33 },
		dark_purple{ 54 },
		gold{ 178 },
		dark_gray{ 235 },
		light_purple{ 99 }
	;
}