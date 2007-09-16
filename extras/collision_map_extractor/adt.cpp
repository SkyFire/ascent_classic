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

#define SMALL_MAPFILES
#define PACK_MAPS
#define MAP_RESOLUTION 256
#define MAP_WALKABLE_RESOLUTION 128

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

//#pragma pack(push, 1)

typedef struct
{
    uint16    AreaID[16/CellsPerTile][16/CellsPerTile];
    uint8    LiquidType[16/CellsPerTile][16/CellsPerTile];
    float    LiquidLevel[16/CellsPerTile][16/CellsPerTile];
    float    Z[MAP_RESOLUTION/CellsPerTile][MAP_RESOLUTION/CellsPerTile];
	uint8    Walkable[MAP_WALKABLE_RESOLUTION/CellsPerTile][MAP_WALKABLE_RESOLUTION/CellsPerTile];
}MapCellInfo; 


///This is temp. method.
typedef struct
{
    uint16    AreaID[16][16];
    uint8    LiquidType[16][16];
    float    LiquidLevel[16][16];
    float    Z[MAP_RESOLUTION][MAP_RESOLUTION];
}MapTileInfo; 

typedef struct
{
    float z[256][256];
}zstore;
//#pragma pack(pop)
vec wmoc;

Cell * cell;
uint32 wmo_count;
mcell *mcells;
zstore * cz;

mcell * mcell_cache[64][64];
zstore * cell_cache[64][64];

/* Converts a global x co-ordinate into a INTERNAL cell x co-ordinate.
Parameter 1: global x co-ordinate.
Parameter 2: the cell x co-ordinate.
Returns the internal x co-ordinate.
*/
inline float ConvertInternalXCoordinate(float x, uint32 cellx)
{
	float X = (_maxX - x);
	X -= (cellx * _cellSize);
	return X;
}

/* Converts a global y co-ordinate into a INTERNAL cell y co-ordinate.
Parameter 1: global y co-ordinate.
Parameter 2: the cell y co-ordinate.
Returns the internal y co-ordinate.
*/
inline float ConvertInternalYCoordinate(float y, uint32 celly)
{
	float Y = (_maxY - y);
	Y -= (celly * _cellSize);
	return Y;
}
/* Converts a global x co-ordinate into a cell x co-ordinate.
Parameter 1: global x co-ordinate.
Returns the cell x co-ordinate.
*/
inline uint32 ConvertGlobalXCoordinate(float x)
{
	float a1 = _maxX - x;
	float a2 = a1 / _cellSize;
	return uint32((_maxX-x)/_cellSize);
}

/* Converts a global y co-ordinate into a cell y co-ordinate.
Parameter 1: global y co-ordinate.
Returns the cell y co-ordinate.
*/
inline uint32 ConvertGlobalYCoordinate(float y)
{
	return uint32((_maxY-y)/_cellSize);
}


