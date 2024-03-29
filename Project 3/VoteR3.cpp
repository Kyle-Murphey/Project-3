#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include "NvraRecord.h"
#include "NvraComparator.h"
#include "Search.h"
#include <limits>
#include "Sorter.h"
#include "OULinkedList.h"
#include "HashTable.h"
#include "NvraHasher.h"
#include "HashTableEnumerator.h"

/*****************************************************************************************************************************
*	VoteR 4.0                                                                                                                *
*	Project 2: CS 2413                                                                                                       *
*	12/6/18                                                                                                                  *
*	Kyle Murphey                                                                                                             *
*	                                                                                                                         *
*	This application reads in a .csv file to a hash table                                                                    *
*	Data can be added, replaced, and removed from the table                                                                  *
*	It also filters out errors and prints them first                                                                         *
*	Can output data to the console or a file, merge data, sort data, purge data, and search for data                         *
******************************************************************************************************************************/


/*
	Taken from my lab: VoteR 3.0

	loops through the valid record Ids that have already been stored
	@param:id = the id of the line
	@param:recordIds = reference to the where the record ids are stored
	@return = whether or not the id already exists
*/
bool doesIDExist(int id, std::vector<int>& recordIds)
{
	bool doesExist = false;

	for (unsigned int index = 0; index < (unsigned int)recordIds.size(); ++index)
	{
		// id exists
		if (id == recordIds.at(index))
		{
			doesExist = true;
		}
	}
	return doesExist;
}

/*
	much of the code is adapted from my lab: VoteR 3.0

	loops through the file and checks for invalid data and duplicates
	@param:file = file to read from
	@param:recordIDs = record IDs
	@param:lines = number of lines in each file
	@param:arrCount = number of valid lines
	@param:list = linked list to hold records
*/
void parseFile(std::ifstream &file, std::vector<int> recordIDs, unsigned int &lines, unsigned int &arrCount, OULinkedList<NvraRecord> &list)
{
	const unsigned int COLUMNS = 24;
	std::string input;
	bool idExists = false; // flag for duplicate IDs
	bool isValid = true;
	NvraRecord nvraRecord;

	std::getline(file, input);

	while (!file.eof())
	{
		++lines; // incrementing line number
		isValid = true; // setting validity flag to true by default
		//loops through each piece of data in the line
		for (unsigned int index = 0; index < COLUMNS; ++index)
		{
			//checking the last element in the line since it doesn't end in a ';'
			if (index == (COLUMNS - 1))
			{
				std::getline(file, input, '\n');
				//making sure value is valid
				if (stoi(input) < 0)
				{
					nvraRecord.addNum(std::numeric_limits<int>::max()); // sending the invalid value to the NvraRecord class so it can reset its counters
					std::cout << "Invalid data at line " << std::to_string(lines) << "." << std::endl;
					isValid = false; // flagging line as invalid
					recordIDs.pop_back(); // removing ID from the recordIDs list
					break;
				}
				else
				{
					nvraRecord.addNum(stoi(input)); // adding value to the nums array
					break;
				}
			}

			//getting the first element of the line
			std::getline(file, input, ',');
			//checking whether or not the line even has data in it
			if (input.empty())
			{
				isValid = false;
				--lines;
				break;
			}
			//checking all int values for validity
			if (index != 3 && index != 11 && index != 12)
			{
				if (stoi(input) < 0)
				{
					nvraRecord.addNum(std::numeric_limits<int>::max()); // sending the invalid value to the NvraRecord class so it can reset its counters
					std::cout << "Invalid data at line " << std::to_string(lines) << "." << std::endl;
					std::getline(file, input, '\n'); // skipping the rest of the line
					isValid = false; // flagging line as invalid
					recordIDs.pop_back(); // removing ID from the recordIDs list
					break;
				}
			}
			//checking the record id for duplicates
			if (index == 0)
			{
				//sends ID to doesIDExist function to check if it is a duplicate
				idExists = doesIDExist(stoi(input), recordIDs);
				//id doesn't exist
				if (!idExists)
				{
					recordIDs.push_back(stoi(input)); // adding ID to the recordIds list
					nvraRecord.addNum(stoi(input)); // adding ID to the nums array
				}
				//id is a duplicate
				else
				{
					idExists = false; // resetting flag
					std::cout << "Duplicate record ID " << std::to_string(stoi(input)) << " at line " << std::to_string(lines) << "." << std::endl;
					std::getline(file, input, '\n'); // skipping the rest of the line
					isValid = false; // flagging as invalid
					++arrCount;
					break;
				}
			}
			//all other elements in the line
			else
			{
				//string elements
				if (index == 3 || index == 11 || index == 12)
				{
					nvraRecord.addString(input);
				}
				//int elements
				else
				{
					nvraRecord.addNum(stoi(input));
				}
			}
		}
		//if the line is valid, it gets added to the nvra list
		if (isValid)
		{
			++arrCount; // keeps track of valid lines in the array

			list.insert(&nvraRecord);

		}
	}
}

