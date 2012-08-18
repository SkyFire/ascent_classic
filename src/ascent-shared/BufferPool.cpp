#include "Common.h"
#include "Log.h"
#include "BufferPool.h"

size_t BufferPool::BufferBucket::buffer_sizes[BUFFER_BUCKET_COUNT] = {
	20,			// 20 bytes
	50,			// 50 bytes
	100,		// 100 bytes
	200,		// 200 bytes
	500,		// 500 bytes
	1000,		// 1 kbyte
	5000,		// 5 kbyte
	10000,		// 10 kbyte (shouldn't be used much)
};

BufferPool g_bufferPool;
