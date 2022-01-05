#pragma once
#include <sysarch.h>
#include <ArgParser.hpp>

#include <utility>

namespace opt {
	class ParamsAPI2 : public ArgContainer {

	public:
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
		 * @brief		Retrieve an argument of a specified type as its actual type; not as a variant.
		 * @tparam Type	Type to search for & return as.
		 * @tparam Name	Input argument name type.
		 * @param name	Argument name to search for.
		 * @param off	Position to start searching at. (inclusive)
		 * @param end	Position to stop searching at. (exclusive)
		 * @returns		std::optional<Type>
		 */
		template<ValidArg Type, ValidInputType Name>
		constexpr const std::optional<Type> typeget(const Name& name, const std::optional<ArgContainerIteratorType>& off = std::nullopt, const std::optional<ArgContainerIteratorType>& end = std::nullopt) const noexcept
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
		constexpr const std::optional<Type> typeget_any(const Names&... names) const noexcept
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
		constexpr const std::vector<Type> typeget_all(const Names&... names) const noexcept
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

		template<CanHaveValueArgument Type, ValidInputType Name> requires std::same_as<Type, Option> || std::same_as<Type, Flag>
		constexpr const std::optional<std::string> typegetv(const Name & name, const std::optional<ArgContainerIteratorType>&off = std::nullopt, const std::optional<ArgContainerIteratorType>&end = std::nullopt) const noexcept(false)
		{
			if (const auto target{ find<Type>(name, off, end) }; target != _args.end())
				return std::get<Type>(*target).getv();
			return std::nullopt;
		}

		template<CanHaveValueArgument... Types, ValidInputType... Names>
		constexpr const std::optional<std::string> typegetv_any(const Names&... names) const
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

		template<var::all_same<Parameter> Type, ValidInputType... Names>
		constexpr const std::vector<std::string> typegetv_all(const Names&... names) const
		{
			std::vector<std::string> vec;
			vec.reserve(_args.size());
			for (auto& it : typeget_all<Type>(InputWrapper(names)...))
				vec.emplace_back(get_name(it));
			vec.shrink_to_fit();
			return vec;
		}
		template<CanHaveValueArgument... Types, ValidInputType... Names> requires var::not_same<Parameter, Types...>
		constexpr const std::vector<std::string> typegetv_all(const Names&... names) const
		{
			constexpr const bool match_any_type{ var::none<Types...> }, match_any_name{ var::none<Names...> };
			std::vector<std::string> vec;
			vec.reserve(_args.size());
			for (auto& it : get_all<Types...>(names...)) {
				if (const auto name{ get_name(it) };  match_any_name || var::variadic_or(InputWrapper(names) == name...)) {
					if constexpr (match_any_type || var::any_same<Option, Types... >) {
						if (is_type<Option>(it))
							if (const auto ty{ std::get<Option>(it) }; ty.hasv())
								vec.emplace_back(ty.getv().value());
					}
					if constexpr (match_any_type || var::any_same<Flag, Types...>) {
						if (is_type<Flag>(it))
							if (const auto ty{ std::get<Flag>(it) }; ty.hasv())
								vec.emplace_back(ty.getv().value());
					}
					if constexpr (match_any_type || var::any_same<Parameter, Types...>) {
						if (is_type<Parameter>(it))
							vec.emplace_back(name);
					}
				}
			}
			vec.shrink_to_fit();
			return vec;
		}
	};
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