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

#ifndef _COLLISION_MODELMANAGET_H
#define _COLLISION_MODELMANAGET_H

#include "Model.h"

namespace Collision
{
	class CModelManager
	{
		/** hashcode->model* mapping
		 */
		typedef HM_NAMESPACE::hash_map<Collision::MODELHASH, Model*> ModelHashMap;
		ModelHashMap m_models;
		Mutex m_modelLock;

		/** hashcode->path mapping
		*/
		typedef HM_NAMESPACE::hash_map<Collision::MODELHASH, string> ModelPathMap;
		ModelPathMap m_modelpaths;

	public:
		void Init();
		void RemoveReferences(Model* model);
		void AddReferences(Model* model);
		Collision::Model * GetModelByHashCode(Collision::MODELHASH hashcode);
		Collision::Model * LoadModel(Collision::MODELHASH hashcode);
	};

	extern CModelManager ModelManager;
}

#endif		// _COLLISION_MODELMANAGET_H
