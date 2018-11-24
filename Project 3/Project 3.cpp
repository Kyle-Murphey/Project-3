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

/*****************************************************************************************************************************
*	VoteR 2.0                                                                                                                *
*	Project 2: CS 2413                                                                                                       *
*	11/5/18                                                                                                                  *
*	Kyle Murphey                                                                                                             *
*	                                                                                                                         *
*	This application reads in a .csv file to a linked list                                                                   *
*	Data can be added, replaced, and removed from the list                                                                   *
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
	HashTable<NvraRecord>* table = new HashTable<NvraRecord>(compare, hasher);



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

				/// #######################################################################################
				OULinkedListEnumerator<NvraRecord> enumerator = list.enumerator();
				NvraRecord temp;
				for (unsigned long index = 0; index < list.getSize(); ++index)
				{
					temp = enumerator.next();
					table->insert(&temp);
				}
			}
			file.close(); // closing file
			/// ########################################################################################3
			for (unsigned long index = 0; index < table->getBaseCapacity(); ++index)
			{
				std::cout << table->returnItem(index);
				std::cout << std::endl;
			}
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
			std::cout << "Enter (o)utput, (s)ort, (f)ind, (m)erge, (p)urge, (r)ecords, or (q)uit: ";
			std::getline(std::cin, input);

			//output
			if (input == "o")
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

			//sort
			else if (input == "s")
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

			//find
			else if (input == "f")
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

			//merge
			else if (input == "m")
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

						//loops through the new file and tries to replace the values linearly
						for (unsigned long index = 0; index < tempList.getSize(); ++index)
						{
							NvraRecord* tempRecord = new NvraRecord();
							*tempRecord = enumerator.peekCurrent();
							replaced = list.replace(tempRecord);

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

			//purge
			else if (input == "p")
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

			//records
			else if (input == "r")
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

			//quit
			else if (input == "q")
			{
				std::cout << std::endl << "Thanks for using VoteR." << std::endl;
			}
		} while (input != "q");
	}
}