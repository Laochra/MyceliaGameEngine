#pragma once

#include "BinaryNode.h"

template <typename T>
class BinaryTree
{
public:
	BinaryNode<T>* root;

	void Add(T newValue) noexcept;

	BinaryNode<T>* Find(T query) const noexcept;
};

template<typename T>
inline void BinaryTree<T>::Add(T newValue) noexcept
{
	if (root != nullptr) root->Add(newValue);
	else root = new BinaryNode<T>(newValue);
}

template<typename T>
inline BinaryNode<T>* BinaryTree<T>::Find(T query) const noexcept
{
	return root ? root->Find(query) : nullptr;
}