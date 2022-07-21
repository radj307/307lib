/**
 * @file	basic_variant_map.hpp
 * @author	radj307
 * @brief	Contains the basic_variant_map class; a keymap using std::variant and variadic templates for the value type(s).
 */
#pragma once
#include <var.hpp>

#include <utility>

namespace var {
	/**
	 * @brief			
	 * @tparam TMap		Class template that determines which associative container type to use.
	 *\n				Recommended types are std::map, std::unordered_map, std::multimap, etc.
	 * @tparam TKey		A single, comparable type to use for the map keys.
	 * @tparam Ts...	Any number of independent types that the value of a key may represent.
	 *\n				(excluding std::monostate)
	 */
	template<template<typename, typename> class TMap, typename TKey, var::not_same<std::monostate>... Ts>
	class basic_variant_map {
		using variant_t = std::variant<std::monostate, Ts...>;
		using pair_t = std::pair<TKey, variant_t>;
		using map_t = TMap<TKey, variant_t>;
		map_t _data;

		template<size_t Idx, var::same_or_convertible<pair_t>... TPairs>
		constexpr void _kvpr_to_map(map_t& map, std::tuple<TPairs...> const& tpl)
		{
			const auto& [key, value] {std::get<Idx>(tpl)};
			map.insert_or_assign(key, variant_t{ value });

			if constexpr (Idx + 1ull < sizeof...(TPairs))
				_kvpr_to_map<Idx + 1ull>(map, tpl);
		}

		template<var::same_or_convertible<pair_t>... TPairs>
		constexpr map_t kvpr_to_map(TPairs&&... pairs)
		{
			map_t m;

			if (sizeof...(TPairs) == 0ull)
				return m;

			std::tuple<TPairs...> container{ std::forward<TPairs>(pairs)... };
			_kvpr_to_map<0ull>(m, container);

			return m;
		}

	public:
		constexpr basic_variant_map() {}

		template<var::same_or_convertible<pair_t>... TPairs>
		constexpr basic_variant_map(TPairs&&... key_value_pairs) : _data{ kvpr_to_map(std::forward<TPairs>(key_value_pairs)...) } {}

		constexpr auto empty() const& { return _data.empty(); }
		constexpr auto size() const& { return _data.size(); }
		constexpr auto& at(const TKey& key)& { return _data.at(key); }
		constexpr auto& at(const TKey& key) const& { return _data.at(key); }

	};
	template<typename TKey, var::not_same<std::monostate>... Ts>
	using variant_map = basic_variant_map<std::map, TKey, Ts...>;
}
