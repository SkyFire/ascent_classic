/*
 * Ascent MMORPG Server
 *   - Collision System
 *
 * Copyright (C) 2005-2007 Burlex <burlex@ascentemu.com>
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

#include "StdAfx.h"
#ifdef COLLISION

//===============================================================================
// Model::LoadModel
// Decreases reference count, deletes if 0.
//===============================================================================

void Collision::Model::DecRef()
{
	if(!--m_refcount)
	{
		Collision::ModelManager.RemoveReferences(this);
		delete this;
	}
}

//===============================================================================
// Model::LoadModel
// Loads a model into memory.
//===============================================================================

bool Collision::Model::LoadModel(const char * filename, Collision::MODELHASH hashcode)
{
	FILE * m_file = fopen(filename, "rb");
	int num_tris;
	char type[4];
	if(m_file == NULL)
		return false;

	fread(type,4,1,m_file);
	fread(&num_tris,4,1,m_file);
	m_numTriangles = num_tris;
	m_triangles = new Vec3d[num_tris];
	m_hashCode = hashcode;

	for(uint32 i = 0; i < num_tris; ++i)
	{
		fread(&m_triangles[i], 12, 1, m_file);
	}

	fclose(m_file);
	return true;
}


void Collision::ModelInstance::Load(char * data)
{
	int crap;
	int iscale_factor;
	float fscale_factor;
	crap = *(int*)&data[0];
	memcpy(&m_position, &data[4], 12);
	memcpy(&m_rotation, &data[16], 12);
	iscale_factor = *(int*)&data[28];
	fscale_factor = (float)iscale_factor;
	m_scale = fscale_factor / 1024.0f;
}

Collision::ModelInstance::ModelInstance(Collision::Model* model) : m_model(model)
{
	m_model->AddRef();
}

Collision::ModelInstance::~ModelInstance()
{
	m_model->DecRef();
}

#endif		// COLLISION
