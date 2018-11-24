#ifndef SEARCH_H
#define SEARCH_H

#include "Exceptions.h"
#include "TemplatedArray.h"
#include "Comparator.h"

// returns FIRST array location matching the given item (based on the comparator)
// if item is not found, returns (-(insertion point) - 1), where insertion point is where the item would be inserted
/*
searches through array with binary search for the matching value in the array that comes first

~~~~~~~~~~~~~~Adapted from the ZyBooks example~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

@param:item = item to search for (key)
@param:array = array to search through
@param:comparator = comparator
@return = index of the value found(returns negative index that the value would be located if val not found)
*/
template <typename T>
long long binarySearch(const T& item, const TemplatedArray<T>& array, const Comparator<T>& comparator) {
	unsigned long low = 0; // low index
	long mid = 0; // middle index
	unsigned long high = array.getSize() - 1; // high index

	//loops through each partition
	while (high >= low)
	{
		mid = (high + low) / 2;

		//item is higher than midpoint
		if (comparator.compare(array[mid], item) == -1)
		{
			low = mid + 1;
		}
		//item is lower than midpoint
		else if (comparator.compare(array[mid], item) == 1)
		{
			//item would be located at the beginning of the array
			if (high == 0)
			{
				return -1;
			}
			high = mid - 1;
		}
		//returns index of first instance in the array
		else
		{
			if (mid > 0)
			{
				//compares values to the left to find the very first instance of the matching val
				while (comparator.compare(array[mid], array[mid - 1]) == 0)
				{
					--mid;
					if (mid == 0) break;
				}
			}
			return mid;
		}
	}
	//item not found
	return -mid;
}

/*
	searches for items with linear search and stores indeces in TemplatedArray<unsigned long> indeces

	@param:item = item searching for (key)
	@param:array = array to search through
	@param:comparator = comparator
	@param:indeces = stored the locations of found items
	@param:found = flag for found vals
*/
template <typename T>
void linearSearch(const T& item, const TemplatedArray<T>& array, const Comparator<T>& comparator, TemplatedArray<unsigned long> &indeces, bool &found)
{
	found = false;

	//loops through array linearly
	for (unsigned long index = 0; index < array.getSize(); ++index)
	{
		if (comparator.compare(array[index], item) == 0)
		{
			indeces.add(&index);
			found = true;
		}
	}
}
#endif