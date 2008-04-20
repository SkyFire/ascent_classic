/****************************************************************************
 *
 * Multiplatform High-Performance Async Network Library
 * Circular Buffer Class
 * Copyright (c) 2007 Burlex
 *
 * This file may be distributed under the terms of the Q Public License
 * as defined by Trolltech ASA of Norway and appearing in the file
 * COPYING included in the packaging of this file.
 *
 * This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
 * WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
 *
 */

#ifndef _NETLIB_CIRCULARBUFFER_H
#define _NETLIB_CIRCUALRBUFFER_H

/**
 * Based on The Bip Buffer concept, 
 * http://www.codeproject.com/KB/IP/bipbuffer.aspx
 */

class CircularBuffer
{
	// allocated whole block pointer
	uint8 * m_buffer;
	uint8 * m_bufferEnd;

	// region A pointer, and size
	uint8 * m_regionAPointer;
	size_t m_regionASize;

	// region size
	uint8 * m_regionBPointer;
	size_t m_regionBSize;

	// pointer magic!
	inline size_t GetAFreeSpace() { return (m_bufferEnd - m_regionAPointer - m_regionASize); }
	inline size_t GetSpaceBeforeA() { return (m_regionAPointer - m_buffer); }
	inline size_t GetSpaceAfterA() { return (m_bufferEnd - m_regionAPointer - m_regionASize); }
	inline size_t GetBFreeSpace() { if(m_regionBPointer == NULL) { return 0; } return (m_regionAPointer - m_regionBPointer - m_regionBSize); }

public:

	/** Constructor
	 */
	CircularBuffer()
	{
		m_buffer = m_bufferEnd = m_regionAPointer = m_regionBPointer = NULL;
		m_regionASize = m_regionBSize = 0;
	}

	/** Destructor
	 */
	~CircularBuffer()
	{
		free(m_buffer);
	}

	/** Read bytes from the buffer
	 * @param destination pointer to destination where bytes will be written
	 * @param bytes number of bytes to read
	 * @return true if there was enough data, false otherwise
	 */
	bool Read(void * destination, size_t bytes)
	{
		// copy as much out of region a
		size_t cnt = bytes;
		size_t aRead = 0, bRead = 0;
		if( (m_regionASize + m_regionBSize) < bytes )
			return false;

		// If we have both region A and region B, always "finish" off region A first, as
		// this will contain the "oldest" data
		if( m_regionASize > 0 )
		{
			aRead = (cnt > m_regionASize) ? m_regionASize : cnt;
			memcpy(destination, m_regionAPointer, aRead);
			m_regionASize -= aRead;
			m_regionAPointer += aRead;
			cnt -= aRead;
		}

		// Data left over? read the data from buffer B
		if( cnt > 0 && m_regionBSize > 0 )
		{
			bRead = (cnt > m_regionBSize) ? m_regionBSize : cnt;
			memcpy((char*)destination + aRead, m_regionBPointer, bRead);
			m_regionBSize -= bRead;
			m_regionBPointer += bRead;
			cnt -= bRead;
		}

		// is buffer A empty? move buffer B to buffer A, to increase future performance
		if( m_regionASize == 0 && m_regionBSize > 0 )
		{
			// push it all to the start of the buffer.
			if( m_regionBPointer != m_buffer )
				memmove(m_buffer, m_regionBPointer, m_regionBSize);

			m_regionAPointer = m_buffer;
			m_regionASize = m_regionBSize;
			m_regionBPointer = NULL;
			m_regionBSize = 0;
		}

		// keep our buffer as optimized as possible.
		// if there is no data in A or B and the write pointer isn't the start, set it
		if( m_regionASize == 0 && m_regionBSize == 0 && m_regionAPointer != m_buffer )
			m_regionAPointer = m_buffer;

		return true;
	}

	void AllocateB()
	{
		printf("[allocating B]\n");
		m_regionBPointer = m_buffer;
	}

