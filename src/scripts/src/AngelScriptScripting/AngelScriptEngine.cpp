/*
 * Ascent MMORPG Server
 * LUA Interface
 * Copyright (C) 2007 Burlex <burlex@gmail.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#include "StdAfx.h"
#include "AngelScriptEngine.h"
#include "AngelScriptFunctions.h"
#include <CoreMemoryAllocator.cpp>

#if PLATFORM != PLATFORM_WIN32
#include <dirent.h>
#endif

bool as_is_starting_up = false;

AngelScriptEngineMgr g_asMgr;

AngelScriptEngine* g_engine;
ScriptMgr* g_scriptMgr;

asIScriptEngine* g_asEngine;
asIScriptContext* g_asEventCtx;
asIScriptContext* g_asCurrentCtx;

extern "C" SCRIPT_DECL uint32 _exp_get_version()
{
	return MAKE_SCRIPT_VERSION(SCRIPTLIB_VERSION_MAJOR, SCRIPTLIB_VERSION_MINOR);
}

extern "C" SCRIPT_DECL uint32 _exp_get_script_type()
{
	return SCRIPT_TYPE_SCRIPT_ENGINE | SCRIPT_TYPE_SCRIPT_ENGINE_AS;
}

extern "C" SCRIPT_DECL void _exp_script_register( ScriptMgr* mgr )
{
	g_scriptMgr = mgr;
	g_asMgr.Startup();
}

void MessageCallback( const asSMessageInfo* msg, void* param )
{
	if( msg->type == asMSGTYPE_WARNING ) 
		Log.Warning( "%s (%d, %d) : %s : %s", msg->section, msg->row, msg->col, msg->message );
	else if( msg->type == asMSGTYPE_ERROR ) 
		Log.Error( "%s (%d, %d) : %s : %s", msg->section, msg->row, msg->col, msg->message );
	else if( msg->type == asMSGTYPE_INFORMATION ) 
		Log.Notice( "%s (%d, %d) : %s : %s", msg->section, msg->row, msg->col, msg->message );
}

template<typename T>
const char* GetTClassName()
{
	return "UNKNOWN";
}

template<>
const char* GetTClassName< Unit >()
{
	return "Unit";
}

template<>
const char* GetTClassName< GameObject >()
{
	return "GameObject";
}

//template<typename T>
//struct RegType
//{
//	const char * name;
//	int(*mfunc)(lua_State*,T*);
//};

//************************************************************************/
//* SCRIPT FUNCTION TABLES                                               */
//************************************************************************/

//RegType<Unit> UnitMethods[] = {
//	{ NULL, NULL },
//};
//
//RegType<GameObject> GOMethods[] = {
//	{ NULL, NULL },
//};

//template<typename T>
//RegType<T>* GetMethodTable() { return NULL; }
//
//template<>
//RegType<Unit>* GetMethodTable<Unit>() { return UnitMethods; }
//
//template<>
//RegType<GameObject>* GetMethodTable<GameObject>() { return GOMethods; }

AngelScriptEngine::AngelScriptEngine()
{
	g_asEngine = NULL;
	g_asEventCtx = NULL;
}

AngelScriptEngine::~AngelScriptEngine()
{
	g_asEngine->Release();
	g_asEngine = NULL;
	g_asEventCtx = NULL;
}

void AngelScriptEngine::Startup()
{
	g_asEngine = asCreateScriptEngine( ANGELSCRIPT_VERSION );
	if( g_asEngine == NULL )
	{
		Log.Warning( "AngelScriptEngineMgr", "Could not spawn AngelScript Engine!!!" );
		return;
	}

	// Register message callback
	g_asEngine->SetMessageCallback( asFUNCTION( MessageCallback ), NULL, asCALL_CDECL );

	// Create the events context
	g_asEventCtx = g_asEngine->CreateContext();
	if( g_asEventCtx == NULL )
	{
		Log.Warning( "AngelScriptEngineMgr", "Failed to create the context!!!" );
		return;
	}
}

void AngelScriptEngine::Shutdown()
{
	g_asEngine->Release();
	g_asEngine = NULL;
}

void AngelScriptEngine::Restart()
{
	Shutdown();
	Startup();
}

void AngelScriptEngine::LoadScripts()
{
	set< string > asText;
	set< string > asByteCode;

#ifdef WIN32

	WIN32_FIND_DATA fd;
	HANDLE h;

	h = FindFirstFile( "scripts\\*.*", &fd );
	if( h == INVALID_HANDLE_VALUE )
		return;

	do 
	{
		char* fn = strrchr( fd.cFileName, '\\' );

		if( !fn )
			fn = fd.cFileName;

        char* ext = strrchr( fd.cFileName, '.' );

		if( !stricmp( ext, ".as" ) )
			asText.insert( string( fn ) );
		//else if( !stricmp( ext, ".asb" ) )
		//	asByteCode.insert( string( fn ) );

	} while( FindNextFile( h, &fd ) );
	FindClose( h );

#else

	struct dirent** list;
	int filecount = scandir( "./scripts", &list, 0, 0 );
	if( filecount <= 0 || !list )
		return;

	while( filecount-- )
	{
		char* ext = strrchr( list[filecount]->d_name, '.' );
		if( ext != NULL && !strcmp( ext, ".as" ) )
		{
			string full_path = string( list[filecount]->d_name );
			asText.insert( string(full_path.c_str() ) );
		}
		//else if( !stricmp( ext, ".asb" ) )
		//{
		//	string full_path = string(list[filecount]->d_name );
		//	asByteCode.insert( string(full_path.c_str() ) );
		//}

		free( list[filecount] );
	}
	free( list );

#endif

	//for( set< string >::iterator itr = asByteCode.begin(); itr != asByteCode.end(); ++itr )
	//{
	//	set< string >::iterator it2 = asText.find( *itr );
	//	if( it2 == asText.end() )
	//		asText.erase( it2 );
	//}

	if( as_is_starting_up )
		Log.Notice( "AngelScriptEngine", "Loading Scripts...");

	char filename[255];

	for( set< string >::iterator itr = asText.begin(); itr != asText.end(); ++itr )
	{
#ifdef WIN32
		snprintf( filename, 200, "scripts\\%s", itr->c_str() );
#else
		snprintf( filename, 200, "scripts/%s", itr->c_str() );
#endif
		if( as_is_starting_up )
			Log.Notice( "AngelScriptEngine", "%s...", itr->c_str() );

		LoadScript( (*itr) );
	}
}

void AngelScriptEngine::LoadScript( string filename )
{

	FILE* stream = NULL;
	std::string txt;
	int size = 0;

	stream = fopen( filename.c_str(), "rb" );

	if( !stream )
	{
		Log.Warning( "AngelScriptEngine", "Could not load %s!!!", filename.c_str() );
		return;
	}

	fseek( stream, 0, SEEK_END );
	size = (int)ftell( stream );
	fseek( stream, 0, SEEK_SET );

	txt.resize(size);

	fread( &txt[0], size, 1, stream );
	fclose( stream );
	stream = NULL;

	if( g_asEngine->AddScriptSection( filename.c_str(), filename.c_str(), &txt[0], size, 0 ) < 0 )
	{
		Log.Warning( "AngelScriptEngine", "There was a problem while loading %s!!!", filename.c_str() );
		return;
	}

	if( g_asEngine->Build( filename.c_str() ) < 0 )
	{
		Log.Warning( "AngelScriptEngine", "Could not compile %s!!!", filename.c_str() );
		return;
	}

	asIScriptContext* m_pContext = g_asEngine->CreateContext();
	if( m_pContext == NULL )
	{
		Log.Warning( "AngelScriptEngine", "Could not create context for %s!!!", filename.c_str() );
		return;
	}

}