/*
	checks for duplicates using binary search
	@param:nvraArray = reference to the array you're looking through
	@param:record = the NvraRecord being search for (at specified column)
	@param:comparator = comparator object used to compare the values
	@param:location = keeps track of the index that the duplicate is located
	@return = whether or not a duplicate was found
*/
bool checkDuplicates(TemplatedArray<NvraRecord> &nvraArray, const NvraRecord &record, const Comparator<NvraRecord>& comparator, unsigned long &location)
{
	long long result = binarySearch(record, nvraArray, comparator);

	//duplicate not found
	if (result < 0)
	{
		return false;
	}
	else
	{
		location = (long)result;
		return true;
	}
}

/*
	correctly maps out the indeces for nvraRecord (since the indeces are split into nums and strings arrays)
	@param:index = index for NvraRecord
	@return = the correct index for the NvraRecord format
*/
int nvraRecordIndex(int index)
{
	if (index >= 0 && index < 3)
	{
		return index;
	}
	//first string index
	else if (index == 3)
	{
		return 0;
	}
	else if (index > 3 && index < 11)
	{
		return index - 1;
	}
	//second string index
	else if (index == 11)
	{
		return 1;
	}
	//third string index
	else if (index == 12)
	{
		return 2;
	}
	else
	{
		return index - 3;
	}
}

/*
	resizes the table when merging data
	@param:table = hash table you want to merge the data into
	@param:size = size of new data coming in
	@param:record = the data to merge
*/
void resizeTable(HashTable<NvraRecord> &table, unsigned long size, OULinkedList<NvraRecord> &record)
{
	NvraComparator* compare = new NvraComparator(0);
	NvraHasher* hasher = new NvraHasher();
	HashTableEnumerator<NvraRecord> hashEnum(&table);
	//temporary table with new size
	HashTable<NvraRecord>* tempTable = new HashTable<NvraRecord>(compare, hasher, table.getSize() + size, 0.9f, 0.4f);

	NvraRecord currentRecord;
	
	//adding all the items from your hash table into the temp table
	try
	{
		while (hashEnum.hasNext())
		{
			currentRecord = hashEnum.next();
			tempTable->insert(&currentRecord);
		}
		try
		{
			currentRecord = hashEnum.peekCurrent();
			tempTable->insert(&currentRecord);
		}
		catch (ExceptionMemoryNotAvailable* e)
		{

		}
	}
	catch (ExceptionEnumerationBeyondEnd* e)
	{

	}

	//enumerator for the list
	OULinkedListEnumerator<NvraRecord> enumerator = record.enumerator();
	NvraRecord temp;
	bool replace = false; // flag for replacing

	for (unsigned long index = 0; index < record.getSize(); ++index)
	{
		temp = enumerator.next();
		replace = tempTable->replace(&temp);
		
		if (replace == false)
		{
			tempTable->insert(&temp);
		}
	}
	table = *tempTable;
}

