#ifndef SORT_H
#define SORT_H

#include "Exceptions.h"
#include "TemplatedArray.h"
#include "Comparator.h"

template <typename T>
class Sorter {
private:
	static void quickSort(TemplatedArray<T>& list, const Comparator<T>& comparator, unsigned long left, unsigned long right);
public:
	static void sort(TemplatedArray<T>& array, const Comparator<T>& comparator);
};

template <typename T>
void Sorter<T>::sort(TemplatedArray<T>& array, const Comparator<T>& fieldComparator) {
	// unsigned long end = array.getSize();
	quickSort(array, fieldComparator, 0UL, array.getSize() - 1);
}

template <typename T>
void Sorter<T>::quickSort(TemplatedArray<T>& list, const Comparator<T>& comparator, unsigned long left, unsigned long right) {
	if (left < right) {
		// Pivot Value Choice Step
		unsigned long pivotPoint = (left + right) / 2;
		T pivot = list[pivotPoint];

		// Partition Step
		unsigned long i = left - 1;
		unsigned long j = right + 1;
		do {
			do {
				i = i + 1;
			} while ((i < list.getSize()) && (comparator.compare(list[i], pivot) <= 0));

			do {
				j = j - 1;
			} while (j >= i && comparator.compare(list[j], pivot) >= 0);

			if (i < j) {
				// swap items at i and j
				T temp1 = list[i];
				T temp2 = list[j];
				list.replaceAt(&temp2, i);
				list.replaceAt(&temp1, j);
			}
		} while (i < j);

		if (i < pivotPoint) {
			j = i;
		}
		// swap items at left and j
		T temp1 = list[pivotPoint];
		T temp2 = list[j];
		list.replaceAt(&temp2, pivotPoint);
		list.replaceAt(&temp1, j);

		// Recursive Steps
		if (j > 0) {
			quickSort(list, comparator, left, j - 1);
		}
		quickSort(list, comparator, j + 1, right);
	}
}

#endif // !SORTER_H