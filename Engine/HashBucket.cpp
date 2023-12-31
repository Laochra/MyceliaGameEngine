#include "HashBucket.h"

HashBucket::HashBucket()
{
	entries = new LinkedList<char*>();
	counts = new LinkedList<int>();
}

HashBucket::~HashBucket()
{
	delete entries;
	delete counts;
}