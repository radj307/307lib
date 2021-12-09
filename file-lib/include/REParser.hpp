#pragma once
#include <TokenizerRE.hpp>
#include <REContainer.hpp>

namespace token::parse {
	struct REParser {
	private:
		const std::string& filename;

	public:
		constexpr REParser(const std::string& filename) : filename{ filename } {}

		operator REContainer::ContainerType() const
		{
			// init temporary vars:
			std::string header{ "" };
			std::optional<REElement::ValueT> value;
			std::optional<REElement::SectionT> section;
			bool setter{ false };
			// init container:
			REContainer::ContainerType cont;

			using enum TokenType;


			return cont;
		}
	};
}