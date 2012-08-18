/*
 * Ascent MMORPG Server
 * Copyright (C) 2005-2008 Ascent Team <http://www.ascentemu.com/>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#include "dbcfile.h"
#include <stdio.h>
//#include "../Common.h"
DBCFile::DBCFile()
{
	data = NULL;
	stringTable = NULL;
}

DBCFile::~DBCFile()
{
	if(	data )
		free ( data );
	if ( stringTable )
		free ( stringTable );
}

bool DBCFile::open(const char*fn)
{
	FILE*pf=fopen(fn,"rb");
	if(!pf)return false;

	fread(header,4,1,pf); // Number of records
	assert(header[0]=='W' && header[1]=='D' && header[2]=='B' && header[3] == 'C');
	fread(&recordCount,4,1,pf); // Number of records
	fread(&fieldCount,4,1,pf); // Number of fields
	fread(&recordSize,4,1,pf); // Size of a record
	fread(&stringSize,4,1,pf); // String size

	data = (unsigned char *)malloc (recordSize*recordCount); 
	stringTable = (unsigned char *)malloc ( stringSize ) ;
	fread( data ,recordSize*recordCount,1,pf);
	fread( stringTable , stringSize,1,pf);

	fclose(pf);
	return true;
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

bool DBCFile::DumpBufferToFile(const char*fn)
{
  FILE * pFile;
  pFile = fopen ( fn , "wb" );
  if(!pFile)
	  return false;

  //write header stuff
  fwrite ((const void *)&header , 4 , 1 , pFile );
  fwrite ((const void *)&recordCount , 4 , 1 , pFile );
  fwrite ((const void *)&fieldCount , 4 , 1 , pFile );
  fwrite ((const void *)&recordSize , 4 , 1 , pFile );
  fwrite ((const void *)&stringSize , 4 , 1 , pFile );

  //now the main part is the data
  fwrite (data , recordSize*recordCount , 1 , pFile );
  fwrite (stringTable , stringSize , 1 , pFile );

  //and pull out
  fclose (pFile);
  return true;
}

int DBCFile::AddRecord() //simply add an empty record to the end of the data section
{
	recordCount++;
	if( data )
	{
		data = (unsigned char *)realloc( data, recordCount*recordSize );
		memset( data + (recordCount - 1) * recordSize, 0, recordSize);//make sure no random values get here
	}
	else 
	{
		//the dbc file is not yet opened
		printf(" Error : adding record to an unopened DBC file\n");
		recordCount = 0;
		return 0;
	}

	//seems like an error ocured
	if ( !data )
	{
		printf(" Error : Could not resize DBC data partition\n");
		recordCount = 0;
		return 0;
	}

	return (recordCount - 1);
}

int DBCFile::AddString(const char *new_string) //simply add an empty record to the end of the string section
{

	size_t new_str_len = strlen( new_string ) + 1;

	if( new_str_len == 0 )
		return 0; //we do not add 0 len strings

	if( stringTable )
		stringTable = (unsigned char *)realloc( stringTable, stringSize + new_str_len );
	else 
	{
		//the dbc file is not yet opened
		printf(" Error : adding string to an unopened DBC file\n");
		stringSize = 0;
		return 0;
	}

	//seems like an error ocured
	if ( !stringTable )
	{
		printf(" Error : Could not resize DBC string partition\n");
		stringSize = 0;
		return 0;
	}

	memcpy( stringTable + stringSize, new_string, new_str_len );

	int ret = stringSize;

	stringSize += (int)new_str_len;

	return ret;
}

