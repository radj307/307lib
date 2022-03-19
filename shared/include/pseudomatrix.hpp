/**
 * @file	matrix.hpp
 * @author	radj307
 * @brief	Matrix object that uses a 1-dimensional array to store elements for speed, while exposing a 2-dimensional interface.
 */
#pragma once
#include <make_exception.hpp>
#include <var.hpp>

#include <concepts>
#include <vector>
#include <array>

namespace matrix {
	template<typename T = long long>
	struct point : protected std::pair<T, T> {
		T& x{ first };
		T& y{ second };

		template<std::convertible_to<T> U>
		point(std::pair<U, U>&& o) : std::pair<T, T>(std::move(static_cast<T>(std::move(o.first))), std::move(static_cast<T>(std::move(o.second)))) {}
		template<std::convertible_to<T> U>
		point(const std::pair<U, U>& o) : std::pair<T, T>(static_cast<T>(o.first), static_cast<T>(o.second)) {}


	};

#pragma region
	/**
	 * @struct	basic_matrix
	 * @brief	Basic pseudo-matrix object that uses a 1-dimensional array internally for performance, but exposes a 2-dimensional coordinate-based interface.
	 */
	template<typename T, size_t SizeX, size_t SizeY>
	struct basic_matrix {
		static constexpr const size_t SIZE{ SizeX * SizeY };
		using type = std::array<T, (SizeX* SizeY)>;

	protected:
		type _arr;

		/**
		 * @brief		Convert a 2-dimensional point to a 1-dimensional index number.
		 * @param x:	X-Axis Position.
		 * @param y:	Y-Axis Position.
		 * @returns		size_t
		 */
		inline static constexpr size_t to1D(const size_t& x, const size_t& y)
		{
			return (SizeX * y) + x;
		}
		/**
		 * @brief		Convert a 2-dimensional point to a 1-dimensional index number.
		 * @param pos:	A 2-Dimensional Point.
		 * @returns		size_t
		 */
		inline static constexpr size_t to1D(const point<size_t>& pos)
		{
			return to1D(pos.x, pos.y);
		}
		/**
		 * @brief		Convert a 1-dimensional index number to a 2-dimensional point.
		 * @param n:	A 1-Dimensional Index Number.
		 * @returns		point<size_t>
		 */
		inline static constexpr point<size_t> from1D(const size_t& n)
		{
			return std::make_pair(n / SizeX, n % SizeX);
		}

	public:
		// @brief	Default Constructor.
		basic_matrix() : _arr{ initArray() } {}

		basic_matrix(basic_matrix<T, SizeX, SizeY>&& o) : _arr{ std::move(o._arr) } {}
		basic_matrix(const basic_matrix<T, SizeX, SizeY>& o) : _arr{ o._arr } {}

		template<var::same_or_convertible<T>... Ts> requires (sizeof...(Ts) == SIZE)
			basic_matrix(Ts&&... values) : _arr{ static_cast<T>(std::forward<Ts>(values))... } {}

		basic_matrix(std::array<T, SIZE>&& arr) : _arr{ std::move(arr) } {}
		basic_matrix(const std::array<T, SIZE>& arr) : _arr{ arr } {}

		/**
		 * @brief		Matrix Copy-Assignment Operator.
		 * @param mtx:	Input Matrix.
		 * @returns		matrix<T, SizeX, SizeY>&
		 */
		matrix<T, SizeX, SizeY>& operator=(const matrix<T, SizeX, SizeY>& mtx)
		{
			_arr = mtx._arr;
			return *this;
		}
		/**
		 * @brief		Matrix Move-Assignment Operator.
		 * @param mtx:	Input Matrix.
		 * @returns		matrix<T, SizeX, SizeY>&
		 */
		matrix<T, SizeX, SizeY>& operator=(matrix<T, SizeX, SizeY>&& mtx)
		{
			_arr = std::move(mtx._arr);
			return *this;
		}
		/**
		 * @brief		Matrix Copy-Assignment Operator.
		 * @param arr:	Input Array.
		 * @returns		matrix<T, SizeX, SizeY>&
		 */
		matrix<T, SizeX, SizeY>& operator=(const type& arr)
		{
			_arr = arr;
			return *this;
		}
		/**
		 * @brief		Matrix Move-Assignment Operator.
		 * @param arr:	Input Array.
		 * @returns		matrix<T, SizeX, SizeY>&
		 */
		matrix<T, SizeX, SizeY>& operator=(type&& arr)
		{
			_arr = std::move(arr);
			return *this;
		}

