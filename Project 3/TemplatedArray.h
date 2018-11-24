#ifndef TEMPLATED_ARRAY_H
#define TEMPLATED_ARRAY_H

#include "Exceptions.h"

const unsigned long DEFAULT_ARRAY_CAPACITY = 10;		// capacity used in no arg constructor

template <typename T>
class TemplatedArray {
private:
	unsigned long capacity = DEFAULT_ARRAY_CAPACITY;	// maximum capacity, in items
	unsigned long size = 0;								// actual number of items currently in array
	T* data = NULL;										// pointer to array of any type
	void doubleCapacity();								// method to double array capacity
	void halveCapacity();								// method to halve array capacity
public:
	TemplatedArray();									// constructs array with default capacity
	TemplatedArray(unsigned long capacity);				// constructs array with specified capacity
	virtual ~TemplatedArray();							// frees array space as object is deleted
	void add(const T* item);							// adds item, increments size, doubles capacity as necessary
	void addAt(const T* item, unsigned long index);		// adds item at index, shifts following, doubles capacity as necessary
	void replaceAt(const T* item, unsigned long index);	// replaces item at index, otherwise unchanged
	void removeAt(unsigned long index);					// removes item at index, shifts following back
	T get(unsigned long index) const;					// returns (copy of) item at index
	unsigned long getCapacity() const;					// returns the current capacity of the array
	unsigned long getSize() const;						// returns the current number of items in the array
	T operator[](unsigned long index) const;			// returns (copy of) item at index
};



//###############################################################################################################################


/*
	default constructor
*/
template<typename T>
TemplatedArray<T>::TemplatedArray()
{
	data = new T[capacity]; // initializes T array with size capacity
	if (data == NULL) throw new ExceptionMemoryNotAvailable;
}

/*
	constructor taking an unsigned int parameter
	@param:capacity = size of the T array
*/
template<typename T>
TemplatedArray<T>::TemplatedArray(unsigned long capacity)
{
	this->capacity = capacity;
	data = new T[capacity];
	if (data == NULL) throw new ExceptionMemoryNotAvailable;
}

/*
	deconstructor
*/
template<typename T>
TemplatedArray<T>::~TemplatedArray()
{
	delete[] data;
	data = NULL;
}

/*
	doubles the capacity of the array
*/
template<typename T>
void TemplatedArray<T>::doubleCapacity()
{
	//temporary T* array with twice the capacity
	T* temp = new T[capacity * 2];
	if (temp == NULL) throw new ExceptionMemoryNotAvailable;

	//loops through the original TemplatedArray and assigns the values to temp[]
	for (unsigned long index = 0; index < size; ++index)
	{
		temp[index] = data[index];
	}

	//doubles the capacity of the original TemplatedArray
	capacity = capacity * 2;
	data = new T[capacity];
	if (data == NULL) throw new ExceptionMemoryNotAvailable;

	//loops through the temp[] and puts them back into the original TemplatedArray
	for (unsigned long index = 0; index < size; ++index)
	{
		data[index] = temp[index];
	}
	//clearing memory
	delete[] temp;
	temp = NULL;
}

/*
	halves the capacity of the array
*/
template<typename T>
void TemplatedArray<T>::halveCapacity()
{
	//temporary T* array
	T* temp = new T[capacity];
	if (temp == NULL) throw new ExceptionMemoryNotAvailable;

	//adds all the values to the temp array
	for (unsigned long index = 0; index < size; ++index)
	{
		temp[index] = data[index];
	}

	//halves the capacity of the data array
	capacity = capacity / 2;
	data = new T[capacity];

	//puts the values back into your data array
	for (unsigned long index = 0; index < size; ++index)
	{
		data[index] = temp[index];
	}
}

/*
	adds a T to the array and dynamically allocates memory as needed
	@param:record = pointer to a T to be added to the TemplatedArray data array
*/
template<typename T>
void TemplatedArray<T>::add(const T* item)
{
	//max capacity reached
	if (size == capacity)
	{
		doubleCapacity();
	}

	data[size] = *item; // adds the T to the data array
	++size; // increments the value of the size of the array
}

