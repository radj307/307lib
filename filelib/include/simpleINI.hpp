#pragma once
/**
 * @file	simpleINI.hpp
 * @author	radj307
 * @brief	Lightweight, fast, & non-tokenizing header-only INI parsers, containers, and manipulators.
 */
#include <sysarch.h>
#include <make_exception.hpp>
#include <var.hpp>
#include <str.hpp>
#include <fileio.hpp>
#include <fileutil.hpp>
#include <event.hpp>

#include <algorithm>
#include <compare>
#include <concepts>
#include <filesystem>
#include <optional>
#include <sstream>
#include <string>
#include <string_view>
#include <unordered_map>

namespace ini {
#pragma region KeyComparators
	/**
	 * @struct	CaseInsensitiveCompare
	 * @brief	**Case-Insensitive** Key Comparator
	 */
	struct CaseInsensitiveCompare {
		bool operator()(const std::string& l, const std::string& r) const noexcept { return str::tolower(l) == str::tolower(r); }
	};
	/**
	 * @struct	CaseSensitiveCompare
	 * @brief	String comparison object that does not ignore case.
	 */
	struct CaseSensitiveCompare {

		bool operator()(const std::string& l, const std::string& r) const noexcept { return l == r; }
	};
#pragma endregion KeyComparators

	$DefineExcept(ini_cast_exception);

#pragma region ini_value
	/**
	 * @class		ini_value
	 * @brief		Wraps the std::string class, and adds additional functionality such as automatic type-casting.
	 * @details		The ini_value class is designed to allow other types to be used for values interchangeably by internally casting them to and from strings.
	 */
	class ini_value {
		std::string _value;
		using this_t = ini_value;

	public:
	#pragma region usings
		using traits_type = typename std::string::traits_type;
		using allocator_type = typename std::string::allocator_type;

		using value_type = typename std::string::value_type;
		using size_type = typename std::string::size_type;
		using difference_type = typename std::string::difference_type;
		using pointer = typename std::string::pointer;
		using const_pointer = typename std::string::const_pointer;
		using reference = typename std::string::reference;
		using const_reference = typename std::string::const_reference;

		using iterator = typename std::string::iterator;
		using const_iterator = typename std::string::const_iterator;

		using reverse_iterator = typename std::string::reverse_iterator;
		using const_reverse_iterator = typename std::string::const_reverse_iterator;
	#pragma endregion usings

	#pragma region constructors
		/// @brief	default ctor
		CONSTEXPR ini_value() {}
		/// @brief	string (move) ctor
		CONSTEXPR ini_value(std::string&& s) : _value{ std::move(s) } {}
		/// @brief	string (copy) ctor
		CONSTEXPR ini_value(std::string const& s) : _value{ s } {}
		/// @brief	cstring ctor
		CONSTEXPR ini_value(const char* s) : _value{ s } {}
		/// @brief	Ctor that accepts a boolean to convert to a string representation.
		CONSTEXPR ini_value(const bool& state) : _value{ str::frombool(state) } {}
		/// @brief	convertible (move) ctor
		template<std::convertible_to<std::string> T> requires (!std::same_as<T, std::string>)
			CONSTEXPR ini_value(T&& v) : _value{ std::string{ std::forward<T>(v) } } {}
		/// @brief	convertible (copy) ctor
		template<std::convertible_to<std::string> T> requires (!std::same_as<T, std::string>)
			CONSTEXPR ini_value(const T& v) : _value{ std::string{ v } } {}
		/**
		 * @brief						Ctor that accepts types that aren't implicitly convertible to std::string.
		 * @param v						A value that cannot be implicitly converted to std::string
		 * @param converter_function	A function that converts from T to std::string
		 */
		template<typename T, var::function<std::string, T> TConverter> requires (!std::convertible_to<T, std::string>)
			CONSTEXPR ini_value(T&& v, const TConverter& converter_function) : _value{ converter_function(std::forward<T>(v)) } {}
		/**
		 * @brief						Ctor that accepts types that aren't implicitly convertible to std::string.
		 * @param v						A value that cannot be implicitly converted to std::string
		 * @param converter_function	A function that converts from T to std::string
		 */
		template<typename T, var::function<std::string, T> TConverter> requires (!std::convertible_to<T, std::string>)
			CONSTEXPR ini_value(const T& v, const TConverter& converter_function) : _value{ converter_function(v) } {}
	#pragma endregion constructors

	#pragma region string_methods
		// Manually forwarded from: https://en.cppreference.com/w/cpp/string/basic_string

		CONSTEXPR this_t& assign(size_type count, value_type ch) { _value.assign(count, ch); return *this; }
		CONSTEXPR this_t& assign(const this_t& str) { _value.assign(str._value); return *this; }
		CONSTEXPR this_t& assign(const std::string& str) { _value.assign(str); return *this; }
		CONSTEXPR this_t& assign(const this_t& str, size_type pos, size_type count = std::string::npos) { _value.assign(str._value, pos, count); return *this; }
		CONSTEXPR this_t& assign(const std::string& str, size_type pos, size_type count = std::string::npos) { _value.assign(str, pos, count); return *this; }
		CONSTEXPR this_t& assign(this_t&& str) noexcept { _value.assign(std::move(str)._value); return *this; }
		CONSTEXPR this_t& assign(std::string&& str) noexcept { _value.assign(std::move(str)); return *this; }
		template<class StringViewLike> CONSTEXPR this_t& assign(const StringViewLike& t, size_type pos, size_type count = std::string::npos) { _value.assign(t, pos, count); return *this; }

		CONSTEXPR allocator_type get_allocator() const noexcept { return _value.get_allocator(); }

		CONSTEXPR reference at(const size_type& pos) { return _value.at(pos); }
		CONSTEXPR const_reference at(const size_type& pos) const { return _value.at(pos); }

		CONSTEXPR reference operator[](const size_t& idx) noexcept { return _value[idx]; }
		CONSTEXPR const_reference operator[](const size_t& idx) const noexcept { return _value[idx]; }

		CONSTEXPR reference front() { return _value.front(); }
		CONSTEXPR const_reference front() const { return _value.front(); }

		CONSTEXPR reference back() { return _value.back(); }
		CONSTEXPR const_reference back() const { return _value.back(); }

		CONSTEXPR pointer data() noexcept { return _value.data(); }
		CONSTEXPR const_pointer data() const noexcept { return _value.data(); }

		CONSTEXPR const_pointer c_str() const noexcept { return _value.c_str(); }

		CONSTEXPR operator std::string_view() const noexcept { return _value.operator std::basic_string_view<char, std::char_traits<char>>(); }

		CONSTEXPR iterator begin() noexcept { return _value.begin(); }
		CONSTEXPR const_iterator begin() const noexcept { return _value.begin(); }
		CONSTEXPR const_iterator cbegin() const noexcept { return _value.cbegin(); }

		CONSTEXPR iterator end() noexcept { return _value.end(); }
		CONSTEXPR const_iterator end() const noexcept { return _value.end(); }
		CONSTEXPR const_iterator cend() const noexcept { return _value.cend(); }

		CONSTEXPR reverse_iterator rbegin() noexcept { return _value.rbegin(); }
		CONSTEXPR const_reverse_iterator rbegin() const noexcept { return _value.rbegin(); }
		CONSTEXPR const_reverse_iterator crbegin() const noexcept { return _value.crbegin(); }

		CONSTEXPR reverse_iterator rend() noexcept { return _value.rend(); }
		CONSTEXPR const_reverse_iterator rend() const noexcept { return _value.rend(); }
		CONSTEXPR const_reverse_iterator crend() const noexcept { return _value.crend(); }

		[[nodiscard]] CONSTEXPR bool empty() const noexcept { return _value.empty(); }

		CONSTEXPR size_type size() const noexcept { return _value.size(); }
		CONSTEXPR size_type length() const noexcept { return _value.length(); }

		CONSTEXPR size_type max_size() const noexcept { return _value.max_size(); }

		CONSTEXPR void reserve(const size_type& newCapacity) { _value.reserve(newCapacity); }

		CONSTEXPR size_type capacity() const noexcept { return _value.capacity(); }

		CONSTEXPR void shrink_to_fit() { _value.shrink_to_fit(); }

		CONSTEXPR void clear() noexcept { _value.clear(); }