		/**
		 * @brief	Get the beginning array iterator.
		 * @returns	type::const_iterator
		 */
		type::const_iterator begin() const { return _arr.begin(); }
		/**
		 * @brief	Get the ending array iterator.
		 * @returns	type::const_iterator
		 */
		type::const_iterator end() const { return _arr.end(); }
		/**
		 * @brief	Check if the matrix is empty.
		 * @returns	bool
		 */
		bool empty() const { return _arr.empty(); }
		/**
		 * @brief	Get the size of the matrix, in elements.
		 * @returns	size_t
		 */
		size_t size() const { return _arr.size(); }

		/**
		 * @brief		Get the element at the specified point.
		 * @param pos:	Target Point.
		 * @returns		T
		 */
		T get(const point<size_t>& pos) const
		{
			return _arr[to1D(pos)];
		}
		/**
		 * @brief			Set the element at the specified point to the given value.
		 * @param pos:		Target Point.
		 * @param value:	Input Value.
		 * @returns	T:		The previous value.
		 */
		T set(const point<size_t>& pos, const T& value)
		{
			const auto& index{ to1D(pos) };
			const auto& copy{ _arr[index] };
			_arr[index] = value;
			return copy;
		}
		/**
		 * @brief		Get the element at the specified point.
		 * @param x:	Target X-Axis Position.
		 * @param y:	Target Y-Axis Position.
		 * @returns		T
		 */
		T get(const size_t& x, const size_t& y) const
		{
			return get({ x, y });
		}
		/**
		 * @brief			Set the element at the specified point to the given value.
		 * @param x:		Target X-Axis Position.
		 * @param y:		Target Y-Axis Position.
		 * @param value:	Input Value.
		 * @returns	T:		The previous value.
		 */
		T set(const size_t& x, const size_t& y, const T& value)
		{
			const auto& index{ to1D(x, y) };
			const auto& copy{ _arr[index] };
			_arr[index] = value;
			return copy;
		}

		/**
		 * @brief		Get a single row from the matrix.
		 * @param y:	The y-axis index of the row to copy.
		 * @returns		std::array<T, SizeX>
		 */
		std::array<T, SizeX> getRow(const size_t& y) const
		{
			std::array<T, SizeX> arr;
			for (size_t x{ 0 }; x < SizeX; ++x)
				arr[x] = _arr[to1D(x, y)];
			return arr;
		}
		/**
		 * @brief		Set a whole row of the matrix to the given row.
		 * @param y:	The y-axis index of the target row to overwrite.
		 * @param row:	Input Row.
		 */
		void setRow(const size_t& y, const std::array<T, SizeX>& row)
		{
			for (size_t x{ 0 }; x < SizeX; ++x)
				_arr[to1D(x, y)] = row[x];
		}
		/**
		 * @brief		Get a single column from the matrix.
		 * @param x:	The x-axis index of the column to copy.
		 * @returns		std::array<T, SizeY>
		 */
		std::array<T, SizeY> getColumn(const size_t& x) const
		{
			std::array<T, SizeY> arr;
			for (size_t y{ 0 }; y < SizeY; ++y)
				arr[y] = _arr[to1D(x, y)];
			return arr;
		}
		/**
		 * @brief		Set a whole column of the matrix to the given column.
		 * @param x:	The x-axis index of the target column to overwrite.
		 * @param col:	Input Column.
		 */
		void setColumn(const size_t& x, const std::array<T, SizeY>& col)
		{
			for (size_t y{ 0 }; y < SizeY; ++y)
				_arr[to1D(x, y)] = col[y];
		}

