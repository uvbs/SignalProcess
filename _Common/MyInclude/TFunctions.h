
#pragma once

//[first, last)
template<typename iteratorType, typename cbArgType>
std::list<typename iteratorType::value_type> TSorter(iteratorType first, iteratorType last, bool (*sorter)(iteratorType& i, cbArgType& cbArg), cbArgType& cbArg)
{
	std::list<iteratorType::value_type> sorterResult;
	for (; first != last; ++first)
	{
		if (sorter(first, cbArg)) sorterResult.push_back(*first);
	}
	return sorterResult;
}