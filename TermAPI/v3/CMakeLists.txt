# TermAPI/v3
cmake_minimum_required(VERSION 3.15)

set(CMAKE_CXX_STANDARD 20)

if (MSVC)
	set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)
	# Fix MSVC __cplusplus macro
	string(APPEND CMAKE_CXX_FLAGS " /Zc:__cplusplus")
endif()

set(HEADERS
	"./include/ANSIDefs.h"
	"./include/color-values.h"
	"./include/color-transform.hpp"

	"./include/format-functions.hpp"
	"./include/FormatFlag.hpp"
	"./include/Layer.hpp"
	"./include/setcolor.hpp"
	"./include/setcolor-functions.hpp"
	"./include/ColorPalette.hpp"

	"./include/Message.hpp"
	"./include/Sequence.hpp"
	"./include/SequenceDefinitions.hpp"
	"./include/TermAPIQuery.hpp"
	"./include/TermAPI.hpp"
	"./include/CursorOrigin.h"

	"./include/LineCharacter.hpp"
)
if(WIN32) # Add windows-specific functionality if target is windows
	list(APPEND HEADERS "./include/TermAPIWin.hpp")
endif()


option(TERMAPI_ENABLE_XLOG "Enable the xLog.hpp header." ON)
if (TERMAPI_ENABLE_XLOG)
	list(APPEND HEADERS "./include/xLog.hpp")
endif()

set(SRC
	"./src/setcolor.cpp"
)

add_library(TermAPI STATIC ${HEADERS} ${SRC})

target_link_libraries(TermAPI PUBLIC shared str-lib)

target_include_directories(TermAPI PUBLIC "./include" "./src")
target_link_directories(TermAPI PUBLIC "./src")