		/**
		 * @brief			1-Dimensional Indexer Operator.
		 * @param index:	1-Dimensional Index Number.
		 * @returns			T&
		 */
		T& operator[](const size_t& index) { return _arr[index]; }
		/**
		 * @brief			1-Dimensional Indexer Operator.
		 * @param index:	1-Dimensional Index Number.
		 * @returns			T
		 */
		T operator[](const size_t& index) const { return _arr[index]; }

		/**
		 * @brief		Stream insertion operator that is available when (T) has a valid operator<< overload.
		 * @param os:	ostream instance.
		 * @param v2d:	Matrix instance.
		 * @returns		std::ostream&
		 */
		friend std::ostream& operator<<(std::ostream& os, const matrix<T, SizeX, SizeY>& v2d) requires var::Streamable<T, std::ostream>
		{
			for (size_t y{ 0ull }; y < SizeY; ++y) {
				for (size_t x{ 0ull }; x < SizeX; ++x) {
					const auto& here{ v2d.get(x, y) };
					os << here;

					if (x < SizeX - 1) {
						const auto& len{ std::log10<T>(here) + 1 + !!here < 0 };
						const auto ind{ indent(8, len) };
						os << ind;
					}
				}
				if (y < SizeY - 1)
					os << '\n';
			}
			return os;
		}

		/**
		 * @brief				Initialize an array where each element is set to the specified value.
		 * @param defaultValue:	The value to initialize each element of the array with. Defaults to zero.
		 * @returns				std::array<T, SIZE>
		 */
		inline static std::array<T, SIZE> initArray()
		{
			std::array<T, SIZE> out;
			memset(&out, 0, sizeof(std::array<T, SIZE>));
			return out;
		}

		/**
		 * @brief				Initialize a matrix where each element is set to the specified value.
		 * @param defaultValue:	The value to initialize each element of the matrix with. Defaults to zero.
		 * @returns				matrix<T, SizeX, SizeY>
		 */
		inline static matrix<T, SizeX, SizeY> initMatrix()
		{
			matrix<T, SizeX, SizeY> out;
			memset(&out, 0, sizeof(matrix<T, SizeX, SizeY>));
			return out;
		}

		/**
		 * @brief	Transpose the rows of the matrix onto columns of the returned matrix.
		 * @returns	matrix<T, SizeY, SizeX>
		 */
		matrix<T, SizeY, SizeX> transpose() const
		{
			return{ _arr };
		}

	};

	/**
	 * @brief			Get the value of this point as a 1-dimensional index number in the range (0 - (\<Max_X\> * \<Max_Y\>)).
	 * @tparam SizeX:	The size of one "row" (chunk) in the virtual matrix.
	 * @returns	size_t:	1D Index Value.
	 */
	template<typename T, size_t SizeX>
	inline static constexpr size_t to1D(const T& x, const T& y) noexcept
	{
		return (x + (y * SizeX));
	}

	/**
	 * @brief			Set the value of this point using a 1-dimensional index number in the range (0 - (Max_X * Max_Y)).
	 * @tparam SizeX:	The size of one "row" (chunk) in the virtual matrix.
	 * @tparam T:		The integral type used by the desired point.
	 * @param index:	1D Index Value.
	 */
	template<typename T, size_t SizeX>
	inline static constexpr point<T> from1D(const size_t& index) noexcept
	{
		return { index / SizeX, index % SizeX };
	}

	template<typename T, size_t SizeX, size_t SizeY>
	using matrix = basic_matrix<T, SizeX, SizeY>;

	/**
	 * @struct	nmatrix
	 * @brief	Provides an interface for performing linear algebra operations on matricies.
	 */
	template<var::numeric T, size_t SizeX, size_t SizeY>
	struct nmatrix {
		static constexpr const size_t SIZE{ SizeX * SizeY };
		using type = std::array<T, (SizeX* SizeY)>;