/*
	outputs the data to the console or a file
	@param:nvraArray = array of data
	@param:list = the linked list of data
	@param:input = user input
	@param:totalLines = total lines
	@param:arrCount = number of valid lines
*/
void output(TemplatedArray<NvraRecord> &nvraArray, OULinkedList<NvraRecord> &list, std::string &input, unsigned int &totalLines, unsigned int &arrCount)
{
	if (nvraArray.getSize() == 0)
	{
		OULinkedListEnumerator<NvraRecord> enumerator = list.enumerator();
		NvraRecord tempRecord;

		for (unsigned long index = 0; index < list.getSize(); ++index)
		{
			tempRecord = enumerator.next();
			nvraArray.add(&tempRecord);
		}
	}

	do
	{
		std::cout << "Enter output file name: ";
		std::getline(std::cin, input);
		std::ofstream file(input);

		//output gets sent to the console
		if (input == "")
		{
			//outputting array contents to console
			for (unsigned long index = 0; index < nvraArray.getSize(); ++index)
			{
				std::cout << nvraArray[index];
				std::cout << std::endl;
			}
			std::cout << "Data lines read: " << totalLines << "; Valid NVRA records read: " << arrCount << "; NVRA records in memory: " << nvraArray.getSize() << std::endl;
			break;
		}
		//file not found
		else if (!file.is_open())
		{
			std::cout << "File is not available." << std::endl;
		}
		//outputting data to a file
		else
		{
			//outputs to specified file
			for (unsigned long index = 0; index < nvraArray.getSize(); ++index)
			{
				file << nvraArray[index];
				file << std::endl;
			}
			file << "Data lines read: " << totalLines << "; Valid NVRA records read: " << arrCount << "; NVRA records in memory: " << nvraArray.getSize() << std::endl;
			break;
			file.close();
		}
	} while (input != ""); //loop will not terminate until either printed to console or to a file
}

/*
	sort data by specified field
	@param:nvraArray = array of data
	@param:list = the linked list of data
	@param:input = user input
	@param:sortedField = field data is sorted by
*/
void sort(TemplatedArray<NvraRecord> &nvraArray, OULinkedList<NvraRecord> &list, std::string &input, unsigned int &sortedField)
{
	std::cout << "Enter sort field (0-23): ";
	std::getline(std::cin, input);

	if (input == "")
	{
		// return to data manipulation loop
	}
	else if (stoi(input) < 0 || stoi(input) > 23)
	{
		// return to data manipulation loop
	}
	//sorts data by given column
	else
	{
		if (nvraArray.getSize() == 0)
		{
			OULinkedListEnumerator<NvraRecord> enumerator = list.enumerator();
			NvraRecord tempRecord;

			for (unsigned long index = 0; index < list.getSize(); ++index)
			{
				tempRecord = enumerator.next();
				nvraArray.add(&tempRecord);
			}
		}

		Sorter<NvraRecord> sorter; // sorter object
		NvraComparator comp(stoi(input)); // comparator object at given column
		sorter.sort(nvraArray, comp); // sorts data
		sortedField = stoi(input); // updates the sortedField var by which field the data is sorted by
	}
}