		CONSTEXPR this_t& insert(const size_type& index, const size_type& count, const value_type& c) { _value.insert(index, count, c); return *this; }
		CONSTEXPR this_t& insert(const size_type& index, const value_type* s) { _value.insert(index, s); return *this; }
		CONSTEXPR this_t& insert(const size_type& index, const value_type* s, const size_type& count) { _value.insert(index, s, count); return *this; }
		CONSTEXPR this_t& insert(const size_type& index, const std::string& str) { _value.insert(index, str); return *this; }
		CONSTEXPR this_t& insert(const size_type& index, const std::string& str, const size_type& index_str, const size_type& count = std::string::npos) { _value.insert(index, str, index_str, count); return *this; }
		CONSTEXPR iterator insert(const_iterator pos, const value_type& ch) { return _value.insert(pos, ch); }
		CONSTEXPR iterator insert(const_iterator pos, const size_type& count, const value_type& ch) { return _value.insert(pos, count, ch); }
		template<class InputIt> CONSTEXPR iterator insert(const_iterator pos, InputIt first, InputIt last) { return _value.insert(pos, first, last); }
		CONSTEXPR iterator insert(const_iterator pos, std::initializer_list<value_type> ilist) { return _value.insert(pos, ilist); }
		template<class StringViewLike> CONSTEXPR this_t& insert(const size_type& pos, const StringViewLike& t) { _value.insert(pos, t); return *this; }
		template<class StringViewLike> CONSTEXPR this_t& insert(const size_type& index, const StringViewLike& t, const size_type& index_str, const size_type& count = std::string::npos) { _value.insert(index, t, index_str, count); return *this; }

		CONSTEXPR this_t& erase(const size_type& index = 0, const size_type& count = std::string::npos) { _value.erase(index, count); return *this; }
		CONSTEXPR iterator erase(const_iterator position) { return _value.erase(position); }
		CONSTEXPR iterator erase(const_iterator first, const_iterator last) { return _value.erase(first, last); }

		CONSTEXPR void push_back(const value_type& ch) { _value.push_back(ch); }
		CONSTEXPR void pop_back() { _value.pop_back(); }

		CONSTEXPR this_t& append(size_type count, value_type ch) { _value.append(count, ch); return *this; }
		CONSTEXPR this_t& append(const this_t& str) { _value.append(str); return *this; }
		CONSTEXPR this_t& append(const this_t& str, size_type pos, size_type count = std::string::npos) { _value.append(str, pos, count); return *this; }
		CONSTEXPR this_t& append(const value_type* s, size_type count) { _value.append(s, count); return *this; }
		CONSTEXPR this_t& append(const value_type* s) { _value.append(s); return *this; }
		template<class InputIt> CONSTEXPR this_t& append(InputIt first, InputIt last) { _value.append(first, last); return *this; }
		CONSTEXPR this_t& append(std::initializer_list<value_type> ilist) { _value.append(ilist); return *this; }
		template<class StringViewLike> CONSTEXPR this_t& append(const StringViewLike& t) { _value.append(t); return *this; }
		template<class StringViewLike> CONSTEXPR this_t& append(const StringViewLike& t, size_type pos, size_type count = std::string::npos) { _value.append(t, pos, count); return *this; }

		CONSTEXPR this_t& operator+=(const std::string& str) { _value += str; return *this; }
		CONSTEXPR this_t& operator+=(value_type ch) { _value += ch; return *this; }
		CONSTEXPR this_t& operator+=(const value_type* s) { _value += s; return *this; }
		CONSTEXPR this_t& operator+=(std::initializer_list<value_type> ilist) { _value += ilist; return *this; }
		template<class StringViewLike> CONSTEXPR this_t& operator+=(const StringViewLike& t) { _value += t; return *this; }

		CONSTEXPR int compare(const this_t& str) const noexcept { return _value.compare(str._value); }
		CONSTEXPR int compare(const std::string& str) const noexcept { return _value.compare(str); }
		CONSTEXPR int compare(size_type pos1, size_type count1, const this_t& str) const { return _value.compare(pos1, count1, str._value); }
		CONSTEXPR int compare(size_type pos1, size_type count1, const std::string& str) const { return _value.compare(pos1, count1, str); }
		CONSTEXPR int compare(size_type pos1, size_type count1, const this_t& str, size_type pos2, size_type count2 = std::string::npos) const { return _value.compare(pos1, count1, str._value, pos2, count2); }
		CONSTEXPR int compare(size_type pos1, size_type count1, const std::string& str, size_type pos2, size_type count2 = std::string::npos) const { return _value.compare(pos1, count1, str, pos2, count2); }
		CONSTEXPR int compare(const value_type* s) const { return _value.compare(s); }
		CONSTEXPR int compare(size_type pos1, size_type count1, const value_type* s) const { return _value.compare(pos1, count1, s); }
		CONSTEXPR int compare(size_type pos1, size_type count1, const value_type* s, size_type count2) const { return _value.compare(pos1, count1, s, count2); }
		template<class StringViewLike> CONSTEXPR int compare(const StringViewLike& t) const noexcept { return _value.compare(t); }
		template<class StringViewLike> CONSTEXPR int compare(size_type pos1, size_type count1, const StringViewLike& t) const { return _value.compare(pos1, count1, t); }
		template<class StringViewLike> CONSTEXPR int compare(size_type pos1, size_type count1, const StringViewLike& t, size_type pos2, size_type count2 = std::string::npos) const { return _value.compare(pos1, count1, t, pos2, count2); }

		CONSTEXPR bool starts_with(std::string_view sv) const noexcept { return _value.starts_with(sv); }
		CONSTEXPR bool starts_with(value_type c) const noexcept { return _value.starts_with(c); }
		CONSTEXPR bool starts_with(const value_type* s) const { return _value.starts_with(s); }

		CONSTEXPR bool ends_with(std::string_view sv) const noexcept { return _value.ends_with(sv); }
		CONSTEXPR bool ends_with(value_type c) const noexcept { return _value.ends_with(c); }
		CONSTEXPR bool ends_with(const value_type* s) const { return _value.ends_with(s); }

	#if LANG_CPP >= 23
		CONSTEXPR bool contains(std::string_view sv) const noexcept { return _value.contains(sv); }
		CONSTEXPR bool contains(value_type c) const noexcept { return _value.contains(c); }
		CONSTEXPR bool contains(const value_type* s) const { return _value.contains(s); }
	#endif

		//< TODO: std::string::replace

		CONSTEXPR this_t substr(size_type pos = 0, size_type count = std::string::npos) const { return{ _value.substr(pos, count) }; }

		CONSTEXPR size_type copy(value_type* dest, size_type count, size_type pos = 0) const { return _value.copy(dest, count, pos); }

		CONSTEXPR void resize(const size_type& count) { _value.resize(count); }
		CONSTEXPR void resize(const size_type& count, value_type ch) { _value.resize(count, ch); }

	#if LANG_CPP >= 23
		template<class Operation> CONSTEXPR void resize_and_overwrite(size_type count, Operation op) { _value.resize_and_overwrite(count, op); }
	#endif

		CONSTEXPR void swap(std::string& other) noexcept { _value.swap(other); }

		CONSTEXPR size_type find(const std::string str, const size_type pos = 0) const noexcept { return _value.find(str, pos); }
		CONSTEXPR size_type find(const value_type* s, const size_type pos, const size_type count) const { return _value.find(s, pos, count); }
		CONSTEXPR size_type find(const value_type* s, const size_type pos = 0) const { return _value.find(s, pos); }
		CONSTEXPR size_type find(const value_type ch, const size_type pos = 0) const noexcept { return _value.find(ch, pos); }
		template<class StringViewLike> CONSTEXPR size_type find(const StringViewLike& t, size_type pos = 0) const noexcept { return _value.find(t, pos); }

		CONSTEXPR size_type rfind(const std::string str, const size_type pos = 0) const noexcept { return _value.rfind(str, pos); }
		CONSTEXPR size_type rfind(const value_type* s, const size_type pos, const size_type count) const { return _value.rfind(s, pos, count); }
		CONSTEXPR size_type rfind(const value_type* s, const size_type pos = 0) const { return _value.rfind(s, pos); }
		CONSTEXPR size_type rfind(const value_type ch, const size_type pos = 0) const noexcept { return _value.rfind(ch, pos); }
		template<class StringViewLike> CONSTEXPR size_type rfind(const StringViewLike& t, size_type pos = 0) const noexcept { return _value.rfind(t, pos); }