	protected:
		type _arr;

		/**
		 * @brief		Convert a 2-dimensional point to a 1-dimensional index number.
		 * @param x:	X-Axis Position.
		 * @param y:	Y-Axis Position.
		 * @returns		size_t
		 */
		inline static constexpr size_t to1D(const size_t& x, const size_t& y)
		{
			return (SizeX * y) + x;
		}
		/**
		 * @brief		Convert a 2-dimensional point to a 1-dimensional index number.
		 * @param pos:	A 2-Dimensional Point.
		 * @returns		size_t
		 */
		inline static constexpr size_t to1D(const point<size_t>& pos)
		{
			return to1D(pos.x, pos.y);
		}
		/**
		 * @brief		Convert a 1-dimensional index number to a 2-dimensional point.
		 * @param n:	A 1-Dimensional Index Number.
		 * @returns		point<size_t>
		 */
		inline static constexpr point<size_t> from1D(const size_t& n)
		{
			return std::make_pair(n / SizeX, n % SizeX);
		}

	public:
		// @brief	Default Constructor.
		nmatrix() : _arr{ initArray() } {}

		nmatrix(matrix<T, SizeX, SizeY>&& o) : _arr{ std::move(o._arr) } {}
		nmatrix(const matrix<T, SizeX, SizeY>& o) : _arr{ o._arr } {}

		template<var::same_or_convertible<T>... Ts> requires (sizeof...(Ts) == SIZE)
			nmatrix(Ts&&... values) : _arr{ static_cast<T>(std::forward<Ts>(values))... } {}

		nmatrix(std::array<T, SIZE>&& arr) : _arr{ std::move(arr) } {}
		nmatrix(const std::array<T, SIZE>& arr) : _arr{ arr } {}

		/**
		 * @brief		Matrix Copy-Assignment Operator.
		 * @param mtx:	Input Matrix.
		 * @returns		matrix<T, SizeX, SizeY>&
		 */
		matrix<T, SizeX, SizeY>& operator=(const matrix<T, SizeX, SizeY>& mtx)
		{
			_arr = mtx._arr;
			return *this;
		}
		/**
		 * @brief		Matrix Move-Assignment Operator.
		 * @param mtx:	Input Matrix.
		 * @returns		matrix<T, SizeX, SizeY>&
		 */
		matrix<T, SizeX, SizeY>& operator=(matrix<T, SizeX, SizeY>&& mtx)
		{
			_arr = std::move(mtx._arr);
			return *this;
		}
		/**
		 * @brief		Matrix Copy-Assignment Operator.
		 * @param arr:	Input Array.
		 * @returns		matrix<T, SizeX, SizeY>&
		 */
		matrix<T, SizeX, SizeY>& operator=(const type& arr)
		{
			_arr = arr;
			return *this;
		}
		/**
		 * @brief		Matrix Move-Assignment Operator.
		 * @param arr:	Input Array.
		 * @returns		matrix<T, SizeX, SizeY>&
		 */
		matrix<T, SizeX, SizeY>& operator=(type&& arr)
		{
			_arr = std::move(arr);
			return *this;
		}

		/**
		 * @brief	Get the beginning array iterator.
		 * @returns	type::const_iterator
		 */
		type::const_iterator begin() const { return _arr.begin(); }
		/**
		 * @brief	Get the ending array iterator.
		 * @returns	type::const_iterator
		 */
		type::const_iterator end() const { return _arr.end(); }
		/**
		 * @brief	Check if the matrix is empty.
		 * @returns	bool
		 */
		bool empty() const { return _arr.empty(); }
		/**
		 * @brief	Get the size of the matrix, in elements.
		 * @returns	size_t
		 */
		size_t size() const { return _arr.size(); }

