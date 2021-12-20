# 307lib/pkg/AutoVersion.cmake
# Contains functions for automatically setting package versions from git tags
cmake_minimum_required(VERSION 3.19)

include(GetGitRevisionDescription)

set(AUTOVERSION_GIT_DESCRIBE_ARGS "--tags --dirty=-d" CACHE STRING "Used by AutoVersion scripts to retrieve the current version number.")
set(AUTOVERSION_FIND_REGEX "^[vV]*([0-9]+)*\.*([0-9]+)*\.*([0-9]+)*(.*)" CACHE STRING "Used by the AutoVersion PARSE_VERSION_STRING function to parse a git tag into a library version, by using capture groups. (1: Major Version ; 2: Minor Version ; 3: Patch Version ; 4: SHA1). All groups except for group 1 should be optional.")

function(parse_version_string _version _out_major _out_minor _out_patch _out_sha)
	# set the "VERSION_TAG" cache variable to the result of `git describe ${AUTOVERSION_GIT_DESCRIBE_ARGS}` in CMAKE_SOURCE_DIR
	git_describe(VERSION_TAG ${AUTOVERSION_GIT_DESCRIBE_ARGS})

	string(FIND ${VERSION_TAG} "fatal" VERSION_ERROR)
	if (VERSION_ERROR NOT EQUAL -1)
		message(WARNING "Failed to retrieve git tag. If you don't have any tags in this repository, this is normal.")
		unset(VERSION_ERROR CACHE)
		RETURN
	endif()

	# Parse the version string using the provided regular expression
	string(REGEX REPLACE ${AUTOVERSION_FIND_REGEX} "\\1" _MAJOR ${VERSION_TAG}) # get Major
	string(REGEX REPLACE ${AUTOVERSION_FIND_REGEX} "\\2" _MINOR ${VERSION_TAG}) # get Minor
	string(REGEX REPLACE ${AUTOVERSION_FIND_REGEX} "\\3" _PATCH ${VERSION_TAG}) # get Patch
	string(REGEX REPLACE ${AUTOVERSION_FIND_REGEX} "\\4" _SHA1 ${VERSION_TAG}) # get SHA1

	if (_MAJOR)

	set(_out_major ${_MAJOR} CACHE STRING "(SEMVER) Major Version portion of the current git tag's version number" FORCE)
	set(_out_minor ${_MINOR} CACHE STRING "(SEMVER) Minor Version portion of the current git tag's version number" FORCE)
	set(_out_patch ${_PATCH} CACHE STRING "(SEMVER) Patch Version portion of the current git tag's version number" FORCE)
	set(_out_sha ${_SHA1} CACHE STRING "(SEMVER) SHA1 portion of the current git tag's version number" FORCE)
	unset(VERSION_ERROR CACHE)
endfunction()