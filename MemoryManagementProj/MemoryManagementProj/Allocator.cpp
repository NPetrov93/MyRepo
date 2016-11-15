#include "Allocator.h"
#include <iostream>

// Memory Management Project

const int POOL_SIZE = 1000;//0000; // should be even
const int MIN_ALLOC = sizeof(void*)* 2;
const size_t headerSize = sizeof(size_t);
const int minGap = MIN_ALLOC + 2 * headerSize;

Allocator::Allocator() {
	expandPoolSize();
	//freeList = reinterpret_cast<FreeList*>(pool + headerSize);
	freeList = reinterpret_cast<FreeList*>(pool + headerSize);
	freeList->next = nullptr;
	freeList->previous = nullptr;

	size_t* beginInfo = reinterpret_cast<size_t*>(pool);
	*beginInfo = POOL_SIZE - 2 * headerSize;
	size_t* endInfo = reinterpret_cast<size_t*>(pool + POOL_SIZE - headerSize);
	*endInfo = POOL_SIZE - 2 * headerSize;

	endPool = pool + POOL_SIZE;
}

Allocator::Allocator(const Allocator& allocator) {
	Allocator();
}

Allocator& Allocator::operator=(const Allocator& allocator) {
	return *this;
}

Allocator::~Allocator()
{
	delete[] pool;
}

void Allocator::expandPoolSize()
{
	if (!pool) {
		pool = new char[POOL_SIZE];
	}
	else {

	}
}

void* Allocator::allocate(size_t sizeNeeded)
{

	if (sizeNeeded < MIN_ALLOC)
		sizeNeeded = MIN_ALLOC;
	else if (sizeNeeded % 2 == 1)
		sizeNeeded++;

	void* memoryToReturn = nullptr;

	if (!freeList)
		return nullptr;

	FreeList* currentFree = freeList;//reinterpret_cast<FreeList*>(pool + headerSize);
	char* c_beginPointer = reinterpret_cast<char*>(currentFree)-headerSize;

	size_t* currentHeader = reinterpret_cast<size_t*>(c_beginPointer);

	while (currentFree->next && c_beginPointer < (pool + POOL_SIZE) && *currentHeader < sizeNeeded)
	{
		currentFree = currentFree->next;
		c_beginPointer = reinterpret_cast<char*>(currentFree) - headerSize;
		currentHeader = reinterpret_cast<size_t*>(c_beginPointer);
	}

	if (c_beginPointer < (pool + POOL_SIZE))
	{
		if (*currentHeader >= sizeNeeded + minGap)
		{
			size_t* newHeaderBegin = reinterpret_cast<size_t*>(c_beginPointer + sizeNeeded + 2 * headerSize);
			size_t* newHeaderEnd = reinterpret_cast<size_t*>(c_beginPointer + headerSize + *currentHeader);
			size_t* headerEndAlloc = reinterpret_cast<size_t*>(c_beginPointer + sizeNeeded + headerSize);

			FreeList* newFree = reinterpret_cast<FreeList*>(c_beginPointer + (headerSize * 3) + sizeNeeded);
			//currentFree = reinterpret_cast<FreeList*>(c_beginPointer + sizeNeeded + 3 * headerSize);
			//c_beginPointer = reinterpret_cast<char*>(currentFree)+headerSize;
			
			addFree(newFree);
			removeFree(currentFree);

			*newHeaderBegin = *currentHeader - sizeNeeded - 2 * headerSize;
			*newHeaderEnd = *currentHeader - sizeNeeded - 2 * headerSize;
			*headerEndAlloc = sizeNeeded;
			*headerEndAlloc = *headerEndAlloc | 1;
			*currentHeader = sizeNeeded;
			*currentHeader = *currentHeader | 1;
		}
		else // if (*currentHeader == sizeNeeded + 2 * sizeof(size_t))
		{
			size_t* end = reinterpret_cast<size_t*>(c_beginPointer + headerSize + *currentHeader);
			*end = *end | 1;
			*currentHeader = *currentHeader | 1;
			removeFree(currentFree);
		}

		memoryToReturn = c_beginPointer + headerSize;
	}

	return memoryToReturn;
	/*
	freeList = reinterpret_cast<FreeList*>(pool + headerSize);

	FreeList* current = freeList;
	char* beginPointer = reinterpret_cast<char*>(current)-headerSize;

	//char* c_currentHeader = m_buffer;

	size_t* currentHeader = reinterpret_cast<size_t*>(beginPointer);

	while (current && beginPointer < (pool + POOL_SIZE) && *currentHeader < size)
	{
	current = current->next;
	beginPointer = reinterpret_cast<char*>(current) - headerSize;
	currentHeader = reinterpret_cast<size_t*>(beginPointer);
	}


	*/
}

