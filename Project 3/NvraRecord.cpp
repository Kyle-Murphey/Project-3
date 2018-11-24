#include "NvraRecord.h"
#include <string>
#include <limits>


/*
	overloads the '<<' operator to make outputting data cleaner
	puts ';' between each element
	@param:os = output stream e.i. 'std::cout'
	@param:record = reference to the line that needs to be printed
	@return = the output
*/
std::ostream & operator<<(std::ostream & os, const NvraRecord & record)
{
	int stringNum = 0; // keeps track of the index in the strings array
	int intNum = 0; // keeps track of the index in the nums array

	//loops through each element in the line
	for (unsigned int index = 0; index < 24; ++index)
	{
		//string elements
		if (index == 3 || index == 11 || index == 12)
		{
			os << record.strings[stringNum] << ";";
			++stringNum;
		}
		//int elements
		else
		{
			//last element doesn't get a ';' printed after it
			if (index == 23)
			{
				os << record.nums[intNum];
				++intNum;
			}
			//all other elements get a ';' after
			else
			{
				os << record.nums[intNum] << ";";
				++intNum;
			}
		}
	}
	return os;
}

/*
	adds the numbers to the nums array
	@param:num = value to be added to the array
*/
void NvraRecord::addNum(unsigned int num)
{
	//invalid data///////////////////////////////////////////////////////////////////FIXM SHITE
	if (num == std::numeric_limits<int>::max())
	{
		numCtr = 0; // resetting index for nums
		strCtr = 0; // restting index for strings
		return;
	}
	//index has reached its limit
	if (numCtr > (MAX_NUMS - 1))
	{
		numCtr = 0; // resetting index for nums
	}
	nums[numCtr] = num; // adds the passed in num to the nums array at index numCtr
	++numCtr; // increment the nums index
}

/*
	adds the string to the strings array
	@param:string = string to be added to the array
*/
void NvraRecord::addString(std::string string)
{
	//index has reached its limit
	if (strCtr > 2)
	{
		strCtr = 0; // resetting index for strings
	}
	strings[strCtr] = string; // adds the passed in string to the strings array at index strCtr
	++strCtr; // increment the strings index
}

/*
	gets value at given index
	@param:index = index of number you are trying to get
	@return = value at given index
*/
unsigned int NvraRecord::getNum(unsigned int index) const
{
	return nums[index];
}

/*
	gets string at given index
	@param:index = index of string you are trying to get
	@return = string at given index
*/
std::string NvraRecord::getString(unsigned int index) const
{
	return strings[index];
}

/*
	sets the number at the index equal to the passed in num
	@param:num = unsigned int you want to pass in at a given index
	@param:index = index of the num you wish to set
*/
void NvraRecord::setNum(unsigned int num, unsigned int index)
{
	nums[index] = num;
}

/*
	sets the string at the index equal to the passed in string
	@param:string = std::string you want to pass in at a given index
	@param:index = index of the string you wish to set
*/
void NvraRecord::setString(std::string string, unsigned int index)
{
	strings[index] = string;
}