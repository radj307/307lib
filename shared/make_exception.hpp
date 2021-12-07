#pragma once
#include <sysarch.h>
#include <exception>
#include <str.hpp>

class except : public std::exception {
	const std::string _message;

public:
	except() = default;
	except(std::string message) : _message{ message } {}
	except(const char* message) : _message{ message } {}

	const char* what() const noexcept override { return _message.c_str(); }
};

template<typename... VT>
except make_exception(VT&&... message)
{
	return except{ str::stringify(std::forward<VT>(message)...).c_str() };
}