#pragma region
/**
 * @file	event.hpp
 * @author	radj307
 * @brief	Provides the basic framework for creating events that are similar in usage to C#'s events.
 */
#include <sysarch.h>

#include <algorithm>
#include <concepts>
#include <compare>
#include <functional>
#include <tuple>
#include <vector>

namespace shared {
#pragma region EventArgs
	/**
	 * @struct	basic_event_args
	 * @brief	Base type for all event argument types.
	 */
	struct basic_event_args {
		/// @brief	When this is set to true, the event notification stops here and does not notify events added after this one. This does not prevent future event notifications from being triggered at all.
		bool handled{ false };

		CONSTEXPR basic_event_args() noexcept = default;
		virtual ~basic_event_args() noexcept = default;

		virtual std::strong_ordering operator<=>(basic_event_args const&) const = default;
	};

	/**
	 * @struct				tuple_event_args
	 * @brief				Event args container that uses a tuple. This is to facilitate quickly defining new event handlers, but gives you less control over the objects structure.
	 * @tparam ...TArgs		Tuple types. These are automatically deduced from the constructor.
	 */
	template<typename... TArgs>
	struct tuple_event_args : public basic_event_args {
		std::tuple<TArgs...> args;

		virtual ~tuple_event_args() = default;

		CONSTEXPR tuple_event_args(TArgs&&... args) : args{ std::forward<TArgs>(args)... } {}
		CONSTEXPR tuple_event_args(TArgs const&... args) : args{ args... } {}
	};
#pragma endregion EventArgs

#pragma region EventHandler
	/**
	 * @struct	basic_event_handler
	 * @brief	Base type for all event handlers.
	 */
	struct basic_event_handler {
		CONSTEXPR basic_event_handler() = default;
		virtual ~basic_event_handler() noexcept = default;

		/**
		 * @brief			Virtual function that is called when this event handler is notified.
		 *\n				This must be overridden in all non-abstract derived classes.
		 * @param ev_args	A pointer to the event arguments.
		 *\n				This can safely be casted to the correct event argument type.
		 */
		virtual void handle(void* sender, basic_event_args* const) noexcept = 0;

		virtual std::strong_ordering operator<=>(basic_event_handler const&) const = default;
	};
	/**
	 * @struct	callback_event_handler
	 * @brief	Event handler that calls a given function or lambda when notified.
	 */
	struct callback_event_handler : public basic_event_handler {
		/// @brief	Callback type
		using callback_t = std::function<void(void*, basic_event_args* const)>;
		/// @brief	Callback method
		callback_t callback;

		/**
		 * @brief			Creates a new callback_event_handler instance.
		 * @tparam TFunc	Any type, including lambdas, that can be converted to a function with the signature `void(basic_event_args* const)`
		 * @param callback	A callback method to call when this event is notified.
		 */
		template<class TFunc> requires std::constructible_from<callback_t, TFunc>
		callback_event_handler(const TFunc& callback) : callback{ callback } {}

		virtual ~callback_event_handler() = default;

		/**
		 * @brief			Virtual function that is called when this event handler is notified.
		 *\n				This must be overridden in all non-abstract derived classes.
		 * @param ev_args	A pointer to the event arguments.
		 *\n				This can safely be casted to the correct event argument type.
		 */
		virtual void handle(void* sender, basic_event_args* const ev_args) noexcept override { callback(sender, ev_args); }
	};
#pragma endregion EventHandler

#pragma region EventDefinition
	/**
	 * @class			eventdef
	 * @brief			An event definition. This is the type that should be defined in the object that has events.
	 * @tparam TArgs	The type to use for event arguments.
	 */
	template<std::derived_from<basic_event_args> TArgs = basic_event_args>
	class eventdef final {
		using this_t = eventdef<TArgs>;

		std::vector<std::unique_ptr<basic_event_handler>> ev_handlers;

	public:
		using EventArgs = TArgs;

		CONSTEXPR auto begin() const noexcept { return ev_handlers.cbegin(); }
		CONSTEXPR auto rbegin() const noexcept { return ev_handlers.crbegin; }
		CONSTEXPR auto end() const noexcept { return ev_handlers.cend(); }
		CONSTEXPR auto rend() const noexcept { return ev_handlers.crend(); }