		CONSTEXPR size_type find_first_of(const std::string& str, const size_type pos = 0) const noexcept { return _value.find_first_of(str, pos); }
		CONSTEXPR size_type find_first_of(const value_type* s, const size_type pos, const size_type count) const { return _value.find_first_of(s, pos, count); }
		CONSTEXPR size_type find_first_of(const value_type* s, const size_type pos = 0) const { return _value.find_first_of(s, pos); }
		CONSTEXPR size_type find_first_of(const value_type ch, const size_type pos = 0) const noexcept { return _value.find_first_of(ch, pos); }
		template<class StringViewLike> CONSTEXPR size_type find_first_of(const StringViewLike& t, const size_type pos = 0) const noexcept { return _value.find_first_of(t, pos); }

		CONSTEXPR size_type find_first_not_of(const std::string& str, const size_type pos = 0) const noexcept { return _value.find_first_not_of(str, pos); }
		CONSTEXPR size_type find_first_not_of(const value_type* s, const size_type pos, const size_type count) const { return _value.find_first_not_of(s, pos, count); }
		CONSTEXPR size_type find_first_not_of(const value_type* s, const size_type pos = 0) const { return _value.find_first_not_of(s, pos); }
		CONSTEXPR size_type find_first_not_of(const value_type ch, const size_type pos = 0) const noexcept { return _value.find_first_not_of(ch, pos); }
		template<class StringViewLike> CONSTEXPR size_type find_first_not_of(const StringViewLike& t, const size_type pos = 0) const noexcept { return _value.find_first_not_of(t, pos); }

		CONSTEXPR size_type find_last_of(const std::string& str, const size_type pos = 0) const noexcept { return _value.find_last_of(str, pos); }
		CONSTEXPR size_type find_last_of(const value_type* s, const size_type pos, const size_type count) const { return _value.find_last_of(s, pos, count); }
		CONSTEXPR size_type find_last_of(const value_type* s, const size_type pos = 0) const { return _value.find_last_of(s, pos); }
		CONSTEXPR size_type find_last_of(const value_type ch, const size_type pos = 0) const noexcept { return _value.find_last_of(ch, pos); }
		template<class StringViewLike> CONSTEXPR size_type find_last_of(const StringViewLike& t, const size_type pos = 0) const noexcept { return _value.find_first_of(t, pos); }

		CONSTEXPR size_type find_last_not_of(const std::string& str, const size_type pos = 0) const noexcept { return _value.find_last_not_of(str, pos); }
		CONSTEXPR size_type find_last_not_of(const value_type* s, const size_type pos, const size_type count) const { return _value.find_last_not_of(s, pos, count); }
		CONSTEXPR size_type find_last_not_of(const value_type* s, const size_type pos = 0) const { return _value.find_last_not_of(s, pos); }
		CONSTEXPR size_type find_last_not_of(const value_type ch, const size_type pos = 0) const noexcept { return _value.find_last_not_of(ch, pos); }
		template<class StringViewLike> CONSTEXPR size_type find_last_not_of(const StringViewLike& t, const size_type pos = 0) const noexcept { return _value.find_last_not_of(t, pos); }
	#pragma endregion string_methods

	#pragma region cast_operators
		CONSTEXPR operator std::string& () noexcept { return _value; }
		CONSTEXPR operator std::string() const noexcept { return _value; }
		/// @brief	Bool casting operator.
		CONSTEXPR explicit operator bool() const noexcept { return str::tobool(_value).value_or(false); }
		/// @brief	Numeric casting operator.
		template<var::numeric T> CONSTEXPR explicit operator T() const noexcept { return str::tonumber<T>(_value); }
	#pragma endregion cast_operators

	#pragma region set
		this_t& set(std::string&& s) { this->operator=(std::move(s)); return *this; }
		this_t& set(std::string const& s) { this->operator=(s); return *this; }
		/// @brief	Sets this instance to a given value type.
		template<std::convertible_to<std::string> T>
		this_t& set(T&& v) { this->operator=(std::string{ std::forward<T>(v) }); return *this; }
		template<std::convertible_to<std::string> T>
		this_t& set(const T& v) { this->operator=(std::string{ v }); return *this; }
		template<typename T, var::function<std::string, T> TConverter> requires (!std::convertible_to<T, std::string>)
			this_t& set(T&& v, const TConverter& converter_function) { this->operator=(converter_function(std::forward<T>(v))); return *this; }
		template<typename T, var::function<std::string, T> TConverter> requires (!std::convertible_to<T, std::string>)
			this_t& set(const T& v, const TConverter& converter_function) { this->operator=(converter_function(v)); return *this; }
	#pragma endregion set

	#pragma region operator=
		template<std::convertible_to<std::string> T> requires (!std::same_as<std::string, T>)
			this_t& operator=(T&& value)
		{
			_value = std::string{ std::move(value) };
			return *this;
		}
		template<std::convertible_to<std::string> T> requires (!std::same_as<std::string, T>)
			this_t& operator=(const T& value)
		{
			_value = std::string{ value };
			return *this;
		}
		template<std::floating_point T>
		this_t& operator=(const T& fp)
		{
			_value = std::to_string(fp);
			return *this;
		}
		template<std::integral T>
		this_t& operator=(const T& integral)
		{
			_value = std::to_string(integral);
			return *this;
		}
		this_t& operator=(const bool& boolean)
		{
			_value = str::bool_to_string(boolean);
			return *this;
		}
		this_t& operator=(const std::string& s)
		{
			_value = s;
			return *this;
		}
		this_t& operator=(std::string&& s)
		{
			_value = std::move(s);
			return *this;
		}
	#pragma endregion operator=

	#pragma region operator<=>
		CONSTEXPR auto operator<=>(const ini_value& o) const = default;
		CONSTEXPR auto operator<=>(const std::string& s) const { return _value <=> s; }
	#pragma endregion operator<=>

	#pragma region cast
		template<std::same_as<std::string> T>
		CONSTEXPR T cast() const noexcept { return _value; }
		template<std::integral T> requires std::convertible_to<long long, T> || std::convertible_to<unsigned long long, T>
		CONSTEXPR T cast(const uint8_t base = 10) const noexcept(false)
		{
			if (const auto& n{ str::tonumber<T>(_value, base) }; n.has_value())
				return n.value();
			else throw ex::make_custom_exception<ini_cast_exception>("Value string '", _value, "' does not specify a valid integral number!");
		}
		template<std::floating_point T> requires std::convertible_to<long double, T>
		CONSTEXPR T cast(const std::chars_format fmt = std::chars_format::general) const noexcept(false)
		{
			if (const auto& n{ str::tonumber<T>(_value, fmt) }; n.has_value())
				return n.value();
			else throw ex::make_custom_exception<ini_cast_exception>("Value string '", _value, "' does not specify a valid floating-point number!");
		}
		template<var::convertible_from<std::string> T>
		CONSTEXPR T cast() const noexcept { return T{ _value }; }
		template<typename T, var::function<T, std::string> TConverter>
		CONSTEXPR T cast(const TConverter& converter_function) const noexcept(false)
		{
			return converter_function(_value);
		}
	#pragma endregion cast

	#pragma region as
		template<std::same_as<std::string> T>
		CONSTEXPR std::optional<T> as() const noexcept { return _value; }
		template<std::integral T>
		CONSTEXPR std::optional<T> as(const uint8_t base = 10) const noexcept
		{
			if ((base < 2 || base > 32) && str::isnumber<T>(_value, base)) {
				if constexpr (std::unsigned_integral<T>)
					return $c(T, std::stoull(_value, nullptr, base));
				else return $c(T, std::stoll(_value, nullptr, base));
			}
			return std::nullopt;
		}
		template<std::floating_point T>
		CONSTEXPR std::optional<T> as(const uint8_t base = 10) const noexcept
		{
			if ((base < 2 || base > 32) && str::isnumber<T>(_value, base))
				return $c(T, std::stold(_value));
			return std::nullopt;
		}
		template<var::convertible_from<std::string> T>
		CONSTEXPR std::optional<T> as() const noexcept { return T{ _value }; }
		template<typename T, var::function<T, std::string> TConverter>
		CONSTEXPR std::optional<T> as(const TConverter& converter_function) const noexcept { return converter_function(_value); }
	#pragma endregion as

