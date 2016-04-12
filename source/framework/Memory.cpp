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
