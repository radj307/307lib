/**
 * @file	xRand.hpp
 * @author	radj307
 * @brief	Updated version of the xRand.h header using C++20 concepts.
 */
#pragma once
#include <sysarch.h>
#include <var.hpp>

#include <random>
#include <chrono>
#include <concepts>

 /**
  * @namespace	rng
  * @brief		Contains objects & methods related to generating random numbers.
  */
namespace rng {
#ifndef USE_OLD_XRAND

	/**
	 * @struct			EngineWrapper
	 * @brief			A wrapper for a randomization engine. This is used internally by the xRand object.
	 * @tparam Engine	A uniform random bit generator.
	 */
	template<std::uniform_random_bit_generator Engine = std::mt19937>
	class EngineWrapper {
	protected:
		Engine _engine;

	public:
		/**
		 * @brief		Constructor.
		 *\n			Seeds the generator using a random_device.
		 * @param rdev	Reference of a std::random_device object.
		 */
		EngineWrapper(std::random_device& rdev) : _engine{ rdev() } {}

		/**
		 * @brief		Constructor.
		 *\n			Seeds the generator using an unsigned int.
		 * @tparam T	Any unsigned integer type.
		 * @param seed	Seed value.
		 */
		template<std::unsigned_integral T>
		EngineWrapper(const T& seed) : _engine{ static_cast<unsigned>(seed) } {}

		/**
		 * @brief			Constructor.
		 *\n				Seeds the generator using the specified duration.
		 * @tparam Rep		Duration Rep Type.
		 * @tparam Period	Duration Period Type.
		 * @param duration	Any std::chrono::duration. The returned value of count() is used to seed the generator.
		 */
		template<typename Rep, class Period>
		EngineWrapper(const std::chrono::duration<Rep, Period>& duration) : _engine{ static_cast<unsigned>(duration.count()) } {}

		/**
		 * @brief	Retrieve a reference to the internal randomization engine.
		 * @returns	Engine&
		 */
		Engine& getEngine()
		{
			return _engine;
		}

		/**
		 * @brief		Reseed the random number generator.
		 *\n			This is not recommended unless you're using an alternative seed method to std::random_device.
		 * @param seed	A seed value.
		 */
		void reseed(const unsigned& seed)
		{
			_engine.seed(seed);
		}

		EngineWrapper(const EngineWrapper<Engine>&) = delete;
		EngineWrapper<Engine>& operator=(const EngineWrapper<Engine>&) = delete;
	};

	/**
	 * @struct			xRand
	 * @brief			Random number generator wrapper object with convenience functions.
	 * @tparam Engine	A uniform random bit generator.
	 */
	template<std::uniform_random_bit_generator Engine = std::mt19937>
	struct xRand {
		using engine_type = Engine;

		std::random_device random_device;
		EngineWrapper<Engine> engine;

		xRand() : random_device{}, engine { random_device } {}

		/**
		 * @brief	Retrieve a reference to the internal randomization engine.
		 * @returns	Engine&
		 */
		Engine& getEngine()
		{
			return engine.getEngine();
		}

		/**
		 * @brief		Retrieve a uniform integral distribution object.
		 * @tparam T	Any integral type.
		 * @param min	Minimum allowable return value.
		 * @param max	Maximum allowable return value.
		 * @returns		T
		 */
		template<std::integral T = int>
		inline static std::uniform_int_distribution<T> uniform_distribution(T const& min, T const& max)
		{
			return std::uniform_int_distribution<T>(min, max);
		}
		/**
		 * @brief		Retrieve a uniform real distribution object.
		 * @tparam T	Any floating-point type.
		 * @param min	Minimum allowable return value.
		 * @param max	Maximum allowable return value.
		 * @returns		T
		 */
		template<std::floating_point T = double>
		inline static std::uniform_real_distribution<T> uniform_distribution(T const& min, T const& max)
		{
			return std::uniform_real_distribution<T>(min, max);
		}
		/**
		 * @brief			Retrieve an exponential distribution object.
		 * @tparam T		Any floating-point type.
		 * @param lambda	The value of lambda in the exponetial distribution curve.
		 *\n				See this graphic: ( https://en.wikipedia.org/wiki/Exponential_distribution#/media/File:Exponential_cdf.svg )
		 * @returns			T
		 */
		template<std::floating_point T = double>
		inline static std::exponential_distribution<T> exponential_distribution(T const& lambda = static_cast<T>(1.0))
		{
			return std::exponential_distribution<T>(lambda);
		}