	#pragma region operators<<&>>
		template<typename TChar, typename TCharTraits>
		friend std::basic_ostream<TChar, TCharTraits>& operator<<(std::basic_ostream<TChar, TCharTraits>& os, const ini_value& v)
		{
			return os << v._value;
		}
		template<typename TChar, typename TCharTraits>
		friend std::basic_istream<TChar, TCharTraits>& operator>>(std::basic_istream<TChar, TCharTraits>& is, ini_value& v)
		{
			return is >> v._value;
		}
	#pragma endregion operators<<&>>
	};
#pragma endregion ini_value

#pragma region ContainerTypes
	/// @brief	An inner section of an INI storage object. Throughout the documentation, this type is known as a 'section' and its keys are known as 'Keys', while its values are known as 'Values'.
	template<class TKeyComparator = CaseInsensitiveCompare>
	using ini_section = typename std::unordered_map<std::string, ini_value, std::hash<std::string>, TKeyComparator>;

	/// @brief	The outer container of an INI storage object. Throughout the documentation, this type is known as a 'map' or 'container' and its keys are known as 'Headers', while its values are known as 'Sections'.
	template<class TKeyComparator = CaseInsensitiveCompare>
	using ini_container = typename std::unordered_map<std::string, ini_section<TKeyComparator>, std::hash<std::string>, TKeyComparator>;

	template<class TKeyComparator = CaseInsensitiveCompare>
	using ini_mask = ini_container<TKeyComparator>;
#pragma endregion ContainerTypes

#pragma region Enum
	/**
	 * @enum	OverrideStyle
	 * @brief	Determines how to handle duplicate keys.
	 */
	enum class OverrideStyle : unsigned char {
		/// @brief	When a duplicate key is encountered, throw an `ini_key_exception`.
		Throw,
		/// @brief	When a duplicate key is encountered, override the existing key.
		Override,
		/// @brief	When a duplicate key is encountered, skip it.
		Skip,
		/// @brief	When a duplicate key is encountered, skip it; unless the *current* value is blank, in which case override it.
		OnlyBlank,
	};
	/**
	 * @enum	SyntaxErrorStyle
	 * @brief	Determines how to handle syntax errors on a line.
	 */
	enum class SyntaxErrorStyle : unsigned char {
		/// @brief	When the syntax on a line is invalid, throw an `ini_syntax_exception`.
		Throw,
		/// @brief	When the syntax on a line is invalid, ignore it and continue. Header lines may have preceding or trailing non-whitespace/comment characters, but they are not parsed!
		Ignore,
	};
	/**
	 * @enum	MaskStyle
	 * @brief	Determines how INI masks are handled.
	 */
	enum class MaskStyle : unsigned char {
		/// @brief	Masking is disabled.
		Disable,
		/// @brief	When a header or key that is not specified in the mask is encountered, ignore it.
		Skip,
		/// @brief	When a header or key that is not specified in the mask is encountered, throw an `ini_mask_exception`
		Throw,
	};
#pragma endregion Enum

	/**
	 * @struct	ini_parser_config
	 * @brief	Exposes properties that can be used to configure the INI parser's behaviour.
	 */
	template<class TKeyComparator = CaseInsensitiveCompare>
	struct ini_parser_config {
	private:
		using this_t = ini_parser_config<TKeyComparator>;
		using container_t = ini_container<TKeyComparator>;
		using mask_t = ini_mask<TKeyComparator>;

	public:
		/// @brief	An optional preconstructed container to use as a mask.
		mask_t mask;
		/// @brief	Determines how the optional mask is used.
		MaskStyle maskStyle{ MaskStyle::Skip };
		/// @brief	When true, any mask headers/keys not present in the input are added to it by the parser.
		bool addMaskToOutput{ true };
		/// @brief	Determines how to handle duplicate key-value pairs.
		OverrideStyle overrideStyle{ OverrideStyle::Override };
		/// @brief	Determines how syntax errors are handled.
		SyntaxErrorStyle syntaxErrorStyle{ SyntaxErrorStyle::Ignore };
		/// @brief	When true, enclosing quotation marks are not included as part of value strings; otherwise the quotes are included in the value string.
		bool stripEnclosingQuotes{ true };
		/// @brief	When true, preceding/trailing whitespace is removed from values. \n (You can enclose values with a pair of single (') or double (") quotes to preserve whitespace located within the quotes.)
		bool stripWhitespaceFromValue{ true };
		/// @brief	When true, all preceding/trailing whitespace within header names is removed.
		bool stripWhitespaceFromHeaders{ false };
		/// @brief	Defines all of the characters that *(individually)* act as escape characters.
		std::string escapeChars{ 1ull, '\\' };
		/// @brief	Defines all of the characters that *(individually)* indicate a line comment.
		std::string commentChars{ std::string{ 1ull, $c(char, '#') } + std::string{ 1ull, $c(char, ';') } };

	#pragma region Setters
		/// @brief	Pipeline operator that sets the value of the `mask` property.
		CONSTEXPR this_t& SetMask(container_t const& v) noexcept { mask = v; return *this; }
		/// @brief	Pipeline operator that sets the value of the `maskStyle` property.
		CONSTEXPR this_t& SetMaskStyle(MaskStyle const& v) noexcept { maskStyle = v; return *this; }
		/// @brief	Pipeline operator that sets the value of the `addMaskToOutput` property.
		CONSTEXPR this_t& SetAddMaskToOutput(bool const& v) noexcept { addMaskToOutput = v; return *this; }
		/// @brief	Pipeline operator that sets the value of the `overrideStyle` property.
		CONSTEXPR this_t& SetOverrideStyle(OverrideStyle const& v) noexcept { overrideStyle = v; return *this; }
		/// @brief	Pipeline operator that sets the value of the `syntaxErrorStyle` property.
		CONSTEXPR this_t& SetSyntaxErrorStyle(SyntaxErrorStyle const& v) noexcept { syntaxErrorStyle = v; return *this; }
		/// @brief	Pipeline operator that sets the value of the `stripEnclosingQuotes` property.
		CONSTEXPR this_t& SetStripEnclosingQuotes(bool const& v) noexcept { stripEnclosingQuotes = v; return *this; }
		/// @brief	Pipeline operator that sets the value of the `stripWhitespaceFromValue` property.
		CONSTEXPR this_t& SetStripWhitespaceFromValue(bool const& v) noexcept { stripWhitespaceFromValue = v; return *this; }
		/// @brief	Pipeline operator that sets the value of the `stripWhitespaceFromHeaders` property.
		CONSTEXPR this_t& SetStripWhitespaceFromHeaders(bool const& v) noexcept { stripWhitespaceFromHeaders = v; return *this; }
		/// @brief	Pipeline operator that sets the value of the `escapeChars` property.
		CONSTEXPR this_t& SetEscapeChars(std::string const& v) noexcept { escapeChars = v; return *this; }
		/// @brief	Pipeline operator that sets the value of the `commentChars` property.
		CONSTEXPR this_t& SetCommentChars(std::string const& v) noexcept { commentChars = v; return *this; }
	#pragma endregion Setters

