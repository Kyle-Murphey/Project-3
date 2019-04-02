#ifndef HASH_TABLE
#define HASH_TABLE

#include "Exceptions.h"
#include "Comparator.h"
#include "OULinkedList.h"
#include "OULinkedListEnumerator.h"
#include "Hasher.h"
#include <cmath>

const unsigned int SCHEDULE_SIZE = 25;			// the number of items in the size schedule
const unsigned int SCHEDULE[SCHEDULE_SIZE] = { 1, 2, 5, 11, 23, 53, 107, 223, 449, 907, 1823, 3659, 7309, 14621, 29243, 58511, 117023, 234067, 468157, 936319, 1872667, 3745283, 7490573, 14981147, 29962343 };		// the size schedule (all primes)
const unsigned int DEFAULT_SCHEDULE_INDEX = 3;	// the default position in the size schedule
const unsigned long DEFAULT_BASE_CAPACITY = SCHEDULE[DEFAULT_SCHEDULE_INDEX]; 	// the default size of the array
const float DEFAULT_MAX_LOAD_FACTOR = 0.9f;		// the default load factor used to determine when to increase the table size
const float DEFAULT_MIN_LOAD_FACTOR = 0.4f;		// the default load factor used to determine when to decrease the table size

template <typename T>
class HashTable {
	template <typename U>
	friend class HashTableEnumerator;						// necessary to allow the enumerator to access the table's private data
private:
	Comparator<T>* comparator = NULL;						// used to determine item equality
	Hasher<T>* hasher = NULL;								// used to compute hash value
	unsigned long size = 0;									// actual number of items currently in hash table
	float maxLoadFactor = DEFAULT_MAX_LOAD_FACTOR;			// the load factor used to determine when to increase the table size
	float minLoadFactor = DEFAULT_MIN_LOAD_FACTOR;			// the load factor used to determine when to decrease the table size
	unsigned int scheduleIndex = DEFAULT_SCHEDULE_INDEX;	// the index of current location in the size schedule
	unsigned long baseCapacity = DEFAULT_BASE_CAPACITY;		// the size of the array
	unsigned long totalCapacity = baseCapacity;				// the size of the array plus chains of more than one link
	OULinkedList<T>** table = NULL;							// table will be an array of pointers to OULinkedLists of type T
	void populateArray(unsigned long sizeOfArray, OULinkedList<T>** array);
	void resize(bool increaseSize);
	// you may add additional member variables and functions here to support the operation of your code
public:
	HashTable(Comparator<T>* comparator, Hasher<T>* hasher);			// creates an empty table of DEFAULT_BASE_CAPACITY
	HashTable(Comparator<T>* comparator, Hasher<T>* hasher,
		// if size given, creates empty table with size from schedule of sufficient capacity (considering maxLoadFactor)
		unsigned long size, float maxLoadFactor, float minLoadFactor);

	virtual ~HashTable();

	// if an equivalent item is not already present, insert item at proper location and return true
	// if an equivalent item is already present, leave table unchanged and return false
	bool insert(T* item);

	// if an equivalent item is already present, replace item and return true
	// if an equivalent item is not already present, leave table unchanged and return false
	bool replace(T* item);

	// if an equivalent item is already present, remove item and return true
	// if an equivalent item is not already present, leave table unchanged and return false
	bool remove(T* item);

	// if an equivalent item is present, return a copy of the item
	// if an equivalent item is not present, throw a new ExceptionHashTableAccess
	T find(const T* item) const;

	unsigned long getSize() const;						// returns the current number of items in the table
	unsigned long getBaseCapacity() const;				// returns the current base capacity of the table
	unsigned long getTotalCapacity() const;				// returns the current total capacity of the table
	float getLoadFactor() const;						// returns the current load factor of the table
	unsigned long getBucketNumber(const T* item) const;	// returns the bucket number for an item using its hash function mod array size

	OULinkedList<T>* returnList(unsigned long index) const; // returns list at given bucket


};

// put implementation for HashTable here