		/**
		 * @brief		Get the element at the specified point.
		 * @param pos:	Target Point.
		 * @returns		T
		 */
		T get(const point<size_t>& pos) const
		{
			return _arr[to1D(pos)];
		}
		/**
		 * @brief			Set the element at the specified point to the given value.
		 * @param pos:		Target Point.
		 * @param value:	Input Value.
		 * @returns	T:		The previous value.
		 */
		T set(const point<size_t>& pos, const T& value)
		{
			const auto& index{ to1D(pos) };
			const auto& copy{ _arr[index] };
			_arr[index] = value;
			return copy;
		}
		/**
		 * @brief		Get the element at the specified point.
		 * @param x:	Target X-Axis Position.
		 * @param y:	Target Y-Axis Position.
		 * @returns		T
		 */
		T get(const size_t& x, const size_t& y) const
		{
			return get({ x, y });
		}
		/**
		 * @brief			Set the element at the specified point to the given value.
		 * @param x:		Target X-Axis Position.
		 * @param y:		Target Y-Axis Position.
		 * @param value:	Input Value.
		 * @returns	T:		The previous value.
		 */
		T set(const size_t& x, const size_t& y, const T& value)
		{
			const auto& index{ to1D(x, y) };
			const auto& copy{ _arr[index] };
			_arr[index] = value;
			return copy;
		}

		/**
		 * @brief		Get a single row from the matrix.
		 * @param y:	The y-axis index of the row to copy.
		 * @returns		std::array<T, SizeX>
		 */
		std::array<T, SizeX> getRow(const size_t& y) const
		{
			std::array<T, SizeX> arr;
			for (size_t x{ 0 }; x < SizeX; ++x)
				arr[x] = _arr[to1D(x, y)];
			return arr;
		}
		/**
		 * @brief		Set a whole row of the matrix to the given row.
		 * @param y:	The y-axis index of the target row to overwrite.
		 * @param row:	Input Row.
		 */
		void setRow(const size_t& y, const std::array<T, SizeX>& row)
		{
			for (size_t x{ 0 }; x < SizeX; ++x)
				_arr[to1D(x, y)] = row[x];
		}
		/**
		 * @brief		Get a single column from the matrix.
		 * @param x:	The x-axis index of the column to copy.
		 * @returns		std::array<T, SizeY>
		 */
		std::array<T, SizeY> getColumn(const size_t& x) const
		{
			std::array<T, SizeY> arr;
			for (size_t y{ 0 }; y < SizeY; ++y)
				arr[y] = _arr[to1D(x, y)];
			return arr;
		}
		/**
		 * @brief		Set a whole column of the matrix to the given column.
		 * @param x:	The x-axis index of the target column to overwrite.
		 * @param col:	Input Column.
		 */
		void setColumn(const size_t& x, const std::array<T, SizeY>& col)
		{
			for (size_t y{ 0 }; y < SizeY; ++y)
				_arr[to1D(x, y)] = col[y];
		}

		/**
		 * @brief			1-Dimensional Indexer Operator.
		 * @param index:	1-Dimensional Index Number.
		 * @returns			T&
		 */
		T& operator[](const size_t& index) { return _arr[index]; }
		/**
		 * @brief			1-Dimensional Indexer Operator.
		 * @param index:	1-Dimensional Index Number.
		 * @returns			T
		 */
		T operator[](const size_t& index) const { return _arr[index]; }

		/**
		 * @brief		Stream insertion operator that is available when (T) has a valid operator<< overload.
		 * @param os:	ostream instance.
		 * @param v2d:	Matrix instance.
		 * @returns		std::ostream&
		 */
		friend std::ostream& operator<<(std::ostream& os, const matrix<T, SizeX, SizeY>& v2d) requires var::Streamable<T, std::ostream>
		{
			for (size_t y{ 0ull }; y < SizeY; ++y) {
				for (size_t x{ 0ull }; x < SizeX; ++x) {
					const auto& here{ v2d.get(x, y) };
					os << here;

					if (x < SizeX - 1) {
						const auto& len{ std::log10<T>(here) + 1 + !!here < 0 };
						const auto ind{ indent(8, len) };
						os << ind;
					}
				}
				if (y < SizeY - 1)
					os << '\n';
			}
			return os;
		}

