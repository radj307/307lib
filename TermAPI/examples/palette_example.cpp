/**
 * @example[{lineno}] palette_example.cpp
 *		This is an example showing how to use the term::palette object as a color manager.
 */

#include <TermAPI.hpp>
#include <palette.hpp>

// enumerator that will be used as a key.
enum class COLOR {
	HOT,
	COLD,
};

static term::palette<COLOR> myPalette {
	std::make_pair(COLOR::HOT, color::setcolor{ color::orange }),
	std::make_pair(COLOR::COLD, color::setcolor{ color::light_blue }),
};

int main(const int argc, char** argv)
{
	for (int i{ 1 }; i < argc; ++i)
		if (argv[i] == "--no-color")
			myPalette.setActive(false); //< Disable all colorized output if the user includes the "--no-color" option.

	std::cout
		<< myPalette(COLOR::HOT)		//< Set the console color to the color associated with the COLOR::HOT key.
		<< "Hell"
		<< myPalette()				//< Reset the console color.
		<< "o "
		<< myPalette(COLOR::COLD)		//< Set the console color to the color associated with the COLOR::COLD key.
		<< "World!"
		<< myPalette()				//< Reset the console color.
		<< std::endl;

	try {
		throw 0;
	} catch (...) {
		// Color palettes expose term::get_...-style methods that only use colors when the palette is active.
		std::cerr << myPalette.get_error() << "This is an error message prefixed with '[ERROR]', which might be colorized." << std::endl;
	}

	return 0;
}