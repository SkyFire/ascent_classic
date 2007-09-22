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

#include "adt.h"
#include "mpq_libmpq.h"

#define MODEL_VISUALIZATION
#include <assert.h>

model::~model()
{
	//printf("Unloading model %s\n", file2);
}

FILE * make_directories(const char * filename)
{
	char * temp = strdup(filename);
	FILE * ret = fopen(filename, "wb");
	char * p = strchr(temp,'\\');
	char * r = (char*)temp;
	char b;
	while(ret == NULL)
	{
		char * q = p+1;
		b = *p;
		*p = 0;
		CreateDirectory(r, NULL);
		*p = b;

		p = strchr(q, '\\');
		ret = fopen(filename, "wb");
	}
	return ret;
}

static Vector bounds[100000];
static uint16 boundTris[50000];

//void makedirectories()
bool model::Convert(const char * filename)
{
	/* fix filename - change .mdx -> .m2 */
	//char * temp_filename = strdup(filename);
	char temp_filename[1024];
	strcpy(temp_filename, filename);
	char * p = strrchr(temp_filename, '.');
	file2 = strdup(filename);
	*(p+1) = 'b';
	*(p+2) = 'm';
	*(p+3) = 'd';

	// done?
	char ofilename[1024];
	sprintf(ofilename, "collisiondata\\%s", temp_filename);
	FILE * of = fopen(ofilename, "rb");
	if(of != NULL)
	{
		//printf("Already done %s\n", strstr(temp_filename, "world"));
		//free(temp_filename);
		fclose(of);
		return true;
	}

	*(p+1) = 'm';
	*(p+2) = '2';
	*(p+3) = '\0';
	
	MPQFile f(temp_filename);
	if(f.isEof())
	{
		//free(temp_filename);
		printf("Error! Could not open %s\n", temp_filename);
		return false;
	}
	
	f.read(&header, sizeof(ModelHeader));
	
	if(!header.nBoundingVertices || !header.nBoundingTriangles)
	{
		//printf("File %s has no bounding volume!\n", filename);
		//free(temp_filename);
		f.close();
		return false;
	}

	printf("Converting model: %s\n", strstr(temp_filename, "world"));

	assert(header.nBoundingVertices < 100000);
	assert(header.nBoundingTriangles < 50000);

	*(p+1) = 'b';
	*(p+2) = 'm';
	*(p+3) = 'd';

	of = make_directories(ofilename);
	assert(of);
	fwrite("BMDL", 4, 1, of);
	unsigned int n_tris = header.nBoundingTriangles;
	fwrite(&n_tris,sizeof(int),1,of);
	
	for (size_t i=0; i<header.nBoundingVertices; i++) {
		f.seek(header.ofsBoundingVertices + 12 * i);
		f.read(&bounds[i], 12);
		bounds[i].Fix();
	}

	f.seek(header.ofsBoundingTriangles);
	f.read(boundTris, header.nBoundingTriangles*sizeof(uint16));

	for(int i = 0; i < header.nBoundingTriangles; ++i)
	{
		size_t v = boundTris[i];
		if(v < header.nBoundingTriangles)
		{
			/* set of 3 vectors, each representing a point in the triangle */
			Vector * vc = &bounds[v];
			fwrite(vc, 12, 1, of);
		}
	}

	fclose(of);
	return true;
}