		/**
		 * @brief				Initialize an array where each element is set to the specified value.
		 * @param defaultValue:	The value to initialize each element of the array with. Defaults to zero.
		 * @returns				std::array<T, SIZE>
		 */
		inline static std::array<T, SIZE> initArray()
		{
			std::array<T, SIZE> out;
			memset(&out, 0, sizeof(std::array<T, SIZE>));
			return out;
		}

		/**
		 * @brief				Initialize a matrix where each element is set to the specified value.
		 * @param defaultValue:	The value to initialize each element of the matrix with. Defaults to zero.
		 * @returns				matrix<T, SizeX, SizeY>
		 */
		inline static matrix<T, SizeX, SizeY> initMatrix()
		{
			matrix<T, SizeX, SizeY> out;
			memset(&out, 0, sizeof(matrix<T, SizeX, SizeY>));
			return out;
		}

		/**
		 * @brief	Transpose the rows of the matrix onto columns of the returned matrix.
		 * @returns	matrix<T, SizeY, SizeX>
		 */
		matrix<T, SizeY, SizeX> transpose() const
		{
			return{ _arr };
		}

		/**
		 * @brief			Perform a matrix multiplication operation on this matrix and another given matrix.
		 *\n				Matrix multiplication requires that the input matrix has the same number of columns as this matrix has rows.
		 * @tparam OtherX	The (variable) width of the other matrix.
		 * @param mtx		Input Matrix to Multiply By.
		 * @returns			matrix<T, OtherX, SizeY>
		 */
		template<size_t OtherX>
		[[nodiscard]] matrix<T, OtherX, SizeY> operator*(const matrix<T, OtherX, SizeX>& mtx) const
		{
			auto out{ initMatrix() };
			for (size_t y{ 0ull }, j{ 0 }; y < SizeY; ++y) { // iterate through row indexes (y value)
				for (size_t x{ 0ull }; x < OtherX; ++x, ++j) { // iterate through column indexes (x value)
					const auto& row{ getRow(y) }, col{ mtx.getColumn(x) };
					for (size_t i{ 0ull }; i < SizeX; ++i)
						out[j] += ((row[i]) * (col[i]));
				}
			}
			return out;
		}

		/**
		 * @brief		Apply the given function to each element of this matrix.
		 * @param f:	A function to apply to each element, accepting each element's value as input.
		 * @returns		matrix<T, SizeX, SizeY>&
		 */
		matrix<T, SizeX, SizeY>& apply(const std::function<T(T)>& f)
		{
			for (size_t i{ 0 }; i < SIZE; ++i)
				_arr[i] = f(_arr[i]);
			return *this;
		}
		/**
		 * @brief		Apply the given function to each element of this matrix and another matrix.
		 * @param f:	A function to apply to each element, accepting both the value of this matrix's corresponding element as well as (mtx)'s.
		 * @param mtx:	Another matrix to use as input.
		 * @returns		matrix<T, SizeX, SizeY>&
		 */
		matrix<T, SizeX, SizeY>& apply(const std::function<T(T, T)>& f, const matrix<T, SizeX, SizeY>& mtx)
		{
			for (size_t i{ 0 }; i < SIZE; ++i)
				_arr[i] = f(_arr[i], mtx._arr[i]);
			return *this;
		}
		/**
		 * @brief		Apply the given function to each element of the matrix, and return the resulting matrix without modifying this one.
		 * @param f:	A function to apply to each element, accepting each element's value as input.
		 * @returns		matrix<T, SizeX, SizeY>
		 */
		matrix<T, SizeX, SizeY> apply(const std::function<T(T)>& f) const
		{
			auto out{ initMatrix() };
			for (size_t i{ 0 }; i < SIZE; ++i)
				out[i] = f(_arr[i]);
			return out;
		}
		/**
		 * @brief		Apply the given function to each element of the matrix and another matrix, and return the resulting matrix without modifying this one.
		 * @param f:	A function to apply to each element, accepting both the value of this matrix's corresponding element as well as (mtx)'s.
		 * @param mtx:	Another matrix to use as input.
		 * @returns		matrix<T, SizeX, SizeY>
		 */
		matrix<T, SizeX, SizeY> apply(const std::function<T(T, T)>& f, const matrix<T, SizeX, SizeY>& mtx) const
		{
			auto out{ initMatrix() };
			for (size_t i{ 0 }; i < SIZE; ++i)
				out[i] = f(_arr[i], mtx._arr[i]);
			return out;
		}

