#pragma once
#include <sysarch.h>
#include <ArgParser.hpp>

#include <utility>

namespace opt {
	struct ParamsAPI2 : ArgContainer {
		/// @brief Default Constructor.
		ParamsAPI2() = default;
		ParamsAPI2(const ParamsAPI2&) = default;
		ParamsAPI2(ParamsAPI2&&) = default;
		/**
		 * @brief				Parsing Constructor.
		 * @param argc			Argument array size from main.
		 * @param argv			Argument array from main.
		 * @param ...captures	Argument names that should be able to capture. Do not include delimiter prefixes, they will be stripped.
		 */
		template<ValidInputType... VT>
		ParamsAPI2(const int argc, char** argv, const VT&... captures) : ArgContainer(parse(vectorize(argc, argv, 1), captures...), argv[0]) {}
		/**
		 * @brief				Parsing Constructor.
		 * @param args			Argument vector.
		 * @param ...captures	Argument names that should be able to capture. Do not include delimiter prefixes, they will be stripped.
		 */
		template<ValidInputType... VT>
		ParamsAPI2(const std::vector<std::string>& args, const VT&... captures) : ArgContainer(parse(args, captures...)) {}

		ParamsAPI2& operator=(const ParamsAPI2&) = default;
		ParamsAPI2& operator=(ParamsAPI2&&) = default;

		/**
		 * @brief		Insert the parsed argument list into an output stream in the order they were read, separated with spaces.
		 * @param os	Output Stream
		 * @param obj	ParamsAPI2 instance.
		 * @returns		std::ostream
		 */
		friend std::ostream& operator<<(std::ostream& os, const ParamsAPI2& obj)
		{
			for (auto arg{ obj.begin() }; arg != obj.end(); ++arg) {
				os << *arg;
				if (arg != obj.end() - 1ll)
					os << ' ';
			}
			return os;
		}

		/**
		 * @brief			Check if a given Flag was specified.
		 * @param flag		The name of a Flag to search for.
		 * @returns			bool
		 */
		[[nodiscard]] WINCONSTEXPR const bool checkflag(const char& flag) const
		{
			return check<opt::Flag>(flag);
		}
		/**
		 * @brief			Check if a given Option was specified.
		 * @param option	The name of an Option to search for.
		 * @returns			bool
		 */
		[[nodiscard]] WINCONSTEXPR const bool checkopt(const std::string& option) const
		{
			return check<opt::Option>(option);
		}
		/**
		 * @brief			Check if a given Parameter was specified.
		 * @param param		The name of a Parameter to search for.
		 * @returns			bool
		 */
		template<ValidInputType Name>
		[[nodiscard]] WINCONSTEXPR const bool checkparam(const Name& param) const
		{
			return check<opt::Parameter>(param);
		}

		/**
		 * @brief			Retrieve a CompoundFlag.
		 * @tparam Type		Compound Flag Type.
		 * @param name		A string containing each flag that composes the compound flag, in order.
		 * @param off		Optional begin iterator position. If not specified, defaults to begin().
		 * @param end		Optional end iterator position. If not specified, defaults to end().
		 * @returns			std::optional<CompoundFlag>
		 */
		template<std::same_as<CompoundFlag> Type>
		[[nodiscard]] constexpr const std::optional<CompoundFlag> typeget(const std::string& name, const std::optional<ArgContainerIteratorType>& off = std::nullopt, const std::optional<ArgContainerIteratorType>& end = std::nullopt) const noexcept
		{
			if (!name.empty()) {
				const auto& pos{ find<CompoundFlag>(name, off, end) };
				const auto& size{ name.size() };
				if (std::distance(pos, _args.end()) >= size) {
					std::vector<char> cmpd;
					size_t i{ 0ull };
					for (auto it{ pos }; it != _args.end(); ++it, ++i) {
						const auto& flag{ std::get<Flag>(*it) };
						cmpd.emplace_back(flag.flag());
						if (i == size - 1ull)
							return std::make_pair(std::move(cmpd), flag.getv());
					}
				}
			}
			return std::nullopt;
		}