void reset()
{
    for(uint32 i = 0; i < 64; ++i)
    {
        for(uint32 j = 0; j < 64; ++j)
        {
            mcell_cache[i][j] = 0;
            cell_cache[i][j] = 0;
        }
    }
}
void CleanCache()
{
    for(uint32 i = 0; i < 64; ++i)
    {
        for(uint32 j = 0; j < 64; ++j)
        {
            if(cell_cache[i][j] != 0)
            {
                delete cell_cache[i][j];
                cell_cache[i][j] = 0;
            }

            if(mcell_cache[i][j] != 0)
            {
                delete mcell_cache[i][j];
                mcell_cache[i][j] = 0;
            }
        }
    }
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
	mcells=new mcell;

	wmoc.x =65.0f*TILESIZE;
	wmoc.z =65.0f*TILESIZE;

	size_t mcnk_offsets[256], mcnk_sizes[256];

	wmo_count=0;
	bool found=false;
	//uint32 fs=mf.getSize ()-3;
	//while (mf.getPos ()<fs)
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
				char * end;
				int c = 0;
				mf.read(buf, size);

				/* this is a list of models seperated by null bytes. */
				while(p)
				{
					end = p;
					while(*end != '\0' && c<size)
					{
						++c;
						++end;
					}
					if(c>=size || *end != '\0')		/* end of the list */
						break;

                    /* add the current one */
					string filename(p);
					//printf("Model[%u]: %s\n", mcells->model_ids.size(), filename.c_str());
					mcells->model_ids.push_back(filename);

					/* next file: */
					p = end + 1;
				}
			}
		}

		if(fourcc == 0x4d444446)			// MDDF
		{
			/* model instances */
			int model_count = size/36;
			for(int i = 0; i < model_count; ++i)
			{
				int model_id;
				model_instance mi;

				mf.read(&model_id, 4);
				mf.read(&mi, sizeof(model_instance) - 4);
				mi.modelid = model_id;
				mi.fix_coords();
				mcells->model_instances.push_back(mi);
				//printf("Model instance : %u at %f %f %f\n", model_id, mi.posx, mi.posy, mi.posz);
                	
			}
		}

		if(fourcc==0x4d43494e) //MCIN
		{
		//	printf("Found chunks info\n");
			// mapchunk offsets/sizes
			for (int i=0; i<256; i++)
			{
				mf.read(&mcnk_offsets[i],4);
				mf.read(&mcnk_sizes[i],4);
				mf.seekRelative(8);
			}
		//break;
		}
		else 
			if(fourcc==0x4d4f4446) //MODF
			{
			
			/*	if(size)
				{	
					//printf("\nwmo count %d\n",size/64);
					wmo_count =size/64;
					for (int i=0; i<wmo_count; i++)
					{
						int id;
						mf.read(&id, 4);
						WMO *wmo = (WMO*)wmomanager.items[wmomanager.get(wmos[id])];
						WMOInstance inst(wmo, mf);
						wmois.push_back(inst);
					}

				}*/
			
			}else 
			if(fourcc==0x4d574d4f)//mwmo
			{
				/*if (size)
				{
					char *buf = new char[size];
					mf.read(buf, size);
					char *p=buf;
					while (p<buf+size)
					{
					std::string path(p);
						p+=strlen(p)+1;
						fixname(path);
						
						wmomanager.add(path);
						wmos.push_back(path);
					}
					delete[] buf;
			    }*/
			}
		//	else mf.seekRelative(-3);

		mf.seek(nextpos);
	}
		//printf("Loading chunks info\n");
		// read individual map chunks
		for (int j=0; j<16; j++) 
			for (int i=0; i<16; i++) 
			{

				mf.seek((int)mcnk_offsets[j*16+i]);
				LoadMapChunk (mf,&(mcells->ch [i][j]));

			}

		/*	for(uint32 t=0;t<wmo_count ;t++)
			{
				wmois[t].draw ();

			}*/

	printf("%s has %u models and %u instances\n", filename, mcells->model_ids.size(), mcells->model_instances.size());

	mf.close ();
	return true;
}



struct MapChunkHeader {
	uint32 flags;
	uint32 ix;
	uint32 iy;
	uint32 nLayers;
	uint32 nDoodadRefs;
	uint32 ofsHeight;
	uint32 ofsNormal;
	uint32 ofsLayer;
	uint32 ofsRefs;
	uint32 ofsAlpha;
	uint32 sizeAlpha;
	uint32 ofsShadow;
	uint32 sizeShadow;
	uint32 areaid;
	uint32 nMapObjRefs;
	uint32 holes;
	uint16 s1;
	uint16 s2;
	uint32 d1;
	uint32 d2;
	uint32 d3;
	uint32 predTex;
	uint32 nEffectDoodad;
	uint32 ofsSndEmitters;
	uint32 nSndEmitters;
	uint32 ofsLiquid;
	uint32 sizeLiquid;
	float  zpos;
	float  xpos;
	float  ypos;
	uint32 textureId;
	uint32 props;
	uint32 effectId;
};

