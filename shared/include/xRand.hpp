/**
 * @file xRand.hpp
 * @author radj307
 * @brief Updated version of the xRand.h library using SFINAE.
 */
#pragma once
#include <sysarch.h>
#include <random>
#include <chrono>

 /**
  * @namespace rng
  * @brief Contains objects & methods related to generating random numbers.
  */
namespace rng {
#if ARCH == 32
	// @brief Used to select random engine based on system architecture.
	using DefaultEngine = std::mt19937;
#elif ARCH == 64
	// @brief Used to select random engine based on system architecture.
	using DefaultEngine = std::mt19937_64;
#else
	// @brief Used to select random engine based on system architecture.
	using DefaultEngine = std::default_random_engine;
#endif

	/**
	 * @brief Constant test expression that checks if a given type is an integral type.
	 * @tparam _Ty	- Type to check.
	 */
	template <class _Ty> inline constexpr bool is_valid_real_type_v = std::_Is_any_of_v<std::remove_cv_t<_Ty>, float, double, long double>;

	/**
	 * @brief Constant test expression that checks if a given type is a floating-point type.
	 * @tparam _Ty	- Type to check.
	 */
	template <class _Ty> inline constexpr bool is_valid_int_type_v = std::_Is_any_of_v<std::remove_cv_t<_Ty>, short, int, long, long long, unsigned short, unsigned int, unsigned long, unsigned long long>;

	/**
	 * @brief Constant test expression that checks if a given type is a char type.
	 * @tparam _Ty	- Type to check.
	 */
	template <class _Ty> inline constexpr bool is_valid_char_type_v = std::_Is_any_of_v<std::remove_cv_t<_Ty>, char, unsigned char, signed char, char>;

	/**
	 * @struct Rand
	 * @brief Virtual base object for tRand, dRand, and sRand.
	 * @tparam Engine	- The random engine to use.
	 */
	template<class Engine> struct Rand {
		Engine _engine;
		bool _seed_every_time{ false };

		/**
		 * @brief Default Constructor
		 */
		Rand() : _engine{ DefaultEngine{} } {}
		virtual ~Rand() = default;

		/**
		 * @brief Seed the random generator. This is called automatically by get().
		 */
		virtual void seed() = 0;

		/**
		 * @brief Retrieve a random value in a given range.
		 * @tparam T	- Integral type.
		 * @param min	- Minimum possible return value.
		 * @param max	- Maximum possible return value.
		 * @returns T
		 */
		template<class T> std::enable_if_t<is_valid_int_type_v<T>, T> operator()(T min, T max)
		{
			seed();
			std::uniform_int_distribution<T> range(min, max);
			return range(_engine);
		}

		/**
		 * @brief Retrieve a random value in a given range.
		 * @tparam T	- Floating-point type.
		 * @param min	- Minimum possible return value.
		 * @param max	- Maximum possible return value.
		 * @returns T
		 */
		template<class T> std::enable_if_t<is_valid_real_type_v<T>, T> operator()(T min, T max)
		{
			seed();
			std::uniform_real_distribution<T> range(min, max);
			return range(_engine);
		}

		/**
		 * @brief Retrieve a random value in a given range.
		 * @tparam T	- Char type.
		 * @param min	- Minimum possible return value.
		 * @param max	- Maximum possible return value.
		 * @returns T
		 */
		template<class T> std::enable_if_t<is_valid_char_type_v<T>, T> operator()(T min, T max)
		{
			seed();
			std::uniform_int_distribution<int> range(static_cast<int>(min), static_cast<int>(max));
			return static_cast<T>(range(_engine));
		}

		/**
		 * @brief Retrieve a random value in a given range.
		 * @tparam T	- Value Type
		 * @param min	- Minimum possible return value.
		 * @param max	- Maximum possible return value.
		 * @returns T
		 */
		template<class T> T get(T min, T max)
		{
			return this->operator()(min, max);
		}

		template<typename Min, typename Max> requires (std::integral<Min>&& std::integral<Max>) || (std::floating_point<Min> && std::floating_point<Max>)
			Min get(Min min, Max max)
		{
			if constexpr (sizeof(Min) > sizeof(Max))
				return this->operator()(min, static_cast<Min>(max));
			else if constexpr (sizeof(Min) < sizeof(Max))
				return this->operator()(static_cast<Max>(min), max);
			else if constexpr (std::floating_point<Min>)
				return static_cast<Min>(this->operator()(static_cast<long double>(min), static_cast<long double>(max)));
			else if constexpr (std::unsigned_integral<Min>)
				return static_cast<Min>(this->operator()(static_cast<size_t>(min), static_cast<size_t>(max)));
			return static_cast<Min>(this->operator()(static_cast<long long>(min), static_cast<long long>(max)));
		}


	};

	/**
	 * @struct tRand
	 * @brief Random number getter that uses time to seed the default random engine.
	 */
	struct tRand : Rand<DefaultEngine> {
		void seed() override { _engine.seed(static_cast<unsigned>(std::chrono::steady_clock::now().time_since_epoch().count())); }
	};

	/**
	 * @struct dRand
	 * @brief Random number getter that uses the system random_device to seed the default random engine.
	 */
	struct dRand : Rand<DefaultEngine> {
		// @brief System random_device used to seed the random engine.
		std::random_device _rd;

		void seed() override { _engine.seed(_rd()); }
	};

	/**
	 * @struct sRand
	 * @brief Random number getter that uses given integers to seed the default random engine.
	 */
	struct sRand : Rand<DefaultEngine> {
		// @brief Integral value used to seed the random engine.
		unsigned _seed_val{ 0 };

		/**
		 * @brief Set the seed value used to generate random numbers to a new value.
		 * @param n	- New seed value.
		 * @returns unsigned
		 */
		unsigned seed(const unsigned n)
		{
			const auto copy{ _seed_val };
			_seed_val = n;
			return copy;
		}

		void seed() override { _engine.seed(_seed_val); }
	};
}