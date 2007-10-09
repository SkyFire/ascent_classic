/*

Purpuse : make modifications in dbc file for ascent project to set values that are missing for spells in able to work

project status : not finished yet..not tested

*/

#include <stdio.h>
#include <tchar.h>
#include "defines.h"
#include <stdlib.h>
#include <assert.h>
#include <conio.h>

#include "../../../src/shared/Database/dbcfile.h"

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
	getch();
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
	printf("will start dumping data into sql file (we will drop not required fields!)\n");

	FILE *fsql=fopen("dbc_spell.sql","w");

	//drop table if already exist
	fprintf(fsql,"%s","DROP TABLE IF EXISTS `dbc_spell`;\n");

	//create the table
	fprintf(fsql,"%s","CREATE TABLE dbc_spell (\n");

	for(int i=0;i<SPELL_COLUMN_COUNT;i++)
		if(sql_translation_table[i][0][0]=='u')
			fprintf(fsql,"\t `%s` INT (11) UNSIGNED DEFAULT '0' NOT NULL,\n",sql_translation_table[i][1]);
		else if(sql_translation_table[i][0][0]=='i')
			fprintf(fsql,"\t `%s` INT (11) DEFAULT '0' NOT NULL,\n",sql_translation_table[i][1]);
		else if(sql_translation_table[i][0][0]=='f')
			fprintf(fsql,"\t `%s` FLOAT DEFAULT '0' NOT NULL,\n",sql_translation_table[i][1]);
		else if(sql_translation_table[i][0][0]=='s')
			fprintf(fsql,"\t `%s` VARCHAR(60),\n",sql_translation_table[i][1]);

	fprintf(fsql,"%s","PRIMARY KEY(id), UNIQUE(id), INDEX(id));\n");

	fprintf(fsql,"\n\n");
	//start dumping the data from the DBC

	char tstr[2000];
	for(unsigned int j=0;j<dbc.getRecordCount();j++)
	{
		//we start a new insert block
		if((j % SQL_INSERTS_PER_QUERY) == 0)
		{
			fprintf(fsql,"%s","INSERT INTO dbc_spell (");
			for(int i=0;i<SPELL_COLUMN_COUNT-1;i++)
					fprintf(fsql,"`%s`,",sql_translation_table[i][1]);
			fprintf(fsql,"`%s`) values \n",sql_translation_table[SPELL_COLUMN_COUNT-1][1]);
			fprintf(fsql," (");
		}
		else
			fprintf(fsql,",(");
		for(int i=0;i<SPELL_COLUMN_COUNT;i++)
		{
			if(i!=0)
				fprintf(fsql,",");
			if(sql_translation_table[i][0][0]=='u')
				fprintf(fsql,"%u",dbc.getRecord(j).getUInt(i));
			else if(sql_translation_table[i][0][0]=='i')
				fprintf(fsql,"%d",dbc.getRecord(j).getInt(i));
			else if(sql_translation_table[i][0][0]=='f')
				fprintf(fsql,"%f",dbc.getRecord(j).getFloat(i));
			else if(sql_translation_table[i][0][0]=='s')
			{
				const char *dstr=dbc.getRecord(j).getString(i);
				int otherindex=0;
				for(unsigned int k=0;k<=strlen(dstr);k++)
					if(dstr[k]=='\'' || dstr[k]=='"')
					{
						tstr[otherindex++] = '\\';
						tstr[otherindex++] = dstr[k];
					}
					else
						tstr[otherindex++] = dstr[k];
				fprintf(fsql,"\"%s\"",tstr);
			}
		}
		//we need to end an insert block
		if(((j+1) % SQL_INSERTS_PER_QUERY)==0)
			fprintf(fsql,");\n");
		else fprintf(fsql,")\n");
	}
	fprintf(fsql,";");

	fprintf(fsql,"\n\n");
	//drop stuff that we do not need visually
	for(int i=0;i<SPELL_COLUMN_COUNT;i++)
		if(sql_translation_table[i][2][0]=='1')
			fprintf(fsql,"ALTER TABLE dbc_spell DROP `%s`;\n",sql_translation_table[i][1]);

	fclose(fsql);
}