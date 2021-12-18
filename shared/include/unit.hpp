#pragma once
#include <tuple>
#include <utility>
#include <string>

/**
 * @def		UNIT_USE_UNFRIENDLY_FUNCS
 * @brief	When defined, uses alternative function names that may conflict with parent type functions.
 */
#ifndef UNIT_USE_FRIENDLY_FUNCS
#define UNIT_USE_FRIENDLY_FUNCS
#undef UNIT_USE_FRIENDLY_FUNCS
#endif


#ifndef UNIT_USE_FRIENDLY_FUNCS
#ifndef PLUGIN
#define PLUGIN pl_
#endif
#ifndef SPY
#define SPY spy_
#endif
#else
#ifndef PLUGIN
#define PLUGIN
#endif
#ifndef SPY
#define SPY
#endif
#endif


 // Method from user BLUEPIXY @ https://stackoverflow.com/a/17700209/8705305
  // This exists so the ## operator can correctly handle macro-expantion-order
#define INTERMEDIARY_PREPROC_CAT(l, r) l##r
// @def PREPROC_CAT @brief Allows concatenating 2 strings using the preprocessor
#define PREPROC_CAT(l, r) INTERMEDIARY_PREPROC_CAT(l, r)


namespace unit {
	namespace plugin {
		struct ModCounter {
		private:
			size_t _count{ 0ull };
			bool _armed{ false };
		public:
			ModCounter() = default;
			ModCounter(const std::tuple<>& tpl) {}
			virtual ~ModCounter() = default;

		#define UP_MC_COUNT PREPROC_CAT(PLUGIN, count)
			virtual size_t UP_MC_COUNT() const
			{
				return _count;
			}
		#define UP_MC_ARMED PREPROC_CAT(PLUGIN, armed)
			virtual bool UP_MC_ARMED() const
			{
				return _armed;
			}
		#define UP_MC_ARM PREPROC_CAT(PLUGIN, arm)
			virtual bool UP_MC_ARM(const bool& reset = true)
			{
				if (reset)
					_count = 0ull;
				return _armed = true;
			}
		#define UP_MC_DISARM PREPROC_CAT(PLUGIN, disarm)
			virtual void UP_MC_DISARM()
			{
				_armed = false;
			}
		#define UP_MC_RESET PREPROC_CAT(PLUGIN, reset)
			virtual void UP_MC_RESET(const bool& arm = false)
			{
				_armed = arm;
				_count = 0ull;
			}
		#define UP_MC_TRIGGER PREPROC_CAT(PLUGIN, trigger)
			virtual size_t UP_MC_TRIGGER(const size_t& n = 1ull)
			{
				if (_armed)
					_count += n;
				return _count;
			}
		};
	}

	/**
	 * @brief			Simple wrapper for a plugin, to be passed to unit objects while accepting a tuple for arguments.
	 * @tparam PluginT	Plugin Type.
	 */
	template<class PluginT>
	struct Payload : public PluginT {
		constexpr Payload() = default;
		template<class... Args>
		Payload(std::tuple<Args...>&& args) : PluginT(std::forward<std::tuple<Args...>>(args)) {}
		virtual operator PluginT& () { return *this; }
	};

	/**
	 * @brief			Publicly inherits from 2 objects of your choice, allowing de-facto object merging at any time
	 * @tparam T		Target Type
	 * @tparam PluginT	Plugin Type, wrapped in a Payload instance.
	 */
	template<class T, class PluginT>
	struct Spy : public T, public Payload<PluginT> {
		template<typename... Args>
		Spy(Args&&... args) : T(std::forward<decltype(args)>(args)...) {}
		template<typename... Args, typename... PluginArgs>
		Spy(const std::tuple<PluginArgs...>& pluginArgs, Args&&... args) : T(std::forward<decltype(args)>(args)...) {}
		virtual operator T& () { return *this; }
	#define UP_SPY_PLUGIN PREPROC_CAT(SPY, plugin)
		virtual PluginT& UP_SPY_PLUGIN() { return *this; }
	};

	template<class T>
	using ModifCounter = Spy<T, Payload<plugin::ModCounter>>;
}

// Cleanup preprocessor

#undef PLUGIN // PLUGIN
#undef SPY // SPY

// Undefine plugin-specific macros
// Spy
#undef UP_SPY_PLUGIN

// plugin::ModCounter
#undef UP_MC_COUNT
#undef UP_MC_ARMED
#undef UP_MC_ARM
#undef UP_MC_DISARM
#undef UP_MC_RESET
#undef UP_MC_TRIGGER
