#pragma once
#include <sysarch.h>
#include <VariantArgumentType.hpp>
#include <str.hpp>
#include <var.hpp>

#include <utility>
#include <string>
#include <optional>
#include <variant>
#include <vector>

namespace opt {
	using ArgContainerType = std::vector<VariantArgumentType>;								///< your
	using ArgContainerIteratorType = ArgContainerType::const_iterator;						///< brain on
	using ArgContainerIteratorContainerType = std::vector<ArgContainerIteratorType>;		///< drugs

	/**
	 * @struct	InputWrapper
	 * @brief	Acts as a wrapper for char & std::string input types, to allow variadic functions to receive either chars or strings as input, and contains some helpful methods of its own.
	 */
	struct InputWrapper {
		const std::string _input;
		_CONSTEXPR InputWrapper(const std::string& input) : _input{ input } {}
		_CONSTEXPR InputWrapper(char input) : _input{ std::move(std::string(1u, input)) } {}
		_CONSTEXPR InputWrapper(const InputWrapper& o) = default;
		_CONSTEXPR InputWrapper(InputWrapper&& o) noexcept = default;
	#ifdef OS_WIN
		_CONSTEXPR operator const std::string() const { return _input; }
	#else
		operator const std::string() const { return _input; }
	#endif
		_CONSTEXPR bool operator==(const std::string& o) const { return _input == o; }
		_CONSTEXPR bool operator==(const char& o) const { return _input.size() == 1ull && _input.front() == o; }
		_CONSTEXPR bool operator!=(auto&& o) const { return !operator==(std::forward<decltype(o)>(o)); }
	};

	/**
	 * @concept		ValidInputType
	 * @tparam T	Input Type
	 * @brief		Concept that allows InputWrapper, char, std::string, or any types that are implicitly convertible to char or std::string.
	 */
	template<typename T> concept ValidInputType = std::same_as<T, InputWrapper> || std::same_as<T, std::string> || std::same_as<T, char> || std::convertible_to<T, char> || std::convertible_to<T, std::string>;

	/**
	 * @struct	ArgContainer
	 * @brief	A container for storing and interacting with parsed commandline arguments.
	 *\n		The container itself is upstream from the parsing functions, see the ParamsAPI2 class.
	 */
	struct ArgContainer {
	protected:
		std::optional<std::string> _arg0;
		ArgContainerType _args;

	public:
		ArgContainer() = default;
		ArgContainer(ArgContainerType&& args, std::optional<std::string> arg0 = std::nullopt) : _arg0{ std::move(arg0) }, _args{ std::move(args) } {}
		template<ValidArg... VT> requires var::at_least_one<VT...>
		explicit ArgContainer(const VT&... args) : _arg0{ std::nullopt }, _args{ var::variadic_accumulate<VariantArgumentType>(args...) } {}

		_CONSTEXPR bool operator==(const ArgContainer& o) const { return _args.size() == o._args.size() && _args == o._args; }
		_CONSTEXPR bool operator!=(auto&& o) const { return !operator==(std::forward<decltype(o)>(o)); }

		_CONSTEXPR operator const ArgContainerType() const { return _args; }

