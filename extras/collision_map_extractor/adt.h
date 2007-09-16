#ifndef ADT_H
#define ADT_H

#include <string>
#include <vector>
using namespace std;

#define TILESIZE (533.33333f)
#define CHUNKSIZE ((TILESIZE) / 16.0f)
#define UNITSIZE (CHUNKSIZE / 8.0f)

typedef unsigned char uint8;
typedef unsigned short uint16;
typedef unsigned int uint32;
class Liquid;


#pragma pack(push,1)

struct Vector
{
	float x;
	float y;
	float z;

	void Fix()
	{
		float save = z;
		z = -y;
		y = save;
	}
};

struct ModelHeader {
	char id[4];
	uint8 version[4];
	uint32 nameLength;
	uint32 nameOfs;
	uint32 type;

	uint32 nGlobalSequences;
	uint32 ofsGlobalSequences;
	uint32 nAnimations;
	uint32 ofsAnimations;
	uint32 nC;
	uint32 ofsC;
	uint32 nD;
	uint32 ofsD;
	uint32 nBones;
	uint32 ofsBones;
	uint32 nF;
	uint32 ofsF;

	uint32 nVertices;
	uint32 ofsVertices;
	uint32 nViews;
	uint32 ofsViews;

	uint32 nColors;
	uint32 ofsColors;

	uint32 nTextures;
	uint32 ofsTextures;

	uint32 nTransparency; // H
	uint32 ofsTransparency;
	uint32 nI;   // always unused ?
	uint32 ofsI;
	uint32 nTexAnims;	// J
	uint32 ofsTexAnims;
	uint32 nK;
	uint32 ofsK;

	uint32 nTexFlags;
	uint32 ofsTexFlags;
	uint32 nY;
	uint32 ofsY;

	uint32 nTexLookup;
	uint32 ofsTexLookup;

	uint32 nTexUnitLookup;		// L
	uint32 ofsTexUnitLookup;
	uint32 nTransparencyLookup; // M
	uint32 ofsTransparencyLookup;
	uint32 nTexAnimLookup;
	uint32 ofsTexAnimLookup;

	float floats[14];

	uint32 nBoundingTriangles;
	uint32 ofsBoundingTriangles;
	uint32 nBoundingVertices;
	uint32 ofsBoundingVertices;
	uint32 nBoundingNormals;
	uint32 ofsBoundingNormals;

	uint32 nO;
	uint32 ofsO;
	uint32 nP;
	uint32 ofsP;
	uint32 nQ;
	uint32 ofsQ;
	uint32 nLights; // R
	uint32 ofsLights;
	uint32 nCameras; // S
	uint32 ofsCameras;
	uint32 nT;
	uint32 ofsT;
	uint32 nRibbonEmitters; // U
	uint32 ofsRibbonEmitters;
	uint32 nParticleEmitters; // V
	uint32 ofsParticleEmitters;

};
#pragma pack(pop)
class model
{
	ModelHeader header;
	Vector boundaries[4];
	// after calculation of boundingbox
	float sizex;
	float sizey;
	char * file2;
public:
	~model();
	bool Load(const char * filename);
	inline float GetSizeX() { return sizex; }
	inline float GetSizeY() { return sizey; }
};
typedef struct {
	float x;
	float y;
	float z;
}svec;

typedef struct {
	double x;
	double y;
	double z;
}vec;

typedef struct{
	vec v[3];

}triangle;	

typedef struct{
	float v9[16*8+1][16*8+1];
	float v8[16*8][16*8];
}Cell;

typedef struct{
	double v9[9][9];
	double v8[8][8];
	uint16 area_id;
	//Liquid *lq;
	float waterlevel;
	uint8 flag;

}chunk;


class WMO;
class WMOManager;
void fixname(std::string &name);
#pragma pack(push, 1)

#define TILESIZE (533.33333f)
#define CHUNKSIZE ((TILESIZE) / 16.0f)
#define UNITSIZE (CHUNKSIZE / 8.0f)
#define ZEROPOINT (32.0f * (TILESIZE))

typedef struct  
{
	int d1;
	float posx;
	float posy;
	float posz;
	float rotx;
	float roty;
	float rotz;
	int scale_factor;
	int modelid;
	/* wtf crack blizz devs smoking? */
	void fix_coords()
	{
		/*float save = posz;
		posz = -posy;
		posy = save;*/
		float save[3] = {posx,posy,posz};
		posx = -(save[2] - ZEROPOINT);
		posy = -(save[0] - ZEROPOINT);
		posz = save[1];
	}
}model_instance;
#pragma pack(pop)

typedef struct
{
	chunk ch[16][16];
	vector<string> model_ids;
	vector<model_instance> model_instances;
}mcell;
class MPQFile;
void LoadMapChunk(MPQFile &,chunk*);
bool LoadWMO(char* filename);
#endif