		/**
		 * @brief			Matrix scalar multiplication operator.
		 * @param mtx:		Input Matrix.
		 * @param scalar:	Scalar multiplier value.
		 * @returns			matrix<T, SizeX, SizeY>
		 */
		template<var::same_or_convertible<T> U>
		[[nodiscard]] friend matrix<T, SizeX, SizeY> operator*(const matrix<T, SizeX, SizeY>& mtx, const U& scalar)
		{
			return mtx.apply([&scalar](const T& v) -> T { return static_cast<T>(scalar) * v; });
		}
		/**
		 * @brief			Matrix scalar multiplication operator.
		 * @param scalar:	Scalar multiplier value.
		 * @returns			matrix<T, SizeX, SizeY>&
		 */
		template<var::same_or_convertible<T> U>
		[[nodiscard]] matrix<T, SizeX, SizeY>& operator*=(const U& scalar)
		{
			return apply([&scalar](const T& v) -> T { return static_cast<T>(scalar) * v; });
		}

		/**
		 * @brief		Matrix negation operator.
		 * @param mtx:	Input Matrix.
		 * @returns		matrix<T, SizeX, SizeY>
		 */
		[[nodiscard]] friend matrix<T, SizeX, SizeY>& operator!(matrix<T, SizeX, SizeY>& mtx)
		{
			return mtx.apply([](const T& v) -> T { return -v; });
		}

		/**
		 * @brief			Matrix addition operator.
		 * @param left:		Input Matrix A.
		 * @param right:	Input Matrix B.
		 * @returns			matrix<T, SizeX, SizeY>
		 */
		[[nodiscard]] friend matrix<T, SizeX, SizeY> operator+(const matrix<T, SizeX, SizeY>& left, const matrix<T, SizeX, SizeY>& right)
		{
			return left.apply([](const T& l, const T& r) -> T { return l + r; }, right);
		}
		/**
		 * @brief		Matrix addition operator.
		 * @param o:	Input matrix.
		 * @returns		matrix<T, SizeX, SizeY>
		 */
		[[nodiscard]] matrix<T, SizeX, SizeY>& operator+=(const matrix<T, SizeX, SizeY>& o)
		{
			return apply([](const T& l, const T& r) -> T { return l + r; }, o);
		}
		/**
		 * @brief			Matrix subtraction operator.
		 * @param left:		Input Matrix A.
		 * @param right:	Input Matrix B.
		 * @returns			matrix<T, SizeX, SizeY>
		 */
		[[nodiscard]] friend matrix<T, SizeX, SizeY> operator-(const matrix<T, SizeX, SizeY>& left, const matrix<T, SizeX, SizeY>& right)
		{
			return left.apply([](const T& l, const T& r) -> T { return l - r; }, right);
		}
		/**
		 * @brief		Matrix subtraction operator.
		 * @param o:	Input matrix.
		 * @returns		matrix<T, SizeX, SizeY>&
		 */
		[[nodiscard]] matrix<T, SizeX, SizeY>& operator-=(const matrix<T, SizeX, SizeY>& o)
		{
			return apply([](const T& l, const T& r) -> T { return l - r; }, o);
		}
	};
#pragma endregion
}