/*
	finds all items equal to the value you search for
	@param:nvraArray = array of data
	@param:list = the linked list of data
	@param:table = hashtable to search through
	@param:input = user input
	@param:sortedField = field data is sorted by
*/
void find(TemplatedArray<NvraRecord> &nvraArray, OULinkedList<NvraRecord> &list, HashTable<NvraRecord> &table, std::string &input, unsigned int &sortedField)
{
	if (nvraArray.getSize() == 0)
	{
		OULinkedListEnumerator<NvraRecord> enumerator = list.enumerator();
		NvraRecord tempRecord;

		for (unsigned long index = 0; index < list.getSize(); ++index)
		{
			tempRecord = enumerator.next();
			nvraArray.add(&tempRecord);
		}
	}

	std::string searchField; //column you're searching data in

	std::cout << "Enter search field (0-23): ";
	std::getline(std::cin, searchField);

	if (searchField == "")
	{
		// return to data manipulation loop
	}
	else if (stoi(searchField) < 0 || stoi(searchField) > 23)
	{
		// return to data manipulation loop
	}
	//string columns
	else if (stoi(searchField) == 3 || stoi(searchField) == 11 || stoi(searchField) == 12)
	{
		NvraComparator comp(stoi(searchField));
		std::cout << "Enter exact text on which to search: ";
		std::getline(std::cin, input);

		if (input == "")
		{
			// return to data manipulation loop
		}
		//searching for specified string
		else
		{
			NvraRecord* record = new NvraRecord; // pointer to a record obj to be used as a 'key' to use in searching
			record->setString(input, nvraRecordIndex(stoi(searchField))); // sets the correct column to the string you are searching for
			if (record == NULL) throw new ExceptionMemoryNotAvailable;

			//gets hit if the field the data is sorted by is the field you are searching through
			if ((unsigned int)stoi(searchField) == sortedField)
			{
				long val = (long)binarySearch(*record, nvraArray, comp); // index at which the first occurence of the data is found
				long index = val; // index
				unsigned long count = 0; // number of results

				//positive result on search *(this conditional works only due to the fact that my binary search returns the first instance of the result)*
				if (val >= 0)
				{
					do
					{
						std::cout << nvraArray[index]; // prints out the result
						std::cout << std::endl;
						++index; // inc index
						++count; // inc count

						//makes sure the index doesn't go out of bounds
						if ((unsigned int)index == nvraArray.getSize()) break;
					} while (comp.compare(*record, nvraArray[index]) == 0); // keeps looping until all positive results are found

					//number of record found in search
					std::cout << "NVRA records found: " << count << "." << std::endl;
				}
				//no records found
				else
				{
					std::cout << "NVRA records found: " << count << "." << std::endl;
				}
			}
			//data is not sorted by the specified field
			else
			{
				TemplatedArray<unsigned long> indeces; // array for holding the indeces of the positive search results
				bool found = false; // flag for positive search results

				//conducts the linear search through the data
				linearSearch(*record, nvraArray, comp, indeces, found);

				//positive result
				if (found)
				{
					//displays the found results
					for (unsigned long index = 0; index < indeces.getSize(); ++index)
					{
						std::cout << nvraArray[indeces[index]];
						std::cout << std::endl;
					}
					std::cout << "NVRA records found: " << indeces.getSize() << "." << std::endl;
				}
				//no records found
				else
				{
					std::cout << "NVRA records found: " << indeces.getSize() << "." << std::endl;
				}
			}
			//clearing memory
			delete record;
			record = NULL;
		}
	}
	//int columns
	else
	{
		NvraComparator comp(stoi(searchField));
		std::cout << "Enter non-negative field value: ";
		std::getline(std::cin, input);

		if (input == "")
		{
			// return to data manipulation loop
		}
		else if (stoi(input) < 0)
		{
			// return to data manipulation loop
		}
		//searching for specified value
		else
		{
			NvraRecord* record = new NvraRecord; // pointer to a record obj to be used as a 'key' to use in searching
			record->setNum(stoi(input), nvraRecordIndex(stoi(searchField))); // sets the correct column to the value you are searching for
			if (record == NULL) throw new ExceptionMemoryNotAvailable;

			//hash table will be used
			if ((unsigned int)stoi(searchField) == 0)
			{
				HashTableEnumerator<NvraRecord> hashEnum(&table);
				OULinkedList<NvraRecord>* tempList = NULL;
				unsigned long count = 0;
				unsigned long bucket = table.getBucketNumber(record); // bucket to search in

				try
				{
					tempList = hashEnum.getBucket(bucket); // sets the list equal to the contents of the bucket
					OULinkedListEnumerator<NvraRecord> linkEnum = tempList->enumerator();

					for (unsigned long index = 0; index < tempList->getSize(); ++index)
					{
						//finding matches
						if (linkEnum.peekCurrent().getNum(0) == (unsigned int)stoi(input))
						{
							std::cout << linkEnum.next();
							std::cout << std::endl;
							++count;
						}
					}
					//number of record found in search
					std::cout << "NVRA records found: " << count << "." << std::endl;
				}
				//no records found
				catch (ExceptionLinkedListAccess* e)
				{
					std::cout << "NVRA records found: " << count << "." << std::endl;
				}
			}
			//gets hit if the field the data is sorted by is the field you are searching through
			else if ((unsigned int)stoi(searchField) == sortedField)
			{
				long val = (long)binarySearch(*record, nvraArray, comp); // index at which the first occurence of the data is found
				long index = val; // index
				unsigned long count = 0; // number of results

				//positive result on search *(this conditional works only due to the fact that my binary search returns the first instance of the result)*
				if (val >= 0)
				{
					do
					{
						std::cout << nvraArray[index]; // prints out the result
						std::cout << std::endl;
						++index;
						++count;

						//makes sure the index doesn't go out of bounds
						if ((unsigned int)index == nvraArray.getSize()) break;
					} while (comp.compare(*record, nvraArray[index]) == 0); // keeps looping until all positive results are found

					//number of record found in search
					std::cout << "NVRA records found: " << count << "." << std::endl;
				}
				//no records found
				else
				{
					std::cout << "NVRA records found: " << count << "." << std::endl;
				}
			}
			//data is not sorted by the specified field
			else
			{
				TemplatedArray<unsigned long> indeces; // array for holding the indeces of the positive search results
				bool found = false; // flag for positive search results

				//conducts the linear search through the data
				linearSearch(*record, nvraArray, comp, indeces, found);

				//positive result
				if (found)
				{
					//displays the found results
					for (unsigned long index = 0; index < indeces.getSize(); ++index)
					{
						std::cout << nvraArray[indeces[index]];
						std::cout << std::endl;
					}
					std::cout << "NVRA records found: " << indeces.getSize() << "." << std::endl;
				}
				//no records found
				else
				{
					std::cout << "NVRA records found: " << indeces.getSize() << "." << std::endl;
				}
			}
			//clearing memory
			delete record;
			record = NULL;
		}
	}
}

