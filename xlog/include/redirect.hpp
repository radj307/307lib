/**
 * @file	redirect.hpp
 * @author	radj307
 * @brief	Contains functions used to redirect the std::clog stream.
 */
#pragma once
#include <sysarch.h>
#include <make_exception.hpp>

#include <iostream>
#include <concepts>

namespace xlog {
	using Buffer = std::streambuf*;

	inline Buffer swap_buffer(std::ostream& os, Buffer new_target) { return os.rdbuf(new_target); }

	class BufferManager {
		std::ostream& _ostream;
		Buffer _main, _alt;

	public:
		BufferManager(std::ostream& bind_to) : _ostream{ bind_to }, _main{ _ostream.rdbuf() }, _alt{ nullptr } {}

		void setAltBuffer(Buffer target) { _alt = target; }
		Buffer getAltBuffer() const { return _alt; }

		void swap() noexcept(false)
		{
			if (_main == nullptr || _alt == nullptr)
				throw make_exception("BufferManager::swap() failed:  Buffer pointer is set to null!");
			auto* current{ _ostream.rdbuf() };
			if (current == _main)
				_ostream.rdbuf(_alt);
			else if (current == _alt)
				_ostream.rdbuf(_main);
			else throw make_exception("BufferManager::swap() failed:  Stream is set to an unrecognized target, and no arguments were specified!");
		}
	};
}
