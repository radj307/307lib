#include <iostream>
#include <vector>
#include <string>

using namespace std;

#include <fileio.hpp>
#include <xml/XML.h>

int main(const int argc, char** argv)
{
	try {
		using namespace xml;

		XMLDocument::read("test2.xml").write("test2-out.xml");

		auto in{ file::read("test2.xml").str() };
		auto out{ file::read("test2-out.xml").str() };
		if (in == out)
			cout << "EQUAL" << endl;
		else
			cout << "NOT EQUAL" << endl;

		return 0;
	} catch (const std::exception& ex) {
		cout << ex.what() << endl;
		return 1;
	} catch (...) {
		cout << "An undefined exception occurred!" << endl;
		return 1;
	}
}