		/**
		 * @brief					Get a random number in the given distribution using the random engine.
		 * @tparam T				Any numerical type.
		 * @tparam Distribution		rvalue of any numeric distribution type that supports type T.
		 * @param distrib			A pre-constructed numeric distribution instance.
		 * @returns					T
		 */
		template<var::numeric T, class Distribution>
		T get(Distribution&& distrib)
		{
			return distrib(engine.getEngine());
		}

		/**
		 * @brief					Get a random number in the given distribution using the random engine.
		 * @tparam T				Any numerical type.
		 * @tparam Distribution		Any numeric distribution type that supports type T.
		 * @param distrib			A pre-constructed numeric distribution instance.
		 * @returns					T
		 */
		template<var::numeric T, class Distribution>
		T get(Distribution& distrib)
		{
			return distrib(engine.getEngine());
		}

		/**
		 * @brief		Get a random number between (min) and (max), using a uniform distribution.
		 * @tparam T	Any numeric type. Both (min) & (max) must be the same type.
		 * @param min	Minimum allowable return value.
		 * @param max	Maximum allowable return value.
		 * @returns		T
		 */
		template<var::numeric T>
		T get(T const& min, T const& max)
		{
			return get<T>(uniform_distribution<T>(min, max));
		}
		/**
		 * @brief		Get a random number between (min) and (max), using a uniform distribution.
		 *\n			This function allows (min) & (max) to use different types, and automatically returns the result in whichever type is larger.
		 * @tparam Min	Any integral type.
		 * @tparam Max	Any integral type.
		 * @param min	Minimum allowable return value.
		 * @param max	Maximum allowable return value.
		 * @returns		Max
		 */
		template<std::integral Min, std::integral Max> requires (sizeof(Min) <= sizeof(Max))
			Max get(Min const& min, Max const& max)
		{
			return get<Max>(uniform_distribution<Max>(static_cast<Max>(min), max));
		}
		/**
		 * @brief		Get a random number between (min) and (max), using a uniform distribution.
		 *\n			This function allows (min) & (max) to use different types, and automatically returns the result in whichever type is larger.
		 * @tparam Min	Any integral type.
		 * @tparam Max	Any integral type.
		 * @param min	Minimum allowable return value.
		 * @param max	Maximum allowable return value.
		 * @returns		Min
		 */
		template<std::integral Min, std::integral Max> requires (sizeof(Min) > sizeof(Max))
			Min get(Min const& min, Max const& max)
		{
			return get<Min>(uniform_distribution<Min>(min, static_cast<Min>(max)));
		}
		/**
		 * @brief		Get a random number between (min) and (max), using a uniform distribution.
		 *\n			This function allows (min) & (max) to use different types, and automatically returns the result in whichever type is larger.
		 * @tparam Min	Any floating point type.
		 * @tparam Max	Any floating point type.
		 * @param min	Minimum allowable return value.
		 * @param max	Maximum allowable return value.
		 * @returns		Max
		 */
		template<std::floating_point Min, std::floating_point Max> requires (sizeof(Min) <= sizeof(Max))
			Max get(Min const& min, Max const& max)
		{
			return get<Max>(uniform_distribution<Max>(static_cast<Max>(min), max));
		}
		/**
		 * @brief		Get a random number between (min) and (max), using a uniform distribution.
		 *\n			This function allows (min) & (max) to use different types, and automatically returns the result in whichever type is larger.
		 * @tparam Min	Any floating point type.
		 * @tparam Max	Any floating point type.
		 * @param min	Minimum allowable return value.
		 * @param max	Maximum allowable return value.
		 * @returns		Min
		 */
		template<std::floating_point Min, std::floating_point Max> requires (sizeof(Min) > sizeof(Max))
			Min get(Min const& min, Max const& max)
		{
			return get<Min>(uniform_distribution<Min>(min, static_cast<Min>(max)));
		}