/*
	merges data
	@param:table = hashtable of data
	@param:list = the linked list of data
	@param:nvraArray = array of data
	@param:lines = lines read through in new file
	@param:input = user input
	@param:compare = comparator
	@param:recordIDs = array of current IDs
	@param:arrCount = number of valid lines
	@param:totalLine = total lines read
*/
void merge(HashTable<NvraRecord> &table, OULinkedList<NvraRecord> &list, TemplatedArray<NvraRecord> &nvraArray, unsigned int &lines, std::string &input, NvraComparator* compare,
	std::vector<int> &recordIDs, unsigned int &arrCount, unsigned int &totalLines)
{
	lines = 0;
	unsigned int validCount = 0; // number of valid data in the new file 

	std::cout << "Enter data file name: ";
	std::getline(std::cin, input);
	std::ifstream file(input);

	//loops until file found or user hits enter
	while (!file.is_open() && input != "")
	{
		//checks if file was opened
		if (!file.is_open())
		{
			if (input != "") std::cout << "File is not available." << std::endl;
		}
		std::cout << "Enter data file name: ";
		std::getline(std::cin, input);
		std::ifstream file(input);
	}

	if (input == "")
	{
		// return to data manipulation loop
	}
	else
	{
		OULinkedList<NvraRecord> tempList(compare); // list of the new file

		parseFile(file, recordIDs, lines, validCount, tempList);

		OULinkedListEnumerator<NvraRecord> enumerator = tempList.enumerator(); // enumerator of the new file
		arrCount += validCount;

		//no valid data
		if (validCount == 0)
		{
			std::cout << "No valid records found." << std::endl;
		}
		//valid data found
		else
		{
			bool replaced = false; // flag for replaced data


			unsigned long sizeOf = tempList.getSize();
			resizeTable(table, sizeOf, tempList);


			//loops through the new file and tries to replace the values linearly
			for (unsigned long index = 0; index < tempList.getSize(); ++index)
			{
				NvraRecord* tempRecord = new NvraRecord();
				*tempRecord = enumerator.peekCurrent();
				replaced = list.replace(tempRecord);
				//table.replace(tempRecord);

				//item not found, so it gets added
				if (replaced == false)
				{
					*tempRecord = enumerator.next();
					list.insert(tempRecord);
				}
				else
				{
					enumerator.next();
				}
			}

			//populating the templated array
			OULinkedListEnumerator<NvraRecord> enumeratorList = list.enumerator();

			NvraRecord tempRecord;

			for (unsigned long index = 0; index < list.getSize(); ++index)
			{
				tempRecord = enumeratorList.next();
				nvraArray.add(&tempRecord);
			}
			totalLines += lines;
		}
	}
}

