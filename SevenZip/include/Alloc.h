#ifndef ALLOC_H
#define ALLOC_H


extern "C" {

	void *MidAlloc(size_t size);
	void MidFree(void *address);

}



#endif