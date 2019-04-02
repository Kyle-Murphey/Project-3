#ifndef HASH_TABLE_ENUMERATOR
#define HASH_TABLE_ENUMERATOR

#include "Enumerator.h"
#include "Exceptions.h"
#include "HashTable.h"

template <typename T>
class HashTableEnumerator : public Enumerator<T>
{
private:
	unsigned long bucket = 0;								// the current bucket during the enumeration process
	OULinkedListEnumerator<T>* chainEnumerator = NULL;		// used to move through the linked list of the current bucket
	HashTable<T>* hashTable = NULL;							// pointer to the HashTable being enumerated
	OULinkedList<T>* bucketList = NULL;						// holder for list in bucket
	// you may add additional member variables and functions here to support the operation of your code
public:
	HashTableEnumerator(HashTable<T>* hashTable);			// constructor needs a pointer to the HashTable to be enumerated
	virtual ~HashTableEnumerator();
	bool hasNext() const;									// true if there are elements that have not yet been returned via next()
	T next();												// throws ExceptionEnumerationBeyondEnd if no next item is available
	T peek() const;											// throws ExceptionEnumerationBeyondEnd if no next item is available
	T peekCurrent() const;
	T* peekCurrentPointer() const;							// gets current in pointer form
	bool currentExists() const;								// verifies current exists
	OULinkedList<T>* getBucket(unsigned long index) const;	// gets the list from index (bucket)
};

// put implementation for HashTableEnumerator here

/*
	constructor
	@param:hashTable = hash table to enumerate
*/
template <typename T>
HashTableEnumerator<T>::HashTableEnumerator(HashTable<T>* hashTable)
{
	this->hashTable = hashTable;

	if (hashTable->getSize() == 0)
	{
		throw new ExceptionHashTableAccess;
	}

	//loops until bucket with data is found
	for (unsigned long index = 0; index < hashTable->getBaseCapacity(); ++index)
	{
		try
		{
			this->bucketList = hashTable->returnList(bucket);
			break;
		}
		catch (ExceptionHashTableAccess* e)
		{
			++bucket;
		}
	}
	chainEnumerator = bucketList->enumeratorPtr();
}

/*
	destructor
*/
template <typename T>
HashTableEnumerator<T>::~HashTableEnumerator()
{
	delete chainEnumerator;
	chainEnumerator = NULL;
}

/*
	checks if another value exists in table
	@return = whether next item exists
*/
template <typename T>
bool HashTableEnumerator<T>::hasNext() const
{
	//out of bounds
	if (bucket >= hashTable->getBaseCapacity())
	{
		return false;
	}

	//has next in same bucket
	if (chainEnumerator->hasNext())
	{
		return true;
	}
	//checks other buckets
	else
	{
		for (unsigned long index = bucket + 1; index < hashTable->getBaseCapacity(); ++index)
		{
			try
			{
				if (hashTable->returnList(index)->getSize() > 0)
				{
					return true;
				}
			}
			catch (ExceptionHashTableAccess* e)
			{
				
			}
		}
	}
	return false;
}

/*
	gets the current value and increments
	@return = current value
*/
template <typename T>
T HashTableEnumerator<T>::next()
{
	//no items left
	if (this->hasNext() == false && this->currentExists() == false)
	{
		throw new ExceptionEnumerationBeyondEnd;
	}
	else
	{
		T* currentItem = new T();
		bool valueStored = false;
		for (unsigned long index = bucket; index < hashTable->getBaseCapacity(); ++index)
		{
			//no more items in bucket
			if (chainEnumerator->hasNext() == false)
			{
				try
				{
					//stores current val
					if (valueStored == false)
					{
						*currentItem = chainEnumerator->peekCurrent();
						valueStored = true;
					}
				}
				catch (ExceptionEnumerationBeyondEnd* e)
				{

				}
				//goes to next bucket in search for more items
				try
				{
					++bucket;
					bucketList = hashTable->returnList(bucket);
					chainEnumerator = bucketList->enumeratorPtr();
					return *currentItem;
				}
				catch (ExceptionHashTableAccess* e)
				{
					continue;
				}
			}
			//more items in same bucket
			else
			{
				*currentItem = chainEnumerator->next();
				return *currentItem;
			}
		}
	}
	throw new ExceptionEnumerationBeyondEnd;
}

/*
	gets the next value to be found
	@return = next value
*/
template <typename T>
T HashTableEnumerator<T>::peek() const
{
	//no more items
	if (this->hasNext() == false)
	{
		throw new ExceptionEnumerationBeyondEnd;
	}
	else
	{
		T* currentItem = new T();
		OULinkedList<T>* tempList = NULL;
		OULinkedListEnumerator<T> tempEnum = 0;

		//loops through table
		for (unsigned long index = bucket; index < hashTable->getBaseCapacity(); ++index)
		{
			//no more items in bucket
			if (chainEnumerator->hasNext() == false)
			{
				try
				{
					tempList = hashTable->returnList(index + 1); // next bucket
					tempEnum = tempList->enumerator();
					try
					{
						*currentItem = tempEnum.peekCurrent();
						return *currentItem;
					}
					catch (ExceptionEnumerationBeyondEnd* e)
					{
						throw new ExceptionEnumerationBeyondEnd;
					}
					
				}
				//empty bucket
				catch (ExceptionHashTableAccess* e)
				{
					continue;
				}
			}
			//same bucket has more items
			else
			{
				*currentItem = chainEnumerator->peek();
				return *currentItem;
			}
		}
	}
	throw new ExceptionEnumerationBeyondEnd;
}

/*
	gets current value
	@return = current value
*/
template <typename T>
T HashTableEnumerator<T>::peekCurrent() const
{
	try
	{
		T* temp = new T();
		*temp = chainEnumerator->peekCurrent();
		return *temp;
	}
	catch (ExceptionEnumerationBeyondEnd* e)
	{
		throw new ExceptionMemoryNotAvailable;
	}
}

/*
	gets current value as pointer
	@return = return current pointer
*/
template <typename T>
T* HashTableEnumerator<T>::peekCurrentPointer() const
{
	try
	{
		T* temp = new T();
		*temp = chainEnumerator->peekCurrent();
		return temp;
	}
	catch (ExceptionEnumerationBeyondEnd* e)
	{
		throw new ExceptionMemoryNotAvailable;
	}
}

/*
	verifies current exists
	@return = whether current exists
*/
template <typename T>
bool HashTableEnumerator<T>::currentExists() const
{
	try
	{
		T* temp = new T();
		*temp = chainEnumerator->peekCurrent();
		return true;
	}
	catch (ExceptionEnumerationBeyondEnd* e)
	{
		return false;
	}
}

/*
	gets list from specified bucket
	@return = list from bucket
*/
template <typename T>
OULinkedList<T>* HashTableEnumerator<T>::getBucket(unsigned long index) const
{
	try
	{
		OULinkedList<T>* tempList = hashTable->returnList(index);
		return tempList;
	}
	catch (ExceptionHashTableAccess* e)
	{
		throw new ExceptionLinkedListAccess;
	}
}

#endif // !HASH_TABLE_ENUMERATOR