/*
	purges data
	@param:table = hashtable of data
	@param:list = the linked list of data
	@param:nvraArray = array of data
	@param:lines = lines read through in new file
	@param:input = user input
	@param:compare = comparator
	@param:recordIDs = array of current IDs
	@param:arrCount = number of valid lines
	@param:totalLine = total lines read
*/
void purge(HashTable<NvraRecord> &table, OULinkedList<NvraRecord> &list, TemplatedArray<NvraRecord> &nvraArray, unsigned int &lines, std::string &input, NvraComparator* compare,
	std::vector<int> &recordIDs, unsigned int &arrCount, unsigned int &totalLines)
{
	lines = 0;
	unsigned int validCount = 0; // number of valid data in the new file 

	std::cout << "Enter data file name: ";
	std::getline(std::cin, input);
	std::ifstream file(input);

	//loops until file found or user hits enter
	while (!file.is_open() && input != "")
	{
		//checks if file was opened
		if (!file.is_open())
		{
			if (input != "") std::cout << "File is not available." << std::endl;
		}
		std::cout << "Enter data file name: ";
		std::getline(std::cin, input);
		std::ifstream file(input);
	}

	if (input == "")
	{
		// return to data manipulation loop
	}
	else
	{
		OULinkedList<NvraRecord> tempList(compare); // list of the new file

		parseFile(file, recordIDs, lines, validCount, tempList);

		OULinkedListEnumerator<NvraRecord> enumerator = tempList.enumerator(); // enumerator of the new file
		arrCount += validCount;

		//no valid data
		if (validCount == 0)
		{
			std::cout << "No valid records found." << std::endl;
		}
		//valid data found
		else
		{
			//loops through list and purges all matching values
			for (unsigned long index = 0; index < tempList.getSize(); ++index)
			{
				NvraRecord* tempRecord = new NvraRecord();
				*tempRecord = enumerator.next();
				list.remove(tempRecord);
				table.remove(tempRecord);
			}
		}

		//populating the templated array
		OULinkedListEnumerator<NvraRecord> enumeratorList = list.enumerator();
		NvraRecord tempRecord;
		///
		for (unsigned long index = 0; index < nvraArray.getSize(); ++index)
		{
			nvraArray.removeAt(index);
		}

		for (unsigned long index = 0; index < list.getSize(); ++index)
		{
			tempRecord = enumeratorList.next();
			nvraArray.add(&tempRecord);
		}
		totalLines += lines;
	}
}

/*
	prints out the records
	@param:list = the linked list of data
	@param:nvraArray = array of data
	@param:input = user input
	@param:arrCount = number of valid lines
	@param:totalLine = total lines read
*/
void records(OULinkedList<NvraRecord> &list, TemplatedArray<NvraRecord> &nvraArray, std::string &input, unsigned int &arrCount, unsigned int &totalLines)
{
	do
	{
		OULinkedListEnumerator<NvraRecord> enumerator = list.enumerator();

		std::cout << "Enter output file name: ";
		std::getline(std::cin, input);
		std::ofstream file(input);

		//output gets sent to the console
		if (input == "")
		{
			//outputting array contents to console
			for (unsigned long index = 0; index < list.getSize(); ++index)
			{
				std::cout << enumerator.next();
				std::cout << std::endl;
			}
			std::cout << "Data lines read: " << totalLines << "; Valid NVRA records read: " << arrCount << "; NVRA records in memory: " << list.getSize() << std::endl;
			break;
		}
		//file not found
		else if (!file.is_open())
		{
			std::cout << "File is not available." << std::endl;
		}
		//outputting data to a file
		else
		{
			//outputs to specified file
			for (unsigned long index = 0; index < nvraArray.getSize(); ++index)
			{
				file << enumerator.next();
				file << std::endl;
			}
			file << "Data lines read: " << totalLines << "; Valid NVRA records read: " << arrCount << "; NVRA records in memory: " << list.getSize() << std::endl;
			break;
			file.close();
		}
	} while (input != ""); //loop will not terminate until either printed to console or to a file
}

