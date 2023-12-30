#pragma once

#include <iostream>

template <typename T>
class BinaryTree;

template <typename T>
class BinaryNode
{
public:
	T value;

	BinaryNode<T>* leftChild;
	BinaryNode<T>* rightChild;

	BinaryNode<T>(T t) noexcept : value(t) { leftChild = nullptr; rightChild = nullptr; }

	void Add(T newValue) noexcept;

	const BinaryNode<T>* Find(T query) const noexcept;
	BinaryNode<T>* Find(T query) noexcept;
};

template<typename T>
inline void BinaryNode<T>::Add(T newValue) noexcept
{
	if (newValue <= value)
	{
		if (leftChild != nullptr)leftChild->Add(newValue);
		else leftChild = new BinaryNode<T>(newValue);
	}
	else
	{
		if (rightChild != nullptr) rightChild->Add(newValue);
		else rightChild = new BinaryNode<T>(newValue);
	}
}

template<typename T>
inline const BinaryNode<T>* BinaryNode<T>::Find(T query) const noexcept
{
	if (query == value)
	{
		const BinaryNode<T>* constThis = this;
		return constThis;
	}
	else if (query < value) return leftChild ? leftChild->Find(query) : nullptr;
	else return rightChild ? rightChild->Find(query) : nullptr;
}

template<typename T>
inline BinaryNode<T>* BinaryNode<T>::Find(T query) noexcept
{
	if (query == value) return this;
	else if (query < value) return leftChild ? leftChild->Find(query) : nullptr;
	else return rightChild ? rightChild->Find(query) : nullptr;
}