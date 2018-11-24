#ifndef NVRA_COMPARATOR_H
#define NVRA_COMPARATOR_H

#include "Comparator.h"
#include "NvraRecord.h"

class NvraComparator : public Comparator<NvraRecord> {
private:
	unsigned int column = 0;
public:
	NvraComparator(unsigned int column);
	int compare(const NvraRecord& item1, const NvraRecord& item2) const;	// returns -1 is item1 < item2, 0 if item1 == item2, +1 if item1 > item2
};

#endif // !NVRA_COMPARATOR_H