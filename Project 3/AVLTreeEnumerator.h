#ifndef AVL_TREE_ENUMERATOR
#define AVL_TREE_ENUMERATOR

#include <stack>
#include "Enumerator.h"
#include "AVLTreeOrder.h"
#include "AVLTree.h"

template <typename T>
class AVLTreeEnumerator : public Enumerator<T>
{
private:
	AVLTreeOrder order;
	std::stack<const AVLTree<T>*> traversalStack;
	void buildTraversalStack(const AVLTree<T>* current);
public:
	AVLTreeEnumerator(const AVLTree<T>* root, AVLTreeOrder order);
	virtual ~AVLTreeEnumerator();
	bool hasNext() const;
	T next();						// throws ExceptionEnumerationBeyondEnd if no next item is available
	T peek() const;					// throws ExceptionEnumerationBeyondEnd if no next item is available
	AVLTreeOrder getOrder();		// returns the order of this enumerator (preorder, inorder, or postorder)
};

// put implementation here

template <typename T>
AVLTreeEnumerator<T>::AVLTreeEnumerator(const AVLTree<T>* root, AVLTreeOrder order)
{

}

template <typename T>
AVLTreeEnumerator<T>::~AVLTreeEnumerator()
{

}

template <typename T>
bool AVLTreeEnumerator<T>::hasNext() const
{

}

template <typename T>
T AVLTreeEnumerator<T>::next()
{

}

template <typename T>
T AVLTreeEnumerator<T>::peek() const
{

}

template <typename T>
AVLTreeOrder AVLTreeEnumerator<T>::getOrder()
{

}

template <typename T>
void AVLTreeEnumerator<T>::buildTraversalStack(const AVLTree<T>* current)
{

}

#endif // !AVL_TREE_ENUMERATOR