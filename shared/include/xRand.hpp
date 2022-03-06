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

 /**
  * @namespace	rng
  * @brief		Contains objects & methods related to generating random numbers.
  */
namespace rng {
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
		virtual void seed() = 0;

		/**
		 * @brief		Retrieve a random value in a given range.
		 * @tparam T	Integral type.
		 * @param min	Minimum possible return value.
		 * @param max	Maximum possible return value.
		 * @returns		T
		 */
		template<std::integral T> [[nodiscard]] T operator()(const T& min, const T& max)
		{
			if (min > max)
				throw make_exception("rng::BasicRand::operator() failed:  Min \'", min, "\' cannot be greater than max \'", max, "\'!");
			seed();
			return std::uniform_int_distribution<T>{ min, max }(engine);
		}

		/**
		 * @brief		Retrieve a random value in a given range.
		 * @tparam T	Floating-point type.
		 * @param min	Minimum possible return value.
		 * @param max	Maximum possible return value.
		 * @returns		T
		 */
		template<std::floating_point T> [[nodiscard]] T operator()(const T& min, const T& max)
		{
			if (min > max)
				throw make_exception("rng::BasicRand::operator() failed:  Min \'", min, "\' cannot be greater than max \'", max, "\'!");
			seed();
			return std::uniform_real_distribution<T>{ min, max }(engine);
		}

		/**
		 * @brief		Retrieve a random value in a given range.
		 * @tparam T	Char type.
		 * @param min	Minimum possible return value.
		 * @param max	Maximum possible return value.
		 * @returns		T
		 */
		template<var::valid_char T> [[nodiscard]] T operator()(const T& min, const T& max)
		{
			if (min > max)
				throw make_exception("rng::BasicRand::operator() failed:  Min \'", min, "\' cannot be greater than max \'", max, "\'!");
			seed();
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
		template<std::integral Tmin, std::integral Tmax> requires (sizeof(Tmin) < sizeof(Tmax)) Tmax get(const Tmin& min, const Tmax& max)
		{
			return operator()(static_cast<Tmax>(min), max);
		}
		/**
		 * @brief		Retrieve a random integral number between min and max.
		 * @tparam Tmin	Minimum Variable Type. (Integral type larger or equal to Tmax, in bytes)
		 * @tparam Tmax	Maximum Variable Type. (Integral type)
		 * @param min	Minimum possible return value.
		 * @param max	Maximum possible return value.
		 * @returns		Tmin
		 */
		template<std::integral Tmin, std::integral Tmax> requires (sizeof(Tmin) >= sizeof(Tmax)) Tmin get(const Tmin& min, const Tmax& max)
		{
			return operator()(min, static_cast<Tmin>(max));
		}
		/**
		 * @brief		Retrieve a random floating-point number between min and max.
		 * @tparam Tmin	Minimum Variable Type. (Floating-Point type)
		 * @tparam Tmax	Maximum Variable Type. (Floating-Point type larger than Tmin, in bytes)
		 * @param min	Minimum possible return value.
		 * @param max	Maximum possible return value.
		 * @returns		Tmax
		 */
		template<std::floating_point Tmin, std::floating_point Tmax> requires (sizeof(Tmin) < sizeof(Tmax)) Tmax get(const Tmin& min, const Tmax& max)
		{
			return operator()(static_cast<Tmax>(min), max);
		}
		/**
		 * @brief		Retrieve a random floating-point number between min and max.
		 * @tparam Tmin	Minimum Variable Type. (Floating-Point type larger or equal to Tmax, in bytes)
		 * @tparam Tmax	Maximum Variable Type. (Floating-Point type)
		 * @param min	Minimum possible return value.
		 * @param max	Maximum possible return value.
		 * @returns		Tmin
		 */
		template<std::floating_point Tmin, std::floating_point Tmax> requires (sizeof(Tmin) >= sizeof(Tmax)) Tmin get(const Tmin& min, const Tmax& max)
		{
			return operator()(min, static_cast<Tmin>(max));
		}

		/**
		 * @brief		Retrieve a random floating-point number between min and max.
		 * @tparam Tmin	Minimum Variable Type. (Floating-Point type)
		 * @tparam Tmax	Maximum Variable Type. (Integral type) This is casted to Tmin before evaluation.
		 * @param min	Minimum possible return value.
		 * @param max	Maximum possible return value.
		 * @returns		Tmin
		 */
		template<std::floating_point Tmin, std::integral Tmax> Tmin get(const Tmin& min, const Tmax& max)
		{
			return operator()(min, static_cast<Tmin>(max)); // cast integral to floating-point
		}

		/**
		 * @brief		Retrieve a random floating-point number between min and max.
		 * @tparam Tmin	Minimum Variable Type. (Integral type) This is casted to Tmax before evaluation.
		 * @tparam Tmax	Maximum Variable Type. (Floating-Point type)
		 * @param min	Minimum possible return value.
		 * @param max	Maximum possible return value.
		 * @returns		Tmax
		 */
		template<std::integral Tmin, std::floating_point Tmax> Tmax get(const Tmin& min, const Tmax& max)
		{
			return operator()(static_cast<Tmax>(min), max); // cast integral to floating-point
		}

		/**
		 * @brief		Retrieve a random number between min and max.
		 * @tparam T	Variable Type
		 * @param min	Minimum possible return value.
		 * @param max	Maximum possible return value.
		 * @returns		T
		 */
		template<typename T> T get(const T& min, const T& max)
		{
			return operator()(min, max);
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
			void seed() override { this->engine.seed(static_cast<unsigned>(std::chrono::steady_clock::now().time_since_epoch().count())); }
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
			void seed() override { this->engine.seed(_rd()); }
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
			void seed() override { this->engine.seed(_seed); }
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
}