		/**
		 * @brief			Retrieve a CompoundFlag.
		 *\n				This function accepts variadic chars instead of a string, `begin()` & `end()` are passed to the typeget function as `off` & `end`, respectively.
		 * @tparam Type		Compound Flag Type.
		 * @tparam Name...	At least one char type.
		 * @param name		At least one character representing the compound flag, in order.
		 * @returns			std::optional<CompoundFlag>
		 */
		template<std::same_as<CompoundFlag> Type, var::same_or_convertible<char>... Name> requires var::at_least_one<Name...>
		[[nodiscard]] constexpr const std::optional<CompoundFlag> typeget(const Name&... name) const noexcept
		{
			return typeget<CompoundFlag>(std::string{ name... }, _args.begin(), _args.end());
		}

		/**
		 * @brief			Retrieve the captured argument from a CompoundFlag.
		 * @tparam Type		Compound Flag Type.
		 * @param name		A string containing each flag that composes the compound flag, in order.
		 * @param off		Optional begin iterator position. If not specified, defaults to begin().
		 * @param end		Optional end iterator position. If not specified, defaults to end().
		 * @returns			std::optional<std::string>
		 */
		template<std::same_as<CompoundFlag> Type>
		[[nodiscard]] constexpr const std::optional<std::string> typegetv(const std::string& name, const std::optional<ArgContainerIteratorType>& off = std::nullopt, const std::optional<ArgContainerIteratorType>& end = std::nullopt) const noexcept
		{
			if (const auto& f{ typeget<CompoundFlag>(name, off, end) }; f.has_value())
				return f.value().getv();
			return std::nullopt;
		}

		/**
		 * @brief			Retrieve the captured argument from a CompoundFlag.
		 *\n				This function accepts variadic chars instead of a string.
		 * @tparam Type		Compound Flag Type.
		 * @tparam Names...	At least one char type.
		 * @param name		At least one character representing the compound flag, in order.
		 * @returns			std::optional<CompoundFlag>
		 */
		template<std::same_as<CompoundFlag> Type, var::same_or_convertible<char>... Names> requires var::at_least_one<Names...>
		[[nodiscard]] constexpr const std::optional<std::string> typegetv(const Names&... names) const noexcept
		{
			return typegetv<CompoundFlag>(std::string{ names... }, _args.begin(), _args.end());
		}

		/**
		 * @brief		Retrieve an argument of a specified type as its actual type; not as a variant.
		 * @tparam Type	Type to search for & return as.
		 * @tparam Name	Input argument name type.
		 * @param name	Argument name to search for.
		 * @param off	Position to start searching at. (inclusive)
		 * @param end	Position to stop searching at. (exclusive)
		 * @returns		std::optional<Type>
		 */
		template<ValidArg Type, ValidInputType Name> requires (!std::same_as<Type, CompoundFlag>)
			[[nodiscard]] constexpr const std::optional<Type> typeget(const Name& name, const std::optional<ArgContainerIteratorType>& off = std::nullopt, const std::optional<ArgContainerIteratorType>& end = std::nullopt) const noexcept
		{
			const auto target{ get<Type>(name, off, end) };
			return (target.has_value() ? std::get<Type>(target.value()) : static_cast<std::optional<Type>>(std::nullopt));
		}

		/**
		 * @brief			Retrieve any arguments of a specified type as its actual type; not as a variant.
		 * @tparam Type		Type to search for & return as.
		 * @tparam Names...	Input argument name types.
		 * @param names		Argument names to search for. If no names are included, any argument with the specified type will be returned.
		 * @returns			std::optional<Type>
		 */
		template<ValidArg Type, ValidInputType... Names>
		[[nodiscard]] constexpr const std::optional<Type> typeget_any(const Names&... names) const noexcept
		{
			if (const auto t{ get_any<Type>(names...) }; t.has_value())
				return std::get<Type>(t.value());
			return std::nullopt;
		}

		/**
		 * @brief			Retrieve all arguments of a specified type, as their actual type; not as a variant.
		 * @tparam Type		Type to search for & return as.
		 * @tparam Names...	Input argument name types.
		 * @param names		Argument name(s) to search for. If no names are included, all arguments of the specified type will be returned.
		 * @returns			std::vector<Type>
		 */
		template<ValidArg Type, ValidInputType... Names>
		[[nodiscard]] constexpr const std::vector<Type> typeget_all(const Names&... names) const noexcept
		{
			if (const auto targets{ get_all<Type>(names...) }; !targets.empty()) {
				std::vector<Type> vec;
				vec.reserve(targets.size());
				for (auto& arg : targets)
					vec.emplace_back(std::get<Type>(arg));
				vec.shrink_to_fit();
				return vec;
			}
			return{};
		}