		/**
		 * @brief		Get a random number between (min) and (max), using a uniform distribution.
		 *\n			This function allows (min) & (max) to use integrals or floating-point numbers independently, and casts the result to a floating-point type.
		 * @tparam Min	Any integral type.
		 * @tparam Max	Any floating point type.
		 * @param min	Minimum allowable return value.
		 * @param max	Maximum allowable return value.
		 * @returns		Min
		 */
		template<std::integral Min, std::floating_point Max>
		Max get(Min const& min, Max const& max)
		{
			return get<Max>(uniform_distribution<Max>(static_cast<Max>(min), max));
		}
		/**
		 * @brief		Get a random number between (min) and (max), using a uniform distribution.
		 *\n			This function allows (min) & (max) to use integrals or floating-point numbers independently, and casts the result to a floating-point type.
		 * @tparam Min	Any floating point type.
		 * @tparam Max	Any integral type.
		 * @param min	Minimum allowable return value.
		 * @param max	Maximum allowable return value.
		 * @returns		Min
		 */
		template<std::floating_point Min, std::integral Max>
		Min get(Min const& min, Max const& max)
		{
			return get<Min>(uniform_distribution<Min>(min, static_cast<Min>(max)));
		}

		/**
		 * @brief		Get a random number between (min) and (max).
		 * @param min	Minimum allowable return value.
		 * @param max	Maximum allowable return value.
		 * @returns		auto
		 */
		auto operator()(auto&& min, auto&& max)
		{
			return get(std::forward<decltype(min)>(min), std::forward<decltype(max)>(max));
		}
	};

	// Random number generation
	using Random = xRand<std::mt19937>;
	// legacy support
	using dRand = Random;
	// legacy support
	using tRand = Random;
	// legacy support
	using sRand = Random;

#else

#if LANG_CPP >= 20 // C++ version is 20 or higher
	/**
	 * @class			BasicRand
	 * @brief			Virtual base object that provides an interface for retrieving random numbers through the get() method.
	 *\n				This object is missing the implementation of seed(), allowing inheriting objects to define their own.
	 * @tparam Engine	The random engine to use.
	 */
	template<class Engine>
	class BasicRand {
	protected:
		Engine engine;
		bool seeded{ false };
	public:
		/**
		 * @brief				Default Constructor
		 * @tparam Args			Optional variadic arguments to pass to the engine's constructor.
		 * @param engine_args	Optional arguments to pass to the engine's constructor.
		 */
		template<typename... Args>
		BasicRand(const Args&... engine_args) : engine{ Engine{ engine_args... } } {}
		virtual ~BasicRand() = default;

		/**
		 * @brief	Seed the random generator. This is called automatically by get().
		 */
		virtual bool seed() = 0;

		/**
		 * @brief		Retrieve a random value in a given range.
		 * @tparam T	Integral type.
		 * @param min	Minimum possible return value.
		 * @param max	Maximum possible return value.
		 * @returns		T
		 */
		template<std::integral T> [[nodiscard]] T operator()(const T& min, const T& max, const bool& reseed = false)
		{
			if (min > max)
				throw make_exception("rng::BasicRand::operator() failed:  Min \'", min, "\' cannot be greater than max \'", max, "\'!");
			if (!seeded || reseed)
				seeded = seed();
			return std::uniform_int_distribution<T>{ min, max }(engine);
		}

		/**
		 * @brief		Retrieve a random value in a given range.
		 * @tparam T	Floating-point type.
		 * @param min	Minimum possible return value.
		 * @param max	Maximum possible return value.
		 * @returns		T
		 */
		template<std::floating_point T> [[nodiscard]] T operator()(const T& min, const T& max, const bool& reseed = false)
		{
			if (min > max)
				throw make_exception("rng::BasicRand::operator() failed:  Min \'", min, "\' cannot be greater than max \'", max, "\'!");
			if (!seeded || reseed)
				seeded = seed();
			return std::uniform_real_distribution<T>{ min, max }(engine);
		}

		/**
		 * @brief		Retrieve a random value in a given range.
		 * @tparam T	Char type.
		 * @param min	Minimum possible return value.
		 * @param max	Maximum possible return value.
		 * @returns		T
		 */
		template<var::valid_char T> [[nodiscard]] T operator()(const T& min, const T& max, const bool& reseed = false)
		{
			if (min > max)
				throw make_exception("rng::BasicRand::operator() failed:  Min \'", min, "\' cannot be greater than max \'", max, "\'!");
			if (!seeded || reseed)
				seeded = seed();
			return static_cast<T>(std::uniform_int_distribution<size_t>{ static_cast<size_t>(min), static_cast<size_t>(max) }(engine));
		}