		CONSTEXPR void reserve(const size_t newCapacity) { ev_handlers.reserve(newCapacity); }
		CONSTEXPR void shrink_to_fit() { ev_handlers.shrink_to_fit(); }
		CONSTEXPR auto size() const noexcept { return ev_handlers.size(); }
		CONSTEXPR auto empty() const noexcept { return ev_handlers.empty(); }
		CONSTEXPR void clear() noexcept { ev_handlers.clear(); }
		CONSTEXPR auto at(const size_t pos) const noexcept(false) { return ev_handlers.at(pos); }

		/**
		 * @brief		Adds the specified event handler as a registered handler for this event instance.
		 * @param ev	An event handler to bind to this event instance.
		 */
		template<std::derived_from<basic_event_handler> THandler>
		CONSTEXPR void add(std::unique_ptr<THandler>&& ev)
		{
			if (ev_handlers.size() == ev_handlers.capacity())
				ev_handlers.reserve(ev_handlers.size() + 1);
			ev_handlers.emplace_back(std::move(ev));
		}
		/**
		 * @brief		Adds the specified event handler as a registered handler for this event instance.
		 * @param ev	An event handler to bind to this event instance.
		 */
		template<std::derived_from<basic_event_handler> THandler>
		CONSTEXPR void add(THandler const& ev)
		{
			if (ev_handlers.size() == ev_handlers.capacity())
				ev_handlers.reserve(ev_handlers.size() + 1);
			ev_handlers.emplace_back(std::make_unique<THandler>(ev));
		}
		/**
		 * @brief		Adds the specified event handler as a registered handler for this event instance.
		 * @param ev	An event handler to bind to this event instance.
		 */
		template<std::derived_from<basic_event_handler> THandler>
		CONSTEXPR void add(THandler&& ev)
		{
			if (ev_handlers.size() == ev_handlers.capacity())
				ev_handlers.reserve(ev_handlers.size() + 1);
			ev_handlers.emplace_back(std::make_unique<THandler>($fwd(ev)));
		}
		/**
		 * @brief		Adds the specified event handler as a registered handler for this event instance, if it doesn't already exist.
		 * @param ev	An event handler to bind to this event instance.
		 * @returns		true when ev is unique and was added as an event handler; otherwise false.
		 */
		template<std::derived_from<basic_event_handler> THandler>
		CONSTEXPR bool add_if_unique(THandler const& ev)
		{
			for (const auto& handler : ev_handlers) {
				if (auto* p = handler.get())
					if (*p == ev)
						return false;
			}
			add(ev);
			return true;
		}
		/**
		 * @brief		Adds the specified event handler as a registered handler for this event instance, if it doesn't already exist.
		 * @param ev	An event handler to bind to this event instance.
		 * @returns		true when ev is unique and was added as an event handler; otherwise false.
		 */
		template<std::derived_from<basic_event_handler> THandler>
		CONSTEXPR bool add_if_unique(THandler&& ev)
		{
			for (const auto& handler : ev_handlers) {
				if (auto* p = handler.get())
					if (*p == $fwd(ev))
						return false;
			}
			add($fwd(ev));
			return true;
		}
		/**
		 * @brief		Removes the most-recently-added matching event handler from the registered handlers for this event instance.
		 * @param ev	An event handler to unbind from this event instance.
		 *\n			Must have a valid operator== overload.
		 */
		template<std::derived_from<basic_event_handler> THandler>
		CONSTEXPR bool remove(THandler const& ev)
		{
			for (int i{ ev_handlers.size() - 1 }; i >= 0; --i) {
				if (auto* p = ev_handlers.at(i).get()) {
					if (*p == ev) {
						const auto& beg{ ev_handlers.begin() + i };
						ev_handlers.erase(beg, beg + 1);
						return true;
					}
				}
			}
			return false;
		}
		/**
		 * @brief			Triggers all bound event handlers using the given event argument instance.
		 * @param args...	Any number of types that can be passed to the event argument constructor. The event argument instance created with these parameters is shared between all event handlers for this notification.
		 */
		template<typename T, typename... Ts> requires std::constructible_from<TArgs, Ts...>
		CONSTEXPR void notify(T* sender, Ts&&... args) const
		{
			TArgs ev_args{ std::forward<Ts>(args)... };
			for (const auto& handler : ev_handlers) {
				if (auto* p = (basic_event_handler*)handler.get())
					p->handle((void*)sender, &ev_args);
				if (ev_args.handled)
					break;
			}
		}
		/**
		 * @brief			Triggers all bound event handlers using the given event argument instance.
		 * @param ev_args	Event arguments for this event instance that are shared between all event handlers for this notification.
		 *\n				Must be implicitly constructible.
		 */
		CONSTEXPR void notify(void* sender = nullptr, TArgs ev_args = {}) const requires std::constructible_from<TArgs>
		{
			for (const auto& handler : ev_handlers) {
				if (auto* p = (basic_event_handler*)handler.get())
					p->handle(sender, &ev_args);
				if (ev_args.handled)
					break;
			}
		}
		/**
		 * @brief			Triggers all bound event handlers using the given event argument instance.
		 * @param ev_args	Event arguments for this event instance that are shared between all event handlers for this notification.
		 *\n				Must be implicitly constructible.
		 */
		CONSTEXPR void notify(TArgs ev_args = {}) const requires std::constructible_from<TArgs>
		{
			for (const auto& handler : ev_handlers) {
				if (auto* p = (basic_event_handler*)handler.get())
					p->handle((void*)this, &ev_args);
				if (ev_args.handled)
					break;
			}
		}