/*
	constructor
	@param:comparator = comparator
	@param:hasher = hasher
*/
template <typename T>
HashTable<T>::HashTable(Comparator<T>* comparator, Hasher<T>* hasher)
{
	this->comparator = comparator;
	this->hasher = hasher;
	this->table = new OULinkedList<T>*[DEFAULT_BASE_CAPACITY];
	populateArray(DEFAULT_BASE_CAPACITY, table);
}

/*
	constructor
	@param:comparator = comparator
	@param:hasher = hasher
	@param:size = size of the data
	@param:maxLoadFactor = maximum load factor
	@param:minLoadFactor = minimum load factor
*/
template <typename T>
HashTable<T>::HashTable(Comparator<T>* comparator, Hasher<T>* hasher, unsigned long size, float maxLoadFactor, float minLoadFactor)
{
	this->comparator = comparator;
	this->hasher = hasher;
	this->maxLoadFactor = maxLoadFactor;
	this->minLoadFactor = minLoadFactor;

	//capacity needed
	unsigned long schedule = (unsigned long)ceil((float)size / (float)maxLoadFactor);

	//finding capacity based on SCHEDULE
	for (unsigned long index = 0; index < SCHEDULE_SIZE; ++index)
	{
		if (SCHEDULE[index] >= schedule)
		{
			scheduleIndex = (unsigned int)index;
			baseCapacity = SCHEDULE[index];
			totalCapacity = baseCapacity;
			break;
		}
	}

	this->table = new OULinkedList<T>*[baseCapacity];
	populateArray(baseCapacity, table); // fills array with blank lists
}

/*
	destructor
*/
template <typename T>
HashTable<T>::~HashTable()
{
	for (unsigned long index = 0; index < getBaseCapacity(); ++index)
	{
		delete table[index];
		table[index] = NULL;
	}
	delete table;
	table = NULL;
}

/*
	inserts data into the appropriate bucket
	@param:item = item to insert
	@return = whether or not item was returned
*/
template <typename T>
bool HashTable<T>::insert(T* item)
{
	bool inserted = false;

	inserted = table[getBucketNumber(item)]->insert(item);
	if (inserted == true)
	{
		++size;
		if (getLoadFactor() >= maxLoadFactor) resize(true); // resize if too big

		if (table[getBucketNumber(item)]->getSize() > 1)
		{
			++totalCapacity;
			if (getLoadFactor() >= maxLoadFactor) resize(true); // resize if too big
		}
	}
	return inserted;
}

/*
	replace item if it exists
	@param:item = item to replace
	@return = whether or not item was replaced
*/
template <typename T>
bool HashTable<T>::replace(T* item)
{
	bool found = false;
	if (table[getBucketNumber(item)]->getSize() < 1) // bucket item should be in is empty
	{
		return found;
	}
	else
	{
		found = table[getBucketNumber(item)]->replace(item);
		return found;
	}
}

/*
	removes item from table
	@param:item = item to remove
	@return = whether or not item was removed
*/
template <typename T>
bool HashTable<T>::remove(T* item)
{
	bool found = false;
	if (table[getBucketNumber(item)]->getSize() < 1)
	{
		return found;
	}
	else
	{
		//removing item will not cause totalCapacity to decrement
		if (table[getBucketNumber(item)]->getSize() == 1)
		{
			found = table[getBucketNumber(item)]->remove(item);
			if (found)
			{
				--size;
				if (getLoadFactor() < minLoadFactor) resize(false);
			}
			return found;
		}
		//removing will cause totalCapacity to decrement
		else
		{
			found = table[getBucketNumber(item)]->remove(item);
			if (found)
			{
				--size;
				--totalCapacity;
				if (getLoadFactor() < minLoadFactor) resize(false);
			}
			return found;
		}
	}
}