	#pragma region Methods
		/// @returns	true when the mask contains the given header, or the mask is empty *(it wasn't specified)*; otherwise false.
		WINCONSTEXPR bool _$maskIncludes(const std::string& header) const noexcept
		{
			return mask.empty() || mask.contains(header);
		}
		/// @returns	true when the mask contains the given header/key, or the mask is empty *(it wasn't specified)*; otherwise false.
		WINCONSTEXPR bool _$maskIncludes(const std::string& header, const std::string& key) const noexcept
		{
			return mask.empty() || (mask.contains(header) && mask.at(header).contains(key));
		}
		/// @returns	true if the given character is an escape character; otherwise false.
		CONSTEXPR bool _$isEscapeChar(const char c) const noexcept
		{
			return (escapeChars.find(c) != std::string::npos);
		}
		/// @returns	true if the given character is a comment character; otherwise false.
		CONSTEXPR bool _$isCommentChar(const char c) const noexcept
		{
			return (commentChars.find(c) != std::string::npos);
		}
		/// @returns	A string_view of the parameter 'line' that excludes any line comments, as determined by the commentStyle & some mild parsing.
		WINCONSTEXPR std::string_view _$stripComments(std::string const& line) const noexcept
		{
			bool escaped{ false }, single_quoted{ false }, double_quoted{ false };
			for (size_t i{ 0 }, end{ line.size() }; i < end; ++i) {
				const char& c{ line.at(i) };

				if (escaped)
					escaped = false;
				else if (_$isEscapeChar(c))
					escaped = true;
				else if (c == '\'')
					single_quoted = !single_quoted;
				else if (c == '\"')
					double_quoted = !double_quoted;
				else if (!single_quoted && !double_quoted && _$isCommentChar(c))
					return std::string_view{ line.begin(), line.begin() + i };
				// else continue
			}
			return std::string_view{ line };
		}
		/// @returns	A pair of iterators pointing to enclosing *(must be matching & both unescaped)* quotation marks in the given string. If no valid enclosing quotes are found, returns { value.end(), value.end() } instead.
		WINCONSTEXPR std::pair<typename std::string::const_iterator, typename std::string::const_iterator> _$findEnclosingQuotes(std::string const& value) const noexcept
		{
			size_t i;
			char delim;
			const auto& findDelim{ [&value, &i, &delim, this](auto&& ch) {
				const auto& idx{ i++ };
				return ($fwd(ch) == delim && (idx == 0 || (idx > 0 && !_$isEscapeChar(value.at(idx)))));
			} };

			// find double quotes
			i = 0ull;
			delim = '\"';
			if (const auto& fst{ std::find_if(value.begin(), value.end(), findDelim) }; fst != value.end()) {
				if (const auto& snd{ std::find_if(fst + 1, value.end(), findDelim) }; snd != value.end()) {
					return{ fst, snd };
				}
			}

			// find single quotes
			i = 0ull;
			delim = '\'';
			if (const auto& fst{ std::find_if(value.begin(), value.end(), findDelim) }; fst != value.end()) {
				if (const auto& snd{ std::find_if(fst + 1, value.end(), findDelim) }; snd != value.end()) {
					return{ fst, snd };
				}
			}
			return{ value.end(), value.end() };
		}
	#pragma endregion Methods
	};

	/// @brief	INI syntax error exception.
	$DefineExcept(ini_syntax_exception);
	/// @brief	Duplicate INI key exception.
	$DefineExcept(ini_key_exception);
	/// @brief	Unexpected header or key exception.
	$DefineExcept(ini_mask_exception);

#pragma region deep_merge
	/**
	 * @brief					Performs a recursive merge of two given INI containers.
	 * @param container			Input container reference.
	 * @param other				Other container constant reference.
	 * @param overrideStyle		The header/key override style to use for duplicate headers/keys.
	 * @returns					The reference of the container parameter, with all of the sections from other merged into it.
	 */
	template<class TKeyComparator = CaseInsensitiveCompare>
	ini_container<TKeyComparator>& deep_merge(ini_container<TKeyComparator>& container, ini_container<TKeyComparator> const& other, OverrideStyle const& overrideStyle = OverrideStyle::Override)
	{
		for (const auto& [header, section] : other) {
			if (const auto& existing{ container.find(header) }; existing != container.end()) {
				auto& [existingHeader, existingSection] { *existing };
				for (const auto& [key, value] : section) {
					if (existingSection.contains(key)) {
						switch (overrideStyle) {
						case OverrideStyle::OnlyBlank:
							if (!section.at(key).empty())
								break; //< break if current value is NOT empty
							else [[fallthrough]];
						case OverrideStyle::Override:
							existingSection.at(key) = value;
							break;
						case OverrideStyle::Throw:
							throw ex::make_custom_exception<ini_key_exception>("Duplicate keys aren't allowed; '", header, (header.empty() ? "" : "::"), key, "' already has value '", section.at(key), "'! (Incoming value: '", value, "')");
						case OverrideStyle::Skip: [[fallthrough]];
						default:break;
						}
					}
					else existingSection[key] = value;
				}
			}
			else container.insert(std::make_pair(header, section));
		}
		return container;
	}
#pragma endregion deep_merge

#pragma region parse
	/**
	 * @brief							Parses the given input stream into an INI container object.
	 * @param is						An input stream to parse data from.
	 * @param config					Optional configuration object that changes the behaviour of the parser.
	 * @returns							An ini_container type that contains all headers, keys, and values from the input stream.
	 * @throws ini_syntax_exception		Input stream contains invalid syntax, and the syntaxErrorStyle specified by the config was SyntaxErrorStyle::Throw
	 * @throws ini_key_exception		Input stream contains duplicate keys, and the overrideStyle specified by the config was OverrideStyle::Throw
	 */
	template<class TKeyComparator = CaseInsensitiveCompare>
	INLINE ini_container<TKeyComparator> parse(std::istream& is, ini_parser_config<TKeyComparator> const& config = {}) noexcept(false)
	{
		static const auto WHITESPACE{ " \t\v\r\n" };

		ini_container<TKeyComparator> ini{};

		size_t ln{ 0 };

		std::string header{};
		bool skip_header{ false };

		for (std::string line; std::getline(is, line, '\n'); is.clear(), ++ln) {
			if (std::all_of(line.begin(), line.end(), str::stdpred::isspace)) continue;

			auto l{ config._$stripComments(line) };

			if (std::all_of(l.begin(), l.end(), str::stdpred::isspace)) continue;

			// find headers
			if (const size_t iOpen{ l.find($c(char, '[')) }, iClose{ l.rfind($c(char, ']')) };
				iOpen != std::string::npos && iClose != std::string::npos) {
				std::string tmpHeader{ l.substr(iOpen + 1, iClose - iOpen - 1) };

				if (config.stripWhitespaceFromHeaders)
					tmpHeader = str::trim(tmpHeader);

				if (const size_t fstNonSpace{ l.find_first_not_of(WHITESPACE) }, lastNonSpace{ l.find_last_not_of(WHITESPACE) };
					(iOpen > fstNonSpace || iClose < lastNonSpace) && config.syntaxErrorStyle == SyntaxErrorStyle::Throw)
					throw ex::make_custom_exception<ini_syntax_exception>("Line ", ln, " contains a header with preceding or trailing non-whitespace characters! '", line, "'");

				skip_header = false;

				if (!config._$maskIncludes(tmpHeader)) {
					switch (config.maskStyle) {
					case MaskStyle::Skip:
						skip_header = true;
						break;
					case MaskStyle::Throw:
						throw ex::make_custom_exception<ini_mask_exception>("Line ", ln, " contains an unexpected header: '", tmpHeader, "'!");
					case MaskStyle::Disable: [[fallthrough]];
					default:break;
					}
				}

				header = tmpHeader;
			}
			else if (!skip_header) {
				// find key-value pairs
				if (const size_t equals{ l.find('=') }; equals != std::string::npos) {
					if (const std::string key{ str::trim(l.substr(0ull, equals)) }; !key.empty()) {
						auto& section{ ini[header] };

						std::string value{ l.substr(equals + 1).data() };

						if (config.stripWhitespaceFromValue)
							value = str::trim(value); //< remove unenclosed preceding/trailing whitespace

						// find enclosing quotes
						if (const auto& [fst, snd] { config._$findEnclosingQuotes(value) };
							fst != value.end() && snd != value.end()) {
							// validate the line if syntax errors are enabled
							if (config.syntaxErrorStyle == SyntaxErrorStyle::Throw) {
								if (fst != value.begin() && !std::all_of<typename std::string::const_iterator>(value.begin(), fst, str::stdpred::isspace))
									throw ex::make_custom_exception<ini_syntax_exception>("Line ", ln, " the setter for key '", key, "' specifies a quote-enclosed value, but there were unexpected characters before the opening quote: {", std::string_view{ value.begin(), fst }, "}!");
								else if (snd != value.end() && !std::all_of<typename std::string::const_iterator>(snd + 1, value.end(), str::stdpred::isspace))
									throw ex::make_custom_exception<ini_syntax_exception>("Line ", ln, " the setter for key '", key, "' specifies a quote-enclosed value, but there were unexpected characters after the closing quote: {", std::string_view{ snd + 1, value.end() }, "}!");
							}

							// update the value; optionally include quotes when the config enables it
							value = { fst + config.stripEnclosingQuotes, snd + !config.stripEnclosingQuotes };
						}

						if (!config._$maskIncludes(header, key)) {
							switch (config.maskStyle) {
							case MaskStyle::Skip:
								continue; //< skip key
							case MaskStyle::Throw:
								throw ex::make_custom_exception<ini_mask_exception>("Line ", ln, " contains unexpected key '", key, "'", (header.empty() ? std::string{ " within section '"s + header + "'!" } : "!"s));
							case MaskStyle::Disable: [[fallthrough]];
							default: break;
							}
						}

						if (section.contains(key)) {
							switch (config.overrideStyle) {
							case OverrideStyle::OnlyBlank:
								if (!section.at(key).empty())
									break; //< break if current value is NOT empty
								else [[fallthrough]];
							case OverrideStyle::Override:
								section.at(key) = value;
								break;
							case OverrideStyle::Throw:
								throw ex::make_custom_exception<ini_key_exception>("Line ", ln, " specifies a duplicate key '", value, "'; '", header, (header.empty() ? "" : "::"), key, "' already has value '", section.at(key), "'!");
							case OverrideStyle::Skip: [[fallthrough]];
							default:break;
							}
						}
						else section.insert(std::make_pair(key, value));
					}
					else if (config.syntaxErrorStyle == SyntaxErrorStyle::Throw)
						throw ex::make_custom_exception<ini_syntax_exception>("Invalid key specified on line ", ln, ": \"", line, '\"');
				}
			}
		}

		if (config.addMaskToOutput)
			deep_merge<TKeyComparator>(ini, config.mask, OverrideStyle::OnlyBlank);

		return ini;
	}
	/**
	 * @brief							Parses the given input stream rvalue into an INI container object.
	 * @param is						The rvalue reference of an input stream to parse data from.
	 * @param config					Optional configuration object that changes the behaviour of the parser.
	 * @returns							An ini_container type that contains all headers, keys, and values from the input stream.
	 * @throws ini_syntax_exception		Input stream contains invalid syntax, and the syntaxErrorStyle specified by the config was SyntaxErrorStyle::Throw
	 * @throws ini_key_exception		Input stream contains duplicate keys, and the overrideStyle specified by the config was OverrideStyle::Throw
	 */
	template<class TKeyComparator>
	INLINE ini_container<TKeyComparator> parse(std::istream&& is, ini_parser_config<TKeyComparator> const& config = {}) noexcept(false)
	{
		return parse<TKeyComparator>(is, config);
	}
#pragma endregion parse

#pragma region ini_printer
	/**
	 * @struct	ini_printer
	 * @brief	Prints INI container objects to an output stream, with configurable formatting.
	 */
	template<class TKeyComparator = CaseInsensitiveCompare>
	struct ini_printer {
		using ostream_t = std::ostream;
		using ini_t = ini_container<TKeyComparator>;