	/** Write bytes to the buffer
	 * @param data pointer to the data to be written
	 * @param bytes number of bytes to be written
	 * @return true if was successful, otherwise false
	 */
	bool Write(const void * data, size_t bytes)
	{
		// If buffer B exists, write to it.
		if( m_regionBPointer != NULL )
		{
			if( GetBFreeSpace() < bytes )
				return false;

			memcpy(&m_regionBPointer[m_regionBSize], data, bytes);
			m_regionBSize += bytes;
			return true;
		}

		// Otherwise, write to buffer A, or initialize buffer B depending on which has more space.
		if( GetAFreeSpace() < GetSpaceBeforeA() )
		{
			AllocateB();
			if( GetBFreeSpace() < bytes )
				return false;

			memcpy(&m_regionBPointer[m_regionBSize], data, bytes);
			m_regionBSize += bytes;
			return true;
		}
		else
		{
			if( GetAFreeSpace() < bytes )
				return false;

			memcpy(&m_regionAPointer[m_regionASize], data, bytes);
			m_regionASize += bytes;
			return true;
		}
	}

	/** Returns the number of available bytes left.
	 */
	size_t GetSpace()
	{
		if( m_regionBPointer != NULL )
			return GetBFreeSpace();
		else
		{
			// would allocating buffer B get us more data?
			if( GetAFreeSpace() < GetSpaceBeforeA() )
			{
				AllocateB();
				return GetBFreeSpace();
			}

			// or not?
			return GetAFreeSpace();
		}
	}

	/** Returns the number of bytes currently stored in the buffer.
	 */
	size_t GetSize()
	{
		return m_regionASize + m_regionBSize;
	}

	/** Returns the number of contiguous bytes (that can be pushed out in one operation)
	 */
	size_t GetContiguiousBytes()
	{
		if( m_regionASize )			// A before B
			return m_regionASize;
		else
			return m_regionBSize;
	}

	/** Removes len bytes from the front of the buffer
	 * @param len the number of bytes to "cut"
	 */
	void Remove(size_t len)
	{
		// remove from A first before we remove from b
		size_t cnt = len;
		size_t aRem, bRem;

		// If we have both region A and region B, always "finish" off region A first, as
		// this will contain the "oldest" data
		if( m_regionASize > 0 )
		{
			aRem = (cnt > m_regionASize) ? m_regionASize : cnt;
			m_regionASize -= aRem;
			m_regionAPointer += aRem;
			cnt -= aRem;
		}

		// Data left over? cut the data from buffer B
		if( cnt > 0 && m_regionBSize > 0 )
		{
			bRem = (cnt > m_regionBSize) ? m_regionBSize : cnt;
			m_regionBSize -= bRem;
			m_regionBPointer += bRem;
			cnt -= bRem;
		}

		// is buffer A empty? move buffer B to buffer A, to increase future performance
		if( m_regionASize == 0 && m_regionBSize > 0 )
		{
			// push it all to the start of the buffer.
			if( m_regionBPointer != m_buffer )
				memmove(m_buffer, m_regionBPointer, m_regionBSize);

			m_regionAPointer = m_buffer;
			m_regionASize = m_regionBSize;
			m_regionBPointer = NULL;
			m_regionBSize = 0;
		}
	}

	/** Returns a pointer at the "end" of the buffer, where new data can be written
	 */
	void * GetBuffer()
	{
		if( m_regionBPointer != NULL )
			return m_regionBPointer + m_regionBSize;
		else
			return m_regionAPointer + m_regionASize;
	}

	/** Allocate the buffer with room for size bytes
	 * @param size the number of bytes to allocate
	 */
	void Allocate(size_t size)
	{
		m_buffer = (uint8*)malloc(size);
		m_bufferEnd = m_buffer + size;
		m_regionAPointer = m_buffer;		// reset A to the start
	}

	/** Increments the "writen" pointer forward len bytes
	 * @param len number of bytes to step
	 */
	void IncrementWritten(size_t len)			// known as "commit"
	{
		if( m_regionBPointer != NULL )
			m_regionBSize += len;
		else
			m_regionASize += len;
	}

	/** Returns a pointer at the "beginning" of the buffer, where data can be pulled from
	 */
	void * GetBufferStart()
	{
		if( m_regionASize > 0 )
			return m_regionAPointer;
		else
			return m_regionBPointer;
			
	}
};

#endif		// _NETLIB_CIRCULARBUFFER_H

