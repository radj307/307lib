#include "setcolor.hpp"

// setcolor function definitions
color::setcolor::operator std::string() const
{
	return _seq;
}
color::ColorFormat color::setcolor::getFormat() const
{
	return _format;
}
color::ColorFormat color::setcolor::setFormat(const FormatFlag& newFormat)
{
	const auto copy{ _format };
	_format = newFormat;
	return copy;
}
color::ColorFormat color::setcolor::addFormat(const FormatFlag& modFormat)
{
	const auto copy{ _format };
	_format |= modFormat;
	return copy;
}
color::ColorFormat color::setcolor::removeFormat(const FormatFlag& modFormat)
{
	const auto copy{ _format };
	_format &= modFormat;
	return copy;
}
bool color::setcolor::operator==(const setcolor& o) const
{
	return _seq == o._seq;
}
bool color::setcolor::operator!=(const setcolor& o) const
{
	return !this->operator==(o);
}
