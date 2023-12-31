#pragma once

#include "HashBucket.h"

#include <vector>
#include <functional>

class HashTable
{
public:
	std::vector<HashBucket*> buckets;

	HashTable() = default;
	HashTable(unsigned int size);

	HashBucket* operator[](const int& key);
	const HashBucket* operator[](const int& key) const;

	bool Contains(const char* query, unsigned int length) const;

	void Add(char* newEntry, unsigned int length);
};

unsigned int Hash(const char* data, unsigned int length);