		// GET FUNCTION OVERLOADS

		/**
		 * @brief		Retrieve a random integral number between min and max.
		 * @tparam Tmin	Minimum Variable Type. (Integral type)
		 * @tparam Tmax	Maximum Variable Type. (Integral type larger than Tmin, in bytes)
		 * @param min	Minimum possible return value.
		 * @param max	Maximum possible return value.
		 * @returns		Tmax
		 */
		template<std::integral Tmin, std::integral Tmax> requires (sizeof(Tmin) < sizeof(Tmax)) Tmax get(const Tmin& min, const Tmax& max, const bool& reseed = false)
		{
			return operator()(static_cast<Tmax>(min), max, reseed);
		}
		/**
		 * @brief		Retrieve a random integral number between min and max.
		 * @tparam Tmin	Minimum Variable Type. (Integral type larger or equal to Tmax, in bytes)
		 * @tparam Tmax	Maximum Variable Type. (Integral type)
		 * @param min	Minimum possible return value.
		 * @param max	Maximum possible return value.
		 * @returns		Tmin
		 */
		template<std::integral Tmin, std::integral Tmax> requires (sizeof(Tmin) >= sizeof(Tmax)) Tmin get(const Tmin& min, const Tmax& max, const bool& reseed = false)
		{
			return operator()(min, static_cast<Tmin>(max), reseed);
		}
		/**
		 * @brief		Retrieve a random floating-point number between min and max.
		 * @tparam Tmin	Minimum Variable Type. (Floating-Point type)
		 * @tparam Tmax	Maximum Variable Type. (Floating-Point type larger than Tmin, in bytes)
		 * @param min	Minimum possible return value.
		 * @param max	Maximum possible return value.
		 * @returns		Tmax
		 */
		template<std::floating_point Tmin, std::floating_point Tmax> requires (sizeof(Tmin) < sizeof(Tmax)) Tmax get(const Tmin& min, const Tmax& max, const bool& reseed = false)
		{
			return operator()(static_cast<Tmax>(min), max, reseed);
		}
		/**
		 * @brief		Retrieve a random floating-point number between min and max.
		 * @tparam Tmin	Minimum Variable Type. (Floating-Point type larger or equal to Tmax, in bytes)
		 * @tparam Tmax	Maximum Variable Type. (Floating-Point type)
		 * @param min	Minimum possible return value.
		 * @param max	Maximum possible return value.
		 * @returns		Tmin
		 */
		template<std::floating_point Tmin, std::floating_point Tmax> requires (sizeof(Tmin) >= sizeof(Tmax)) Tmin get(const Tmin& min, const Tmax& max, const bool& reseed = false)
		{
			return operator()(min, static_cast<Tmin>(max), reseed);
		}

		/**
		 * @brief		Retrieve a random floating-point number between min and max.
		 * @tparam Tmin	Minimum Variable Type. (Floating-Point type)
		 * @tparam Tmax	Maximum Variable Type. (Integral type) This is casted to Tmin before evaluation.
		 * @param min	Minimum possible return value.
		 * @param max	Maximum possible return value.
		 * @returns		Tmin
		 */
		template<std::floating_point Tmin, std::integral Tmax> Tmin get(const Tmin& min, const Tmax& max, const bool& reseed = false)
		{
			return operator()(min, static_cast<Tmin>(max), reseed); // cast integral to floating-point
		}

		/**
		 * @brief		Retrieve a random floating-point number between min and max.
		 * @tparam Tmin	Minimum Variable Type. (Integral type) This is casted to Tmax before evaluation.
		 * @tparam Tmax	Maximum Variable Type. (Floating-Point type)
		 * @param min	Minimum possible return value.
		 * @param max	Maximum possible return value.
		 * @returns		Tmax
		 */
		template<std::integral Tmin, std::floating_point Tmax> Tmax get(const Tmin& min, const Tmax& max, const bool& reseed = false)
		{
			return operator()(static_cast<Tmax>(min), max, reseed); // cast integral to floating-point
		}