inline
void LoadMapChunk(MPQFile & mf, chunk*_chunk)
{
	
	float h;
	uint32 fourcc;
	uint32 size;
	MapChunkHeader header;
	
	mf.seekRelative(4);
	mf.read(&size, 4);

	size_t lastpos = mf.getPos() + size;
	mf.read(&header, 0x80);
	_chunk->area_id =header.areaid ;
	_chunk->flag =0;

	float xbase = header.xpos;
	float ybase = header.ypos;
	float zbase = header.zpos;
	zbase = TILESIZE*32-zbase;
	xbase = TILESIZE*32-xbase;
	if(wmoc.x >xbase)wmoc.x =xbase;
	if(wmoc.z >zbase)wmoc.z =zbase;
	int chunkflags = header.flags;
	float zmin=999999999.0f;
	float zmax=-999999999.0f;
	//must be there, bl!zz uses some crazy format
	int nTextures;
	while (mf.getPos ()<lastpos)
	{
		mf.read(&fourcc,4);
		mf.read(&size, 4);
	//	if(size!=580)
	//	printf("\n sz=%d",size);
		size_t nextpos = mf.getPos()  + size;

		if(fourcc==0x4d435654) //MCVT
		 {
			for (int j=0; j<17; j++)
			for (int i=0; i<((j%2)?8:9); i++) 
				{
					mf.read(&h,4);
					float z=h+ybase;
					if (j%2)
						_chunk->v8[i][j/2] = z;
					else
						_chunk->v9[i][j/2] = z;

					if(z>zmax)zmax=z;
				//	if(z<zmin)zmin=z;
				}
		}
		 else
			if(fourcc==0x4d434e52) //MCNR
			{
			nextpos = mf.getPos() + 0x1C0; // size fix

			}
		else
			/*if(fourcc==0x4d434c51) //MCLQ
			{
				// liquid / water level
			//	bool haswater;
				char fcc1[5];
				mf.read(fcc1,4);
				flipcc(fcc1);
				fcc1[4]=0;

				if (!strcmp(fcc1,"MCSE"))
                {
                    for(int i=0;i<9;i++)
                        for(int j=0;j<9;j++)
                            _chunk->waterlevel[i][j]=-999999; // no liquid/water
                }
                else
				{
                    float maxheight;
                    mf.read(&maxheight, 4);
					
                    for(int j=0;j<9;j++)
                        for(int i=0;i<9;i++)
                        {
                            mf.read(&h, 4);
                            mf.read(&h, 4);
                            if(h > maxheight)
                                _chunk->waterlevel[i][j]=-999999;
                            else
                                _chunk->waterlevel[i][j]=h;
                        }

                    if(chunkflags & 4 || chunkflags & 8)
						_chunk->flag |=1;
					if(chunkflags & 16)
						_chunk->flag |=2;



				}
				

			break;*/
            if(fourcc==0x4d434c51)
            {
                // liquid / water level
                //	bool haswater;
                char fcc1[5];
                mf.read(fcc1,4);
                flipcc(fcc1);
                fcc1[4]=0;
                if (strcmp(fcc1,"MCSE"))

                    //	if(size)





                {
                    //chunkflag


                    mf.seekRelative(-4);
                    mf.read(&_chunk->waterlevel,4);









                    /*if(chunkflags & 4 || chunkflags & 8)
                        _chunk->flag |=1;
                    if(chunkflags & 16)
                        _chunk->flag |=2;*/
                    if(!_chunk->flag)
                        _chunk->flag = chunkflags;
                    else
                        printf("%02X", _chunk->flag);



                }


                break;
			}else if (fourcc==0x4d434c59)
			{
			// texture info
			nTextures = (int)size;
			}else if (fourcc==0x4d43414c)
			{
			
			if (nTextures<=0) 
			continue;
			}
		


		mf.seek(nextpos);
	}
	
	
	printf("");
}



double solve (vec *v,vec *p)
{
double a = v[0].y *(v[1].z - v[2].z) + v[1].y *(v[2].z - v[0].z) + v[2].y *(v[0].z - v[1].z);
double b = v[0].z *(v[1].x - v[2].x) + v[1].z *(v[2].x - v[0].x) + v[2].z *(v[0].x - v[1].x);
double c = v[0].x *(v[1].y - v[2].y) + v[1].x *(v[2].y - v[0].y) + v[2].x *(v[0].y - v[1].y);
double d = v[0].x *(v[1].y*v[2].z - v[2].y*v[1].z) + v[1].x* (v[2].y*v[0].z - v[0].y*v[2].z) + v[2].x* (v[0].y*v[1].z - v[1].y*v[0].z);
//-d

//plane equation ax+by+cz+d=0
return ((a*p->x+c*p->z-d)/b);
}