	#pragma region ForwardVectorFunctions
		_CONSTEXPR auto begin() const { return _args.begin(); }
		_CONSTEXPR auto end() const { return _args.end(); }
		_CONSTEXPR auto rbegin() const { return _args.rbegin(); }
		_CONSTEXPR auto rend() const { return _args.rend(); }
		_CONSTEXPR const auto at(auto&& pos) const { return _args.at(std::forward<decltype(pos)>(pos)); }
		_CONSTEXPR bool empty() const noexcept { return _args.empty(); }
		_CONSTEXPR auto size() const { return _args.size(); }
		_CONSTEXPR auto max_size() const { return _args.max_size(); }
		_CONSTEXPR auto capacity() const { return _args.capacity(); }
		_CONSTEXPR void reserve(const size_t& new_size) { _args.reserve(new_size); }
		/// @brief Shrink the capacity of the argument container to fit the current number of elements. Returns the number of elements the container was shrunk by. @returns size_t
		_CONSTEXPR const auto shrink_to_fit() { const auto capacity{ _args.capacity() }; _args.shrink_to_fit(); return _args.capacity() - capacity; }
		/// @brief Insert an argument into the back of the container. @returns VariantArgumentType&	- The reference of the argument in the container.
		_CONSTEXPR auto emplace_back(auto&& argument) noexcept(false) { return _args.emplace_back(std::forward<decltype(argument)>(argument)); }
		_CONSTEXPR auto push_back(auto&& argument) noexcept(false) { return _args.push_back(std::forward<decltype(argument)>(argument)); }
		/// @brief Remove & return the element at the back of the container. @returns VariantArgumentType
		auto pop_back() noexcept(false) { const auto back{ _args.back() }; _args.pop_back(); return back; }
		/// @brief Remove & return the element at the front of the container. @returns VariantArgumentType
		auto pop_front() noexcept(false)
		{
			const auto front{ _args.front() };
			for (auto pos{ _args.begin() }; pos != _args.end() - 1ll; ++pos)
				pos = (pos + 1ll);
			_args.pop_back();
			return front;
		}
	#pragma endregion ForwardVectorFunctions

		/// @brief Retrieve the optional argv[0] argument. @returns std::optional<std::string>
		auto arg0() const { return _arg0; }

		/// @brief Set the value of the optional argv[0] argument, and return the previous value. @returns std::optional<std::string>
		auto arg0(std::optional<std::string> arg0) { const auto copy{ _arg0 }; _arg0 = arg0; return copy; }

	#pragma region FindFunctions
		/**
		 * @brief			Find an argument in the container.
		 * @tparam Types...	If none are specified, allows arguments with any type to be a match. If at least one is specified, only allows arguments with the specified types to be a match.
		 * @tparam Name		Input name type. (string | char)
		 * @param name		Argument name to search for.
		 * @param off		Option begin iterator position. If not specified, defaults to begin().
		 * @param end		Optional end iterator position. If not specified, defaults to end().
		 * @returns			ArgContainerIteratorType
		 */
		template<ValidArg... Types, ValidInputType Name>
		_CONSTEXPR ArgContainerIteratorType find(const Name& name, const std::optional<ArgContainerIteratorType>& off = std::nullopt, const std::optional<ArgContainerIteratorType>& end = std::nullopt) const
		{
			_CONSTEXPR const bool match_any_type{ sizeof...(Types) == 0 };
			for (auto it{ off.value_or(_args.begin()) }; it != end.value_or(_args.end()); ++it)
				if ((match_any_type || var::variadic_or(it->index() == get_index<Types>()...)) && get_name(*it) == InputWrapper(name))
					return it;
			return _args.end();
		}

		/**
		 * @brief			Find any argument with at least one matching criteria.
		 *\n				To be considered a valid match, an argument must have a matching type AND a matching name.
		 *\n				If at least one type is included, arguments must have (any) of the specified types to be considered a match. If no types are specified, any type may be considered a match.
		 *\n				If at least one name is included, arguments must have (any) of the specified names to be considered a match. If no names are specified, any name may be considered a match.
		 * @tparam Types...	Zero or more argument type(s) to consider as matching.
		 * @tparam Names...	Zero or more argument name(s) to consider as matching.
		 * @param ...names	Argument name(s) to search for. If left empty, any argument name will be considered a match, so long as it has a matching type.
		 * @returns			ArgContainerIteratorType
		 */
		template<ValidArg... Types, ValidInputType Name>
		_CONSTEXPR ArgContainerIteratorType find_any(const Name& name, const std::optional<ArgContainerIteratorType>& off = std::nullopt, const std::optional<ArgContainerIteratorType>& end = std::nullopt) const
		{
			return find<Types...>(name, off, end);
		}

