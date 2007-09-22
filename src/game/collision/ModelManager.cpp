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

Collision::CModelManager Collision::ModelManager;

void Collision::CModelManager::Init()
{
	FILE * m_file = fopen("collisiondata\\modelindex.ind", "rb");
	int count, i;
	string filename;
	char c[2] = {0,0};
	Collision::MODELHASH hash;
	if(m_file == NULL)
		return;

	fread(&count,4,1,m_file);
    for(i = 0; i < count; ++i)
	{
		fread(&hash, 4, 1, m_file);
		filename.clear();
		while((c[0] = fgetc(m_file)) != '\0')
			filename.append(c);

		m_modelpaths.insert(make_pair(hash, filename));
	}
	fclose(m_file);
}

void Collision::CModelManager::AddReferences(Model* model)
{
	m_modelLock.Acquire();
	m_models.insert( make_pair( model->GetHashCode(), model ) );
	m_modelLock.Release();
}

void Collision::CModelManager::RemoveReferences(Model* model)
{
	m_modelLock.Acquire();
	m_models.erase(model->GetHashCode());
	m_modelLock.Release();
}

Collision::Model * Collision::CModelManager::GetModelByHashCode(Collision::MODELHASH hashcode)
{
	ModelHashMap::iterator itr;
	
	m_modelLock.Acquire();
	itr = m_models.find(hashcode);
	m_modelLock.Release();

	return (itr != m_models.end()) ? itr->second : LoadModel(hashcode);
}

Collision::Model * Collision::CModelManager::LoadModel(Collision::MODELHASH hashcode)
{
	ModelPathMap::iterator itr = m_modelpaths.find(hashcode);
	if(itr == m_modelpaths.end())
		return NULL;
	else
	{
		Collision::Model * model = new Collision::Model();
		Log.Debug("ModelManager", "Loading Model 0x%.8X from %s", hashcode, itr->second.c_str());
		if(model->LoadModel(itr->second.c_str(), hashcode))
		{
			AddReferences(model);
			return model;
		}
		else
		{
			delete model;
			return NULL;
		}
	}
}

#endif		// COLLISION