//void LuaEngine::OnUnitEvent(Unit * pUnit, const char * FunctionName, uint32 EventType, Unit * pMiscUnit, uint32 Misc)
//{
//	if(FunctionName==NULL)
//		return;
//
//	m_Lock.Acquire();
//	lua_pushstring(L, FunctionName);
//	lua_gettable(L, LUA_GLOBALSINDEX);
//	if(lua_isnil(L,-1))
//	{
//		printf("Tried to call invalid LUA function '%s' from Ascent (Unit)!\n", FunctionName);
//		m_Lock.Release();
//		return;
//	}
//
//	Lunar<Unit>::push(L, pUnit);
//	lua_pushinteger(L,EventType);
//	if(pMiscUnit!=NULL)
//		Lunar<Unit>::push(L, pMiscUnit);
//	else
//		lua_pushnil(L);
//	lua_pushinteger(L,Misc);
//	
//	int r = lua_pcall(L,4,LUA_MULTRET,0);
//	if(r)
//		report(L);
//
//	m_Lock.Release();
//}

//void LuaEngine::OnQuestEvent(Player * QuestOwner, const char * FunctionName, uint32 QuestID, uint32 EventType, Object * QuestStarter)
//{
//	if(FunctionName==NULL)
//		return;
//
//	m_Lock.Acquire();
//	lua_pushstring(L, FunctionName);
//	lua_gettable(L, LUA_GLOBALSINDEX);
//	if(lua_isnil(L,-1))
//	{
//		printf("Tried to call invalid LUA function '%s' from Ascent (Quest)!\n", FunctionName);
//		m_Lock.Release();
//		return;
//	}
//
//	if (QuestOwner)
//		Lunar<Unit>::push(L, (Unit*)QuestOwner);
//	else
//		lua_pushnil(L);
//
//	lua_pushinteger(L,EventType);
//
//	if(QuestStarter!=NULL && QuestStarter->GetTypeId() == TYPEID_UNIT)
//		Lunar<Unit>::push(L, (Unit*)QuestStarter);
//	else
//		lua_pushnil(L);
//
//	int r = lua_pcall(L,3,LUA_MULTRET,0);
//	if(r)
//		report(L);
//
//	m_Lock.Release();
//}

//void LuaEngine::CallFunction(Unit * pUnit, const char * FuncName)
//{
//	m_Lock.Acquire();
//	lua_pushstring(L, FuncName);
//	lua_gettable(L, LUA_GLOBALSINDEX);
//	if(lua_isnil(L,-1))
//	{
//		printf("Tried to call invalid LUA function '%s' from Ascent (Unit)!\n", FuncName);
//		m_Lock.Release();
//		return;
//	}
//
//	Lunar<Unit>::push(L, pUnit);
//	int r = lua_pcall(L,1,LUA_MULTRET,0);
//	if(r)
//		report(L);
//
//	m_Lock.Release();
//}

//void LuaEngine::OnGameObjectEvent(GameObject * pGameObject, const char * FunctionName, uint32 EventType, Unit * pMiscUnit)
//{
//	if(FunctionName==NULL)
//		return;
//
//	m_Lock.Acquire();
//	lua_pushstring(L, FunctionName);
//	lua_gettable(L, LUA_GLOBALSINDEX);
//	if(lua_isnil(L,-1))
//	{
//		printf("Tried to call invalid LUA function '%s' from Ascent! (GO)\n", FunctionName);
//		m_Lock.Release();
//		return;
//	}
//
//	Lunar<GameObject>::push(L, pGameObject);
//	lua_pushinteger(L,EventType);
//	if(!pMiscUnit)
//		lua_pushnil(L);
//	else
//		Lunar<Unit>::push(L, pMiscUnit);
//
//	int r = lua_pcall(L,3,LUA_MULTRET,0);
//	if(r)
//		report(L);
//
//	m_Lock.Release();
//}

//static int RegisterUnitEvent(lua_State * L);
//static int RegisterQuestEvent(lua_State * L);
//static int RegisterGameObjectEvent(lua_State * L);

//void LuaEngine::RegisterCoreFunctions()
//{
//	lua_pushcfunction(L, &RegisterUnitEvent);
//	lua_setglobal(L, "RegisterUnitEvent");
//
//	lua_pushcfunction(L, &RegisterGameObjectEvent);
//	lua_setglobal(L, "RegisterGameObjectEvent");
//
//	lua_pushcfunction(L, &RegisterQuestEvent);
//	lua_setglobal(L, "RegisterQuestEvent");
//
//	Lunar<Unit>::Register(L);
//	Lunar<GameObject>::Register(L);
//	//Lunar<Quest>::Register(L); quest isn't a class
//}

//static int RegisterUnitEvent(lua_State * L)
//{
//	int entry = luaL_checkint(L, 1);
//	int ev = luaL_checkint(L, 2);
//	const char * str = luaL_checkstring(L, 3);
//
//	if(!entry || !ev || !str || !lua_is_starting_up)
//		return 0;
//
//	g_luaMgr.RegisterUnitEvent(entry,ev,str);
//	return 0;
//}

//static int RegisterQuestEvent(lua_State * L)
//{
//	int entry = luaL_checkint(L, 1);
//	int ev = luaL_checkint(L, 2);
//	const char * str = luaL_checkstring(L, 3);
//
//	if(!entry || !ev || !str || !lua_is_starting_up)
//		return 0;
//
//	g_luaMgr.RegisterQuestEvent(entry,ev,str);
//	return 0;
//}

//static int RegisterGameObjectEvent(lua_State * L)
//{
//	int entry = luaL_checkint(L, 1);
//	int ev = luaL_checkint(L, 2);
//	const char * str = luaL_checkstring(L, 3);
//
//	if(!entry || !ev || !str || !lua_is_starting_up)
//		return 0;
//
//	g_luaMgr.RegisterGameObjectEvent(entry,ev,str);
//	return 0;
//}