		/**
		 * @brief			Find any argument with at least one matching criteria.
		 *\n				To be considered a valid match, an argument must have a matching type AND a matching name.
		 *\n				If at least one type is included, arguments must have (any) of the specified types to be considered a match. If no types are specified, any type may be considered a match.
		 *\n				If at least one name is included, arguments must have (any) of the specified names to be considered a match. If no names are specified, any name may be considered a match.
		 * @tparam Types...	Zero or more argument type(s) to consider as matching.
		 * @tparam Names...	Zero or more argument name(s) to consider as matching.
		 * @param ...names	Argument name(s) to search for. If left empty, any argument name will be considered a match, so long as it has a matching type.
		 * @returns			ArgContainerIteratorType
		 */
		template<ValidArg... Types, ValidInputType... Names>
		_CONSTEXPR ArgContainerIteratorType find_any(const Names&... names) const
		{
			_CONSTEXPR const bool match_any_type{ sizeof...(Types) == 0 }, match_any_name{ sizeof...(Names) == 0 };
			for (auto it{ begin() }; it != end(); ++it)
				if (match_any_type || var::variadic_or(is_type<Types>(*it)...)) {
					if (match_any_name)
						return it;
					else if (const auto name{ get_name(*it) }; var::variadic_or(InputWrapper(names) == name...))
						return it;
				}
			return end();
		}

		/**
		 * @brief			Find all arguments with matching criteria.
		 *\n				To be considered a valid match, an argument must have a matching type AND a matching name.
		 *\n				If at least one type is included, arguments must have (any) of the specified types to be considered a match. If no types are specified, any type may be considered a match.
		 *\n				If at least one name is included, arguments must have (any) of the specified names to be considered a match. If no names are specified, any name may be considered a match.
		 * @tparam Types...	Zero or more argument type(s) to consider as matching.
		 * @tparam Names...	Zero or more argument name(s) to consider as matching.
		 * @param ...names	Argument name(s) to search for. If left empty, any argument name will be considered a match, so long as it has a matching type.
		 * @returns			ArgContainerIteratorContainerType
		 */
		template<ValidArg... Types, ValidInputType... Names>
		_CONSTEXPR const ArgContainerIteratorContainerType find_all(const Names&... names) const
		{
			_CONSTEXPR const bool match_any_type{ sizeof...(Types) == 0 }, match_any_name{ sizeof...(Names) == 0 };
			ArgContainerIteratorContainerType vec;
			vec.reserve(_args.size());
			for (auto it{ begin() }; it != end(); ++it)
				if (match_any_type || var::variadic_or(is_type<Types>(*it)...)) {
					if (match_any_name)
						vec.emplace_back(it);
					else if (const auto name{ get_name(*it) }; var::variadic_or(InputWrapper(names) == name...))
						vec.emplace_back(it);
				}
			vec.shrink_to_fit();
			return vec;
		}
	#pragma endregion FindFunctions

	#pragma region CheckFunctions
		/**
		 * @brief			Check if a given argument was included on the commandline.
		 * @tparam Types...	Zero or more argument type(s) to consider as matching.
		 * @tparam Name		Argument name type. (string|char)
		 * @param name		Argument name to check.
		 * @param off		Option begin iterator position. If not specified, defaults to begin().
		 * @param end		Optional end iterator position. If not specified, defaults to end().
		 * @returns			bool
		 */
		template<ValidArg... Types, ValidInputType Name>
		_CONSTEXPR const bool check(const Name& name, const std::optional<ArgContainerIteratorType>& off = std::nullopt, const std::optional<ArgContainerIteratorType>& end = std::nullopt) const
		{
			return find<Types...>(name, off, end) != _args.end();
		}
		/**
		 * @brief			Check if a given argument was included on the commandline.
		 * @tparam Types...	Zero or more argument type(s) to consider as matching.
		 * @tparam Name		Argument name type. (string|char)
		 * @param name		Argument name to check.
		 * @param off		Option begin iterator position. If not specified, defaults to begin().
		 * @param end		Optional end iterator position. If not specified, defaults to end().
		 * @returns			bool
		 */
		template<ValidArg... Types, ValidInputType Name>
		_CONSTEXPR const bool check_any(const Name& name, const std::optional<ArgContainerIteratorType>& off = std::nullopt, const std::optional<ArgContainerIteratorType>& end = std::nullopt) const { return check<Types...>(name, off, end); }
		/**
		 * @brief			Check if any arguments of a given type were included on the commandline.
		 * @tparam Types...	Zero or more argument type(s) to consider as matching. Leaving this blank is the equivalent of calling !empty().
		 * @returns			bool
		 */
		template<ValidArg... Types>
		_CONSTEXPR const bool check() const
		{
			return find_any<Types...>() != _args.end();
		}
		/**
		 * @brief			Check if any argument with matching criteria was included on the commandline.
		 * @tparam Types...	Zero or more argument type(s) to consider as matching. Leaving this blank is the equivalent of calling empty().
		 * @tparam Names...	Zero or more argument name(s) to consider as matching.
		 * @param ...names	Argument name(s) to search for. If left empty, any argument name will be considered a match, so long as it has a matching type.
		 * @returns			bool
		 */
		template<ValidArg... Types, ValidInputType... Names>
		_CONSTEXPR const bool check_any(const Names&... names) const
		{
			return find_any<Types...>(names...) != _args.end();
		}
		/**
		 * @brief			Check if all of the given argument names were inlcuded on the commandline.
		 * @tparam Names...	Zero or more argument name(s) to consider as matching.
		 * @param ...names	Argument name(s) to search for. If left empty, any argument name will be considered a match, so long as it has a matching type.
		 * @returns			bool
		 */
		template<ValidInputType... Names>
		_CONSTEXPR const bool check_all(const Names&... names) const
		{
			return var::variadic_and(find(names) != _args.end()...);
		}
	#pragma endregion CheckFunctions

