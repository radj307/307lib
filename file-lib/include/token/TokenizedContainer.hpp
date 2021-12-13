#pragma once
#include <token/TokenizerBase.hpp>

namespace token {
	/**
	 * @struct TokenizedContainer
	 * @brief A container for storing sequences of tokens, as pairs where the first element is the string & the second is the type of token.
	 */
	struct TokenizedContainer {
		using ContainerT = std::vector<AccessibleToken>;
		ContainerT tokens;

		TokenizedContainer(ContainerT tokens) : tokens{ std::move(tokens) } {}
		template<class TokenizerType> requires std::derived_from<TokenizerType, TokenizerBase>
		TokenizedContainer(TokenizerType&& tkiz, const size_t& reserve_size = 0ull) : tokens{ std::move(tkiz.tokenize(reserve_size)) } {}

		operator ContainerT() const { return tokens; }

		[[nodiscard]] constexpr auto begin() const { return tokens.begin(); }
		[[nodiscard]] constexpr auto end() const { return tokens.end(); }
		[[nodiscard]] constexpr auto rbegin() const { return tokens.rbegin(); }
		[[nodiscard]] constexpr auto rend() const { return tokens.rend(); }
		[[nodiscard]] constexpr auto empty() const { return tokens.empty(); }
		[[nodiscard]] constexpr auto at(auto&& pos) const { return tokens.at(std::forward<decltype(pos)>(pos)); }
		constexpr void reserve(auto&& size) { tokens.reserve(std::forward<decltype(size)>(size)); }
		constexpr void shrink_to_fit() { tokens.shrink_to_fit(); }

		/**
		 * @brief Remove all tokens with any of the given types using the erase-remove idiom.
		 * @tparam ...VT	- Variadic TokenType.
		 * @param ...types	- One or more TokenTypes to remove.
		 */
		template<class... VT>
		void strip_types(VT... types)
		{
			tokens.erase(std::remove_if(tokens.begin(), tokens.end(), [&types...](auto&& pr) {
				return var::variadic_or(pr.second == types...);
			}), tokens.end());
		}
	};
}