/*
	outputs the data in the hash table
	@param:table = hashtable of data
	@param:input = user input
	@param:totalLine = total lines read
	@param:arrCount = number of valid lines
*/
void hashTable(HashTable<NvraRecord> &table, std::string &input, unsigned int &totalLines, unsigned int &arrCount)
{
	do
	{
		std::cout << "Enter output file name: ";
		std::getline(std::cin, input);
		std::ofstream file(input);

		//output gets sent to the console
		if (input == "")
		{
			//outputting array contents to console
			HashTableEnumerator<NvraRecord> hashEnum(&table);
			NvraRecord currentRecord;
			NvraRecord previousRecord;
			bool firstItemRead = false; // flag used to stay one item behind
			bool newBucket = false; // flag for moving buckets

			try
			{
				//loops through the table
				while (hashEnum.hasNext())
				{
					currentRecord = hashEnum.next();

					//sets perviousRecord to value that isn't the same as current
					if (firstItemRead == false)
					{
						previousRecord.setNum(currentRecord.getNum(0) + 1, 0);
						firstItemRead = true;
					}

					//same hash and same bucket so it is overflow
					if ((table.getBucketNumber(&currentRecord) == table.getBucketNumber(&previousRecord)) && newBucket == false)
					{
						std::cout << "OVERFLOW: " << currentRecord;
						std::cout << std::endl;
						if (table.getBucketNumber(&currentRecord) != table.getBucketNumber(hashEnum.peekCurrentPointer()))
						{
							std::cout << std::endl;
						}
					}
					//same hash as the next item so it leaves room for overflow
					else if (table.getBucketNumber(&currentRecord) == table.getBucketNumber(hashEnum.peekCurrentPointer()))
					{
						std::cout << table.getBucketNumber(&currentRecord) << ": " << currentRecord;
						std::cout << std::endl;
						newBucket = false;
					}
					//no overflow
					else
					{
						std::cout << table.getBucketNumber(&currentRecord) << ": " << currentRecord;
						std::cout << std::endl;
						std::cout << std::endl;

						newBucket = true;
					}
					previousRecord = currentRecord;
				}
				//making sure the last item is read
				try
				{
					currentRecord = hashEnum.peekCurrent();

					if ((table.getBucketNumber(&currentRecord) == table.getBucketNumber(&previousRecord)) && newBucket == false)
					{
						std::cout << "OVERFLOW: " << currentRecord;
						std::cout << std::endl;
						std::cout << std::endl;
					}
					else
					{
						std::cout << table.getBucketNumber(&currentRecord) << ": " << currentRecord;
						std::cout << std::endl;
						std::cout << std::endl;
					}
				}
				catch (ExceptionMemoryNotAvailable* e)
				{

				}
			}
			catch (ExceptionEnumerationBeyondEnd* e)
			{

			}
			std::cout << "Base Capacity: " << table.getBaseCapacity() << "; Total Capacity: " << table.getTotalCapacity() << "; Load Factor: " << table.getLoadFactor() << std::endl;
			std::cout << "Data lines read: " << totalLines << "; Valid NVRA records read: " << arrCount << "; NVRA records in memory: " << table.getSize() << std::endl;
			break;
		}
		//file not found
		else if (!file.is_open())
		{
			std::cout << "File is not available." << std::endl;
		}
		//outputting data to a file
		else
		{
			//outputs to specified file
			HashTableEnumerator<NvraRecord> hashEnum(&table);
			NvraRecord currentRecord;
			NvraRecord previousRecord;
			bool firstItemRead = false;
			bool newBucket = false;

			try
			{
				//same as before
				while (hashEnum.hasNext())
				{
					currentRecord = hashEnum.next();
					if (firstItemRead == false)
					{
						previousRecord.setNum(currentRecord.getNum(0) + 1, 0);
						firstItemRead = true;
					}

					if ((table.getBucketNumber(&currentRecord) == table.getBucketNumber(&previousRecord)) && newBucket == false)
					{
						file << "OVERFLOW: " << currentRecord;
						file << std::endl;
					}
					else if (table.getBucketNumber(&currentRecord) == table.getBucketNumber(hashEnum.peekCurrentPointer()))
					{
						file << table.getBucketNumber(&currentRecord) << ": " << currentRecord;
						file << std::endl;
						newBucket = false;
					}
					else
					{
						file << table.getBucketNumber(&currentRecord) << ": " << currentRecord;
						file << std::endl;
						file << std::endl;

						newBucket = true;
					}
					previousRecord = currentRecord;
				}
				try
				{
					currentRecord = hashEnum.peekCurrent();

					if ((table.getBucketNumber(&currentRecord) == table.getBucketNumber(&previousRecord)) && newBucket == false)
					{
						file << "OVERFLOW: " << currentRecord;
						file << std::endl;
						file << std::endl;
					}
					else
					{
						file << table.getBucketNumber(&currentRecord) << ": " << currentRecord;
						file << std::endl;
						file << std::endl;
					}
				}
				catch (ExceptionMemoryNotAvailable* e)
				{

				}
			}
			catch (ExceptionEnumerationBeyondEnd* e)
			{

			}
			file << "Base Capacity: " << table.getBaseCapacity() << "; Total Capacity: " << table.getTotalCapacity() << "; Load Factor: " << table.getLoadFactor() << std::endl;
			file << "Data lines read: " << totalLines << "; Valid NVRA records read: " << arrCount << "; NVRA records in memory: " << table.getSize() << std::endl;
			break;
			file.close();
		}
	} while (input != ""); //loop will not terminate until either printed to console or to a file
}

