#pragma once
#include <sysarch.h>

#include <concepts>
#include <string>
#include <vector>

namespace shared {

	/**
	 * @enum	PostVersion
	 * @brief	Defines the types of postfix version numbers used by semver v2.
	 */
	enum class PostVersion : uint8_t {
		/// @brief	A release candidate
		ReleaseCandidate = 0,
		/// @brief	A Pre-Release
		PreRelease = 1,
		/**
		 * @brief	Indicates the lack of a postfix version number.
		 *\n		To preserve the sorting order of semver numbers,
		 *			 all **POST-RELEASE** versions must be numbered **HIGHER** than this;
		 *           all **PRE-RELEASE** versions must be numbered **LOWER** than this.
		 */
		 None = 2,
		 /// @brief	A revision release.
		 Revision = 3,
	};

	/**
	 * @struct		semversion
	 * @brief		Represents a version number using Semantic Versioning v2.
	 * @tparam T	The type of integral number to use for member variables.
	 */
	template<std::integral T>
	class semversion {
		using this_t = semversion<T>;

	public:
		T major{ 0 };
		T minor{ 0 };
		T patch{ 0 };
		PostVersion extra_type{ PostVersion::None };
		T extra{ 0 };

		semversion() = default;
		semversion(const T major, const T minor = 0, const T patch = 0, const PostVersion extra_type = PostVersion::None, const T extra = 0) : major{ major }, minor{ minor }, patch{ patch }, extra_type{ extra_type }, extra{ extra } {}

		semversion(this_t&&) noexcept = default;
		semversion(this_t const&) = default;
		virtual ~semversion() = default;
		semversion& operator=(this_t&&) noexcept = default;
		semversion& operator=(this_t const&) = default;

		static this_t parse(std::string s)
		{
			if (s.starts_with('v'))
				s = s.substr(1ull);

			std::vector<std::string> segments;
			size_t iPrev{ 0ull };
			for (size_t i{ s.find_first_of(".-+") }; i != std::string::npos; iPrev = i + 1ull, i = s.find_first_of(".-+", i + 1ull)) {
				switch (str::tolower(s.at(i))) {
				case '.':
				case '-':
				case '+':
					segments.emplace_back(s.substr(iPrev, i - iPrev));
					break;
				default:
					break;
				}
			}
			this_t v{};

			if (segments.size() >= 1) v.major = str::tonumber<T>(segments.at(0));
			if (segments.size() >= 2) v.minor = str::tonumber<T>(segments.at(1));
			if (segments.size() >= 3) v.patch = str::tonumber<T>(segments.at(2));
			if (segments.size() >= 4) {
				std::string extra_s{ segments.at(3) };
				if (const size_t firstDigit{ extra_s.find_first_of("0123456789") }; firstDigit != std::string::npos) {
					if (extra_s.find("rc") == 0)
						v.extra_type = PostVersion::ReleaseCandidate;
					else if (extra_s.find("pr") == 0)
						v.extra_type = PostVersion::PreRelease;
					else v.extra_type = PostVersion::Revision;

					v.extra = str::tonumber<T>(extra_s.substr(firstDigit));
				}
			}
			return v;
		}

		semversion(std::string const& str) : semversion(parse(str)) {}
		semversion(const char* s) : semversion(parse({ s })) {}

		auto operator<=>(const this_t& o) const noexcept
		{
			if (major != o.major)	return major - o.major;
			if (minor != o.minor)	return minor - o.minor;
			if (patch != o.patch)	return patch - o.patch;
			if (extra_type != o.extra_type) return $c(uint8_t, extra_type) - $c(uint8_t, o.extra_type);
			if (extra != o.extra) return extra - o.extra;
			return 0;
		}
	};

	using semver = semversion<uint16_t>;
}