		ini_t* ini{ nullptr };
		bool explicitGlobalHeader{ false };
		bool insertNewlineBetweenSections{ true };

		ini_printer(ini_t* ini = nullptr) : ini{ ini } {}

		friend ostream_t& operator<<(ostream_t& os, const ini_printer& printer)
		{
			if (printer.ini != nullptr) {
				bool fst{ true }, fstHeader{ true };
				for (const auto& [header, section] : (*printer.ini)) {
					if (printer.insertNewlineBetweenSections) {
						if (fst) fst = false;
						else os << '\n';
					}

					if (!header.empty() || !fstHeader || printer.explicitGlobalHeader)
						os << $c(char, '[') << header << $c(char, ']') << '\n';

					for (const auto& [key, val] : section)
						os << key << $c(char, ' ') << $c(char, '=') << $c(char, ' ') << val << '\n';

					fstHeader = false;
				}
			}
			return os;
		}
	};
#pragma endregion ini_printer

	/// @brief	INI file wasn't found at the specified path.
	$DefineExcept(ini_file_not_found_exception);

	/// @brief	The global header.
	inline constexpr const auto GLOBAL{ "" };

#pragma region basic_ini
	/**
	 * @class	basic_ini
	 * @brief	Inherits directly from ini_container, and provides extra methods for reading/writing, handling nested keys, and type conversions.
	 */
	template<class TKeyComparator = CaseInsensitiveCompare>
	class basic_ini {
	#pragma region using
		using section_t = ini_section<TKeyComparator>;
		using container_t = ini_container<TKeyComparator>;
		using this_t = basic_ini<TKeyComparator>;
		using istream_t = std::istream;
		using config_t = ini_parser_config<TKeyComparator>;
		using mask_t = ini_mask<TKeyComparator>;
	#pragma endregion using

	protected:
		container_t map;

	public:
		using ParserConfig = ini_parser_config<TKeyComparator>;
		using Printer = ini_printer<TKeyComparator>;

	#pragma region events
		/// @brief	Event arguments for the onRead event.
		struct onReadArgs : shared::basic_event_args {
			const std::filesystem::path path;
			onReadArgs(std::filesystem::path const& path) : path{ path } {}
		};
		/// @brief	Event callback triggered when the INI is read from disk using read().
		shared::eventdef<onReadArgs> onRead;
		/// @brief	Event arguments for the onWrite event.
		struct onWriteArgs : shared::basic_event_args {
			const std::filesystem::path path;
			const bool success;
			onWriteArgs(std::filesystem::path const& path, const bool success) : path{ path }, success{ success } {}
		};
		/// @brief	Event callback triggered when the INI is written to disk using write().
		shared::eventdef<onWriteArgs> onWrite;

		/// @brief	Event callback triggered when another INI container or mask is merged into this instance using deep_merge().
		shared::eventdef<shared::basic_event_args> onMerge;

		/// @brief	Event callback triggered when this instance's destructor is called.
		shared::eventdef<shared::basic_event_args> onDestruct;
	#pragma endregion events

	#pragma region constructors
		/// @brief	Default ctor
		basic_ini() {}
		/**
		 * @brief			Parsing ctor
		 * @param istream	Input stream ref that contains the input data.
		 * @param config	Parser configuration instance.
		 */
		basic_ini(istream_t& istream, config_t const& config = {})
			: map(parse<TKeyComparator>(istream, config))
		{}
		/**
		 * @brief			Parsing ctor
		 * @param istream	Input stream rvalue that contains the input data.
		 * @param config	Parser configuration instance.
		 */
		basic_ini(istream_t&& istream, config_t const& config = {})
			: map(parse<TKeyComparator>(std::forward<istream_t>(istream), config))
		{}
		/**
		 * @brief						Reading/Parsing ctor
		 * @param path					The location of an INI file in the local filesystem.
		 *\n							When the specified files exists, it is parsed into this instance.
		 *\n							If the specified file doesn't exist,
		 * @param config				Parser configuration instance.
		 * @param throwIfFileNotFound	When true & the specified file doesn't exist, an exception is thrown; otherwise when false, the object is initialized as an empty instance.
		 */
		basic_ini(std::filesystem::path const& path, config_t const& config = {}, const bool throwIfFileNotFound = false)
			: map([](auto&& path, auto&& config, auto&& throwIfFileNotFound) { if (file::exists(path)) return parse<TKeyComparator>(file::read(path, std::ios_base::binary), config); else if (throwIfFileNotFound) throw make_custom_exception<ini_file_not_found_exception>("File Not Found:  ", path); else return container_t{}; }(path, config, throwIfFileNotFound))
		{}

		template<var::same_or_convertible<std::pair<std::string, section_t>>... Ts>
		basic_ini(Ts&&... sections) : map{ std::forward<Ts>(sections)... } {}
	#pragma endregion constructors

	#pragma region destructor
		/// @brief	Default Destructor. Triggers the `onDestruct` event.
		~basic_ini() noexcept
		{
			try {
				onDestruct.notify(this);
			} catch (...) {}
		}
	#pragma endregion destructor

	#pragma region deep_merge
		/// @brief	See ::ini::deep_merge
		this_t& deep_merge(container_t const& other, OverrideStyle const& overrideStyle = OverrideStyle::Override)
		{
			for (const auto& [header, section] : other) {
				if (const auto& existing{ this->map.find(header) }; existing != this->map.end()) {
					auto& [existingHeader, existingSection] { *existing };
					for (const auto& [key, value] : section) {
						if (existingSection.contains(key)) {
							switch (overrideStyle) {
							case OverrideStyle::OnlyBlank:
								if (!section.at(key).empty())
									break; //< break if current value is NOT empty
								else [[fallthrough]];
							case OverrideStyle::Override:
								existingSection.at(key) = value;
								break;
							case OverrideStyle::Throw:
								throw ex::make_custom_exception<ini_key_exception>("Duplicate keys aren't allowed; '", header, (header.empty() ? "" : "::"), key, "' already has value '", section.at(key), "'! (Incoming value: '", value, "')");
							case OverrideStyle::Skip: [[fallthrough]];
							default:break;
							}
						}
						else existingSection[key] = value;
					}
				}
				else this->map.insert(std::make_pair(header, section));
			}
			onMerge.notify(this);
			return *this;
		}
	#pragma endregion deep_merge

