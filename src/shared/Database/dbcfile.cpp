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

#include "dbcfile.h"
#include <stdio.h>
#include "../Common.h"
DBCFile::DBCFile()
{
	
}

bool DBCFile::open(const char*fn)
{
	FILE*pf=fopen(fn,"rb");
	if(!pf)return false;
	char header[4];

	fread(header,4,1,pf); // Number of records
	assert(header[0]=='W' && header[1]=='D' && header[2]=='B' && header[3] == 'C');
	fread(&recordCount,4,1,pf); // Number of records
	fread(&fieldCount,4,1,pf); // Number of fields
	fread(&recordSize,4,1,pf); // Size of a record
	fread(&stringSize,4,1,pf); // String size

#ifdef USING_BIG_ENDIAN
	recordCount = swap32(recordCount);
	fieldCount = swap32(fieldCount);
	recordSize = swap32(recordSize);
	stringSize = swap32(stringSize);
#endif
	
	data = new unsigned char[recordSize*recordCount+stringSize];
	stringTable = data + recordSize*recordCount;
	fread(data,recordSize*recordCount+stringSize,1,pf);

	/* swap all the rows */
#ifdef USING_BIG_ENDIAN
	/* burlex: this is a real hack. it'll mess up floats. i'm gonna rewrite the dbc interface soon :P */
	uint32 * tbl = (uint32*)data;
	for(int i = 0; i < (fieldCount * recordCount); ++i)
		tbl[i] = swap32((uint32)tbl[i]);
#endif
	fclose(pf);
	return true;
}

DBCFile::~DBCFile()
{
	delete [] data;
}

DBCFile::Record DBCFile::getRecord(size_t id)
{
	assert(data);
	return Record(*this, data + id*recordSize);
}

DBCFile::Iterator DBCFile::begin()
{
	assert(data);
	return Iterator(*this, data);
}
DBCFile::Iterator DBCFile::end()
{
	assert(data);
	return Iterator(*this, stringTable);
}

