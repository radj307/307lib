/**
 * @file	matrix.hpp
 * @author	radj307
 * @brief	Matrix object that uses a 1-dimensional array to store elements for speed, while exposing a 2-dimensional interface.
 */
#pragma once
#include <var.hpp>

#include <array>

namespace matrix {
	template<std::integral T = size_t>
	struct point : std::pair<T, T> {
		T& x{ first };
		T& y{ second };

		// @brief	Default Constructor.
		constexpr point() = default;

		/**
		 * @brief			Get the value of this point as a 1-dimensional index number in the range (0 - (\<Max_X\> * \<Max_Y\>)).
		 * @tparam SizeX:	The size of one "row" (chunk) in the virtual matrix.
		 * @returns	size_t:	1D Index Value.
		 */
		template<size_t SizeX> [[nodiscard]] inline constexpr size_t to1D() const noexcept
		{
			return (x + (y * SizeX));
		}
		/**
		 * @brief			Set the value of this point using a 1-dimensional index number in the range (0 - (Max_X * Max_Y)).
		 * @tparam SizeX:	The size of one "row" (chunk) in the virtual matrix.
		 * @param index:	1D Index Value.
		 */
		template<size_t SizeX> [[nodiscard]] inline constexpr void from1D(const size_t& index) noexcept
		{
			x = index / SizeX;
			y = index % SizeX;
		}

		/**
		 * @brief			Set the value of this point using a 1-dimensional index number in the range (0 - (Max_X * Max_Y)).
		 * @tparam SizeX:	The size of one "row" (chunk) in the virtual matrix.
		 * @tparam T:		The integral type used by the desired point.
		 * @param index:	1D Index Value.
		 */
		template<size_t SizeX, typename T = size_t> [[nodiscard]] inline static constexpr point<T> from1D(const size_t& index) noexcept
		{
			point<T> pos;
			pos.x = index / SizeX;
			pos.y = index % SizeX;
			return pos;
		}
	};

	template<typename T, size_t SizeX, size_t SizeY>
	struct matrix {
	private:
		const size_t SIZE{ SizeX * SizeY };
		std::array<T, (SizeX* SizeY)> _arr;

		inline static constexpr size_t to1D(const size_t& x, const size_t& y)
		{
			return (SizeX * y) + x;
		}
		inline static constexpr size_t to1D(const point& pos)
		{
			return to1D(pos.x, pos.y);
		}
		inline static constexpr point from1D(const size_t& n)
		{
			return{ n / SizeX, n % SizeX };
		}

	public:
		matrix() = default;
		template<std::same_as<T>... Ts> requires (sizeof...(Ts) == (SizeX * SizeY))
			matrix(Ts const&... values) : _arr{ values... } {}

		T get(const point& pos) const
		{
			return _arr[to1D(pos)];
		}
		T get(const size_t& x, const size_t& y) const
		{
			return get({ x, y });
		}

		T set(const point& pos, const T& value)
		{
			const auto& index{ to1D(pos) };
			const auto& copy{ _arr[index] };
			_arr[index] = value;
			return copy;
		}

		friend std::ostream& operator<<(std::ostream& os, const matrix<T, SizeX, SizeY>& v2d) requires var::Streamable<T, std::ostream>
		{
			for (size_t y{ 0ull }; y < SizeY; ++y) {
				for (size_t x{ 0ull }; x < SizeX; ++x) {
					const auto& here{ v2d.get(x, y) };
					os << here;

					if (x < SizeX - 1) {
						const auto& len{ str::getNumberLength(here) };
						const auto ind{ indent(8, len) };
						os << ind;
					}
				}
				if (y < SizeY - 1)
					os << '\n';
			}
			return os;
		}
	};
}
