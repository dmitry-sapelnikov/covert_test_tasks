#pragma once

#include <ranges>
#include <vector>
#include <algorithm>

///	Removes every second element from the input vector using a simple loop
template <typename T>
void remove_second(std::vector<T> &target)
{
	// Note: (target.size() + 1) / 2 overflows when size = std::numeric_limits<size_t>::max()
	// We can replace / 2 and % 2 with >> 1 and & 1 respectively
	const size_t new_size = (target.size() / 2) + (target.size() % 2);
	for (size_t i = 1; i < new_size; ++i)
	{
		target[i] = std::move(target[i * 2]);
	}
	// We may use shrink_to_fit() instead of resize()
	target.resize(new_size);
}

///	Removes every second element from the input vector using std::remove_if
template <typename T>
void remove_second_using_remove_if(std::vector<T> &target)
{
	target.erase(
		std::remove_if(
			target.begin(),
			target.end(),
			[index = 0](const T &) mutable
			{ return (index++) & 1; }),
		target.end());
	// Optionally we may add shrink_to_fit() here
}

///	Removes every second element from the input vector using C++23 ranges
template <typename T>
void remove_second_using_ranges(std::vector<T> &target)
{
	std::ranges::move(target | std::views::stride(2), target.begin());
	target.resize((target.size() / 2) + (target.size() % 2));
}
