#pragma once
#include <cstddef>

struct FreeList
{
	FreeList* next;
	FreeList* previous;
};

class Allocator
{
private:
	FreeList* freeList;
	char* pool;
	void expandPoolSize();
	void addFree(FreeList* added);
	void removeFree(FreeList* removed);

	char* endPool;

public:
	Allocator();
	Allocator(const Allocator&);
	Allocator& operator=(const Allocator &allocator);
	~Allocator();

	void* allocate(size_t);
	void deallocate(void*);

	int getFreeCells() const;
	int getUsedCells() const;
};