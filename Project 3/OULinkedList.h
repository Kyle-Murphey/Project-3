#ifndef OU_LINKED_LIST
#define OU_LINKED_LIST

#include "OULink.h"
#include "Comparator.h"
#include "OULinkedListEnumerator.h"

// OULinkedList stands for Ordered, Unique Linked List. It is a linked list that is always maintained in
// order (based on the comparator provided to it when the list is created) and that only contains unique
// items (that is, duplicates are not allowed)
template <typename T>
class OULinkedList {
private:
	Comparator<T>* comparator = NULL;				// used to determine list order and item equality
	unsigned long size = 0;							// actual number of items currently in list
	OULink<T>* first = NULL;						// pointer to first link in list
	OULink<T>* last = NULL;							// pointer to last link in list
	// you may add additional member variables and functions here to support the operation of your code
public:
	OULinkedList(Comparator<T>* comparator);		// creates empty linked list with comparator
	virtual ~OULinkedList();						// deletes all links and their data items

	// if an equivalent item is not already present, insert item in order and return true
	// if an equivalent item is already present, leave list unchanged and return false
	bool insert(T* item);

	// if item is greater than item at last, append item at end and return true
	// if item is less than or equal to item at last, leave list unchanged and return false
	bool append(const T* item);

	// if an equivalent item is already present, replace item and return true
	// if an equivalent item is not already present, leave list unchanged and return false
	bool replace(T* item);

	// if an equivalent item is already present, remove item and return true
	// if an equivalent item is not already present, leave list unchanged and return false
	bool remove(T* item);

	// if any items are present, return a copy of the first item
	// if no items are present, throw new ExceptionLinkedListAccess
	T get() const;

	// if an equivalent item is present, return a copy of the item
	// if an equivalent item is not present, throw a new ExceptionLinkedListAccess
	T find(const T* item) const;

	unsigned long getSize() const;					// returns the current number of items in the list

	OULinkedListEnumerator<T> enumerator() const;	// create an enumerator for this linked list
};

/*
	constructor
	@param:comparator = comparator for the items
*/
template <typename T>
OULinkedList<T>::OULinkedList(Comparator<T>* comparator)
{
	this->comparator = comparator;
}

/*
	destructor
*/
template <typename T>
OULinkedList<T>::~OULinkedList()
{
	delete first;
	first = NULL;
	last = NULL;
}

/*
	if an equivalent item is not already present, insert item in order and return true
	if an equivalent item is already present, leave list unchanged and return false
	@param:item = item to be inserted
	@return = whether of not item was inserted
*/
template <typename T>
bool OULinkedList<T>::insert(T* item)
{
	bool greater = append(item); // try to append first

	if (greater == false)
	{
		OULinkedListEnumerator<T> enumerator = this->enumerator();
		T* tempItem = new T();
		OULink<T>* currentLink = new OULink<T>(tempItem); // current node
		OULink<T>* previousLink = new OULink<T>(tempItem); // previous node
		OULink<T>* tempLink = new OULink<T>(item); // item node

		currentLink = first;

		//item is less than the first node
		if (comparator->compare(*item, *first->data) == -1)
		{
			first = tempLink;
			first->next = currentLink;
			++size;
			return true;
		}
		else
		{
			//linearly searches for a value greater than item
			while (enumerator.hasNext() && comparator->compare(*item, *currentLink->data) != -1)
			{
				//identical item found
				if (comparator->compare(*item, *currentLink->data) == 0)
				{
					return false;
				}
				previousLink = currentLink;
				enumerator.next();
				currentLink = currentLink->next;
			}

			previousLink->next = tempLink;
			tempLink->next = currentLink;
			++size;
			return true;
		}
	}
	return true;
}

/*
	if item is greater than item at last, append item at end and return true
	if item is less than or equal to item at last, leave list unchanged and return false
	@param:item = item to append
	@return = whether or not item was appended
*/
template <typename T>
bool OULinkedList<T>::append(const T* item)
{
	OULink<T>* tempItem = new OULink<T>(item);

	//list is empty
	if (first == NULL)
	{
		first = tempItem;
		last = tempItem;
		++size;
		return true;
	}
	//item greater than last node
	else if (comparator->compare(*item, *last->data) == 1)
	{
		last->next = tempItem;
		last = tempItem;
		++size;
		return true;
	}
	else
	{
		return false;
	}
}

