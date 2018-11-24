#include "NvraComparator.h"

/*
	constructor
	@param:column = the column at which values are to be compared
*/
NvraComparator::NvraComparator(unsigned int column)
{
	if (column > 23) throw new ExceptionIndexOutOfRange;
	this->column = column;
}

/*
	compares NvraRecords and returns an int value
	@param:item1 = first item to compare
	@param:item2 = second item to compare
	@return = -1 is item1 < item2, 0 if item1 == item2, +1 if item1 > item2
*/
int NvraComparator::compare(const NvraRecord & item1, const NvraRecord & item2) const
{
	if (column < 3)
	{
		if (item1.getNum(column) > item2.getNum(column)) return 1;
		else if (item1.getNum(column) < item2.getNum(column)) return -1;
		else return 0;
	}
	//first string index
	else if (column == 3)
	{
		if (item1.getString(0) > item2.getString(0)) return 1;
		else if (item1.getString(0) < item2.getString(0)) return -1;
		else return 0;
	}
	else if (column > 3 && column < 11)
	{
		if (item1.getNum(column - 1) > item2.getNum(column - 1)) return 1;
		else if (item1.getNum(column - 1) < item2.getNum(column - 1)) return -1;
		else return 0;
	}
	//second string index
	else if (column == 11)
	{
		if (item1.getString(1) > item2.getString(1)) return 1;
		else if (item1.getString(1) < item2.getString(1)) return -1;
		else return 0;
	}
	//third string index
	else if (column == 12)
	{
		if (item1.getString(2) > item2.getString(2)) return 1;
		else if (item1.getString(2) < item2.getString(2)) return -1;
		else return 0;
	}
	else
	{
		if (item1.getNum(column - 3) > item2.getNum(column - 3)) return 1;
		else if (item1.getNum(column - 3) < item2.getNum(column - 3)) return -1;
		else return 0;
	}
}