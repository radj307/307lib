#include "../include/env.hpp"

#ifdef OS_WIN
#include <memory>
#include <Windows.h>
#endif


std::optional<std::string> env::getvar(const std::string_view& name) noexcept
{
	#ifdef OS_WIN
    char* buff{ nullptr };
    char* tmp;

    for (DWORD rc{ 1ul }, buffsz; ; ) {
        tmp = (char*)realloc(buff, rc);

        if (!tmp) {
            free(buff);
            return std::nullopt;
        }

        buff = tmp;
        buffsz = rc;

        rc = GetEnvironmentVariable(name.data(), buff, buffsz);

        if (rc == 0 || rc == buffsz || rc > 32767) {
            free(buff);
            return std::nullopt;
        }

        if (rc < buffsz)
            return buff;
    }
	#else
	#pragma warning (disable: 4996) // disable deprecation warning
	const auto var{ std::getenv(name.data()) };
	return(var != nullptr ? std::string{ var } : static_cast<std::optional<std::string>>(std::nullopt));
	#pragma warning (default: 4996)
	#endif
}