/*
	find a certain item
	@param:item = item to search for
	@return = item if found
*/
template <typename T>
T HashTable<T>::find(const T* item) const
{
	try
	{
		if (table[getBucketNumber(item)]->getSize() < 1) throw new ExceptionHashTableAccess; //bucket it should be in is empty
		else
		{
			T* temp = new T();
			OULinkedList<T>* tempList = new OULinkedList<T>(comparator);
			tempList = table[getBucketNumber(item)];
			*temp = tempList->find(item);
			return *temp;
		}
	}
	catch (ExceptionLinkedListAccess* e)
	{
		throw new ExceptionHashTableAccess;
	}
}

/*
	resizes the hashtable
	@param:increaseSize = if true, the hashtable will increase to the next SCHEDULE. if false, it will decrease.
*/
template <typename T>
void HashTable<T>::resize(bool increaseSize)
{
	if (increaseSize)
	{
		scheduleIndex += 1;
	}
	else
	{
		scheduleIndex -= 1;
	}

	totalCapacity = SCHEDULE[scheduleIndex];
	OULinkedList<T>** tempArray = new OULinkedList<T>*[totalCapacity]; // temp hash table
	populateArray(totalCapacity, tempArray);

	OULinkedList<T>* tempList = new OULinkedList<T>(comparator);
	OULinkedListEnumerator<T> chainEnum = tempList->enumerator();

	//loops through table to grab all values
	for (unsigned long index = 0; index < getBaseCapacity(); ++index)
	{
		tempList = table[index];
		chainEnum = tempList->enumerator();
		T* tempItem = new T();

		if (tempList->getSize() > 0)
		{
			for (unsigned long node = 0; node < tempList->getSize(); ++node)
			{
				*tempItem = chainEnum.next();
				tempArray[getBucketNumber(tempItem)]->insert(tempItem);

				//if rehashed items end up in same bucket, totalCapacity is incremented
				if (tempArray[getBucketNumber(tempItem)]->getSize() > 1)
				{
					++totalCapacity;
				}
			}
		}
	}

	//creating new table
	baseCapacity = SCHEDULE[scheduleIndex];
	this->table = new OULinkedList<T>*[baseCapacity];
	populateArray(getBaseCapacity(), this->table);

	//filling new table up in correct buckets
	for (unsigned long index = 0; index < getBaseCapacity(); ++index)
	{
		table[index] = tempArray[index];
	}
}

/*
	returns list at given bucket
	@param:index = bucket to grab
	@return = list in bucket
*/
template <typename T>
OULinkedList<T>* HashTable<T>::returnList(unsigned long index) const
{
	OULinkedList<T>* temp = new OULinkedList<T>(comparator);
	temp = table[index];
	OULinkedListEnumerator<T> enumerator = temp->enumerator();

	if (temp->getSize() > 0)
	{
		return temp;
	}
	//list is empty
	else
	{
		throw new ExceptionHashTableAccess;
	}
}


/*
	fills the hashtable with blank linked lists
	@param:sizeOfArray = the size you wish to make the hashtable
	@param:array = the hashtable to populate
*/
template <typename T>
void HashTable<T>::populateArray(unsigned long sizeOfArray, OULinkedList<T>** array)
{
	for (unsigned long index = 0; index < sizeOfArray; ++index)
	{
		array[index] = new OULinkedList<T>(comparator);
	}
}

/*
	gets size
	@return size
*/
template <typename T>
unsigned long HashTable<T>::getSize() const
{
	return this->size;
}

/*
	gets base capacity
	@return = base capacity
*/
template <typename T>
unsigned long HashTable<T>::getBaseCapacity() const
{
	return baseCapacity;
}

/*
	gets total capacity
	@return =  total capacity
*/
template <typename T>
unsigned long HashTable<T>::getTotalCapacity() const
{
	return totalCapacity;
}

/*
	gets load factor
	@return = load factor
*/
template <typename T> 
float HashTable<T>::getLoadFactor() const
{
	return (float)this->size / (float)this->totalCapacity;
}

/*
	gets bucket number by hashing
	@return = bucket number
*/
template <typename T>
unsigned long HashTable<T>::getBucketNumber(const T* item) const
{
	return hasher->hash(*item) % SCHEDULE[scheduleIndex];
}
#endif // !HASH_TABLE