#include <windows.h>
#include <stdlib.h>

#include "Alloc.h"

void *MidAlloc(size_t size)
{
	if (size == 0)
		return NULL;


	return VirtualAlloc(NULL, size, MEM_COMMIT, PAGE_READWRITE);
}

void MidFree(void *address)
{

	if (!address)
		return;
	VirtualFree(address, 0, MEM_RELEASE);
}

