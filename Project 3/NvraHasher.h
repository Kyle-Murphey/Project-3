#ifndef NVRA_HASHER_H
#define NVRA_HASHER_H

#include "Hasher.h"
#include "NvraRecord.h"

class NvraHasher : public Hasher<NvraRecord> {
public:
	unsigned long hash(const NvraRecord& item) const;	// implement a hash function appropriate for NvraRecords in NvraHasher.cpp
};

unsigned long NvraHasher::hash(const NvraRecord& item) const
{
	return item.getNum(0);
}
#endif //!NVRA_HASHER_H