	#pragma region mask
		/**
		 * @brief					Applies the given mask to this instance.
		 * @param mask				An ini_mask container.
		 * @param overrideStyle		The override style to use when merging the mask into this instance's map.
		 * @returns					The reference of this instance.
		 */
		this_t& mask(mask_t&& mask, OverrideStyle const& overrideStyle = OverrideStyle::Override)
		{
			return deep_merge(std::move(mask), overrideStyle);
		}
		/**
		 * @brief					Applies the given mask to this instance.
		 * @param mask				An ini_mask container.
		 * @param overrideStyle		The override style to use when merging the mask into this instance's map.
		 * @returns					The reference of this instance.
		 */
		this_t& mask(const mask_t& mask, OverrideStyle const& overrideStyle = OverrideStyle::Override)
		{
			return deep_merge(mask, overrideStyle);
		}
		/**
		 * @brief					Applies the given mask to this instance.
		 * @param cfg				An ini_parser_config instance.
		 * @returns					The reference of this instance.
		 */
		this_t& mask(ParserConfig&& cfg)
		{
			return this->mask($fwd(cfg).mask, $fwd(cfg).overrideStyle);
		}
		/**
		 * @brief					Applies the given mask to this instance.
		 * @param cfg				An ini_parser_config instance.
		 * @returns					The reference of this instance.
		 */
		this_t& mask(const ParserConfig& cfg)
		{
			return this->mask(cfg.mask, cfg.overrideStyle);
		}
	#pragma endregion mask

	#pragma region read
		/**
		 * @brief					Reads the specified INI config file and merges its contents into this instance according to the given OverrideStyle.
		 * @param path				The location of the config file.
		 * @param config			An optional ini_parser_config instance to use when parsing the file.
		 * @param overrideStyle		The OverrideStyle to use when deep merging the parsed data into this instance.
		 */
		void read(std::filesystem::path const& path, ParserConfig const& config, OverrideStyle const& overrideStyle = OverrideStyle::Override) noexcept(false)
		{
			this->deep_merge(parse<TKeyComparator>(file::read(path), config), overrideStyle);
			onRead.notify(this, path);
		}
		/**
		 * @brief					Reads the specified INI config file and merges its contents into this instance according to the given OverrideStyle.
		 * @param path				The location of the config file.
		 * @param overrideStyle		The OverrideStyle to use when deep merging the parsed data into this instance.
		 */
		void read(std::filesystem::path const& path, OverrideStyle const& overrideStyle = OverrideStyle::Override) noexcept(false)
		{
			this->deep_merge(parse<TKeyComparator>(file::read(path)), overrideStyle);
			onRead.notify(this, path);
		}
	#pragma endregion read

	#pragma region write
		/**
		 * @brief		Reads the specified INI config file and merges its contents into this instance according to the given OverrideStyle.
		 * @param path	The location of the config file.
		 * @returns		true when the file was successfully written to; otherwise false.
		 */
		bool write(std::filesystem::path const& path) const noexcept(false)
		{
			const auto& result{ file::write(path, Printer((container_t*)&map)) };
			onWrite.notify(this, path, result);
			return result;
		}
	#pragma endregion write

	#pragma region map_methods
		/// @returns	An iterator to the beginning of the range of headers & sections.
		auto begin() noexcept { return map.begin(); }
		/// @returns	An iterator to the end of the range of headers & sections.
		auto end() noexcept { return map.end(); }
		/// @returns	A const_iterator to the beginning of the range of headers & sections.
		auto begin() const noexcept { return map.begin(); }
		/// @returns	A const_iterator to the end of the range of headers & sections.
		auto end() const noexcept { return map.end(); }
		/// @returns	An iterator to the beginning of the read-only range of headers & sections.
		auto cbegin() const noexcept { return map.cbegin(); }
		/// @returns	An iterator to the end of the read-only range of headers & sections.
		auto cend() const noexcept { return map.cend(); }
		/// @returns	The current number of header-section pairs in the map.
		auto size() const noexcept { return map.size(); }
		/// @returns	The maximum number of header-section pairs that the map is able to hold.
		auto max_size() const noexcept { return map.max_size(); }
		/// @returns	true when there are no header-section pairs in th map.
		auto empty() const noexcept { return map.empty(); }
		/// @returns	An iterator to the header-section pair with a matching header.
		auto find(auto&& header) const noexcept { return map.find($fwd(header)); }
		/// @returns	An iterator to the header-section pair with a matching header.
		auto find_if(auto&& predicate) const { return std::find_if(map.begin(), map.end(), $fwd(predicate)); }
		/// @returns	true when the map contains the specified header; otherwise false.
		auto contains(auto&& header) const noexcept { return map.contains($fwd(header)); }
		/// @returns	true when the map contains the specified header; otherwise false.
		auto contains(auto&& header, auto&& key) const noexcept
		{
			if (const auto& it{ map.find($fwd(header)) }; it != map.end())
				return it->contains($fwd(key));
			return false;
		}
	#pragma endregion map_methods

	#pragma region at
		/**
		 * @brief			Gets a mutable reference to the section with the given header.
		 * @param header	The name of the target header. (Leave blank for global)
		 * @returns			A reference to the section with the given header.
		 * @throws			An out-of-range exception is thrown when the specified header doesn't exist.
		 */
		section_t& at(std::string const& header) noexcept(false)
		{
			return map.at(header);
		}
		/**
		 * @brief			Gets an immutable reference to the section with the given header.
		 * @param header	The name of the target header. (Leave blank for global)
		 * @returns			A const reference to the section with the given header.
		 * @throws			An out-of-range exception is thrown when the specified header doesn't exist.
		 */
		const section_t& at(std::string const& header) const noexcept(false)
		{
			return map.at(header);
		}
		/**
		 * @brief			Gets a mutable reference to the value of a specified key.
		 * @param header	The name of the target header. (Leave blank for global)
		 * @param key		The name of the target key.
		 * @returns			A reference to the value of the specified key.
		 * @throws			An out-of-range exception is thrown when the specified header or key doesn't exist.
		 */
		ini_value& at(std::string const& header, std::string const& key) noexcept(false)
		{
			return map.at(header).at(key);
		}
		/**
		 * @brief			Gets an immutable reference to the value of a specified key.
		 * @param header	The name of the target header. (Leave blank for global)
		 * @param key		The name of the target key.
		 * @returns			A const reference to the value of the specified key.
		 * @throws			An out-of-range exception is thrown when the specified header or key doesn't exist.
		 */
		const ini_value& at(std::string const& header, std::string const& key) const noexcept(false)
		{
			return map.at(header).at(key);
		}
	#pragma endregion at

	#pragma region get
		/**
		 * @brief			Gets the value of the specified key.
		 * @param header	The name of the target header. (Leave blank for global)
		 * @param key		The name of the target key.
		 * @returns			The value of the specified key, or std::nullopt if the specified header or key doesn't exist.
		 */
		std::optional<ini_value> get(std::string const& header, std::string const& key) const noexcept
		{
			// find the target header-section pair:
			if (const auto& headerSectionPr{ map.find(header) }; headerSectionPr != map.end()) {
				// find the target key:
				if (const auto& k{ headerSectionPr->second.find(key) }; k != headerSectionPr->second.end()) {
					return k->second;
				}
			}
			return std::nullopt;
		}
		/**
		 * @brief			Gets the value of the specified key.
		 * @param header	The name of the target header. (Leave blank for global)
		 * @param key		The name of the target key.
		 * @returns			The value of the specified key, or std::nullopt if the specified header or key doesn't exist.
		 */
		std::optional<ini_value> get(std::string&& header, std::string&& key) const noexcept
		{
			// find the target header-section pair:
			if (const auto& headerSectionPr{ map.find($fwd(header)) }; headerSectionPr != map.end()) {
				// find the target key:
				if (const auto& k{ headerSectionPr->second.find($fwd(key)) }; k != headerSectionPr->second.end()) {
					return k->second;
				}
			}
			return std::nullopt;
		}
	#pragma endregion get

