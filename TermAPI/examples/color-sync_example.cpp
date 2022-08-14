/**
 * @example[{lineno}] color-sync_example.cpp
 *		This is an example showing how to use the color::basic_sync functor.
 */
#include <color-sync.hpp>

#include <iostream>

int main()
{
	using color::sync;		//< Sync object for the `char` type
	using color::setcolor;	//< Setcolor object for the `char` type
	using color::FormatFlag;//< Text formatting flags


	color::sync s;

	std::cout << s(setcolor::red, FormatFlag::Bold) << "This is red and bold/bright text!" << s() << '\n';

	s.setEnabled(false);

	std::cout << s(FormatFlag::Reset | FormatFlag::Bold);

	std::cout << s(setcolor::red, FormatFlag::Bold) << "This is regular text" << s() << '\n';
}
