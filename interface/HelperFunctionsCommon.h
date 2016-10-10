#ifndef HELPER_FUNCTIONS_COMMON_H
#define HELPER_FUNCTIONS_COMMON_H

#include <vector>
#include <stdexcept>
#include <exception>
#include <algorithm>

template <typename IntType>
std::vector<IntType> range(IntType start, IntType stop, IntType step)
{
	if (step == IntType(0))
	{
		throw std::invalid_argument("step for range must be non-zero");
	}
	std::vector<IntType> result;
	IntType i = start;
	while ((step > 0) ? (i < stop) : (i > stop))
	{
		result.push_back(i);
		i += step;
	}
	return result;
}
template <typename IntType>
std::vector<IntType> range(IntType start, IntType stop)
{
	return range(start, stop, IntType(1));
}
template <typename IntType>
std::vector<IntType> range(IntType stop)
{
	return range(IntType(0), stop, IntType(1));
}

// template <typename T, class UnaryPredicate>
// std::vector<T> filter(std::vector<T> vectorToFilter, UnaryPredicate pred)
// {
// 	std::vector<T> filteredVector(vectorToFilter.size());
// 	std::copy_if(vectorToFilter.begin(), vectorToFilter.end(), filteredVector.begin(), std::move(pred));
// 	filteredVector.shrink_to_fit();
// 	return filteredVector;
// }

template <class T, class UnaryPredicate>
std::vector<T> filter(const std::vector<T>& vectorToFilter, UnaryPredicate predicate)
{
	std::vector<T> result;
	for(auto it = vectorToFilter.begin(); it != vectorToFilter.end(); ++it)
	{
		if(predicate(*it)) result.push_back(*it);
	}
	return result;
}

#endif