inline
double GetZ(double x,double z)
{

	vec v[3];
	vec p;
	
	bool inWMO=false;
	
	if(!inWMO)
	{
		//find out quadrant
		int xc=(int)(x/UNITSIZE);
		int zc=(int)(z/UNITSIZE);
		if(xc>127)xc=127;
		if(zc>127)zc=127;

		double lx=x-xc*UNITSIZE;
		double lz=z-zc*UNITSIZE;
		p.x=lx;
		p.z=lz;
	

		v[0].x=UNITSIZE/2;
		v[0].y =cell->v8[xc][zc];
		v[0].z=UNITSIZE/2;

	
		if(lx>lz)
		{
				v[1].x=UNITSIZE;
				v[1].y =cell->v9[xc+1][zc];
				v[1].z=0;
		}else
		{
				v[1].x=0.0;
				v[1].y =cell->v9[xc][zc+1];
				v[1].z=UNITSIZE;
		}

		if(lz>UNITSIZE-lx)
		{
				v[2].x=UNITSIZE;
				v[2].y =cell->v9[xc+1][zc+1];
				v[2].z=UNITSIZE;

		}else
		{
				v[2].x=0;
				v[2].y=cell->v9[xc][zc];
				v[2].z=0;
		}

		return -solve(v,&p);
	}
	
}

/*inline
void TransformWaterData()
{
    cell= new Cell;

    for(int x=0;x<128;x++)
        for(int y=0;y<128;y++)
            cell->v9[x][y] = mcells->ch[x/8][y/8].waterlevel[x%8][y%8];

    //and the last 1
    cell->v9[128][128] = mcells->ch[15][15].waterlevel[8][8];
}*/

inline
void TransformData()
{
	cell= new Cell;

	for(int x=0;x<128;x++)
	{
		for(int y=0;y<128;y++)
		{
            cell->v8[x][y] = (float)mcells->ch[x/8][y/8].v8[x%8][y%8];
            cell->v9[x][y] = (float)mcells->ch[x/8][y/8].v9[x%8][y%8];
		}

		//extra 1 point on bounds
		cell->v9[x][128] = (float)mcells->ch[x/8][15].v9[x%8][8];
		//x==y
		cell->v9[128][x] = (float)mcells->ch[15][x/8].v9[8][x%8];

	}


	//and the last 1
	cell->v9[128][128] = (float)mcells->ch[15][15].v9[8][8];
}

