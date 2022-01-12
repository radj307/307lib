/**
 * @file	fileio.hpp
 * @author	radj307
 * @brief	Includes both filei.hpp and fileo.hpp, and undefines the read & write preprocessor definitions.
 */
#pragma once
#include <sysarch.h>

#ifdef read
#undef read
#endif
#ifdef write
#undef write
#endif

#include <filei.hpp>
#include <fileo.hpp>
