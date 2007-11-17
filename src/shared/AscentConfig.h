/*
 * Ascent MMORPG Server
 * Copyright (C) 2005-2007 Ascent Team <http://www.ascentemu.com/>
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

// Ascent Configuration Header File
// Feel free to customize to your needs.

#ifndef __ASCENTCONFIG_H
#define __ASCENTCONFIG_H

/** If you are building on OSX, enable this define
 * if you are on a big-endian machine.
 * Default: Disabled
 */
//#define USING_BIG_ENDIAN 1

/** Enable/disable LUA scripting system.
 * Default: Disabled
 */
//#define ENABLE_LUA_SCRIPTING 1

/** Enable/disable Ascent world server cluster support.
 * Warning: Ascent will no longer function independantly when built in cluster mode. 
 * It will require a realmserver to function.
 * As of last update, it is nowhere near complete, only enable for dev use.
 * Default: Disabled
 */
//#define CLUSTERING 1

/** Enable/disable 3D geometry calculations
 * Warning: This may be resource heavy.
 * As of last update, it was nowhere near complete. Only enable for dev use.
 * Default: Disabled
 */
//#define COLLISION 1

#endif		// __ASCENTCONFIG_H