		/**
		 * @brief			Retrieve the captured argument of the specified option or flag.
		 * @tparam Type		Option or Flag type.
		 * @tparam Name		Argument name type.
		 * @param name		Argument name to search for.
		 * @param off		Optional begin iterator position. If not specified, defaults to begin().
		 * @param end		Optional end iterator position. If not specified, defaults to end().
		 * @returns			std::optional<std::string>
		 *\n				Value is std::nullopt when no match was found.
		 */
		template<CanHaveValueArgument Type, ValidInputType Name> requires std::same_as<Type, Option> || std::same_as<Type, Flag>
		[[nodiscard]] constexpr const std::optional<std::string> typegetv(const Name & name, const std::optional<ArgContainerIteratorType>&off = std::nullopt, const std::optional<ArgContainerIteratorType>&end = std::nullopt) const noexcept(false)
		{
			if (const auto target{ find<Type>(name, off, end) }; target != _args.end())
				return std::get<Type>(*target).getv();
			return std::nullopt;
		}

		/**
		 * @brief			Retrieve the captured argument of the specified option or flag, and cast it to the specified type using a given conversion function.
		 * @tparam RetType	Type to cast the result to before returning it.
		 * @tparam Type		Option or Flag type.
		 * @tparam Name		Argument name type.
		 * @param converter	A function that can perform the conversion from `std::string` => `RetType`. This is only called if the argument was found.
		 * @param name		Argument name to search for.
		 * @param off		Optional begin iterator position. If not specified, defaults to begin().
		 * @param end		Optional end iterator position. If not specified, defaults to end().
		 * @returns			std::optional<RetType>
		 *\n				Value is std::nullopt when no match was found.
		 */
		template<typename RetType, CanHaveValueArgument Type, ValidInputType Name> requires std::same_as<Type, Option> || std::same_as<Type, Flag>
		[[nodiscard]] constexpr const std::optional<RetType> castgetv(const std::function<RetType(std::string)>&converter, const Name & name, const std::optional<ArgContainerIteratorType>&off = std::nullopt, const std::optional<ArgContainerIteratorType>&end = std::nullopt) const noexcept(false)
		{
			if (const auto target{ find<Type>(name, off, end) }; target != _args.end())
				return converter(std::get<Type>(*target).getv());
			return std::nullopt;
		}

		/**
		 * @brief			Retrieve the captured argument of the specified option or flag as the specified type.
		 *\n				This is an overload of the castgetv function for types that can be constructed from std::string.
		 * @tparam RetType	Type to cast the result to before returning it. Must be constructible from a std::string.
		 * @tparam Type		Option or Flag type.
		 * @tparam Name		Argument name type.
		 * @param name		Argument name to search for.
		 * @param off		Optional begin iterator position. If not specified, defaults to begin().
		 * @param end		Optional end iterator position. If not specified, defaults to end().
		 * @returns			std::optional<RetType>
		 *\n				Value is std::nullopt when no match was found.
		 */
		template<std::constructible_from<std::string> RetType, CanHaveValueArgument Type, ValidInputType Name> requires std::same_as<Type, Option> || std::same_as<Type, Flag>
		[[nodiscard]] constexpr const std::optional<RetType> castgetv(const Name & name, const std::optional<ArgContainerIteratorType>&off = std::nullopt, const std::optional<ArgContainerIteratorType>&end = std::nullopt) const noexcept(false)
		{
			if (const auto target{ find<Type>(name, off, end) }; target != _args.end())
				return RetType{ std::get<Type>(*target).getv() };
			return std::nullopt;
		}

