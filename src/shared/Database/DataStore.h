/*
 * Ascent MMORPG Server
 * Copyright (C) 2005-2007 Ascent Team <http://www.ascentemu.com/>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#ifndef __DATASTORE_H
#define __DATASTORE_H

#include "Common.h"
#include "Singleton.h"
#include "DBC.h"

template <class T>
class SERVER_DECL DataStore : public Singleton<DataStore<T> >
{
protected:
	DBC d;
public:
	DataStore(const char* filename)
	{
		d.Load(filename);
		if((sizeof(T) / 4) != d.GetCols())
			printf("[DBC] Warning: Incompatible structures. (%s, %u / %u)\n", filename, sizeof(T) / 4, d.GetCols());
	}
	virtual ~DataStore() {}

	virtual T *LookupEntry(const uint32 row)
	{
		if(!d.IsLoaded() || (uint32)d.GetRows() < row) return NULL;
		return (T*)d.GetRow(row);
	}
	const char* LookupString(const uint32 offset)
	{
		if(!d.IsLoaded() || (uint32)d.GetDBSize() < offset) return NULL;
		return d.LookupString(offset);
	}
	virtual T* LookupEntryForced(const uint32 row)
	{
		return DataStore<T>::LookupEntry(row);
	}
	uint32 GetNumRows()
	{
		return d.GetRows();
	}
};

template <class T>
class SERVER_DECL FastIndexedDataStore : public DataStore<T>
{
	T ** dataIndex;
	uint32 high;
	uint32 first;
public:
	FastIndexedDataStore(const char * filename) : DataStore<T>(filename)
	{
		std::map<uint32, void*> itemp;
		high = 0;
		first = 0;
		T * data;
		
		uint32 index;

		for(uint32 row=0;row<(uint32)DataStore<T>::d.GetRows();row++)
		{
			data = (T*)DataStore<T>::d.GetRow(row);
			index = *(int*)data;
			
			if(!first)
				first = index;

			itemp[index] = (void*)data;
			if(index > high)
				high = index;
		}

		++high;

		// dump to index array
		dataIndex = new T*[high];
		memset(dataIndex, 0, sizeof(uint32) * high);
	std::map<uint32, void*>::iterator itr = itemp.begin();
		for(; itr != itemp.end(); ++itr)
			dataIndex[itr->first] = (T*)itr->second;
	}

	~FastIndexedDataStore()
	{
		delete [] dataIndex;
	}

	T * LookupEntry(const uint32 row)
	{
		if(row >= high || dataIndex[row] == 0) return dataIndex[first];
		return dataIndex[row];
	}

	T * LookupEntryForced(const uint32 row)
	{
		if(row >= high) return 0;
		return dataIndex[row];
	}

	T *LookupRow(const uint32 row)
	{
		return (T*)DataStore<T>::d.GetRow(row);
	}

	void SetRow(uint32 row, T* data)
	{
		ASSERT(dataIndex[row]==0 && row<high);
		dataIndex[row]=data;
	}
};

template <class T>
class SERVER_DECL IndexedDataStore : public DataStore<T>
{
protected:
	std::map<uint32,uint32> indexMap;
public:
	IndexedDataStore(const char* filename) : DataStore<T>(filename)
	{
		for(uint32 row=0;row<(uint32)DataStore<T>::d.GetRows();row++)
			indexMap[*(int*)DataStore<T>::d.GetRow(row)] = row;
	}
	~IndexedDataStore() { indexMap.clear(); }

	virtual T *LookupEntry(const uint32 row)
	{
		if(!DataStore<T>::d.IsLoaded()) return NULL;
		return (T*)DataStore<T>::d.GetRow(indexMap[row]);
	}

	virtual T *LookupRow(const uint32 row)
	{
		if(!DataStore<T>::d.IsLoaded()) return NULL;
		return (T*)DataStore<T>::d.GetRow(row);
	}
};

#define defineDBCStore(name,structname) \
class SERVER_DECL name : public DataStore<structname> \
{ \
public: \
	name(const char* filename); \
	~name(); \
}

#define implementDBCStore(name,structname) \
initialiseSingleton(name); \
initialiseSingleton(DataStore< structname >); \
name::name(const char* filename) : DataStore<structname>(filename) {} \
name::~name() {} \

#define defineIndexedDBCStore(name,structname) \
class SERVER_DECL name : public FastIndexedDataStore<structname> \
{ \
public: \
	name(const char* filename); \
	~name(); \
}

#define implementIndexedDBCStore(name,structname) \
initialiseSingleton(name); \
initialiseSingleton(DataStore< structname >); \
name::name(const char* filename) : FastIndexedDataStore<structname>(filename) {} \
name::~name() {} 


#endif
