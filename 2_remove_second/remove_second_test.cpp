#include <chrono>
#include <iostream>
#include <vector>
#include "remove_second.h"

/// Type of the test value
using test_value_type = size_t;

/// Type of the test vector
using test_vector_type = std::vector<test_value_type>;

/// Type alias for function pointer type
using test_function_type =
	std::pair<void (*)(test_vector_type &), std::string>;

/**
	\brief Creates a test vector of given size
	The vector contains values from 1 to size.
*/
test_vector_type create_test_vector(size_t size)
{
	auto range = std::views::iota(test_value_type(1), test_value_type(size) + 1);
	return {range.begin(), range.end()};
}

/**
	\brief Run simple validation tests on the provided functions
	\note It would be better to formalize this using
	a unit testing framework like gtest,
	but for simplicity and self-containment we do it just this way.

	\throw std::runtime_error if any test fails
*/
void run_validation_tests(
	const std::vector<test_function_type> &functions)
{
	static const std::vector<test_vector_type> TEST_VECTORS = {
		{},
		{},
		{1, 2},
		{1, 2, 3},
		{1, 2, 3, 4},
		{1, 2, 3, 4, 5},
		{1, 2, 3, 4, 5, 6}};

	static const std::vector<test_vector_type> EXPECTED_RESULTS = {
		{},
		{},
		{1},
		{1, 3},
		{1, 3},
		{1, 3, 5},
		{1, 3, 5}};

	for (size_t test_ind = 0; test_ind < TEST_VECTORS.size(); ++test_ind)
	{
		for (const auto &[func, name] : functions)
		{
			// Make a copy of the original vector for each function
			test_vector_type test_vector = TEST_VECTORS[test_ind];
			func(test_vector);
			if (test_vector != EXPECTED_RESULTS[test_ind])
			{
				std::stringstream error_message;
				error_message << "ERROR: " << name << " failed for the test vector of size ";
				error_message << TEST_VECTORS[test_ind].size() << "!\n";
				throw std::runtime_error(error_message.str());
			}
		}
	}
}

/**
	\brief Run performance tests on the provided functions,
	also checking for correctness
*/
int run_performance_tests(
	const std::vector<test_function_type> &functions)
{
	constexpr std::array<size_t, 6> PERFORMANCE_TEST_SIZES = {
		1'000,
		10'000,
		100'000,
		1'000'000,
		10'000'000,
		50'000'000};

	// Lambda to check if all vectors in a vector are equal
	auto check_if_all_equal = [](const auto &v) -> bool
	{
		return std::adjacent_find(
				   v.begin(),
				   v.end(),
				   std::not_equal_to<>()) == v.end();
	};

	for (const size_t test_size : PERFORMANCE_TEST_SIZES)
	{
		const test_vector_type vec = create_test_vector(test_size);

		std::cout << "\nTesting with vector size: " << test_size << "\n";
		std::vector<test_vector_type> results;
		for (const auto &[function, function_name] : functions)
		{
			auto start = std::chrono::high_resolution_clock::now();
			function(results.emplace_back(vec));
			auto end = std::chrono::high_resolution_clock::now();

			std::chrono::duration<double, std::milli> duration = end - start;
			std::cout << "'" << function_name << "' took " << duration.count() << " ms\n";
		}

		// In addition to performance, check that all results are the same
		if (!check_if_all_equal(results))
		{
			throw std::runtime_error(
				"ERROR: results differ between methods for size " +
				std::to_string(test_size));
		}
	}
}

/// Main function to run validation and performance tests
int main()
{
	try
	{
		const std::vector<test_function_type> functions_to_test = {
			{remove_second<test_value_type>, "remove_second"},
			{remove_second_using_remove_if<test_value_type>, "remove_second_using_remove_if"},
			{remove_second_using_ranges<test_value_type>, "remove_second_using_ranges"}};

		run_validation_tests(functions_to_test);
		run_performance_tests(functions_to_test);
		return 0;
	}
	catch (const std::exception &e)
	{
		std::cerr << e.what() << '\n';
		return 1;
	}
	catch (...)
	{
		std::cerr << "Unknown exception occurred\n";
		return 1;
	}
}