bool ConvertADT(uint32 x, uint32 y, FILE * out_file, char* name)
{
    // Figure out what rock we're under :P
    uint32 tile_x = x / CellsPerTile;
    uint32 tile_y = y / CellsPerTile;
	float xbase = tile_x * TILESIZE;
	float ybase = tile_y * TILESIZE;

    // For some odd reason, this stuff is reversed.. who knows why..
    char mpq_filename[256];
    sprintf(mpq_filename, "World\\Maps\\%s\\%s_%u_%u.adt", name, name, tile_y, tile_x);
    
    // See if we have it cached first.
    if(mcell_cache[tile_y][tile_x] == 0 && cell_cache[tile_y][tile_x] == 0)
    {
        if(!LoadADT(mpq_filename))
            return false;

        TransformData();
        mcell_cache[tile_y][tile_x] = mcells;

        // Cache z information.
        cz = new zstore;
        for(uint32 cx = 0; cx < MAP_RESOLUTION; ++cx)
        {
            for(uint32 cy = 0; cy < MAP_RESOLUTION; ++cy)
            {
                float cfz=(float)GetZ(
                    (((double)(cy))*TILESIZE)/((double)(iRes-1)),
                    (((double)(cx))*TILESIZE)/((double)(iRes-1)));
                cz->z[cx][cy] = cfz;
            }
        }

        cell_cache[tile_y][tile_x] = cz;
        delete cell;
    }
    else
    {
        mcells = mcell_cache[tile_y][tile_x];
        cz = cell_cache[tile_y][tile_x];
    }

    // This is our output data.
    MapCellInfo out;

    // Write out the area/water information.
    for(uint32 xc=(x%CellsPerTile)*16/CellsPerTile;xc<(x%CellsPerTile)*16/CellsPerTile+16/CellsPerTile;xc++)
    {
        for(uint32 yc=(y%CellsPerTile)*16/CellsPerTile;yc<(y%CellsPerTile)*16/CellsPerTile+16/CellsPerTile;yc++)
        {
            uint32 lx=xc%(16/CellsPerTile);
            uint32 ly=yc%(16/CellsPerTile);
            out.AreaID[lx][ly] = mcells->ch[yc][xc].area_id;
            out.LiquidType[lx][ly] = mcells->ch[yc][xc].flag;
            out.LiquidLevel[ly][ly] = mcells->ch[yc][xc].waterlevel;
        }
    }

    // Convert it into antrix cell format.
    for(uint32 cx=(x%CellsPerTile)*MAP_RESOLUTION/CellsPerTile;cx<(x%CellsPerTile)*MAP_RESOLUTION/CellsPerTile+MAP_RESOLUTION/CellsPerTile;cx++)
    {
        for(uint32 cy=(y%CellsPerTile)*MAP_RESOLUTION/CellsPerTile;cy<(y%CellsPerTile)*MAP_RESOLUTION/CellsPerTile+MAP_RESOLUTION/CellsPerTile;cy++)
        {
            uint32 lx=cx%(MAP_RESOLUTION/CellsPerTile);
            uint32 ly=cy%(MAP_RESOLUTION/CellsPerTile);
            out.Z[lx][ly] = cz->z[cx][cy];
        }
    }

	/* null out the walkable space */
	for(uint32 xc=(x%CellsPerTile)*MAP_WALKABLE_RESOLUTION/CellsPerTile;xc<(x%CellsPerTile)*MAP_WALKABLE_RESOLUTION/CellsPerTile+MAP_WALKABLE_RESOLUTION/CellsPerTile;xc++)
	{
		for(uint32 yc=(y%CellsPerTile)*MAP_WALKABLE_RESOLUTION/CellsPerTile;yc<(y%CellsPerTile)*MAP_WALKABLE_RESOLUTION/CellsPerTile+MAP_WALKABLE_RESOLUTION/CellsPerTile;yc++)
		{
			uint32 lx=xc%(MAP_WALKABLE_RESOLUTION/CellsPerTile);
			uint32 ly=yc%(MAP_WALKABLE_RESOLUTION/CellsPerTile);
			
		}
	}

	if(mcells->model_instances.size())
	{
		vector<model*> loaded_models;
		for(int j = 0; j < mcells->model_instances.size(); ++j)
			loaded_models.push_back(NULL);

		model_instance * i;
		model * m;
		for(vector<model_instance>::iterator itr = mcells->model_instances.begin(); itr != mcells->model_instances.end(); ++itr)
		{
			i = &(*itr);
			if(loaded_models[i->modelid] == NULL)
			{
				m = new model;
				if(!m->Load(mcells->model_ids[i->modelid].c_str()))
				{
					delete m;
					continue;
				}
				else
				{
					loaded_models[i->modelid] = m;
					/*i->posx -= xbase;
					i->posy -= ybase;*/
				}
			}
			else
				m=loaded_models[i->modelid];

			/* mark walkable area in the cell based on the bounding box */
			float sx = m->GetSizeX();
			float sy = m->GetSizeY();
			sx /= 2;
			sy /= 2;

			float min_x = i->posx - sx;
			float min_y = i->posy - sy;
			float max_x = i->posx + sx;
			float max_y = i->posy + sy;
#define check_x_coord(p) if(p < _minX) { p = _minX; } if(p > _maxX) { p = _maxX; }
#define check_y_coord(p) if(p < _minY) { p = _minY; } if(p > _maxY) { p = _maxY; }

			check_x_coord(min_x);
			check_x_coord(max_x);
			check_y_coord(min_y);
			check_y_coord(max_y);

			if(min_x == max_x || min_y == max_y)
			{
				printf("Instance %s at\n   %f %f %f is out of range.\n", mcells->model_ids[i->modelid].c_str(),i->posx, i->posy, i->posz);
				continue;
			}

			uint32 MIN_CELL_X = ConvertGlobalXCoordinate(min_x)/8;
			uint32 MAX_CELL_Y = ConvertGlobalYCoordinate(max_y)/8;
			uint32 MAX_CELL_X = ConvertGlobalXCoordinate(max_x)/8;
			uint32 MIN_CELL_Y = ConvertGlobalYCoordinate(min_y)/8;
			if(MIN_CELL_X != tile_x || MAX_CELL_X != tile_x)
			{
				printf("Instance %s at\n   %f %f %f is on incorrect tile. {X}\n", mcells->model_ids[i->modelid].c_str(),i->posx, i->posy, i->posz);
				continue;
			}

			if(MIN_CELL_Y != tile_y || MAX_CELL_Y != tile_y)
			{
				printf("Instance %s at\n   %f %f %f is on incorrect tile. {Y}\n", mcells->model_ids[i->modelid].c_str(),i->posx, i->posy, i->posz);
				continue;
			}

			printf("woot1");

		}
	}

    // Write out to file.
    fwrite(&out, sizeof(out), 1, out_file);
	return true;
}