		/// @brief	Adds an event handler. *(Note that duplicates are allowed)*
		friend CONSTEXPR this_t& operator+=(this_t& ev, auto&& handler) noexcept { ev.add($fwd(handler)); return ev; }
		/// @brief	Removes an event handler. *(Note that this will only remove the **first** handler it finds; to remove duplicates, this must be called once for each duplicate.)*
		friend CONSTEXPR this_t& operator-=(this_t& ev, auto&& handler) noexcept { ev.remove($fwd(handler)); return ev; }
	};
#pragma endregion EventDefinition

#pragma region make_handler
	/**
	 * @brief				Creates a new event handler that invokes the given callback function.
	 * @param callback		A callback function with the signature: `void (void*, basic_event_args* const) noexcept`.
	 * @returns				A new callback_event_handler instance.
	 */
	template<std::invocable<void*, basic_event_args* const> TFunc> requires std::constructible_from<callback_event_handler, TFunc>
	CONSTEXPR callback_event_handler make_handler(const TFunc& callback) { return callback_event_handler(callback); }
#pragma endregion make_handler

#pragma region CSharpNames
#ifndef EVENT_HPP__NO_CSHARP_NAMES

	/**
	 * @def		EVENT_HPP__NO_CSHARP_NAMES
	 * @brief	Disables C#-style pascal case alternative names for the event types in <event.hpp>
	 */
#define EVENT_HPP__NO_CSHARP_NAMES
#undef EVENT_HPP__NO_CSHARP_NAMES

	 /**
	  * @brief				Base EventArgs type.
	  */
	using EventArgs = basic_event_args;

	/**
	 * @brief				`EventArgs`-derivative that stores its members in a std::tuple.
	 * @details				This reduces the setup requirement for defining events; allowing you to quickly create a `std::tuple` *(named `args`)* with the specified types that you can access via
	 *						```cpp
	 *						void handle(basic_event_args* const ev_args) noexcept override
	 *						{
	 *							auto& [arg1, arg2] { ((myEventArgsTypeNameHere*)ev_args)->args };
	 *						}
	 *						```
	 * @tparam ...Ts		The argument types.
	 */
	template<typename... Ts> using EventTupleArgs = tuple_event_args<Ts...>;

	/**
	 * @brief				An event handler.
	 */
	using EventHandler = basic_event_handler;

	/**
	 * @brief				An event handler for lambdas.
	 */
	using CallbackEventHandler = callback_event_handler;

	/**
	 * @brief				An event definition.
	 * @tparam THandlerArgs	Type to use as polymorphic event arguments.
	 *\n					Must be derived from `EventArgs`
	 */
	template<std::derived_from<EventArgs> THandlerArgs>
	using Event = eventdef<THandlerArgs>;

#endif // EVENT_HPP__NO_CSHARP_NAMES
#pragma endregion CSharpNames
}
