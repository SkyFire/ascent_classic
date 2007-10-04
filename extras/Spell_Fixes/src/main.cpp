/*

Purpuse : make modifications in dbc file for ascent project to set values that are missing for spells in able to work

project status : not finished yet..not tested

*/

#include <stdio.h>
#include <tchar.h>
#include "defines.h"
#include <stdlib.h>
#include <assert.h>

//#include "../../../src/shared/Database/dbcfile.h"
#include "dbcfile.h"

//make this after the main
void do_fixes(TCHAR *inf);
void dump_as_sql(TCHAR *inf);

void print_usage()
{
	printf("Usage: -doall spell_fix.exe inf=input_spell.dbc\n");
	printf("parameters: -h output this help message\n");
	printf("parameters: -doall start making custom fixes\n");
	printf("parameters: -conv dump DBC as sql\n");
	printf("parameters: inf= specify the input filename(no spaces)\n");
}

void main(int argc, TCHAR* argv[])
{
	int dotask=0;
	TCHAR file_name[300];//store the input filename

	//make sure it is morron proof. Ever met a guy clicking all the next buttons in an install kit without reading 1 line ? :P
	if(argc<=1)
	{
		printf("For safety mesures this program cannot be run without parameters. Type -h for help");
		print_usage();
		return;
	}
	//maybe right now we do not have a lot of options but what the hack, let's make it fancy :)
	for (int i=1; i<argc; i++)
	{
		int arglen=(int)strlen(argv[i]);
		if (strnicmp(argv[i],"-h",2)==0) print_usage();
		if (strnicmp(argv[i],"-doall",6)==0) dotask=1;
		if (strnicmp(argv[i],"-conv",5)==0) dotask=2;
		if (strnicmp(argv[i],"inf=",4)==0) strcpy(file_name,argv[i]+4);
		
	}

	switch(dotask)
	{
		case 1:		do_fixes(file_name);	break;
		case 2:		dump_as_sql(file_name);	break;
		default:							break;
	}
}

__int64 getfilesize(TCHAR *infname)
{
	FILE *inf=fopen(infname,"r");
	if(!inf)
	{
		printf("error ocured while opening file\n");
		exit(1);
	}
	fpos_t len;
	fseek(inf,0,SEEK_END);
	fgetpos(inf,&len);
	fclose(inf);
	return len;
}

void do_fixes(TCHAR *inf)
{
/*	char *buffer;
	unsigned int buffer_len=(int)getfilesize(inf);
	buffer = (char *)malloc(buffer_len);
	if(!buffer)
	{
		printf("error while alocating buffer with size %u \n",buffer_len);
		exit(1);
	}*/
	DBCFile dbc;
	dbc.open(inf);

	if(!dbc.getFieldCount())
	{
		printf("error, could not open dbc file\n");
		exit(1);
	}
	else printf("Opened DBC with %u fields and %u rows\n",(int)dbc.getFieldCount(),(int)dbc.getRecordCount());

	uint32 cnt = (uint32)dbc.getRecordCount();
/*	uint32 effect;
	uint32 All_Seal_Groups_Combined=0;
	for(uint32 x=0; x < cnt; x++)
	{
		uint32 result = 0;
		// SpellID
		uint32 spellid = dbc.getRecord(x).getUInt(0);
		// Description field
		char* desc = (char*)dbc.getRecord(x).getString(157); 
		const char* ranktext = dbc.getRecord(x).getString(140);
		const char* nametext = dbc.getRecord(x).getString(123);
	}*/
}

void dump_as_sql(TCHAR *inf)
{
	DBCFile dbc;
	dbc.open(inf);

	if(!dbc.getFieldCount())
	{
		printf("error, could not open dbc file\n");
		exit(1);
	}
	else printf("Opened DBC with %u fields and %u rows\n",(int)dbc.getFieldCount(),(int)dbc.getRecordCount());

	uint32 cnt = (uint32)dbc.getRecordCount();
}