		/**
		 * @brief			Retrieve the capture argument of the first matching argument in the list.
		 * @tparam Types...	Argument type(s) to search for. If no types are specified, arguments of any type are considered valid matches.
		 * @tparam Names...	Variadic argument name type(s).
		 * @param names		Argument name(s) to search for. If no names are specified, arguments with any name are considered valid matches.
		 * @returns			std::optional<std::string>
		 *\n				Value is std::nullopt when no match was found.
		 */
		template<CanHaveValueArgument... Types, ValidInputType... Names>
		[[nodiscard]] constexpr const std::optional<std::string> typegetv_any(const Names&... names) const
		{
			constexpr const bool match_opts{ std::disjunction_v<std::is_same<Option, Types>...> }, match_flags{ std::disjunction_v<std::is_same<Flag, Types>...> }, match_any_name{ var::none<Names...> };
			if (const auto target{ find_any<Types...>(names...) }; target != _args.end()) {
				if (match_any_name || var::variadic_or(names == InputWrapper(get_name(*target))...)) {
					if constexpr (match_opts) {
						if (is_type<Option>(*target))
							return std::get<Option>(*target).getv();
					}
					if constexpr (match_flags) {
						if (is_type<Flag>(*target))
							return std::get<Flag>(*target).getv();
					}
				}
			}
			return std::nullopt;
		}
		/**
		 * @brief			Retrieve the capture argument of the first matching argument in the list, and cast it to the specified type using a given conversion function.
		 * @tparam RetType	Type to cast the result to before returning it.
		 * @tparam Types...	Argument type(s) to search for. If no types are specified, arguments of any type are considered valid matches.
		 * @tparam Names...	Variadic argument name type(s).
		 * @param converter	A function that can perform the conversion from `std::string` => `RetType`. This is only called if the argument was found.
		 * @param names		Argument name(s) to search for. If no names are specified, arguments with any name are considered valid matches.
		 * @returns			std::optional<RetType>
		 *\n				Value is std::nullopt when no match was found.
		 */
		template<typename RetType, CanHaveValueArgument... Types, ValidInputType... Names>
		[[nodiscard]] constexpr const std::optional<RetType> castgetv_any(const std::function<RetType(std::string)>& converter, const Names&... names) const
		{
			constexpr const bool match_opts{ std::disjunction_v<std::is_same<Option, Types>...> }, match_flags{ std::disjunction_v<std::is_same<Flag, Types>...> }, match_any_name{ var::none<Names...> };
			if (const auto target{ find_any<Types...>(names...) }; target != _args.end()) {
				if (match_any_name || var::variadic_or(names == InputWrapper(get_name(*target))...)) {
					if constexpr (match_opts) {
						if (is_type<Option>(*target))
							if (const auto& opt{ std::get<Option>(*target) }; opt.hasv())
								return converter(opt.getv().value());
					}
					if constexpr (match_flags) {
						if (is_type<Flag>(*target))
							if (const auto& fl{ std::get<Flag>(*target) }; fl.hasv())
								return converter(fl.getv().value());
					}
				}
			}
			return std::nullopt;
		}

		/**
		 * @brief			Retrieve the capture argument of the first matching argument in the list as the specified type.
		 *\n				This is an overload of the castgetv_any function for types that can be constructed from std::string.
		 * @tparam RetType	Type to cast the result to before returning it. Must be constructible from a std::string.
		 * @tparam Types...	Argument type(s) to search for. If no types are specified, arguments of any type are considered valid matches.
		 * @tparam Names...	Variadic argument name type(s).
		 * @param names		Argument name(s) to search for. If no names are specified, arguments with any name are considered valid matches.
		 * @returns			std::optional<RetType>
		 *\n				Value is std::nullopt when no match was found.
		 */
		template<std::constructible_from<std::string> RetType, CanHaveValueArgument... Types, ValidInputType... Names>
		[[nodiscard]] constexpr const std::optional<RetType> castgetv_any(const Names&... names) const
		{
			constexpr const bool match_opts{ std::disjunction_v<std::is_same<Option, Types>...> }, match_flags{ std::disjunction_v<std::is_same<Flag, Types>...> }, match_any_name{ var::none<Names...> };
			if (const auto target{ find_any<Types...>(names...) }; target != _args.end()) {
				if (match_any_name || var::variadic_or(names == InputWrapper(get_name(*target))...)) {
					if constexpr (match_opts) {
						if (is_type<Option>(*target))
							if (const auto& opt{ std::get<Option>(*target) }; opt.hasv())
								return RetType{ opt.getv().value() };
					}
					if constexpr (match_flags) {
						if (is_type<Flag>(*target))
							if (const auto& fl{ std::get<Flag>(*target) }; fl.hasv())
								return RetType{ fl.getv().value() };
					}
				}
			}
			return std::nullopt;
		}