/*
	adds a T to the array at the given index and dynamically allocates memory as needed
	@param:record = pointer to a T to be added to the TemplatedArray data array
	@param:location = index to add item
*/
template<typename T>
void TemplatedArray<T>::addAt(const T* item, unsigned long location)
{
	//index out of bounds
	if (location >= size)
	{
		throw new ExceptionIndexOutOfRange;
	}
	else
	{
		//temporary T* array with twice the capacity
		T* temp = new T[capacity * 2];
		if (temp == NULL) throw new ExceptionMemoryNotAvailable;

		bool added = false; // flag for when the value gets added

		//doubles capacity
		if (size == capacity)
		{
			doubleCapacity();
		}

		//adds value at the given index to temp array
		for (unsigned long index = 0; index < size; ++index)
		{
			//index to insert
			if (index == location)
			{
				temp[index] = *item; // inserting item
				added = true; // flagging added
				location = size + 1; // setting location's size to unreachable value
				--index; // goes back to value that was supposed to go in index
			}
			else
			{
				if (added) temp[index + 1] = data[index]; // item* already added so index gets bumped up by one
				else temp[index] = data[index]; // populating temp[]
			}
		}
		//puts values back into your array
		for (unsigned long index = 0; index < size + 1; ++index)
		{
			data[index] = temp[index];
		}
		++size; // data array increases in size by 1

		//clearing memory
		delete[] temp;
		temp = NULL;
	}
}

/*
	replaces a item at a given index
	@param:record = pointer to a T to be added to the TemplatedArray data array
	@param:index = index of item to be replaced by given item
*/
template<typename T>
void TemplatedArray<T>::replaceAt(const T* item, unsigned long index)
{
	if (index >= size) throw new ExceptionIndexOutOfRange; // index out of bounds
	else data[index] = *item; // replaces item at index
}

/*
	removes the item at the given index and dynamically deallocates memory as needed
	@param:location = index of item to be removed
*/
template<typename T>
void TemplatedArray<T>::removeAt(unsigned long location)
{
	//index out of bounds
	if (location >= size)
	{
		throw new ExceptionIndexOutOfRange;
	}
	else
	{
		//temporary T* array
		T* temp = new T[capacity];
		if (temp == NULL) throw new ExceptionMemoryNotAvailable;

		bool removed = false; // flag for when the item is removed

		//populates the temp[] with every item except the one at location
		for (unsigned long index = 0; index < size; ++index)
		{
			if (index == location)
			{
				location = size + 1;
				removed = true;
			}
			else
			{
				if (removed) temp[index - 1] = data[index];
				else temp[index] = data[index];
			}
		}

		//puts the items back into your data[] without the item at location
		for (unsigned long index = 0; index < size - 1; ++index)
		{
			data[index] = temp[index];
		}
		--size; // decreases the data array size by 1

		//capacity gets halved when the number of items in the array is less than half of the capacity
		if (size <= (capacity / 2))
		{
			halveCapacity();
		}
		//clearing memory
		delete[] temp;
		temp = NULL;
	}
}




/*
/
	swaps values at given indeces

	@param:index1 = location of first item to be swapped
	@param:index2 = location of second item to be swapped
/
template<typename T>
void TemplatedArray<T>::swap(unsigned long index1, unsigned long index2)
{
	T temp;

	temp = data[index2];
	data[index2] = data[index1];
	data[index1] = temp;
}
*/




/*
	gets the T at the given index
	@param:index = index of the requested T
	@return = the T at the given index
*/
template<typename T>
T TemplatedArray<T>::get(unsigned long index) const
{
	if (index >= size) throw new ExceptionIndexOutOfRange; // index out of bounds
	else return data[index];
}

/*
	gets the capacity of the data array
	@return = the capacity
*/
template<typename T>
unsigned long TemplatedArray<T>::getCapacity() const
{
	return capacity;
}

/*
	gets the actual number of elements in the data array
	returns = the size
*/
template<typename T>
unsigned long TemplatedArray<T>::getSize() const
{
	return size;
}

/*
	returns (reference to) item at index
	@param:index = index of the item
	@return = (reference to) item at index
*/
template<typename T>
T TemplatedArray<T>::operator[](unsigned long index) const
{
	if (index >= size) throw new ExceptionIndexOutOfRange; // index out of bounds
	else return data[index];
}


#endif // !TEMPLATED_ARRAY_H