/*
	if an equivalent item is already present, replace item and return true
	if an equivalent item is not already present, leave list unchanged and return false
	@param:item = item to replace
	@return = whether or not item was replaced
*/
template <typename T>
bool OULinkedList<T>::replace(T* item)
{
	//empty list
	if (first == NULL)
	{
		return false;
	}
	else
	{
		T* tempItem = new T();
		OULink<T>* currentLink = new OULink<T>(tempItem); // current node
		OULink<T>* previousLink = NULL; // previous node
		OULink<T>* tempLink = new OULink<T>(item);
		currentLink = first;
		OULinkedListEnumerator<T> enumerator = this->enumerator();
		bool found = false; // flag for found value

		//loops through list looking for a match
		for (int index = 0; 0 < size; ++index)
		{
			if (comparator->compare(*item, *currentLink->data) == 0)
			{
				found = true;
				break;
			}

			previousLink = currentLink;
			if (enumerator.hasNext())
			{
				enumerator.next();
			}

			currentLink = currentLink->next;
			if (currentLink == NULL)
			{
				found = false;
				break;
			}
		}
		//match found
		if (found)
		{
			//item is first node
			if (previousLink == NULL)
			{
				//item is last node
				if (currentLink->next == NULL) last = tempLink;

				tempLink->next = first->next;
				first = tempLink;
				currentLink = NULL;
				return true;
			}
			else
			{
				//item is last node
				if (currentLink->next == NULL) last = tempLink;

				tempLink->next = currentLink->next;
				previousLink->next = tempLink;
				return true;
			}
		}
		else
		{
			return false;
		}
	}
}

/*
	if an equivalent item is already present, remove item and return true
	if an equivalent item is not already present, leave list unchanged and return false
	@param:item = item to be removed
	@return = whether or not it was removed
*/
template <typename T>
bool OULinkedList<T>::remove(T* item)
{
	//empty list
	if (first == NULL)
	{
		return false;
	}
	else
	{
		T* tempItem = new T();
		OULink<T>* currentLink = new OULink<T>(tempItem);
		OULink<T>* previousLink = NULL;
		currentLink = first;
		bool found = false;
		OULinkedListEnumerator<T> enumerator = this->enumerator();

		//loop through list searching for a match
		for (int index = 0; 0 < size; ++index)
		{
			//match found
			if (comparator->compare(*item, *currentLink->data) == 0)
			{
				found = true;
				break;
			}

			previousLink = currentLink;
			if (enumerator.hasNext())
			{
				enumerator.next();
			}

			currentLink = currentLink->next;
			if (currentLink == NULL)
			{
				found = false;
				break;
			}
		}
		//match found
		if (found)
		{
			//first node
			if (previousLink == NULL)
			{
				first = first->next;
				currentLink = NULL;
				--size;
				return true;
			}
			else
			{
				//last node
				if (currentLink->next == NULL) last = previousLink;

				previousLink->next = currentLink->next;
				currentLink = NULL;
				--size;
				return true;
			}
		}
		return false;
	}
}

/*
	if any items are present, return a copy of the first item
	if no items are present, throw new ExceptionLinkedListAccess
	@return = first node
*/
template <typename T>
T OULinkedList<T>::get() const
{
	if (first == NULL) throw new ExceptionLinkedListAccess;
	else return *first->data;
}

/*
	if an equivalent item is present, return a copy of the first such item
	if an equivalent item is not present, throw a new ExceptionLinkedListAccess
	@param:item = item to find
	@return = found item
*/
template <typename T>
T OULinkedList<T>::find(const T* item) const
{
	T* tempItem = new T();
	OULink<T>* currentLink = new OULink<T>(tempItem);
	currentLink = first;
	bool found = false;

	OULinkedListEnumerator<T> enumerator = this->enumerator();

	//loop through list
	for (int index = 0; 0 < size; ++index)
	{
		//found item
		if (comparator->compare(*item, *currentLink->data) == 0)
		{
			found = true;
			break;
		}

		if (enumerator.hasNext())
		{
			enumerator.next();
		}

		currentLink = currentLink->next;
		if (currentLink == NULL)
		{
			found = false;
			break;
		}
	}

	//item found
	if (found) return *currentLink->data;
	//item not found
	else throw new ExceptionLinkedListAccess;
}

/*
	get the size of the list
	@return = size
*/
template <typename T>
unsigned long OULinkedList<T>::getSize() const
{
	return size;
}

/*
	enumerator for the list
	@return = enumerator with current set to the first node
*/
template<typename T>
OULinkedListEnumerator<T> OULinkedList<T>::enumerator() const
{
	OULinkedListEnumerator<T> enumerator(first);
	return enumerator;
}

#endif // !OU_LINKED_LIST