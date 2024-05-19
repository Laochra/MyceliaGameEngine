#pragma once

#include "MathIncludes.h"

#include <vector>
using std::vector;

template<class H> class Heirarchical
{
public:
	H* GetParent() const noexcept;
	void SetParent(H* parentInit) noexcept;

	const vector<H*>* GetChildren() const noexcept;
	vector<H*>* GetChildren() noexcept;
	void AddChild(H* child) noexcept;
	void RemoveChild(H* child) noexcept;

	void MoveTo(int newIndex) noexcept;
	void MoveChildTo(H* child, int newIndex) noexcept;

	bool ContainsChild(const H* child, bool recursive = false) const noexcept;
	int GetIndex() const noexcept;
	int GetChildIndex(const H* child) const noexcept;

protected:
	H* parent = nullptr;
	vector<H*> children = vector<H*>();
};