void AngelScriptEngineMgr::Startup()
{
	Log.Notice( "AngelScriptEngineMgr", "Spawning AngelScript Engine..." );

	as_is_starting_up = true;

	m_engine = new AngelScriptEngine();
	g_engine = m_engine;

	m_engine->Startup();
	m_engine->LoadScripts();

	as_is_starting_up = false;

	// Register AngelScript Plugins
	//RegisterScriptString( g_asEngine );

	// Register class object types
	g_asEngine->RegisterObjectType( "Unit", sizeof( Unit ), 0 );
	g_asEngine->RegisterObjectType( "Object", sizeof( Object ), 0 );
	g_asEngine->RegisterObjectType( "MapMgr", sizeof( MapMgr ), 0 );
	g_asEngine->RegisterObjectType( "Player", sizeof( Player ), 0 );
	g_asEngine->RegisterObjectType( "Creature", sizeof( Creature ), 0 );
	g_asEngine->RegisterObjectType( "GameObject", sizeof( GameObject ), 0 );
	g_asEngine->RegisterObjectType( "AIInterface", sizeof( AIInterface ), 0 );
	g_asEngine->RegisterObjectType( "WayPointMap", sizeof( WayPointMap ), 0 );

	// Register Player class methods
	g_asEngine->RegisterObjectMethod( "Player", "void GetName()", asMETHOD( Player, GetName ), asCALL_THISCALL );
	
	// Register Object class methods
	g_asEngine->RegisterObjectMethod( "Object", "MapMgr* GetMapMgr() const", asMETHOD( Object, GetMapMgr ), asCALL_THISCALL );
	
	// Register Creature class methods
	g_asEngine->RegisterObjectMethod( "Creature", "void GetCreatureName()", asMETHOD( Creature, GetCreatureName ), asCALL_THISCALL );
	g_asEngine->RegisterObjectMethod( "Creature", "void DestroyCustomWaypointMap()", asMETHOD( Creature, DestroyCustomWaypointMap ), asCALL_THISCALL );
	g_asEngine->RegisterObjectMethod( "Creature", "bool HasCustomWayPointMap()", asMETHOD( Creature, HasCustomWayPointMap ), asCALL_THISCALL );
	g_asEngine->RegisterObjectMethod( "Creature", "void Despawn( uint32 delay, uint32 respawntime ", asMETHOD( Creature, Despawn ), asCALL_THISCALL );

	// Register Unit class methods
	g_asEngine->RegisterObjectMethod( "Unit", "const uint32& GetUInt32Value( uint32 index ) const", asMETHOD( Unit, GetUInt32Value ), asCALL_THISCALL );
	g_asEngine->RegisterObjectMethod( "Unit", "void EventAddEmote( EmoteType emote, uint32 time )", asMETHOD( Unit, EventAddEmote ), asCALL_THISCALL );
	g_asEngine->RegisterObjectMethod( "Unit", "void Emote( EmoteType emote )", asMETHOD( Unit, Emote ), asCALL_THISCALL );
	g_asEngine->RegisterObjectMethod( "Unit", "bool IsPlayer()", asMETHOD( Unit, IsPlayer ), asCALL_THISCALL );
	g_asEngine->RegisterObjectMethod( "Unit", "bool IsCreature()", asMETHOD( Unit, IsCreature ), asCALL_THISCALL );
	g_asEngine->RegisterObjectMethod( "Unit", "void SendChatMessage( uint8 type, uint32 lang, const char* msg )", asMETHOD( Unit, SendChatMessage ), asCALL_THISCALL );
	g_asEngine->RegisterObjectMethod( "Unit", "AIInterface* GetAIInterface()", asMETHOD( Unit, GetAIInterface ), asCALL_THISCALL );

	// Register AIInterface class methods
	g_asEngine->RegisterObjectMethod( "AIInterface", "void setMoveType( uint32 movetype )", asMETHOD( AIInterface, setMoveType ), asCALL_THISCALL );
	g_asEngine->RegisterObjectMethod( "AIInterface", "void SetWaypointMap( WayPointMap* m )", asMETHOD( AIInterface, SetWaypointMap ), asCALL_THISCALL );
	g_asEngine->RegisterObjectMethod( "AIInterface", "Unit* GetMostHated()", asMETHOD( AIInterface, GetMostHated ), asCALL_THISCALL );
	g_asEngine->RegisterObjectMethod( "AIInterface", "Unit* GetSecondHated()", asMETHOD( AIInterface, GetSecondHated ), asCALL_THISCALL );
	g_asEngine->RegisterObjectMethod( "AIInterface", "bool StopMovement( uint32 time )", asMETHOD( AIInterface, StopMovement ), asCALL_THISCALL );
	g_asEngine->RegisterObjectMethod( "AIInterface", "Unit* getTauntedBy()", asMETHOD( AIInterface, getTauntedBy ), asCALL_THISCALL );
	g_asEngine->RegisterObjectMethod( "AIInterface", "bool GetIsTaunted()", asMETHOD( AIInterface, GetIsTaunted ), asCALL_THISCALL );
	g_asEngine->RegisterObjectMethod( "AIInterface", "bool taunt( Unit* caster, bool apply = true )", asMETHOD( AIInterface, taunt ), asCALL_THISCALL );
	g_asEngine->RegisterObjectMethod( "AIInterface", "bool GetIsSoulLinked()", asMETHOD( AIInterface, GetIsSoulLinked ), asCALL_THISCALL );
	g_asEngine->RegisterObjectMethod( "AIInterface", "Unit* getSoullinkedWith()", asMETHOD( AIInterface, getSoullinkedWith ), asCALL_THISCALL );
	g_asEngine->RegisterObjectMethod( "AIInterface", "void SetNextTarget( Unit* nextTarget )", asMETHOD( AIInterface, SetNextTarget ), asCALL_THISCALL );
	g_asEngine->RegisterObjectMethod( "AIInterface", "void setMoveType( uint32 movetype )", asMETHOD( AIInterface, setMoveType ), asCALL_THISCALL );
	g_asEngine->RegisterObjectMethod( "AIInterface", "void MoveTo(float x, float y, float z, float o)", asMETHOD( AIInterface, MoveTo ), asCALL_THISCALL );
	g_asEngine->RegisterObjectMethod( "AIInterface", "SetDisableCombat( bool value )", asMETHOD( AIInterface, SetDisableCombat ), asCALL_THISCALL );
	g_asEngine->RegisterObjectMethod( "AIInterface", "SetDisableMelee( bool value )", asMETHOD( AIInterface, SetDisableMelee ), asCALL_THISCALL );
	g_asEngine->RegisterObjectMethod( "AIInterface", "SetDisableRanged( bool value )", asMETHOD( AIInterface, SetDisableRanged ), asCALL_THISCALL );
	g_asEngine->RegisterObjectMethod( "AIInterface", "SetDisableSpell( bool value )", asMETHOD( AIInterface, SetDisableSpell ), asCALL_THISCALL );
	g_asEngine->RegisterObjectMethod( "AIInterface", "SetDisableTargeting( bool value )", asMETHOD( AIInterface, SetDisableTargeting ), asCALL_THISCALL );

	// Register events
}

void AngelScriptEngineMgr::Shutdown()
{
	m_engine->Shutdown();
	delete m_engine;
	m_engine = NULL;
	g_engine = NULL;
}

void AngelScriptEngineMgr::Restart()
{
	Shutdown();
	Startup();
}

void AngelScriptEngineMgr::RegisterUnitEvent( uint32 Id, uint32 Event, const char* FunctionName )
{
//	UnitBindingMap::iterator itr = m_unitBinding.find(Id);
//	if(itr == m_unitBinding.end())
//	{
//		LuaUnitBinding ub;
//		memset(&ub,0,sizeof(LuaUnitBinding));
//		ub.Functions[Event] = strdup(FunctionName);
//		m_unitBinding.insert(make_pair(Id,ub));
//	}
//	else
//	{
//		if(itr->second.Functions[Event]!=NULL)
//			free((void*)itr->second.Functions[Event]);
//
//		itr->second.Functions[Event]=strdup(FunctionName);
//	}
}

void AngelScriptEngineMgr::RegisterQuestEvent( uint32 Id, uint32 Event, const char* FunctionName )
{
//	QuestBindingMap::iterator itr = m_questBinding.find(Id);
//	if(itr == m_questBinding.end())
//	{
//		LuaQuestBinding qb;
//		memset(&qb,0,sizeof(LuaQuestBinding));
//		qb.Functions[Event] = strdup(FunctionName);
//		m_questBinding.insert(make_pair(Id,qb));
//	}
//	else
//	{
//		if(itr->second.Functions[Event]!=NULL)
//			free((void*)itr->second.Functions[Event]);
//
//		itr->second.Functions[Event]=strdup(FunctionName);
//	}
}

void AngelScriptEngineMgr::RegisterGameObjectEvent( uint32 Id, uint32 Event, const char* FunctionName )
{
//	GameObjectBindingMap::iterator itr = m_gameobjectBinding.find(Id);
//	if(itr == m_gameobjectBinding.end())
//	{
//		LuaGameObjectBinding ub;
//		memset(&ub,0,sizeof(LuaGameObjectBinding));
//		ub.Functions[Event] = strdup(FunctionName);
//		m_gameobjectBinding.insert(make_pair(Id,ub));
//	}
//	else
//	{
//		if(itr->second.Functions[Event]!=NULL)
//			free((void*)itr->second.Functions[Event]);
//
//		itr->second.Functions[Event]=strdup(FunctionName);
//	}
}

