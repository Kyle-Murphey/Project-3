#ifndef OU_LINKED_LIST_ENUMERATOR
#define OU_LINKED_LIST_ENUMERATOR

#include "Enumerator.h"
#include "Exceptions.h"
#include "OULink.h"

template <typename T>
class OULinkedListEnumerator : public Enumerator<T>
{
private:
	OULink<T>* current;
	OULink<T>* firstItem; // first node
public:
	OULinkedListEnumerator(OULink<T>* first);
	virtual ~OULinkedListEnumerator();
	bool hasNext() const;
	T next();			// throws ExceptionEnumerationBeyondEnd if no next item is available
	T peek() const;		// throws ExceptionEnumerationBeyondEnd if no next item is available
	T peekCurrent() const; // return current node without incrementing
};

/*
	constructor
	@param:first = first node to be set to current
*/
template <typename T>
OULinkedListEnumerator<T>::OULinkedListEnumerator(OULink<T>* first)
{
	current = first;
	firstItem = first;
}

/*
	destructor
*/
template <typename T>
OULinkedListEnumerator<T>::~OULinkedListEnumerator(){}

/*
	checks if next node exists
	@return = whether or not next node is there
*/
template <typename T>
bool OULinkedListEnumerator<T>::hasNext() const
{
	if (current == NULL) return false;
	else if (current->next != NULL) return true;
	else return false;
}

/*
	get the current node and increment to the next
	@return = current node
*/
template <typename T>
T OULinkedListEnumerator<T>::next()
{
	if (current == NULL)
	{
		throw new ExceptionEnumerationBeyondEnd;
	}
	else
	{
		T* temp = new T();
		OULink<T>* tempLink = new OULink<T>(temp);
		tempLink->data = current->data;
		current = current->next;
		return *tempLink->data;
	}
}

/*
	see the next node without incrementing
	@return = next node
*/
template <typename T>
T OULinkedListEnumerator<T>::peek() const
{
	if (firstItem == NULL)
	{
		throw new ExceptionEnumerationBeyondEnd;
	}
	else
	{
		T* temp = new T();
		OULink<T>* tempLink = new OULink<T>(temp);
		tempLink->data = current->data;
		return *tempLink->data;
	}
}

/*
	get the current node
	@return = current node
*/
template <typename T>
T OULinkedListEnumerator<T>::peekCurrent() const
{
	if (current == NULL)
	{
		throw new ExceptionEnumerationBeyondEnd;
	}
	else
	{
		T* temp = new T();
		OULink<T>* tempLink = new OULink<T>(temp);
		tempLink->data = current->data;
		return *tempLink->data;
	}
}

#endif // !OU_LINKED_LIST_ENUMERATOR