/*
	provides interface for the user to select a file to manipulate.
	user can merge, output, sort, or find
*/
int main()
{
	std::string input = ""; // input from the user
	unsigned int lines = 0; // lines in each file
	unsigned int totalLines = 0; // lines for all files added together
	unsigned int arrCount = 0; // number of valid lines
	unsigned int sortedField = 0; // field last sorted
	bool dataEntered = false; // flag for file entry
	std::vector<int> recordIDs(0); // storage for record IDs

	TemplatedArray<NvraRecord> nvraArray; // creation of the TemplatedArray<NvraRecord> object
	NvraComparator* compare = new NvraComparator(0); // creationg of the comparator for the list
	OULinkedList<NvraRecord> list(compare); // creation of the OULinkedList<NvraRecord> object
	NvraHasher* hasher = new NvraHasher();
	HashTable<NvraRecord> table(compare, hasher);

	//data input loop
	do
	{
		lines = 0; // reset the lines for each file
		std::cout << "Enter data file name: ";
		std::getline(std::cin, input);
		std::ifstream file(input);

		//checks if file was opened
		if (!file.is_open())
		{
			if (input != "") std::cout << "File is not available." << std::endl;
		}
		else
		{
			//parses the file i.e. gets rid of errors and duplicates, formats the lines, and populates the list
			parseFile(file, recordIDs, lines, arrCount, list);

			if (arrCount == 0)
			{
				std::cout << "No valid records found." << std::endl;
			}
			else
			{
				dataEntered = true; // data has been put into the list

				//inserting items into the hashtable
				OULinkedListEnumerator<NvraRecord> enumerator = list.enumerator();
				NvraRecord temp;
				for (unsigned long index = 0; index < list.getSize(); ++index)
				{
					temp = enumerator.next();
					table.insert(&temp);
				}
			}
			file.close(); // closing file
		}
		totalLines += lines; // adding up total lines

		//continuing on to data manipulation loop after file has been read in
		if (dataEntered)
		{
			break;
		}
	} while (input != ""); // loop continues until user hits enter without any input



	//data manipulation loop
	if (dataEntered)
	{
		do
		{
			std::cout << "Enter (o)utput, (s)ort, (f)ind, (m)erge, (p)urge, (r)ecords, (h)ash table, or (q)uit: ";
			std::getline(std::cin, input);

			//output
			if (input == "o")
			{
				output(nvraArray, list, input, totalLines, arrCount);
			}

			//sort
			else if (input == "s")
			{
				sort(nvraArray, list, input, sortedField);
			}

			//find
			else if (input == "f")
			{
				find(nvraArray, list, table, input, sortedField);
			}

			//merge
			else if (input == "m")
			{
				merge(table, list, nvraArray, lines, input, compare, recordIDs, arrCount, totalLines);
			}

			//purge
			else if (input == "p")
			{
				purge(table, list, nvraArray, lines, input, compare, recordIDs, arrCount, totalLines);
			}

			//records
			else if (input == "r")
			{
				records(list, nvraArray, input, arrCount, totalLines);
			}

			//hash table
			else if (input == "h")
			{
				hashTable(table, input, totalLines, arrCount);
			}

			//quit
			else if (input == "q")
			{
				std::cout << "Thanks for using VoteR." << std::endl;
			}
		} while (input != "q");
	}
}