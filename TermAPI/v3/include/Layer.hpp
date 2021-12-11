/**
 * @file Layer.hpp
 * @author radj307
 * @brief Contains the Layer struct, a pseudo-enum used to define a target layer (foreground/background) for the setcolor functor.
 */
#pragma once
#include <sysarch.h>
#include <ANSIDefs.h>

#include <ostream>
#include <utility>

#ifndef _CONSTEXPR
#if CPP >= 20
#define _CONSTEXPR constexpr
#else
#define _CONSTEXPR
#endif
#endif

namespace color {
	/**
	 * @struct Layer
	 * @brief Determines which layer to apply colors to.
	 */
	struct Layer {
		const std::string _layer;

		/**
		 * @brief Constexpr Constructor
		 * @param layer	- 1u == Foreground, 0u == Background
		 */
		_CONSTEXPR Layer(std::string layer) : _layer{ std::move(layer) } {}

		_CONSTEXPR operator const std::string() const { return _layer; }

		static const Layer FOREGROUND, BACKGROUND;

		friend std::ostream& operator<<(std::ostream& os, const Layer& layer)
		{
			os << layer.operator const std::string();
			return os;
		}
	};

	/** @brief Foreground/text color layer */
	inline const Layer Layer::FOREGROUND{ ANSI::FORE };
	/** @brief Background color layer, appears "behind" text. */
	inline const Layer Layer::BACKGROUND{ ANSI::BACK };

}
#ifndef COLOR_NO_GLOBALS
/// @brief Allows specifying the color::Layer object with a shorter syntax. Define "COLOR_NO_GLOBALS" to disable.
using Layer = color::Layer;
#endif