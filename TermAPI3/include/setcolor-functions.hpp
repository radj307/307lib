#pragma once
#include <color-values.h>
#include <setcolor.hpp>
namespace color {

	/**
	 * @namespace foreground
	 * @brief Foreground colorization functions.
	 */
	namespace foreground {
		struct setcolor : color::setcolor { setcolor(const short color) : color::setcolor(color, Layer::FOREGROUND) {} };

		inline std::ostream& red(std::ostream& os) { os << setcolor(color::red); return os; }
		inline std::ostream& green(std::ostream& os) { os << setcolor(color::green); return os; }
		inline std::ostream& blue(std::ostream& os) { os << setcolor(color::blue); return os; }
		inline std::ostream& yellow(std::ostream& os) { os << setcolor(color::yellow); return os; }
		inline std::ostream& magenta(std::ostream& os) { os << setcolor(color::magenta); return os; }
		inline std::ostream& cyan(std::ostream& os) { os << setcolor(color::cyan); return os; }
		inline std::ostream& white(std::ostream& os) { os << setcolor(color::white); return os; }
		inline std::ostream& gray(std::ostream& os) { os << setcolor(color::gray); return os; }
		inline std::ostream& black(std::ostream& os) { os << setcolor(color::black); return os; }
		inline std::ostream& orange(std::ostream& os) { os << setcolor(color::orange); return os; }
	}
	namespace f = foreground;

	/**
	 * @namespace background
	 * @brief Background colorization functions.
	 */
	namespace background {
		struct setcolor : color::setcolor { setcolor(const short color) : color::setcolor(color, Layer::BACKGROUND) {} };

		inline std::ostream& red(std::ostream& os) { os << setcolor(color::red); return os; }
		inline std::ostream& green(std::ostream& os) { os << setcolor(color::green); return os; }
		inline std::ostream& blue(std::ostream& os) { os << setcolor(color::blue); return os; }
		inline std::ostream& yellow(std::ostream& os) { os << setcolor(color::yellow); return os; }
		inline std::ostream& magenta(std::ostream& os) { os << setcolor(color::magenta); return os; }
		inline std::ostream& cyan(std::ostream& os) { os << setcolor(color::cyan); return os; }
		inline std::ostream& white(std::ostream& os) { os << setcolor(color::white); return os; }
		inline std::ostream& gray(std::ostream& os) { os << setcolor(color::gray); return os; }
		inline std::ostream& black(std::ostream& os) { os << setcolor(color::black); return os; }
		inline std::ostream& orange(std::ostream& os) { os << setcolor(color::orange); return os; }
	}
	namespace b = background;
}