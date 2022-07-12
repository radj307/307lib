#include "../include/xml/XML.h"

#include <fileio.hpp>

using namespace xml;
using namespace file;

#if LANG_CPP >= 17
XMLDocument xml::XMLDocument::read(const std::filesystem::path& path)
#else
XMLDocument xml::XMLDocument::read(const std::string& path)
#endif
{
	if (!exists(path))
		throw make_exception("XMLDocument::read() File Not Found:  '", path, '\'');

	return parser::XMLParser(file::read(path)).parse();
}

#if LANG_CPP >= 17
bool xml::XMLDocument::write(const std::filesystem::path& path, XMLDocument&& doc)
#else
bool xml::XMLDocument::write(const std::string& path, XMLDocument&& doc)
#endif
{
	return file::write(path, std::forward<XMLDocument>(doc));
}



#if LANG_CPP >= 17
void xml::XMLDocument::read_from(const std::filesystem::path& path)
#else
void xml::XMLDocument::read_from(const std::string& path)
#endif
{
	(*this) = std::move(xml::parser::XMLParser(file::read(path)).parse());
}


#if LANG_CPP >= 17
bool xml::XMLDocument::write(const std::filesystem::path& path)
#else
bool xml::XMLDocument::write(const std::string& path)
#endif
{
	return file::write(path, *this);
}
