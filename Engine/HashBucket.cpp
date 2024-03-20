#include "HashBucket.h"

#include "GeneralMacros.h"

HashBucket::HashBucket()
{
	entries = new LinkedList<char*>();
	counts = new LinkedList<int>();
}

HashBucket::~HashBucket()
{
	del(entries);
	del(counts);
}