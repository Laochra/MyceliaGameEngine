#pragma once

#include "Heirarchical.h"

template<class H> H* Heirarchical<H>::GetParent() const noexcept
{
	return parent;
}
template<class H> void Heirarchical<H>::SetParent(H* parentInit) noexcept
{
	if (parentInit == nullptr)
	{
		if (parent == nullptr) return;

		gameObjectManager->Add((H*)this);
		parent->RemoveChild((H*)this);
	}
	else
	{
		if (parent == nullptr)
		{
			gameObjectManager->Remove((H*)this);
		}
		else
		{
			parent->RemoveChild((H*)this);
		}
		parentInit->AddChild((H*)this);
	}

	parent = parentInit;
}

template<class H> const vector<H*>* Heirarchical<H>::GetChildren() const noexcept
{
	return &children;
}
template<class H> void Heirarchical<H>::AddChild(H* child) noexcept
{
	children.push_back(child);
}
template<class H> void Heirarchical<H>::RemoveChild(H* child) noexcept
{
	for (int i = 0; i < children.size(); i++)
	{
		if (*children[i] == *child)
		{
			children.erase(children.begin() + i);
			break;
		}
	}
}

template<class H> void Heirarchical<H>::MoveTo(int newIndex) noexcept
{
	if (parent == nullptr)
	{
		gameObjectManager->Move((H*)this, newIndex);
	}
	else
	{
		parent->MoveChildTo((H*)this, newIndex);
	}
}
template<class H> void Heirarchical<H>::MoveChildTo(H* child, int newIndex) noexcept
{
	int i = child->GetIndex();

	if (i == newIndex) return;

	int direction;
	if (i < newIndex) direction = 1;
	else direction = -1;

	while (i != newIndex)
	{
		if (i + direction < 0 || i + direction >= children.size()) return;

		children[i] = children[i + direction];

		i += direction;
	}

	children[i] = child;
}

template<class H> bool Heirarchical<H>::ContainsChild(const H* child, bool recursive) const noexcept
{
	if (!recursive)
	{
		for (int i = 0; i < children.size(); i++)
		{
			if (children[i] == child)
			{
				return true;
			}
		}
	}
	else
	{
		for (int i = 0; i < children.size(); i++)
		{
			if (children[i] == child || children[i]->ContainsChild(child, true))
			{
				return true;
			}
		}
	}
	return false;
}
template<class H> int Heirarchical<H>::GetIndex() const noexcept
{
	if (parent == nullptr)
	{
		return gameObjectManager->GetIndexOf((H*)this);
	}
	else
	{
		return parent->GetChildIndex((H*)this);
	}
}
template<class H> int Heirarchical<H>::GetChildIndex(const H* child) const noexcept
{
	for (int i = 0; i < children.size(); i++)
	{
		if (children[i] == child || children[i]->ContainsChild(child, true))
		{
			return i;
		}
	}

	return -1;
}