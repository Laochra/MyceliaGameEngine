#pragma once

template <typename T>
class LinkedList;

template <typename T>
class LinkedNode
{
public:
	T value;

	LinkedNode<T>* previous;
	LinkedNode<T>* next;

	LinkedList<T>* list;

	LinkedNode(T t) noexcept : value(t), previous(nullptr), next(nullptr), list(nullptr) {}
};