	#pragma region GetFunctions
		/**
		 * @brief			Get a specified argument from within the container. This function is the equivalent of safely dereferencing the result of find() and returning it as an optional.
		 * @tparam Types...	Zero or more argument type(s) to consider as matching. Leaving this blank will allow any argument types to match.
		 * @tparam Name		Input Argument Name Type
		 * @param name		Argument name to search for.
		 * @param off		Position in the container to start searching. (Inclusive)
		 * @param end		Position in the container to stop searching. (Exclusive)
		 * @returns			std::optional<VariantArgumentType>
		 */
		template<ValidArg... Types, ValidInputType Name>
		_CONSTEXPR const std::optional<VariantArgumentType> get(const Name& name, const std::optional<ArgContainerIteratorType>& off = std::nullopt, const std::optional<ArgContainerIteratorType>& end = std::nullopt) const noexcept
		{
			if (const auto target{ find<Types...>(name, off, end) }; target != _args.end())
				return *target;
			return std::nullopt;
		}

		/**
		 * @brief			Get a specified argument from within the container. This function is the equivalent of safely dereferencing the result of find_any() and returning it as an optional.
		 * @tparam Types...	Zero or more argument type(s) to consider as matching. Leaving this blank will allow any argument types to match.
		 * @tparam Name		Input Argument Name Type
		 * @param name		Argument name to search for.
		 * @param off		Position in the container to start searching. (Inclusive)
		 * @param end		Position in the container to stop searching. (Exclusive)
		 * @returns			std::optional<VariantArgumentType>
		 */
		template<ValidArg... Types, ValidInputType Name>
		_CONSTEXPR const std::optional<VariantArgumentType> get_any(const Name& name, const std::optional<ArgContainerIteratorType>& off = std::nullopt, const std::optional<ArgContainerIteratorType>& end = std::nullopt) const noexcept
		{
			return get<Types...>(name, off, end);
		}

		/**
		 * @brief			Get any argument with matching criteria. This function is the equivalent of safely dereferencing the result of find_any() and returning it as an optional.
		 * @tparam Types...	Zero or more argument type(s) to consider as matching. Leaving this blank will allow any argument types to match.
		 * @tparam Names...	Zero or more argument name(s) to consider as matching. Leaving this blank will allow any argument names to match.
		 * @param ...names	Argument name(s) to search for. If left empty, any argument name will be considered a match, so long as it has a matching type.
		 * @returns			std::optional<VariantArgumentType>
		 */
		template<ValidArg... Types, ValidInputType... Names>
		_CONSTEXPR const std::optional<VariantArgumentType> get_any(const Names&... names) const noexcept
		{
			if (const auto target{ find_any<Types...>(names...) }; target != _args.end())
				return *target;
			return std::nullopt;
		}