void Allocator::deallocate(void* pointer)
{
	if (!pointer)
		return;
	size_t* beginHeader = static_cast<size_t*>(pointer)-1;
	//size_t amount = *beginHeader;
	char* c_begin = reinterpret_cast<char*>(beginHeader);
	size_t* endHeader = reinterpret_cast<size_t*>(c_begin + *beginHeader + headerSize - 1);
	//amount = *endHeader;
	char* c_end = reinterpret_cast<char*>(endHeader);

	*beginHeader &= ~(1 << 0);
	*endHeader &= ~(1 << 0);

	FreeList* currentFree = reinterpret_cast<FreeList*>(c_begin + headerSize);
	addFree(currentFree);

	if (c_begin - headerSize > pool)	// try coalesce left
	{
		size_t* leftHeaderEnd = reinterpret_cast<size_t*>(beginHeader - 1);
		//amount = *leftHeaderEnd;
		if ((*leftHeaderEnd & 1) == 0)	// coalesce left
		{
			char* c_leftEnd = reinterpret_cast<char*>(leftHeaderEnd);
			size_t* leftHeaderBegin = reinterpret_cast<size_t*>(c_leftEnd - *leftHeaderEnd - headerSize);

			*leftHeaderBegin += *beginHeader + headerSize * 2;
			beginHeader = leftHeaderBegin;

			removeFree(currentFree);
		}
	}
	if (c_begin + headerSize < pool + POOL_SIZE)
	{
		size_t* rightHeaderBegin = reinterpret_cast<size_t*>(endHeader + 1);
		//amount = *rightHeaderBegin;
		if ((*rightHeaderBegin & 1) == 0)
		{
			char* c_rightBegin = reinterpret_cast<char*>(rightHeaderBegin);
			size_t* rightHeaderEnd = reinterpret_cast<size_t*>(c_rightBegin + *rightHeaderBegin + headerSize);

			FreeList* rightFree = reinterpret_cast<FreeList*>(c_rightBegin + headerSize);
			removeFree(rightFree);

			*rightHeaderEnd += *beginHeader + headerSize * 2;
			*beginHeader = *rightHeaderEnd;
		}
	}
}

void Allocator::addFree(FreeList* free) {
	if (freeList)
	{
		free->next = freeList;
		free->previous = nullptr;
		(free->next)->previous = free;
		freeList = free;
	}
	else
	{
		freeList = free;
		freeList->next = nullptr;
		freeList->previous = nullptr;
	}
}

void Allocator::removeFree(FreeList* removed) {
	if (removed->next) {
		if (removed->previous) {
			removed->next->previous = removed->previous;
			removed->previous->next = removed->next;
		}
		else {
			removed->next->previous = nullptr;
			freeList = removed->next;
		}
	}
	else if (removed->previous)
	{
		removed->previous->next = nullptr;
	}
	else
	{
		freeList = nullptr;
	}
}

int Allocator::getFreeCells() const
{
	int result = 0;
	size_t* currentHeader;
	char* c_currentHeader = pool;
	//cout << "free: ";
	do
	{
		currentHeader = reinterpret_cast<size_t*>(c_currentHeader);
		//cout << *currentHeader << " ";
		if (reinterpret_cast<char*>(currentHeader) < (pool + POOL_SIZE))
		{
			if (!(*currentHeader & 1))
			{
				result++;
				c_currentHeader += *currentHeader + 2 * headerSize;
			}
			else
			{
				c_currentHeader = c_currentHeader + *currentHeader + 2 * headerSize - 1;
			}
		}
		//else
		//{
		//	c_currentHeader = c_currentHeader + *currentHeader + 2 * headerSize;
		//}
	} while (reinterpret_cast<char*>(currentHeader) < (pool + POOL_SIZE));

	//cout << endl;
	return result;
}

int Allocator::getUsedCells() const
{
	int result = 0;
	size_t* currentHeader;
	char* c_currentHeader = pool;
	//cout << "used: ";

	do
	{
		currentHeader = reinterpret_cast<size_t*>(c_currentHeader);
		//cout << *currentHeader << " ";
		if (reinterpret_cast<char*>(currentHeader) < (pool + POOL_SIZE) )//&& ((*currentHeader) & 1))
		{
			if ((*currentHeader) & 1)
			{
				result++;
				c_currentHeader = c_currentHeader + *currentHeader + 2 * headerSize - 1;
			}
			else
			{
				c_currentHeader = c_currentHeader + *currentHeader + 2 * headerSize;
			}
		}
	} while (reinterpret_cast<char*>(currentHeader) < (pool + POOL_SIZE));

	//cout << endl;
	return result;
}