		/**
		 * @brief			Overload of the typegetv_all function that allows retrieval of Parameter arguments with the same syntax.
		 *\n				Rather than retrieving captured values, this function retrieves the names of the parameters.
		 * @tparam Type		Parameter type.
		 * @tparam Names...	Input argument name types. (string/char)
		 * @param names		Argument name(s) to search for. If no names are included, all arguments of the specified type will be returned.
		 * @returns			std::vector<std::string>
		 */
		template<std::same_as<Parameter> Type, ValidInputType... Names>
		[[nodiscard]] constexpr const std::vector<std::string> typegetv_all(const Names&... names) const
		{
			std::vector<std::string> vec;
			vec.reserve(_args.size());
			for (auto& it : typeget_all<Type>(InputWrapper(names)...))
				vec.emplace_back(get_name(it));
			vec.shrink_to_fit();
			return vec;
		}

		/**
		 * @brief			Overload of the typegetv_all function that allows retrieval of Parameter arguments with the same syntax.
		 *\n				Rather than retrieving captured values, this function retrieves the names of the parameters, and casts them to the specified type using a given conversion function.
		 * @tparam RetType	Type to cast the result to before returning it.
		 * @tparam Type		Parameter type.
		 * @tparam Names...	Input argument name types. (string/char)
		 * @param converter	A function that can perform the conversion from `std::string` => `RetType`. This is only called if the argument was found.
		 * @param names		Argument name(s) to search for. If no names are included, all arguments of the specified type will be returned.
		 * @returns			std::vector<std::string>
		 */
		template<typename RetType, std::same_as<Parameter> Type, ValidInputType... Names>
		[[nodiscard]] constexpr const std::vector<RetType> castgetv_all(const std::function<RetType(std::string)>& converter, const Names&... names) const
		{
			std::vector<RetType> vec;
			vec.reserve(_args.size());
			for (auto& it : typeget_all<Type>(InputWrapper(names)...))
				vec.emplace_back(converter(get_name(it)));
			vec.shrink_to_fit();
			return vec;
		}

		/**
		 * @brief			Retrieve the captured values of all specified arguments.
		 *\n				If a Parameter type is specified, the parameter itself is returned instead of a capture value.
		 * @tparam Types...	Argument Types to search for. If no types are specified, arguments of any type are considered valid matches.
		 * @tparam Names...	Input argument name types. (string/char)
		 * @param names		Argument name(s) to search for. If no names are included, all arguments of the specified type will be returned.
		 * @returns			std::vector<std::string>
		 *\n				Vector is empty if no matches were found.
		 */
		template<CanHaveValueArgument... Types, ValidInputType... Names> requires var::not_same<Parameter, Types...>
		[[nodiscard]] constexpr const std::vector<std::string> typegetv_all(const Names&... names) const
		{
			constexpr const bool match_any_type{ var::none<Types...> }, match_any_name{ var::none<Names...> };
			std::vector<std::string> vec;
			vec.reserve(_args.size());
			for (auto& it : get_all<Types...>(names...)) {
				if (const auto name{ get_name(it) };  match_any_name || var::variadic_or(InputWrapper(names) == name...)) {
					if constexpr (match_any_type || var::any_same<Option, Types... >) {
						if (is_type<Option>(it)) {
							if (const auto ty{ std::get<Option>(it) }; ty.hasv())
								vec.emplace_back(ty.getv().value());
							continue;
						}
					}
					if constexpr (match_any_type || var::any_same<Flag, Types...>) {
						if (is_type<Flag>(it)) {
							if (const auto ty{ std::get<Flag>(it) }; ty.hasv())
								vec.emplace_back(ty.getv().value());
							continue;
						}
					}
					if constexpr (match_any_type || var::any_same<Parameter, Types...>) {
						if (is_type<Parameter>(it)) {
							vec.emplace_back(name);
							continue;
						}
					}
				}
			}
			vec.shrink_to_fit();
			return vec;
		}

