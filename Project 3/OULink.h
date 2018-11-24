#ifndef OU_LINK_H
#define OU_LINK_H

template <typename T>
class OULink {
	template <typename F>
	friend class OULinkedList;
	template <typename F>
	friend class OULinkedListEnumerator;
private:
	T* data = NULL;										// pointer to data item of any type
	OULink* next = NULL;								// pointer to next link
public:
	OULink(const T* item);
	virtual ~OULink();
};


/*
	constructor for the new link
	@param:item: node
*/
template <typename T>
OULink<T>::OULink(const T* item)
{
	data = new T(*item);
}

/*
	destructor
*/
template <typename T>
OULink<T>::~OULink()
{
	if (data != NULL)
	{
		delete data;
	}
	data = NULL;

	if (next != NULL)
	{
		delete next;
	}
	next = NULL;
}

#endif // !OU_LINK_H