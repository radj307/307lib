/**
 * @example make_exception.cpp
 *			This is an example of how to use the <make_exception.hpp> header to
 *			 streamline the creation of exception messages in a platform-agnostic way.
 */
#include <var.hpp>
#include <make_exception.hpp>

#include <iostream>
#include <concepts>

 // this is a minimal ex::except derivative, and is used to improve the try catch block in main.
struct missing_arguments_except : public ex::except {
	// inherit base constructor:
	using ex::except::except;
};

struct my_advanced_custom_except : public ex::except {
	std::vector<std::string> extra;

	template<std::same_as<std::string>... Messages>
	my_advanced_custom_except(std::string&& msg, Messages&&... extras) : except(std::forward<std::string>(msg)), extra{ std::forward<Messages>(extras)... } {}

protected:
	virtual void format() const noexcept override
	{
		auto* msg{ get_message() }; // get a pointer to the message string
		msg->append("\n");
		for (const auto& it : extra) {
			msg->append(it); // append all extra elements to the message string
		}
	}
};

int main(const int argc, char** argv)
{
	try {
		if (argc == 1)
			throw make_exception<missing_arguments_except>();

		for (int i{ 1 }; i < argc; ++i) {
			if (argv[i] == "is foo bar fubar?") throw make_exception<my_advanced_custom_except>(
				"Illegal Question Detected!",
				"Argument [", i, "] cannot be answered!  (\"", argv[i], "\")"
			);
			else std::cout << '[' << i << "]:\t" << "yes" << '\n';
		}

		if (argc > 5)
			throw make_exception("You're being detained for asking too many questions."); ///< this throws an exception of type ex::except

		return 0;
	} catch (const std::exception& ex) {
		std::cerr << ex.what() << std::endl;
	} catch (const missing_arguments_except& ex) {
		std::cerr << "You must include at least one argument!" << std::endl << ex.what() << std::endl;
	} catch (...) { ///< this won't be triggered.
		std::cerr << "An undefined exception occured!" << std::endl;
	}
	return 1;
}
