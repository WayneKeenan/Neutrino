#pragma once
#include "Types.h"
#include <stdlib.h>

#define ALIGN_DEFAULT	(16)
#define NEWX			::new(ALIGN_DEFAULT)
#define DELETEX			::delete

#define SAFE_FREE(p)			do{ if(p) { free(p); (p)=0; } }while(0)
#define SAFE_DELETE(p)			do{ if(p) { DELETEX(p); (p)=0; } }while(0)
#define SAFE_DELETE_ARRAY(p)	do{ if(p) { DELETEX[](p); (p)=0; } }while(0)

#if defined(WIN32)

void *operator new(size_t _size);
void *operator new[](size_t _size);
void operator delete(void* _mem);
void operator delete[](void* _mem);

void *operator new(size_t _size, size_t _align);
void *operator new[](size_t _size, size_t _align);
void operator delete(void* _mem, size_t _align);
void operator delete[](void* _mem, size_t _align);

#endif

#if defined(__linux__)


void *operator new(size_t _size);
void *operator new[](size_t _size);

/*
void operator delete(void* _mem);
void operator delete[](void* _mem) ;
*/

void *operator new(size_t _size, size_t _align);
void *operator new[](size_t _size, size_t _align);

#endif

