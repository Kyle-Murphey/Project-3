#include "NvraHasher.h"

unsigned long NvraHasher::hash(const NvraRecord& item) const
{
	return item.getNum(0);
}