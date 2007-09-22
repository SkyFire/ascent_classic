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

bool Collision::Tile::Load(uint32 mapid)
{
	FILE * f;
	char filename[1024];
	char modeldata[32];
	int model_count;
	Collision::MODELHASH modelhash;
	Model * model;
	int i;

	m_mapid = mapid;
	snprintf(filename,1024,"collisiondata\\tiles\\%u-%u_%u.mti", mapid, m_x, m_y);
	f = fopen(filename, "rb");
	if(f == NULL)
		return false;

	fread(&model_count, 4, 1, f);
	m_modelInstances = new ModelInstance*[model_count];

	for(i = 0; i < model_count; ++i)
	{
		fread(&modelhash, 4, 1, f);
		fread(modeldata, 32, 1, f);
		model = Collision::ModelManager.GetModelByHashCode(modelhash);
		if(model == NULL)
		{
			m_modelInstances[i] = NULL;
			continue;
		}

		m_modelInstances[i] = new ModelInstance(model);
		m_modelInstances[i]->Load(modeldata);
	}

	fclose(f);
	return true;
}

void Collision::Tile::DecRef()
{
	if(!--m_refcount)
	{
		Collision::GetTileManager(m_mapid)->RemoveReference(this);		
		delete this;
	}
}

#endif		// COLLISION