		/**
		 * @brief		Retrieve a random number between min and max.
		 * @tparam T	Variable Type
		 * @param min	Minimum possible return value.
		 * @param max	Maximum possible return value.
		 * @returns		T
		 */
		template<typename T> T get(const T& min, const T& max, const bool& reseed = false)
		{
			return operator()(min, max, reseed);
		}
	};
#else // C++ version is lower than 20

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
		Engine engine;

		/**
		 * @brief Default Constructor
		 */
		Rand() : engine{ Engine{} } {}
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
			return range(engine);
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
			return range(engine);
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
			return static_cast<T>(range(engine));
		}

		/**
		 * @brief Retrieve a random value in a given range.
		 * @tparam T	- Value Type
		 * @param min	- Minimum possible return value.
		 * @param max	- Maximum possible return value.
		 * @returns T
		 */
		template<typename T> T get(T min, T max)
		{
			return operator()(min, max);
		}
	};
#endif // LANG_CPP >= 20

	/**
	 * @namespace	seed
	 * @brief		Contains various seeding implementation methods & sources, as objects inhertiting from Rand.
	 */
	namespace seed {
		/**
		 * @struct			FromTime
		 * @brief			Uses the time since epoch reported by <chrono>'s steady_clock as a seed value.
		 *\n				The seed is automatically refreshed from the current time since epoch each time get() is called.
		 * @tparam Engine	Randomization algorithm.
		 */
		template<class Engine>
		struct FromTime : public ::rng::BasicRand<Engine> {
			bool seed() override
			{
				this->engine.seed(static_cast<unsigned>(std::chrono::steady_clock::now().time_since_epoch().count()));
				return true;
			}
		};
		/**
		 * @struct			FromRandomDevice
		 * @brief			Uses the host system's random_device if one is available to seed the generator. This is often the preferred method on newer hardware.
		 *\n				The seed is automatically refreshed from the system's random_device each time get() is called.
		 * @tparam Engine	Randomization algorithm.
		 */
		template<class Engine>
		struct FromRandomDevice : public ::rng::BasicRand<Engine> {
		protected:
			std::random_device _rd{};
		public:
			bool seed() override
			{
				this->engine.seed(_rd());
				return true;
			}
		};
		/**
		 * @struct			FromIntegral
		 * @brief			Uses a user-specified unsigned integral number to seed the generator.
		 *\n				The seed must be changed manually each time in order to have random output.
		 * @tparam Engine	Randomization algorithm.
		 */
		template<class Engine>
		struct FromIntegral : public ::rng::BasicRand<Engine> {
		protected:
			unsigned _seed{ 0 };
		public:
			/**
			 * @brief	Set the randomization seed to a new value, and return the previous one.
			 * @param n	An unsigned integral value to use as the seed.
			 * @returns	unsigned
			 */
			unsigned seed(const unsigned& n)
			{
				const auto prev{ _seed };
				_seed = n;
				return prev;
			}
			bool seed() override
			{
				this->engine.seed(_seed);
				return true;
			}
		};
	}

#pragma region SelectArchitecture
#if ARCH == 32
	// @brief Used to select random engine based on system architecture.
	using DefaultEngine = ::std::mt19937;
#elif ARCH == 64
	// @brief Used to select random engine based on system architecture.
	using DefaultEngine = ::std::mt19937_64;
#else
	// @brief Used to select random engine based on system architecture.
	using DefaultEngine = ::std::default_randomengine;
#endif
#pragma endregion SelectArchitecture

	/**
	 * @brief			Templated using statement for rng engines.
	 * @tparam Engine	Randomization engine.
	 * @tparam Seeder	Seeding algorithm implementation.
	 */
	template<class Engine, template<class> class Seeder>
	using xRand = Seeder<Engine>;
	/// @brief	Random number generator using the time since epoch (retrieved from std::chrono::steady_clock) as a seed.
	using tRand = xRand<DefaultEngine, seed::FromTime>;
	/// @brief	Random number generator using the system's random_device as the seed.
	using dRand = xRand<DefaultEngine, seed::FromRandomDevice>;
	/// @brief	Random number generator using a user-specified unsigned integral as the seed.
	using sRand = xRand<DefaultEngine, seed::FromIntegral>;

#endif
}