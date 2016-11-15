#include <iostream>
#include "Allocator.h"
using namespace std;

int main()
{
	Allocator alloc;


	void * a1 = alloc.allocate(20);	
	void * a2 = alloc.allocate(3);
	void * a3 = alloc.allocate(22);
	void * a4 = alloc.allocate(12);	
	void * a5 = alloc.allocate(89);
	cout << alloc.getUsedCells() << " " << alloc.getFreeCells() << endl;

	//alloc.deallocate(a1);
	alloc.deallocate(a3);
	cout << alloc.getUsedCells() << " " << alloc.getFreeCells() << endl;
	alloc.deallocate(a2);
	cout << alloc.getUsedCells() << " " << alloc.getFreeCells() << endl;
	alloc.deallocate(a4);
	
	cout << alloc.getUsedCells() << " " << alloc.getFreeCells() << endl;

	void* a6 = alloc.allocate(22);

	cout << alloc.getUsedCells() << " " << alloc.getFreeCells() << endl;
	
	//alloc.deallocate(a2);
	cout << alloc.getUsedCells() << " " << alloc.getFreeCells() << endl;
	//alloc.deallocate(a4);
	cout << alloc.getUsedCells() << " " << alloc.getFreeCells() << endl;
	//alloc.deallocate(a6);
	cout << alloc.getUsedCells() << " " << alloc.getFreeCells() << endl;
	alloc.deallocate(a1);
	//alloc.deallocate(a5);
	cout << alloc.getUsedCells() << " " << alloc.getFreeCells() << endl;
	
	system("pause");
	return 0;
}