		/**
		 * @brief			Retrieve the captured values of all specified arguments, casted to the specified type.
		 * @attention		When writing conversion functions, keep in mind that in addition to the captured values of types `Option` & `Flag`,
		 *					 the names of `Parameter`-type arguments may be passed to the converter, if the `Parameter` type is included in 'Types...'.
		 * @tparam RetType	Type to cast the result to before returning it.
		 * @tparam Types...	Argument Types to search for. If no types are specified, arguments of any type are considered valid matches.
		 * @tparam Names...	Input argument name types. (string/char)
		 * @param converter	A function that can perform the conversion from `std::string` => `RetType`. This is only called if the argument was found.
		 * @param names		Argument name(s) to search for. If no names are included, all arguments of the specified type will be returned.
		 * @returns			std::vector<RetType>
		 *\n				Vector is empty if no matches were found.
		 */
		template<typename RetType, CanHaveValueArgument... Types, ValidInputType... Names> requires var::not_same<Parameter, Types...>
		[[nodiscard]] constexpr const std::vector<RetType> castgetv_all(const std::function<RetType(std::string)>& converter, const Names&... names) const
		{
			constexpr const bool match_any_type{ var::none<Types...> }, match_any_name{ var::none<Names...> };
			std::vector<RetType> vec;
			vec.reserve(_args.size());
			for (auto& it : get_all<Types...>(names...)) {
				if (const auto name{ get_name(it) };  match_any_name || var::variadic_or(InputWrapper(names) == name...)) {
					if constexpr (match_any_type || var::any_same<Option, Types... >) {
						if (is_type<Option>(it)) {
							if (const auto ty{ std::get<Option>(it) }; ty.hasv())
								vec.emplace_back(converter(ty.getv().value()));
							continue;
						}
					}
					if constexpr (match_any_type || var::any_same<Flag, Types...>) {
						if (is_type<Flag>(it)) {
							if (const auto ty{ std::get<Flag>(it) }; ty.hasv())
								vec.emplace_back(converter(ty.getv().value()));
							continue;
						}
					}
					if constexpr (match_any_type || var::any_same<Parameter, Types...>) {
						if (is_type<Parameter>(it)) {
							vec.emplace_back(converter(name));
							continue;
						}
					}
				}
			}
			vec.shrink_to_fit();
			return vec;
		}

		/**
		 * @brief			Retrieve the captured values of all specified arguments as the specified type.
		 *\n				This is an overload of the castgetv_all function for types that can be constructed from std::string.
		 * @tparam RetType	Type to cast the result to before returning it. Must be constructible from a std::string.
		 * @tparam Types...	Argument Types to search for. If no types are specified, arguments of any type are considered valid matches.
		 * @tparam Names...	Input argument name types. (string/char)
		 * @param names		Argument name(s) to search for. If no names are included, all arguments of the specified type will be returned.
		 * @returns			std::vector<RetType>
		 *\n				Vector is empty if no matches were found.
		 */
		template<std::constructible_from<std::string> RetType, CanHaveValueArgument... Types, ValidInputType... Names> requires var::not_same<Parameter, Types...>
		[[nodiscard]] constexpr const std::vector<RetType> castgetv_all(const Names&... names) const
		{
			constexpr const bool match_any_type{ var::none<Types...> }, match_any_name{ var::none<Names...> };
			std::vector<RetType> vec;
			vec.reserve(_args.size());
			for (auto& it : get_all<Types...>(names...)) {
				if (const auto name{ get_name(it) };  match_any_name || var::variadic_or(InputWrapper(names) == name...)) {
					if constexpr (match_any_type || var::any_same<Option, Types... >) {
						if (is_type<Option>(it)) {
							if (const auto ty{ std::get<Option>(it) }; ty.hasv())
								vec.emplace_back(RetType{ ty.getv().value() });
							continue;
						}
					}
					if constexpr (match_any_type || var::any_same<Flag, Types...>) {
						if (is_type<Flag>(it)) {
							if (const auto ty{ std::get<Flag>(it) }; ty.hasv())
								vec.emplace_back(RetType{ ty.getv().value() });
							continue;
						}
					}
					if constexpr (match_any_type || var::any_same<Parameter, Types...>) {
						if (is_type<Parameter>(it)) {
							vec.emplace_back(RetType{ name });
							continue;
						}
					}
				}
			}
			vec.shrink_to_fit();
			return vec;
		}
	};

	// @brief	Alias for the ParamsAPI2 argument manager type.
	using Args = ParamsAPI2;

	/**
	 * @brief				Converts a vector of Parameters to a vector of strings.
	 * @param param_vec		A vector of Parameters.
	 * @returns				std::vector<std::string>
	 */
	inline WINCONSTEXPR const std::vector<std::string> paramVecToStrVec(const std::vector<opt::Parameter>& param_vec)
	{
		std::vector<std::string> strvec;
		strvec.reserve(param_vec.size());
		for (auto& it : param_vec)
			strvec.emplace_back(it.name());
		strvec.shrink_to_fit();
		return strvec;
	}
}