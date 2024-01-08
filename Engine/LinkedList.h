#pragma once

#include <iostream>

#include "LinkedNode.h"

template <typename T>
class LinkedList
{
public:
	LinkedNode<T>* first = nullptr;
	LinkedNode<T>* last = nullptr;

private:
	int count = 0;

public:
	int GetCount() const noexcept;
	bool IsEmpty() const noexcept;

	void InsertAtStart(T t) noexcept;
	void InsertAtEnd(T t) noexcept;
	void InsertBefore(T t, LinkedNode<T>* targetNode) noexcept;

	void RemoveFirst() noexcept;
	void RemoveLast() noexcept;
	void Remove(LinkedNode<T>* targetNode) noexcept;


	// Pass 'numeric' as false to allow non-numeric types.
	// This function only works on types with the < operator.
	// Feel free to define your own cases.
	void Sort(bool numeric = true);
};



template<typename T>
inline int LinkedList<T>::GetCount() const noexcept
{
	return 0;
}
template<typename T>
inline bool LinkedList<T>::IsEmpty() const noexcept
{
	return count <= 0;
}

template<typename T>
inline void LinkedList<T>::InsertAtStart(T t) noexcept
{
	LinkedNode<T>* newNode = new LinkedNode<T>(t);

	if (first != nullptr)
	{
		first->previous = newNode;
		newNode->next = first;
	}

	if (last == nullptr)
	{
		last = newNode;
	}

	first = newNode;
	newNode->list = this;

	count++;
}
template<typename T>
inline void LinkedList<T>::InsertAtEnd(T t) noexcept
{
	LinkedNode<T>* newNode = new LinkedNode<T>(t);

	if (last != nullptr)
	{
		last->next = newNode;
		newNode->previous = last;
	}

	if (first == nullptr)
	{
		first = newNode;
	}

	last = newNode;
	newNode->list = this;

	count++;
}
template<typename T>
inline void LinkedList<T>::InsertBefore(T t, LinkedNode<T>* targetNode) noexcept
{
	if (targetNode->list != this) return;

	if (targetNode == first)
	{
		InsertAtStart(t);
		return;
	}

	LinkedNode<T>* newNode = new LinkedNode<T>(t);

	LinkedNode<T>* previousNode = targetNode->previous;

	newNode->next = targetNode;
	if (previousNode != nullptr)
	{
		newNode->previous = previousNode;
		previousNode->next = newNode;
	}
	targetNode->previous = newNode;

	newNode->list = this;

	count++;
}

template<typename T>
inline void LinkedList<T>::RemoveFirst() noexcept
{
	if (IsEmpty()) return;
	if (first == nullptr) return;

	if (first->next != nullptr)
	{
		first->next->previous = nullptr;
		first = first->next;
	}
	else
	{
		first = nullptr;
	}

	count--;
}
template<typename T>
inline void LinkedList<T>::RemoveLast() noexcept
{
	if (IsEmpty()) return;

	if (count == 1)
	{
		RemoveFirst();
		return;
	}

	if (last->previous != nullptr)
	{
		last->previous->next = nullptr;
	}

	last = last->previous;

	count--;
}
template<typename T>
inline void LinkedList<T>::Remove(LinkedNode<T>* targetNode) noexcept
{
	if (targetNode->list != this) return;
	if (IsEmpty()) return;

	if (targetNode == first)
	{
		RemoveFirst();
		return;
	}
	if (targetNode == last)
	{
		RemoveLast();
		return;
	}

	if (targetNode->previous != nullptr && targetNode->next != nullptr)
	{
		targetNode->previous->next = targetNode->next;
		targetNode->next->previous = targetNode->previous;
	}
	else
	{
		if (targetNode->previous != nullptr) targetNode->previous->next = nullptr;
		else if (targetNode->next != nullptr) targetNode->next->previous = nullptr;
	}

	targetNode->previous = nullptr;
	targetNode->next = nullptr;
	count--;
}

template<typename T>
inline void LinkedList<T>::Sort(bool numeric)
{
	if (count <= 1) return;

	LinkedNode<T>* currentNode = first;
	if (numeric == true && std::is_arithmetic<T>::value == false)
	{
		std::cout
			<< "\nWarning: LinkedList.Sort() has only been defined to work with < checks.\n"
			<< "This function only allows numeric types by default for safety and a more helpful error message.\n"
			<< "You can pass the numeric parameter as false to allow non-numeric types.\n"
			<< "(This will cause compile errors for types without the < operator)\n"
			
			<< "\nFeel free to define your own cases for types without the < operator.\n\n";
		return;
	} // Ensure numeric safety by default

	bool sorted = false;
	while (sorted == false)
	{
		bool changeMade = false;

		while (currentNode != nullptr && currentNode->next != nullptr)
		{
			if (currentNode->value < currentNode->next->value)
			{
				LinkedNode<T>* otherNode = currentNode->next;

				if (currentNode->previous != nullptr) currentNode->previous->next = otherNode;
				if (otherNode->next != nullptr) otherNode->next->previous = currentNode;

				currentNode->next = otherNode->next;
				otherNode->previous = currentNode->previous;

				currentNode->previous = otherNode;
				otherNode->next = currentNode;

				if (first == currentNode) first = otherNode;
				if (last == otherNode) last = currentNode;

				changeMade = true;
			}

			currentNode = currentNode->next;
		}

		if (!changeMade) sorted = true;
		else currentNode = first;
	}
}