#ifdef WIN32
#include <windows.h>
#endif

#include <string.h>
#include <stdio.h>
#include <math.h>
#include <string>
#include <map>
#include <vector>
#include <set>
#include <assert.h>

#include "adt.h"
#include "mpq_libmpq.h"

//#include <windows.h>
unsigned int iRes=256;
extern uint16*areas;
#define TilesCount 64
#define TileSize 533.33333f
#define _minY (-TilesCount*TileSize/2)
#define _minX (-TilesCount*TileSize/2)

#define _maxY (TilesCount*TileSize/2)
#define _maxX (TilesCount*TileSize/2)

#define CellsPerTile 8
#define _cellSize (TileSize/CellsPerTile)
#define _sizeX (TilesCount*CellsPerTile)
#define _sizeY (TilesCount*CellsPerTile)

bool LoadADT2(char * sfilename, uint32 x,uint32 y, uint32 mapid)
{
	size_t size;
	MPQFile mf(sfilename);
	FILE * f;
	char filename[1024];

	if(mf.isEof ())
	{
		//printf("No such file.\n");	
		return false;
	}

	vector<string> model_names;
	vector<pair<model_instance, uint32> > minstance;
	while (!mf.isEof  ())
	{
		uint32 fourcc;		
		mf.read(&fourcc,4);
		mf.read(&size, 4);

		size_t nextpos = mf.getPos () + size;
		if(fourcc == 0x4d4d4458)			// MMDX
		{
			if(size)
			{
				char * buf = new char[size];
				char * p = buf;
				mf.read(buf, size);

				/* this is a list of models seperated by null bytes. */
				while(p<buf+size)
				{
					/* add the current one */
					string filename(p);
					model_names.push_back(filename);
					/* next file: */
					p = p + strlen(p) + 1;
				}
				delete [] buf;
			}
		}
		if(fourcc == 0x4d444446)			// MDDF
		{
            int num = size/36;
			int id;
			model_instance instance;
			char * p;
			for(int i =0 ;i < num; ++i)
			{
                mf.read(&id,4);
				/* valid? */
				if(id < model_names.size())
				{
					string& modelpath = model_names.at(id);
					assert(modelpath.size());
					mf.read(&instance, sizeof(instance)-4);
					sprintf(filename,"collisiondata\\models\\%s", modelpath.c_str());
                    p = strrchr(filename, '.');
					*(p+1) = 'b';
					*(p+2) = 'm';
					*(p+3) = 'd';

					f = fopen(filename, "rb");
					if(f != NULL)
					{
						instance.fix_coords();
						pair<model_instance,uint32> r(instance, crc32( (const unsigned char*)filename, strlen(filename)));
						minstance.push_back(r);
						fclose(f);
					}
				}
			}
		}
		mf.seek(nextpos);
	}

	if(minstance.size())
	{
		sprintf(filename, "collisiondata\\tiles\\%u-%u_%u.mti", mapid,x,y);
		f = fopen(filename, "wb");
		assert(f);
		
		int c = minstance.size();
		fwrite(&c,4,1,f);
		pair<model_instance,uint32> * v;
		for(int i = 0; i < c; ++i)
		{
			v = &(minstance[i]);
			//int slen = v->second.size();
			//fwrite(&slen,4,1,f);
			//fwrite(v->second.data(), slen, 1, f);
			fwrite(&v->second, 4, 1, f);
			fwrite(&v->first, sizeof(model_instance)-4,1,f);
		}
		fclose(f);
	}
	mf.close ();
	return true;
}

bool LoadADT(char* filename)
{
	size_t size;
	MPQFile mf(filename);

	if(mf.isEof ())
	{
		//printf("No such file.\n");	
		return false;
	}

	while (!mf.isEof  ())
	{
		uint32 fourcc;		
		mf.read(&fourcc,4);
		mf.read(&size, 4);

		size_t nextpos = mf.getPos () + size;
		if(fourcc == 0x4d4d4458)			// MMDX
		{
			if(size)
			{
				char * buf = new char[size];
				char * p = buf;
				mf.read(buf, size);

				/* this is a list of models seperated by null bytes. */
				while(p<buf+size)
				{
                    /* add the current one */
					string filename(p);
					//printf("Model[%u]: %s\n", mcells->model_ids.size(), filename.c_str());
					model m;
					m.Convert(filename.c_str());

					/* next file: */
					p = p + strlen(p) + 1;
				}
				delete [] buf;
			}
		}

		if(fourcc == 0x4d444446)			// MDDF
		{

		}
		mf.seek(nextpos);
	}
	mf.close ();
	return true;
}

bool ConvertADT(uint32 x, uint32 y, FILE * out_file, char* name)
{
	// For some odd reason, this stuff is reversed.. who knows why..
	char mpq_filename[256];
	sprintf(mpq_filename, "World\\Maps\\%s\\%s_%u_%u.adt", name, name, y, x);

	if(!LoadADT(mpq_filename))
		return false;

	return true;
}

bool ConvertADT2(uint32 x, uint32 y, uint32 mapid, char* name)
{
	// For some odd reason, this stuff is reversed.. who knows why..
	char mpq_filename[256];
	sprintf(mpq_filename, "World\\Maps\\%s\\%s_%u_%u.adt", name, name, y, x);

	if(!LoadADT2(mpq_filename, y, x, mapid))
		return false;

	return true;
}