		/**
		 * @brief			Get any argument with matching criteria. This function is the equivalent of dereferencing each element of the result from find_all().
		 * @tparam Types...	Zero or more argument type(s) to consider as matching. Leaving this blank will allow any argument types to match.
		 * @tparam Names...	Zero or more argument name(s) to consider as matching. Leaving this blank will allow any argument names to match.
		 * @param ...names	Argument name(s) to search for. If left empty, any argument name will be considered a match, so long as it has a matching type.
		 * @returns			ArgContainerType
		 */
		template<ValidArg... Types, ValidInputType... Names>
		_CONSTEXPR const ArgContainerType get_all(const Names&... names) const noexcept
		{
			if (const auto targets{ find_all<Types...>(names...) }; !targets.empty()) {
				std::vector<VariantArgumentType> vec;
				vec.reserve(targets.size());
				for (auto& it : targets)
					vec.emplace_back(*it);
				vec.shrink_to_fit();
				return vec;
			}
			return{}; // return empty
		}
	#pragma endregion GetFunctions

	#pragma region GetRangeFunction
		/**
		 * @brief				Retrieve a range of arguments from the list, with an optional predicate function for filtering.
		 * @param from			The beginning position in the list.
		 * @param to			The ending position in the list.
		 * @param inclusive		When true, includes the element at the "to" iterator in the container.
		 * @param pred			An optional predicate function that accepts a VariantArgumentType and returns a boolean.
		 * @returns				ArgContainerType
		 */
		_CONSTEXPR const ArgContainerType get_range(ArgContainerIteratorType from, const std::optional<ArgContainerIteratorType>& to = std::nullopt, const bool& inclusive = true, const std::optional<std::function<bool(VariantArgumentType)>>& pred = std::nullopt) const noexcept(false)
		{
			if (empty())
				throw std::exception("ArgContainer::get_range() failed because the container is empty!");
			if (const auto it_end{ to.value_or(_args.end()) }; inclusive ? it_end >= from : it_end > from) {
				ArgContainerType vec;
				vec.reserve(to.value_or(_args.end()) - from);
				for (; from != (inclusive ? (to.value_or(_args.end() - 1ll) + 1ll) : to.value()) && from != _args.end(); ++from)
					if (!pred.has_value() || pred.value()(*from))
						vec.emplace_back(*from);
				vec.shrink_to_fit();
				return vec;
			}
			throw std::exception("ArgContainer::get_range()\tIterator position was greater than the given ending position!");
		}

		/**
		 * @brief				Retrieve a range of arguments from the list, with an optional predicate function for filtering.
		 * @param from			The beginning index in the list.
		 * @param to			The ending index in the list.
		 * @param inclusive		When true, includes the element at the "to" iterator in the container.
		 * @param pred			An optional predicate function that accepts a VariantArgumentType and returns a boolean.
		 * @returns				ArgContainerType
		 */
		_CONSTEXPR const ArgContainerType get_range(const size_t& from, const std::optional<size_t>& to = std::nullopt, const bool& inclusive = true, const std::optional<std::function<bool(VariantArgumentType)>>& pred = std::nullopt) const noexcept(false)
		{
			// if from is less than the size of the list, and to doesn't have a value, or if to is less than the list size AND greater or equal to from
			if (from < _args.size() && (!to.has_value() || (to.value() < _args.size() && to.value() >= from)))
				return get_range(_args.begin() + from, (to.has_value() ? _args.begin() + to.value() : _args.end()), inclusive, pred);
			throw std::exception(str::stringify("ArgContainer::get_range()\tInvalid indexes where from == ", from, " && to == ", to.value_or(_args.size()), ". (Argument list size is ", _args.size(), ')').c_str());
		}

		virtual const ArgContainer get_range_copy(const ArgContainerIteratorType& from, const std::optional<ArgContainerIteratorType>& to = std::nullopt, const bool& inclusive = true, const std::optional<std::function<bool(VariantArgumentType)>>& pred = std::nullopt) const
		{
			ArgContainerType cont;
			for (auto& it : get_range(from, to, inclusive, pred))
				cont.emplace_back(std::move(it));
			return decltype(*this){ std::move(cont), _arg0 };
		}
	#pragma endregion GetRangeFunction
	};
}