#include "Memory.h"

#if defined(WIN32)

void *operator new(size_t _size)
{
	return ::_aligned_malloc(_size, ALIGN_DEFAULT);
}

void *operator new[](size_t _size)
{
	return ::_aligned_malloc(_size, ALIGN_DEFAULT);
}

void operator delete(void* _mem)
{
	::_aligned_free(_mem);
}

void operator delete[](void* _mem)
{
	::_aligned_free(_mem);
}

void *operator new(size_t _size, size_t _align)
{
	return ::_aligned_malloc(_size, _align);
}

void *operator new[](size_t _size, size_t _align)
{
	return ::_aligned_malloc(_size, _align);
}

void operator delete(void* _mem, size_t _align)
{
	::_aligned_free(_mem);
}

void operator delete[](void* _mem, size_t _align)
{
	::_aligned_free(_mem);
}

#endif

#if defined(__linux__)

void *operator new(size_t _size)
{
	return ::aligned_alloc(ALIGN_DEFAULT, _size);
}

void *operator new[](size_t _size)
{
	return ::aligned_alloc(ALIGN_DEFAULT, _size);
}

void operator delete(void* _mem)
{
	::free(_mem);
}

void operator delete[](void* _mem)
{
	::free(_mem);
}

void *operator new(size_t _size, size_t _align)
{
	return ::aligned_alloc(_align, _size);
}

void *operator new[](size_t _size, size_t _align)
{
	return ::aligned_alloc(_align, _size);
}



#endif
