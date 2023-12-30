#pragma once

#include "LinkedList.h"

class HashBucket
{
public:
	LinkedList<char*>* entries;
	LinkedList<int>* counts;

	HashBucket();
	~HashBucket();
};