///************************************************************************/
///* SCRIPT FUNCTION IMPLEMENTATION                                       */
///************************************************************************/
//#define CHECK_TYPEID(expected_type) if(!ptr || !ptr->IsInWorld() || ptr->GetTypeId() != expected_type) { return 0; }
//#define CHECK_TYPEID_RET(expected_type) if(!ptr || !ptr->IsInWorld() || ptr->GetTypeId() != expected_type) { lua_pushboolean(L,0); return 0; }
//#define CHECK_TYPEID_RET_INT(expected_type) if(!ptr || !ptr->IsInWorld() || ptr->GetTypeId() != expected_type) { lua_pushinteger(L,0); return 0; }
//
//int luaUnit_IsPlayer(lua_State * L, Unit * ptr)
//{
//	if(!ptr)
//	{
//		lua_pushboolean(L, 0);
//		return 1;
//	}
//
//	if(ptr->IsPlayer())
//		lua_pushboolean(L, 1);
//	else
//		lua_pushboolean(L, 0);
//
//	return 1;
//}
//
//int luaUnit_IsCreature(lua_State * L, Unit * ptr)
//{
//	if(!ptr)
//	{
//		lua_pushboolean(L, 0);
//		return 1;
//	}
//
//	if(ptr->GetTypeId()==TYPEID_UNIT)
//		lua_pushboolean(L, 1);
//	else
//		lua_pushboolean(L, 0);
//
//	return 1;
//}
//
//int luaUnit_Emote(lua_State * L, Unit * ptr)
//{
//	if(ptr==NULL) return 0;
//	uint32 emote_id = luaL_checkint(L, 1);
//	uint32 time = luaL_checkint(L, 1);
//	if(emote_id==NULL) 
//		return 0;
//	if (time)
//		ptr->EventAddEmote((EmoteType)emote_id,time);
//	else
//		ptr->Emote((EmoteType)emote_id);
//	return 1;
//}
//
//int luaUnit_GetManaPct(lua_State * L, Unit * ptr)
//{
//	if(!ptr) 
//		return 0;
//	if (ptr->GetPowerType() == POWER_TYPE_MANA)
//		lua_pushnumber(L, (int)(ptr->GetUInt32Value(UNIT_FIELD_POWER1) * 100.0f / ptr->GetUInt32Value(UNIT_FIELD_MAXPOWER1)));
//	else
//		lua_pushnil(L);
//	return 1;
//}
//
//int luaUnit_GetName(lua_State * L, Unit * ptr)
//{
//	if(!ptr)
//		return 0;
//
//	switch(ptr->GetTypeId())
//	{
//	case TYPEID_UNIT:
//		lua_pushstring(L, ((Creature*)ptr)->GetCreatureName() ? ((Creature*)ptr)->GetCreatureName()->Name : "Unknown");
//		break;
//
//	case TYPEID_PLAYER:
//		lua_pushstring(L, ((Player*)ptr)->GetName());
//		break;
//
//	default:
//		lua_pushstring(L, "Unknown");
//		break;
//	}
//
//	return 1;
//}
//
//int luaUnit_SendChatMessage(lua_State * L, Unit * ptr)
//{
//	CHECK_TYPEID(TYPEID_UNIT);
//	uint32 typ = luaL_checkint(L, 1);
//	uint32 lang = luaL_checkint(L, 2);
//	const char * message = luaL_checklstring(L, 3, NULL);
//	if(message == NULL)
//		return 0;
//
//	ptr->SendChatMessage(typ, lang, message);
//	return 0;
//}
//
//int luaUnit_MoveTo(lua_State * L, Unit * ptr)
//{
//	CHECK_TYPEID(TYPEID_UNIT);
//	double x = luaL_checknumber(L, 1);
//	double y = luaL_checknumber(L, 2);
//	double z = luaL_checknumber(L, 3);
//	double o = luaL_checknumber(L, 4);
//
//	if(x == 0 || y == 0 || z == 0)
//		return 0;
//
//	ptr->GetAIInterface()->MoveTo( (float)x, (float)y, (float)z, (float)o );
//	return 0;
//}
//
//int luaUnit_SetMovementType(lua_State * L, Unit * ptr)
//{
//	CHECK_TYPEID(TYPEID_UNIT);
//	uint32 typ = luaL_checkint(L, 1);
//	ptr->GetAIInterface()->setMoveType(typ);
//	return 0;
//}
//
//int luaUnit_GetX(lua_State * L, Unit * ptr)
//{
//	if(ptr==NULL) return 0;
//	lua_pushnumber(L, (double)ptr->GetPositionX());
//	return 1;
//}
//
//int luaUnit_GetY(lua_State * L, Unit * ptr)
//{
//	if(ptr==NULL) return 0;
//	lua_pushnumber(L, (double)ptr->GetPositionY());
//	return 1;
//}
//
//int luaUnit_GetZ(lua_State * L, Unit * ptr)
//{
//	if(ptr==NULL) return 0;
//	lua_pushnumber(L, (double)ptr->GetPositionZ());
//	return 1;
//}
//
//int luaUnit_GetO(lua_State * L, Unit * ptr)
//{
//	if(ptr==NULL) return 0;
//	lua_pushnumber(L, (double)ptr->GetOrientation());
//	return 1;
//}
//
//int luaUnit_CastSpell(lua_State * L, Unit * ptr)
//{
//	CHECK_TYPEID(TYPEID_UNIT);
//	uint32 sp = luaL_checkint(L, 1);
//
//	if(sp==0)
//		return 0;
//
//	Spell * spp = new Spell(ptr, dbcSpell.LookupEntry(sp), true, NULL);
//	SpellCastTargets tar(ptr->GetGUID());
//	spp->prepare(&tar);
//	return 0;
//}
//
//int luaUnit_FullCastSpell(lua_State * L, Unit * ptr)
//{
//	CHECK_TYPEID(TYPEID_UNIT);
//	uint32 sp = luaL_checkint(L, 1);
//
//	if(sp==0)
//		return 0;
//
//	Spell * spp = new Spell(ptr, dbcSpell.LookupEntry(sp), false, NULL);
//	SpellCastTargets tar(ptr->GetGUID());
//	spp->prepare(&tar);
//	return 0;
//}
//
//int luaUnit_CastSpellOnTarget(lua_State * L, Unit * ptr)
//{
//	CHECK_TYPEID(TYPEID_UNIT);
//	uint32 sp = luaL_checkint(L, 1);
//	Unit * target = Lunar<Unit>::check(L, 2);
//
//	if(sp==0 || target==NULL)
//		return 0;
//
//	Spell * spp = new Spell(ptr, dbcSpell.LookupEntry(sp), false, NULL);
//	SpellCastTargets tar(target->GetGUID());
//	spp->prepare(&tar);
//	return 0;
//}
//
//int luaUnit_FullCastSpellOnTarget(lua_State * L, Unit * ptr)
//{
//	CHECK_TYPEID(TYPEID_UNIT);
//	uint32 sp = luaL_checkint(L, 1);
//	Unit * target = Lunar<Unit>::check(L, 2);
//
//	if(sp==0 || target==NULL)
//		return 0;
//
//	Spell * spp = new Spell(ptr, dbcSpell.LookupEntry(sp), false, NULL);
//	SpellCastTargets tar(target->GetGUID());
//	spp->prepare(&tar);
//	return 0;
//}
//
//int luaUnit_SpawnCreature(lua_State * L, Unit * ptr)
//{
//	if(ptr == NULL) return 0;
//	uint32 entry_id = luaL_checkint(L, 1);
//	double x = luaL_checknumber(L, 2);
//	double y = luaL_checknumber(L, 3);
//	double z = luaL_checknumber(L, 4);
//	double o = luaL_checknumber(L, 5);
//	uint32 faction = luaL_checkint(L, 6);
//	uint32 duration = luaL_checkint(L, 7);
//
//	if( !x || !y || !z || !entry_id || !faction /*|| !duration*/) //Shady: is it really required?
//	{
//		lua_pushnil(L);
//		return 1;
//	}
//
//	CreatureProto * proto = CreatureProtoStorage.LookupEntry(entry_id);
//	CreatureInfo * inf = CreatureNameStorage.LookupEntry(entry_id);
//	if(!proto || !inf)
//	{
//		lua_pushnil(L);
//		return 1;
//	}
//
//	Creature * pC = ptr->GetMapMgr()->CreateCreature();
//	pC->spawnid=0;
//	pC->m_spawn=0;
//	pC->Load(proto, (float)x, (float)y, (float)z);
//	float fo = (float)o;
//	pC->SetOrientation(fo);
//	pC->SetUInt32Value(UNIT_FIELD_FACTIONTEMPLATE, faction);
//	pC->_setFaction();
//	pC->SetMapId(ptr->GetMapId());
//	pC->SetInstanceID(ptr->GetInstanceID());
//	pC->PushToWorld(ptr->GetMapMgr());
//	if(duration)
//		pC->Despawn(duration, 0);
//
//	Lunar<Unit>::push(L, pC, false);
//	return 1;
//}
//
//int luaUnit_SpawnGameObject(lua_State * L, Unit * ptr)
//{
//	if(ptr == NULL) return 0;
//	uint32 entry_id = luaL_checkint(L, 1);
//	double x = luaL_checknumber(L, 2);
//	double y = luaL_checknumber(L, 3);
//	double z = luaL_checknumber(L, 4);
//	double o = luaL_checknumber(L, 5);
//	uint32 duration = luaL_checkint(L, 6);
//
//	if(!entry_id || !duration)
//	{
//		lua_pushnil(L);
//		return 1;
//	}
//
//	GameObjectInfo * goi = GameObjectNameStorage.LookupEntry(entry_id);
//	if(!goi)
//	{
//		lua_pushnil(L);
//		return 1;
//	}
//
//	GameObject * pC = ptr->GetMapMgr()->CreateGameObject();
//	pC->spawnid=0;
//	pC->m_spawn=0;
//	pC->CreateFromProto(entry_id, ptr->GetMapId(), (float)x, (float)y, (float)z, (float)o);
//	pC->SetMapId(ptr->GetMapId());
//	pC->SetInstanceID(ptr->GetInstanceID());
//	pC->PushToWorld(ptr->GetMapMgr());
//	if(duration)
//		sEventMgr.AddEvent(pC, &GameObject::ExpireAndDelete, EVENT_GAMEOBJECT_EXPIRE, duration, 1, EVENT_FLAG_DO_NOT_EXECUTE_IN_WORLD_CONTEXT);
//
//	Lunar<GameObject>::push(L, pC, false);
//	return 1;
//}
//
//int luaGameObject_GetName(lua_State * L, GameObject * ptr)
//{
//	if(!ptr||ptr->GetTypeId()!=TYPEID_GAMEOBJECT||!ptr->GetInfo())
//	{
//		lua_pushstring(L,"Unknown");
//		return 1;
//	}
//
//	lua_pushstring(L,ptr->GetInfo()->Name);
//	return 1;
//}
//
//int luaUnit_RegisterEvent(lua_State * L, Unit * ptr)
//{
//	if(!ptr||ptr->GetTypeId()!=TYPEID_UNIT)
//		return 0;
//
//	const char * func_to_call = luaL_checkstring(L,1);
//	int delay=luaL_checkint(L,2);
//	int repeats=luaL_checkint(L,3);
//
//	if(!func_to_call||!delay)
//		return 0;
//
//	Creature * pCreature = ((Creature*)ptr);
//	string strFunc = string(func_to_call);
//	sEventMgr.AddEvent(pCreature, &Creature::TriggerScriptEvent, strFunc, EVENT_CREATURE_UPDATE, (uint32)delay, (uint32)repeats, EVENT_FLAG_DO_NOT_EXECUTE_IN_WORLD_CONTEXT);
//	return 0;
//}
//int luaUnit_RemoveEvents(lua_State * L, Unit * ptr)
//{
//	if(!ptr||ptr->GetTypeId()!=TYPEID_UNIT)
//		return 0;
//
//	Creature * pCreature = ((Creature*)ptr);
//	sEventMgr.RemoveEvents(pCreature);
//	return 0;
//}
//
//int luaUnit_SetFaction(lua_State * L, Unit * ptr)
//{
//	CHECK_TYPEID(TYPEID_UNIT);
//	int faction = luaL_checkint(L,1);
//	if(!faction)
//		return 0;
//
//	ptr->SetUInt32Value(UNIT_FIELD_FACTIONTEMPLATE,faction);
//	ptr->_setFaction();
//	return 0;
//}
//int luaUnit_SetStandState(lua_State * L, Unit * ptr) //states 0..8
//{
//	if (!ptr)
//		return 0;
//	int state = luaL_checkint(L,1);
//	if(state<=0)
//		return 0;
//	ptr->SetStandState(state);
//	return 0;
//}
//int luaUnit_IsInCombat(lua_State * L, Unit * ptr)
//{
//	if (!ptr)
//		return 0;
//	lua_pushboolean(L,(ptr->CombatStatus.IsInCombat())?1:0);
//	return 1;
//}
//
//int luaUnit_SetScale(lua_State * L, Unit * ptr)
//{
//	CHECK_TYPEID(TYPEID_UNIT);
//	double faction = luaL_checknumber(L,1);
//	if(faction==0.0)
//		return 0;
//
//	ptr->SetFloatValue(OBJECT_FIELD_SCALE_X,(float)faction);
//	return 0;
//}
//
//int luaUnit_SetModel(lua_State * L, Unit * ptr)
//{
//	if(!ptr||!ptr->IsUnit())
//		return 0;
//
//	int modelid = luaL_checkint(L,1);
//	if(modelid==0)
//		return 0;
//
//	ptr->SetUInt32Value(UNIT_FIELD_DISPLAYID,modelid);
//	return 0;
//}
//
//int luaUnit_SetNPCFlags(lua_State * L, Unit * ptr)
//{
//	CHECK_TYPEID(TYPEID_UNIT);
//	int flags = luaL_checkint(L,1);
//	if(!flags)
//		return 0;
//
//	ptr->SetUInt32Value(UNIT_NPC_FLAGS,flags);
//	return 0;
//}
//
//int luaUnit_SetCombatCapable(lua_State * L, Unit * ptr)
//{
//	CHECK_TYPEID(TYPEID_UNIT);
//	int enabled = luaL_checkint(L,1);
//	ptr->GetAIInterface()->disable_combat = (enabled > 0) ? true : false;
//	return 0;
//}
//
//int luaUnit_SetCombatMeleeCapable(lua_State * L, Unit * ptr)
//{
//	CHECK_TYPEID(TYPEID_UNIT);
//	int enabled = luaL_checkint(L,1);
//	ptr->GetAIInterface()->disable_melee = (enabled > 0) ? true : false;
//	return 0;
//}
//
//int luaUnit_SetCombatRangedCapable(lua_State * L, Unit * ptr)
//{
//	CHECK_TYPEID(TYPEID_UNIT);
//	int enabled = luaL_checkint(L,1);
//	ptr->GetAIInterface()->disable_ranged = (enabled > 0) ? true : false;
//	return 0;
//}
//
//int luaUnit_SetCombatSpellCapable(lua_State * L, Unit * ptr)
//{
//	CHECK_TYPEID(TYPEID_UNIT);
//	int enabled = luaL_checkint(L,1);
//	ptr->GetAIInterface()->disable_spell = (enabled > 0) ? true : false;
//	return 0;
//}
//
//int luaUnit_SetCombatTargetingCapable(lua_State * L, Unit * ptr)
//{
//	CHECK_TYPEID(TYPEID_UNIT);
//	int enabled = luaL_checkint(L,1);
//	ptr->GetAIInterface()->disable_targeting = (enabled > 0) ? true : false;
//	return 0;
//}
//
//int luaUnit_DestroyCustomWaypointMap(lua_State * L, Unit * ptr)
//{
//	CHECK_TYPEID(TYPEID_UNIT);
//	((Creature*)ptr)->DestroyCustomWaypointMap();
//	return 0;
//}
//
//int luaUnit_CreateCustomWaypointMap(lua_State * L, Unit * ptr)
//{
//	CHECK_TYPEID(TYPEID_UNIT);
//	Creature * pCreature = (Creature*)ptr;
//	if(pCreature->m_custom_waypoint_map)
//	{
//		for(WayPointMap::iterator itr = pCreature->m_custom_waypoint_map->begin(); itr != pCreature->m_custom_waypoint_map->end(); ++itr)
//			delete (*itr);
//		delete pCreature->m_custom_waypoint_map;
//	}
//
//	pCreature->m_custom_waypoint_map = new WayPointMap;
//	pCreature->GetAIInterface()->SetWaypointMap(pCreature->m_custom_waypoint_map);
//	return 0;
//}
//
//int luaUnit_CreateWaypoint(lua_State * L, Unit * ptr)
//{
//	CHECK_TYPEID(TYPEID_UNIT);
//	float x = (float)luaL_checknumber(L,1);
//	float y = (float)luaL_checknumber(L,2);
//	float z = (float)luaL_checknumber(L,3);
//	float o = (float)luaL_checknumber(L,4);
//	int waittime = luaL_checkint(L,5);
//	int flags = luaL_checkint(L,6);
//	int modelid = luaL_checkint(L,7);
//
//	Creature * pCreature = ((Creature*)ptr);
//	if(!pCreature->m_custom_waypoint_map)
//	{
//		pCreature->m_custom_waypoint_map = new WayPointMap;
//		pCreature->GetAIInterface()->SetWaypointMap(pCreature->m_custom_waypoint_map);
//	}
//
//	if(!modelid)
//		modelid = pCreature->GetUInt32Value(UNIT_FIELD_DISPLAYID);
//
//	WayPoint * wp = new WayPoint;
//	wp->id = (uint32)pCreature->m_custom_waypoint_map->size() + 1;
//	wp->x = x;
//	wp->y = y;
//	wp->z = z;
//	wp->o = o;
//	wp->flags = flags;
//	wp->backwardskinid = modelid;
//	wp->forwardskinid = modelid;
//	wp->backwardemoteid = wp->forwardemoteid = 0;
//	wp->backwardemoteoneshot = wp->forwardemoteoneshot = false;
//	wp->waittime = waittime;
//	pCreature->m_custom_waypoint_map->push_back(wp);
//
//	return 0;
//}
//
//int luaUnit_MoveToWaypoint(lua_State * L, Unit * ptr)
//{
//	CHECK_TYPEID(TYPEID_UNIT);
//	int id = luaL_checkint(L,1);
//	//if(!id)
//	//	return 0;
//
//	ptr->MoveToWaypoint(id);
//	return 0;
//}
//
//int luaUnit_RemoveItem(lua_State * L, Unit * ptr)
//{
//	CHECK_TYPEID(TYPEID_PLAYER);
//	int id = luaL_checkint(L,1);
//	int count = luaL_checkint(L,2);
//
//	((Player*)ptr)->GetItemInterface()->RemoveItemAmt(id,count);
//	return 0;
//}
//
//int luaUnit_AddItem(lua_State * L, Unit * ptr)
//{
//	CHECK_TYPEID(TYPEID_PLAYER);
//	int id = luaL_checkint(L,1);
//	int count = luaL_checkint(L,2);
//
//	Player * plr = (Player*)ptr;
//	ItemPrototype * proto = ItemPrototypeStorage.LookupEntry(id);
//	if(proto==NULL)
//		return 0;
//
//	Item * add = plr->GetItemInterface()->FindItemLessMax(id,count,false);
//	if(add==NULL)
//	{
//		add=objmgr.CreateItem(id,plr);
//		add->SetUInt32Value(ITEM_FIELD_STACK_COUNT,count);
//		if(plr->GetItemInterface()->AddItemToFreeSlot(add))
//			plr->GetSession()->SendItemPushResult(add,false,true,false,true,plr->GetItemInterface()->LastSearchItemBagSlot(),plr->GetItemInterface()->LastSearchItemSlot(),count);
//		else
//			delete add;
//	}
//	else
//	{
//		add->ModUInt32Value(ITEM_FIELD_STACK_COUNT,count);
//		plr->GetSession()->SendItemPushResult(add,false,true,false,false,plr->GetItemInterface()->GetBagSlotByGuid(add->GetGUID()),0xFFFFFFFF,count);
//	}
//
//	return 0;
//}
//int luaUnit_GetInstanceID(lua_State * L, Unit * ptr)
//{
//	CHECK_TYPEID(TYPEID_UNIT);
//	/*lua_pushinteger(L,ptr->GetInstanceID());*///Commented cause don't work :)
//	return 1;
//}
//
//int luaUnit_GetClosestPlayer(lua_State * L, Unit * ptr)
//{
//	if(!ptr)
//		return 0;
//
//	float dist, d2;
//	Player * ret=NULL;
//
//	for(set<Player*>::iterator itr = ptr->GetInRangePlayerSetBegin(); itr != ptr->GetInRangePlayerSetEnd(); ++itr)
//	{
//		d2=(*itr)->GetDistanceSq(ptr);
//		if(!ret||d2<dist)
//		{
//			dist=d2;
//			ret=*itr;
//		}
//	}
//
//	if(ret==NULL)
//		lua_pushnil(L);
//	else
//		Lunar<Unit>::push(L,((Unit*)ret),false);
//
//	return 1;
//}
//int luaUnit_GetRandomPlayer(lua_State * L, Unit * ptr)
//{
//	if( ptr == NULL )
//		return 0;
//
//	int flag = luaL_checkint( L, 1 );
//
//	Player* ret = NULL;
//
//	switch( flag )
//	{
//	case RANDOM_ANY:
//		{
//			uint32 count = (uint32)ptr->GetInRangePlayersCount();
//			uint32 r = RandomUInt(count-1);
//			count=0;
//
//			for(set<Player*>::iterator itr = ptr->GetInRangePlayerSetBegin(); itr != ptr->GetInRangePlayerSetEnd(); ++itr)
//			{
//				if (count==r)
//				{
//					ret=*itr;
//					break;
//				}
//				++count;
//			}
//		}
//		break;
//	case RANDOM_IN_SHORTRANGE:
//		{
//			uint32 count = 0;
//			for(set<Player*>::iterator itr = ptr->GetInRangePlayerSetBegin(); itr != ptr->GetInRangePlayerSetEnd(); ++itr)
//			{
//				Player* obj = (Player*)(*itr);
//				if (obj && obj->CalcDistance(obj,ptr)<=8)
//					++count;
//			}
//			if (count)
//			{
//				uint32 r = RandomUInt(count-1);
//				count=0;
//				for(set<Player*>::iterator itr = ptr->GetInRangePlayerSetBegin(); itr != ptr->GetInRangePlayerSetEnd(); ++itr)
//				{
//					Player* obj = (Player*)(*itr);
//					if (obj && obj->CalcDistance(obj,ptr)<=8 && count==r)
//					{
//						ret=obj;
//						break;
//					}
//					++count;
//				}
//			}
//		}
//		break;
//	case RANDOM_IN_MIDRANGE:
//		{
//			uint32 count = 0;
//			for(set<Player*>::iterator itr = ptr->GetInRangePlayerSetBegin(); itr != ptr->GetInRangePlayerSetEnd(); ++itr)
//			{
//				Player* obj = (Player*)(*itr);
//				if (!obj)
//					continue;
//				float distance = obj->CalcDistance(obj,ptr);
//				if (distance<20 && distance>8)
//					++count;
//			}
//			if (count)
//			{
//				uint32 r = RandomUInt(count-1);
//				count=0;
//				for(set<Player*>::iterator itr = ptr->GetInRangePlayerSetBegin(); itr != ptr->GetInRangePlayerSetEnd(); ++itr)
//				{
//					Player* obj = (Player*)(*itr);
//					if (!obj)
//						continue;
//					float distance = obj->CalcDistance(obj,ptr);
//					if (distance<20 && distance>8 && count==r)
//					{
//						ret=obj;
//						break;
//					}
//					++count;
//				}
//			}
//		}
//		break;
//	case RANDOM_IN_LONGRANGE:
//		{
//			uint32 count = 0;
//			for(set<Player*>::iterator itr = ptr->GetInRangePlayerSetBegin(); itr != ptr->GetInRangePlayerSetEnd(); ++itr)
//			{
//				Player* obj = (Player*)(*itr);
//				if (obj && obj->CalcDistance(obj,ptr)>=20)
//					++count;
//			}
//			if (count)
//			{
//				uint32 r = RandomUInt(count-1);
//				count=0;
//				for(set<Player*>::iterator itr = ptr->GetInRangePlayerSetBegin(); itr != ptr->GetInRangePlayerSetEnd(); ++itr)
//				{
//					Player* obj = (Player*)(*itr);
//					if (obj && obj->CalcDistance(obj,ptr)>=20 && count==r)
//					{
//						ret=obj;
//						break;
//					}
//					++count;
//				}
//			}
//		}
//		break;
//	case RANDOM_WITH_MANA:
//		{
//			uint32 count = 0;
//			for(set<Player*>::iterator itr = ptr->GetInRangePlayerSetBegin(); itr != ptr->GetInRangePlayerSetEnd(); ++itr)
//			{
//				Player* obj = (Player*)(*itr);
//				if (obj && obj->GetPowerType() == POWER_TYPE_MANA)
//					++count;
//			}
//			if (count)
//			{
//				uint32 r = RandomUInt(count-1);
//				count=0;
//				for(set<Player*>::iterator itr = ptr->GetInRangePlayerSetBegin(); itr != ptr->GetInRangePlayerSetEnd(); ++itr)
//				{
//					Player* obj = (Player*)(*itr);
//					if (obj && obj->GetPowerType() == POWER_TYPE_MANA && count==r)
//					{
//						ret=obj;
//						break;
//					}
//					++count;
//				}
//			}
//		}
//		break;
//	case RANDOM_WITH_ENERGY:
//		{
//			uint32 count = 0;
//			for(set<Player*>::iterator itr = ptr->GetInRangePlayerSetBegin(); itr != ptr->GetInRangePlayerSetEnd(); ++itr)
//			{
//				Player* obj = (Player*)(*itr);
//				if (obj && obj->GetPowerType() == POWER_TYPE_ENERGY)
//					++count;
//			}
//			if (count)
//			{
//				uint32 r = RandomUInt(count-1);
//				count=0;
//				for(set<Player*>::iterator itr = ptr->GetInRangePlayerSetBegin(); itr != ptr->GetInRangePlayerSetEnd(); ++itr)
//				{
//					Player* obj = (Player*)(*itr);
//					if (obj && obj->GetPowerType() == POWER_TYPE_ENERGY && count==r)
//					{
//						ret=obj;
//						break;
//					}
//					++count;
//				}
//			}
//		}
//		break;
//	case RANDOM_WITH_RAGE:
//		{
//			uint32 count = 0;
//			for(set<Player*>::iterator itr = ptr->GetInRangePlayerSetBegin(); itr != ptr->GetInRangePlayerSetEnd(); ++itr)
//			{
//				Player* obj = (Player*)(*itr);
//				if (obj && obj->GetPowerType() == POWER_TYPE_RAGE)
//					++count;
//			}
//			if (count)
//			{
//				uint32 r = RandomUInt(count-1);
//				count=0;
//				for(set<Player*>::iterator itr = ptr->GetInRangePlayerSetBegin(); itr != ptr->GetInRangePlayerSetEnd(); ++itr)
//				{
//					Player* obj = (Player*)(*itr);
//					if (obj && obj->GetPowerType() == POWER_TYPE_RAGE && count==r)
//					{
//						ret=obj;
//						break;
//					}
//					++count;
//				}
//			}
//		}
//		break;
//	case RANDOM_NOT_MAINTANK:
//		{
//			uint32 count = 0;
//			Unit* mt = ptr->GetAIInterface()->GetMostHated();
//			if (!mt->IsPlayer())
//				return 0;
//
//			for(set<Player*>::iterator itr = ptr->GetInRangePlayerSetBegin(); itr != ptr->GetInRangePlayerSetEnd(); ++itr)
//			{
//				Player* obj = (Player*)(*itr);
//				if (obj != mt)
//					++count;
//			}
//			if (count)
//			{
//				uint32 r = RandomUInt(count-1);
//				count=0;
//				for(set<Player*>::iterator itr = ptr->GetInRangePlayerSetBegin(); itr != ptr->GetInRangePlayerSetEnd(); ++itr)
//				{
//					Player* obj = (Player*)(*itr);
//					if (obj && obj != mt && count==r)
//					{
//						ret=obj;
//						break;
//					}
//					++count;
//				}
//			}
//		}
//		break;
//	}
//
//	if(ret==NULL)
//		lua_pushnil(L);
//	else
//		Lunar<Unit>::push(L,((Unit*)ret),false);
//
//	return 1;
//}
//int luaUnit_GetRandomFriend(lua_State * L, Unit * ptr)
//{
//	if(!ptr)
//		return 0;
//
//	Unit * ret=NULL;
//	uint32 count = 0;
//
//	for(set<Object*>::iterator itr = ptr->GetInRangeSetBegin(); itr != ptr->GetInRangeSetEnd(); ++itr)
//	{
//		Object* obj = (Object*)(*itr);
//		if (obj->IsUnit() && isFriendly(obj,ptr))
//			++count;
//	}
//
//	if (count)
//	{
//		uint32 r = RandomUInt(count-1);
//		count=0;
//		for(set<Object*>::iterator itr = ptr->GetInRangeSetBegin(); itr != ptr->GetInRangeSetEnd(); ++itr)
//		{
//			Object* obj = (Object*)(*itr);
//			if (obj->IsUnit() && isFriendly(obj,ptr) && count==r)
//			{
//				ret=(Unit*)obj;
//				break;
//			}
//			++count;
//		}
//	}
//	if(ret==NULL)
//		lua_pushnil(L);
//	else
//		Lunar<Unit>::push(L,(ret),false);
//	return 1;
//}
//
//int luaUnit_StopMovement(lua_State * L, Unit * ptr)
//{
//	CHECK_TYPEID(TYPEID_UNIT);
//	int tim = luaL_checkint(L,1);
//	ptr->GetAIInterface()->StopMovement(tim);
//	return 0;
//}
//
//int luaUnit_RemoveAura(lua_State * L, Unit * ptr)
//{
//	if(!ptr)return 0;
//	int auraid = luaL_checkint(L,1);
//	ptr->RemoveAura(auraid);
//	return 0;
//}
//
//int luaUnit_PlaySoundToSet(lua_State * L, Unit * ptr)
//{
//	if(!ptr) return 0;
//	int soundid = luaL_checkint(L,1);
//	ptr->PlaySoundToSet(soundid);
//	return 0;
//}
//
//int luaUnit_GetUnitBySqlId(lua_State * L, Unit * ptr)
//{
//	if(!ptr) return 0;
//	int sqlid = luaL_checkint(L,1);
//	return 0;
//}
//
//int luaUnit_Despawn(lua_State * L, Unit * ptr)
//{
//	CHECK_TYPEID(TYPEID_UNIT);
//	int delay = luaL_checkint(L,1);
//	int respawntime = luaL_checkint(L,2);
//	((Creature*)ptr)->Despawn(delay,respawntime);
//	return 0;
//}
//
//int luaUnit_GetHealthPct(lua_State * L, Unit * ptr)
//{
//	if(!ptr)
//		lua_pushinteger(L,0);
//	else
//		lua_pushinteger(L, ptr->GetHealthPct());
//
//	return 1;
//}
//int luaUnit_SetHealthPct(lua_State * L, Unit * ptr)
//{
//	int val = luaL_checkint(L,1);
//	if (val>0)
//		ptr->SetHealthPct(val);
//	return 1;
//}
//int luaUnit_GetItemCount(lua_State * L, Unit * ptr)
//{
//	CHECK_TYPEID_RET_INT(TYPEID_PLAYER);
//	int itemid = luaL_checkint(L,1);
//	lua_pushinteger(L, ((Player*)ptr)->GetItemInterface()->GetItemCount(itemid,false));
//	return 1;
//}
//
//int luaUnit_GetMainTank(lua_State * L, Unit * ptr)
//{
//	CHECK_TYPEID_RET_INT(TYPEID_UNIT);
//	Unit* ret = ptr->GetAIInterface()->GetMostHated();
//	if(ret==NULL)
//		lua_pushnil(L);
//	else
//		Lunar<Unit>::push(L,(ret),false);
//	return 1;
//}
//int luaUnit_GetAddTank(lua_State * L, Unit * ptr)
//{
//	CHECK_TYPEID_RET_INT(TYPEID_UNIT);
//	Unit* ret = ptr->GetAIInterface()->GetSecondHated();
//	if(ret==NULL)
//		lua_pushnil(L);
//	else
//		Lunar<Unit>::push(L,(ret),false);
//	return 1;
//}
//int luaUnit_ClearThreatList(lua_State * L, Unit * ptr)
//{
//	CHECK_TYPEID_RET_INT(TYPEID_UNIT);
//	ptr->ClearHateList();
//	return 1;
//}
//
//int luaUnit_GetTauntedBy(lua_State * L, Unit * ptr)
//{
//	CHECK_TYPEID(TYPEID_UNIT)
//
//		if (!ptr->GetAIInterface()->getTauntedBy())
//			lua_pushnil(L);
//		else
//			Lunar<Unit>::push(L,ptr->GetAIInterface()->getTauntedBy(),false);
//	return 1;
//}
//int luaUnit_SetTauntedBy(lua_State * L, Unit * ptr)
//{
//	CHECK_TYPEID(TYPEID_UNIT)
//		Unit * target = Lunar<Unit>::check(L, 2);
//
//	if (!target || ptr->GetAIInterface()->GetIsTaunted() || target==ptr)
//		return 0;
//	else
//		ptr->GetAIInterface()->taunt(target);
//	return 1;
//}
//
//int luaUnit_GetSoulLinkedWith(lua_State * L, Unit * ptr)
//{
//	CHECK_TYPEID(TYPEID_UNIT)
//
//		if (!ptr->GetAIInterface()->GetIsSoulLinked())
//			lua_pushnil(L);
//		else
//			Lunar<Unit>::push(L,ptr->GetAIInterface()->getSoullinkedWith(),false);
//	return 1;
//}
//int luaUnit_SetSoulLinkedWith(lua_State * L, Unit * ptr)
//{
//	CHECK_TYPEID(TYPEID_UNIT)
//		Unit * target = Lunar<Unit>::check(L, 2);
//
//	if (!target || ptr->GetAIInterface()->GetIsSoulLinked() || target==ptr)
//		return 0;
//	else
//		ptr->GetAIInterface()->SetSoulLinkedWith(ptr);
//	return 1;
//}
//int luaUnit_ChangeTarget(lua_State * L, Unit * ptr)
//{
//	CHECK_TYPEID(TYPEID_UNIT)
//		Unit * target = Lunar<Unit>::check(L, 2);
//
//	if (!target || !isHostile(ptr,target) || ptr==target)
//		return 0;
//	else
//		ptr->GetAIInterface()->SetNextTarget(target);
//	return 1;
//}
//
//int luaUnit_HasFinishedQuest(lua_State * L, Unit * ptr)
//{
//	CHECK_TYPEID_RET(TYPEID_PLAYER);
//	int questid = luaL_checkint(L,1);
//	if(((Player*)ptr)->HasFinishedQuest(questid))
//		lua_pushboolean(L,1);
//	else
//		lua_pushboolean(L,0);
//
//	return 1;
//}
//
//int luaUnit_UnlearnSpell(lua_State * L, Unit * ptr)
//{
//	CHECK_TYPEID(TYPEID_PLAYER);
//	int spellid = luaL_checkint(L,1);
//	((Player*)ptr)->removeSpell(spellid,false,false,0);
//	return 0;
//}
//
//int luaUnit_LearnSpell(lua_State * L, Unit* ptr)
//{
//	CHECK_TYPEID(TYPEID_PLAYER);
//	int spellid = luaL_checkint(L,1);
//	((Player*)ptr)->addSpell(spellid);
//	return 0;
//}
//
//int luaUnit_MarkQuestObjectiveAsComplete(lua_State * L, Unit * ptr)
//{
//	CHECK_TYPEID(TYPEID_PLAYER);
//	int questid = luaL_checkint(L,1);
//	int objective = luaL_checkint(L,2);
//	Player * pl = ((Player*)ptr);
//	QuestLogEntry * qle = pl->GetQuestLogForEntry(questid);
//	qle->SetMobCount(objective, qle->GetQuest()->required_mobcount[objective]);
//	qle->SendUpdateAddKill(objective);
//	if(qle->CanBeFinished())
//		qle->SendQuestComplete();
//
//	return 0;
//}
//
//int luaUnit_KnockBack(lua_State * L, Unit * ptr)
//{
//	if(!ptr) return 0;
//	double dx = luaL_checknumber(L,1);
//	double dy = luaL_checknumber(L,2);
//	double affect1 = luaL_checknumber(L,3);
//	double affect2 = luaL_checknumber(L,4);
//	WorldPacket data(SMSG_MOVE_KNOCK_BACK, 30);
//	data << ptr->GetNewGUID();
//	data << getMSTime();
//	data << dx << dy << affect1 << affect2;
//	ptr->SendMessageToSet(&data, true);
//
//	return 0;
//}
//
//int luaUnit_SendAreaTriggerMessage(lua_State * L, Unit * ptr)
//{
//	CHECK_TYPEID(TYPEID_PLAYER);
//	const char * msg = luaL_checkstring(L,1);
//	if(!msg) return 0;
//	((Player*)ptr)->SendAreaTriggerMessage(msg);
//	return 0;
//}
//
//int luaUnit_SendBroadcastMessage(lua_State * L, Unit * ptr)
//{
//	CHECK_TYPEID(TYPEID_PLAYER);
//	const char * msg = luaL_checkstring(L,1);
//	if(!msg) return 0;
//	((Player*)ptr)->BroadcastMessage(msg);
//	return 0;
//}
//
//int luaUnit_TeleportUnit(lua_State * L, Unit * ptr)
//{
//	CHECK_TYPEID(TYPEID_PLAYER);
//	int mapId = luaL_checkint(L, 1);
//	float posX = (float)luaL_checknumber(L, 2);
//	float posY = (float)luaL_checknumber(L, 3);
//	float posZ = (float)luaL_checknumber(L, 4);
//	//if(!mapId || !posX || !posY || !posZ)
//	//	return 0;
//
//	LocationVector vec(posX, posY, posZ);
//	((Player*)ptr)->SafeTeleport((uint32)mapId, 0, vec);
//	return 0;
//}
//// Player Teleport (GO)
//int luaGameObject_Teleport(lua_State * L, GameObject * ptr)
//{
//	//CHECK_TYPEID(TYPEID_PLAYER);
//	CHECK_TYPEID(TYPEID_GAMEOBJECT);
//	Player* target = Lunar<Player>::check(L, 1);
//	int mapId = luaL_checkint(L, 2);
//	double posX = luaL_checknumber(L, 3);
//	double posY = luaL_checknumber(L, 4);
//	double posZ = luaL_checknumber(L, 5);
//	//if(!mapId || !posX || !posY || !posZ)
//	//	return 0;
//
//	LocationVector vec((float)posX, (float)posY, (float)posZ);
//	((Player*)target)->SafeTeleport((uint32)mapId, 0, vec);
//	return 0;
//}
//
