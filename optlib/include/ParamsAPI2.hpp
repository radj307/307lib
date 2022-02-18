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

		template<std::same_as<CompoundFlag> Type>
		[[nodiscard]] constexpr const std::optional<CompoundFlag> typeget(const std::string& compoundFlag, const std::optional<ArgContainerIteratorType>& off = std::nullopt, const std::optional<ArgContainerIteratorType>& end = std::nullopt) const noexcept
		{
			if (!compoundFlag.empty()) {
				const auto& pos{ find<CompoundFlag>(compoundFlag, off, end) };
				const auto& size{ compoundFlag.size() };
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

		template<std::same_as<CompoundFlag> Type>
		[[nodiscard]] constexpr const std::optional<std::string> typegetv(const std::string& name, const std::optional<ArgContainerIteratorType>& off = std::nullopt, const std::optional<ArgContainerIteratorType>& end = std::nullopt) const noexcept
		{
			if (const auto& f{ typeget<CompoundFlag>(name, off, end) }; f.has_value())
				return f.value().getv();
			return std::nullopt;
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
		 * @tparam Names	Input argument name types.
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
		 * @tparam Names	Input argument name types.
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
		 * @brief			Retrieve the capture argument of the first matching argument in the list.
		 * @tparam Types	Argument type(s) to search for. If no types are specified, arguments of any type are considered valid matches.
		 * @tparam Names	Variadic argument name type(s).
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
		 * @brief			Overload of the typegetv_all function that allows retrieval of Parameter arguments with the same syntax.
		 *\n				Rather than retrieving captured values, this function retrieves the names of the parameters.
		 * @tparam Type		Parameter type.
		 * @tparam Names	Input argument name types. (string/char)
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
		 * @brief			Retrieve the captured values of all specified arguments.
		 *\n				If a Parameter type is specified, the parameter itself is returned instead of a capture value.
		 * @tparam Types	Argument Types to search for. If no types are specified, arguments of any type are considered valid matches.
		 * @tparam Names	Input argument name types. (string/char)
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