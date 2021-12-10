#include <xRand.hpp>
#include <TermAPI.hpp>

#include <bitset>
#include <vector>
#include <utility>

template<size_t size>
struct flagset {
private:
	std::bitset<size> _set;

	template<size_t i = 0ull, std::same_as<bool>... Vals>
	static std::bitset<size> set(std::bitset<size>& bitset, const std::tuple<Vals...>& vals)
	{
		bitset.set(i, std::get<i>(vals));
		if constexpr (i + 1ull < size)
			bitset = set<i + 1ull>(bitset, vals);
		return bitset;
	}

public:
	template<std::same_as<bool>... Flags>
	flagset(const Flags&... flags)
	{
		_set = set(_set, std::make_tuple(flags...));
	}
	flagset() = default;

	operator std::bitset<size>() const { return _set; }

	bool operator[](const size_t& pos) const { return _set[pos]; }

	void randomize()
	{
		rng::dRand rng;
		for (size_t i{ 0ull }; i < size; ++i)
			_set.set(i, rng.get(0, 1));
	}

	friend std::ostream& operator<<(std::ostream& os, const flagset<size>& set)
	{
		using namespace sys::term;
		os << setCharacterSet(CharacterSet::DEC_LINE_DRAWING);

		os << LineCharacter::CORNER_TOP_LEFT;
		for (size_t i{ 0ull }; i < size; ++i)
			os << LineCharacter::LINE_HORIZONTAL;
		os << LineCharacter::CORNER_TOP_RIGHT << '\n' << LineCharacter::LINE_VERTICAL;

		for (size_t i{ 0ull }; i < size; ++i)
			os << set._set[i] ? '1' : '0';

		os << LineCharacter::LINE_VERTICAL << '\n' << LineCharacter::CORNER_BOTTOM_LEFT;
		for (size_t i{ 0ull }; i < size; ++i)
			os << LineCharacter::LINE_HORIZONTAL;
		os << LineCharacter::CORNER_BOTTOM_RIGHT;

		os << setCharacterSet(CharacterSet::ASCII);
		return os;
	}
};

inline auto run_flagset_random_test()
{
	std::vector<std::pair<long double, long double>> res;

	const size_t test_count{ 106 };

	for (size_t i{ 0ull }; i < test_count; ++i) {
		flagset<128> fs128;
		fs128.randomize();

		std::cout << fs128 << std::endl;

		const auto on{ fs128.operator std::bitset<128Ui64>().count() };
		const auto on_percent{ static_cast<long double>(on) / 128.0l * 100.0l }, off_percent{ static_cast<long double>(128ull - on) / 128.0l * 100.0l };

		std::cout << "On:\t" << on_percent << "%\nOff:\t" << off_percent << "%\n";
		res.emplace_back(std::make_pair(on_percent, off_percent));
	}

	const auto& [total_on, total_off, total_eq] { [&res]() -> std::tuple<size_t, size_t, size_t> {
		size_t on_count{ 0ull }, off_count{ 0ull }, eq_count{ 0ull };
		for (auto& [on, off] : res) {
			if (on > off)
				++on_count;
			else if (off > on)
				++off_count;
			else
				++eq_count;
		}
		return { on_count, off_count, eq_count };
	}()};

	std::cout << "Total On:   \t" << (static_cast<long double>(total_on) / test_count * 100) << "%\n";
	std::cout << "Total Off:  \t" << (static_cast<long double>(total_off) / test_count * 100) << "%\n";
	std::cout << "Total Equal:\t" << (static_cast<long double>(total_eq) / test_count * 100) << "%\n";

	std::cout << color::setcolor(color::intense_green) << str::stringify("Hello", ' ', "World!", '\n') << color::reset;

}