	#pragma region set
		/**
		 * @brief			Sets the value of the specified key.
		 * @param header	The name of the target header. (Leave blank for global)
		 * @param key		The name of the target key.
		 * @param value		The value to set the specified key to. **Must be implicitly convertible to std::string.**
		 */
		template<std::convertible_to<std::string> TValue> requires (!std::same_as<std::decay_t<TValue>, std::string>)
			void set(std::string const& header, std::string const& key, TValue&& value) noexcept
		{
			map[header][key] = std::string{ std::forward<TValue>(value) };
		}
		/**
		 * @brief			Sets the value of the specified key.
		 * @param header	The name of the target header. (Leave blank for global)
		 * @param key		The name of the target key.
		 * @param value		The value to set the specified key to.
		 */
		void set(std::string const& header, std::string const& key, std::string&& value) noexcept
		{
			map[header][key] = std::forward<std::string>(value);
		}
		/**
		 * @brief			Sets the value of the specified key.
		 * @param header	The name of the target header. (Leave blank for global)
		 * @param key		The name of the target key.
		 * @param value		The value to set the specified key to.
		 */
		void set(std::string const& header, std::string const& key, std::string const& value) noexcept
		{
			map[header][key] = value;
		}
	#pragma endregion set

	#pragma region check
		/**
		 * @brief			Checks if the specified key exists.
		 * @param header	The name of the target header. (Leave blank for global)
		 * @param key		The name of the target key.
		 * @returns			true when the specified key exists; otherwise false.
		 */
		CONSTEXPR bool check(std::string const& header, std::string const& key) const noexcept { return this->get(header, key).has_value(); }
		/**
		 * @brief			Checks if the specified key exists.
		 * @param header	The name of the target header. (Leave blank for global)
		 * @param key		The name of the target key.
		 * @returns			true when the specified key exists; otherwise false.
		 */
		CONSTEXPR bool check(std::string&& header, std::string&& key) const noexcept { return this->get($fwd(header), $fwd(key)).has_value(); }
		/**
		 * @brief					Compares the current value of the specified key to the given expected_value.
		 * @param header			The name of the target header. (Leave blank for global)
		 * @param key				The name of the target key.
		 * @param expected_value	An ini_value to compare to the current value of the specified key.
		 * @returns					true when the value of the specified key matches the expected_value; otherwise false. If the specified key doesn't exist, returns false.
		 */
		CONSTEXPR bool checkv(std::string const& header, std::string const& key, ini_value const& expected_value) const noexcept
		{
			const auto& v{ this->get(header, key) };
			return (v.has_value() ? (v.value() == expected_value) : false);
		}
		/**
		 * @brief					Compares the current value of the specified key to the given expected_value.
		 * @param header			The name of the target header. (Leave blank for global)
		 * @param key				The name of the target key.
		 * @param expected_value	An ini_value to compare to the current value of the specified key.
		 * @returns					true when the value of the specified key matches the expected_value; otherwise false. If the specified key doesn't exist, returns false.
		 */
		CONSTEXPR bool checkv(std::string&& header, std::string&& key, ini_value&& expected_value) const noexcept
		{
			const auto& v{ this->get($fwd(header), $fwd(key)) };
			return (v.has_value() ? (v.value() == $fwd(expected_value)) : false);
		}
	#pragma endregion check

	#pragma region operator<=>
		/// @brief	Default comparison operators.
		auto operator<=>(this_t const&) const = default;
	#pragma endregion operator<=>

	#pragma region operator[]
		/**
		 * @brief			Gets the specified section of the INI, or create it if it doesn't exist.
		 * @param header	The name of the target header. (Leave blank for global)
		 * @returns			The reference of the specified header.
		 */
		section_t& operator[](const std::string& header) noexcept
		{
			return map[header];
		}
	#pragma endregion operator[]

	#pragma region operator()
		/**
		 * @brief			Gets the value of the specified key, or creates it if it doesn't exist.
		 * @param header	The name of the target header. (Leave blank for global)
		 * @param key		The name of the target key.
		 * @returns			A reference to the value of the target key.
		 */
		CONSTEXPR ini_value& operator()(const std::string& header, const std::string& key) noexcept { return map.operator[](header)[key]; }
		/**
		 * @brief			Gets the value of the specified key, or creates it if it doesn't exist.
		 * @param header	The name of the target header. (Leave blank for global)
		 * @param key		The name of the target key.
		 * @returns			A reference to the value of the target key.
		 */
		CONSTEXPR ini_value& operator()(std::string&& header, std::string&& key) noexcept { return map.operator[]($fwd(header))[$fwd(key)]; }
		/**
		 * @brief			Gets the value of the specified key, or throws an exception if it doesn't exist.
		 * @param header	The name of the target header. (Leave blank for global)
		 * @param key		The name of the target key.
		 * @returns			A reference to the value of the target key.
		 */
		CONSTEXPR ini_value operator()(const std::string& header, const std::string& key) const noexcept(false) { return map.at(header).at(key); }
		/**
		 * @brief			Gets the value of the specified key, or throws an exception if it doesn't exist.
		 * @param header	The name of the target header. (Leave blank for global)
		 * @param key		The name of the target key.
		 * @returns			A reference to the value of the target key.
		 */
		CONSTEXPR ini_value operator()(std::string&& header, std::string&& key) const noexcept(false) { return map.at($fwd(header)).at($fwd(key)); }
		/**
		 * @brief					Compares the current value of the specified key to the given expected_value.
		 * @param header			The name of the target header. (Leave blank for global)
		 * @param key				The name of the target key.
		 * @param expected_value	An ini_value to compare to the current value of the specified key.
		 * @returns					true when the value of the specified key matches the expected_value; otherwise false. If the specified key doesn't exist, returns false.
		 */
		CONSTEXPR bool operator()(std::string const& header, std::string const& key, ini_value const& expected_value) const noexcept { return checkv(header, key, expected_value); }
		/**
		 * @brief					Compares the current value of the specified key to the given expected_value.
		 * @param header			The name of the target header. (Leave blank for global)
		 * @param key				The name of the target key.
		 * @param expected_value	An ini_value to compare to the current value of the specified key.
		 * @returns					true when the value of the specified key matches the expected_value; otherwise false. If the specified key doesn't exist, returns false.
		 */
		CONSTEXPR bool operator()(std::string&& header, std::string&& key, ini_value&& expected_value) const noexcept { return checkv($fwd(header), $fwd(key), $fwd(expected_value)); }

	#pragma endregion operator()

	#pragma region operators<<&>>
		/// @brief	Inserts the plaintext data contained by ini into the output stream in the correct format using ini_printer.
		friend std::ostream& operator<<(std::ostream& os, const this_t& ini)
		{
			return os << ini_printer<TKeyComparator>{ &ini };
		}
		/// @brief	Extracts the data contained by the std::istream and parses it, storing the result in the given ini instance. **Note that using this means you can't use a parser config!**
		friend std::istream& operator>>(std::istream& is, this_t& ini)
		{
			ini.deep_merge(parse<TKeyComparator>($fwd(is)));
			return is;
		}
	#pragma endregion operators<<&>>
	};
#pragma endregion basic_ini

#pragma region usings
	/// @brief	INI class that uses *case-insensitive* key comparisons, and narrow-width chars.
	using INI = basic_ini<CaseInsensitiveCompare>;
	/// @brief	INI class that uses *case-sensitive* key comparisons, and narrow-width chars.
	using cINI = basic_ini<CaseSensitiveCompare>;

	using Section = ini_section<CaseInsensitiveCompare>;
	using cSection = ini_section<CaseSensitiveCompare>;
	using Container = ini_container<CaseInsensitiveCompare>;
	using cContainer = ini_container<CaseSensitiveCompare>;
	using Value = ini_value;
	using ParserConfig = ini_parser_config<CaseInsensitiveCompare>;
	using cParserConfig = ini_parser_config<CaseSensitiveCompare>;
	using Printer = ini_printer<CaseInsensitiveCompare>;
	using cPrinter = ini_printer<CaseSensitiveCompare>;
#pragma endregion usings
}
