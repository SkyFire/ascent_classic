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

#ifndef COLLISION_MODEL_H
#define COLLISION_MODEL_H

namespace Collision
{
	class Model
	{
		/** Models are only held in memory as long as they're being used.
		 */
		uint32 m_refcount;

		/** Bounding triangle array
		 */
		Vec3d * m_triangles;
		uint32 m_numTriangles;

		/** Our hash code.
		 */
		Collision::MODELHASH m_hashCode;

	public:
		Model() : m_triangles(NULL), m_numTriangles(0), m_refcount(0), m_hashCode(0xFFFFFFFF)
		{
			
		}

		~Model()
		{
			if(m_triangles != NULL)
				delete [] m_triangles;

			ASSERT(m_refcount == 0);
		}

        /** Increases the reference count by one.
		 */
		inline void AddRef() { ++m_refcount; }

		/** Decreases the reference coutn by one. If this leaves the count at zero,
		 * deallocates the object.
		 */
		void DecRef();
		inline uint32 GetRefCount() { return m_refcount; }

		/** Loads a model from the supplied file and hash.
		 * @return true if success
		 */
		bool LoadModel(const char * filename, Collision::MODELHASH hashcode);
		inline Collision::MODELHASH GetHashCode() { return m_hashCode; }
	};

	class ModelInstance
	{
		/** Position
		 */
		Vec3d m_position;
		Vec3d m_rotation;
		float m_scale;
		Model * m_model;

	public:
		/** Creates a model instance using a model pointer.
		 * Increases reference count.
		 */
		ModelInstance(Collision::Model* model);

		/** On delete, decreases reference count.
		 */
		~ModelInstance();

		/** Loads model instance from supplied pointer.
		 */
		void Load(char * data);
	};
};

#endif		// _COLLISION_MODEL_H
