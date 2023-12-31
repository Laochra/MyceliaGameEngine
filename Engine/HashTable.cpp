#include "HashTable.h"

HashTable::HashTable(unsigned int size)
{
	buckets = std::vector<HashBucket*>(size);
	for (unsigned int i = 0; i < size; i++)
	{
		buckets[i] = new HashBucket();
	}
}

HashBucket* HashTable::operator[](const int& key)
{
	return buckets[key];
}

const HashBucket* HashTable::operator[](const int& key) const
{
	return buckets[key];
}

bool HashTable::Contains(const char* query, unsigned int length) const
{
	unsigned int hashedKey = Hash(query, length) % buckets.size();

	if (buckets[hashedKey]->entries == nullptr) return false;

	std::string queryStr = std::string(query, length);

	LinkedNode<char*>* currentEntry = buckets[hashedKey]->entries->first;
	LinkedNode<int>* currentCount = buckets[hashedKey]->counts->first;
	while (currentEntry != nullptr)
	{
		if (std::string(currentEntry->value, currentCount->value) == queryStr) return true;
		currentEntry = currentEntry->next;
		currentCount = currentCount->next;
	}
	return false;
}

void HashTable::Add(char* newEntry, unsigned int length)
{
	if (Contains(newEntry, length)) return;

	unsigned int hashedKey = Hash(newEntry, length) % buckets.size();
	buckets[hashedKey]->entries->InsertAtEnd(newEntry);
	buckets[hashedKey]->counts->InsertAtEnd(length);
}



unsigned int Hash(const char* data, unsigned int length)
{
	unsigned int hash = 0;
	for (unsigned int i = 0; i < length; i++)
	{
		hash += data[i] + i;

		if (data[i] % 3 == 0)
		{
			for (int d = 0; d < data[i] / 3; d++) hash += (int)"Fizz";
		}
		else hash += data[i] % 3;

		if (data[i] % 5 == 0)
		{
			for (int d = 0; d < data[i] / 5; d++) hash += (int)"Buzz";
		}
		else hash += data[i] % 5;
	}
	return hash;
}