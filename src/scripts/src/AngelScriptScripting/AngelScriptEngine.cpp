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

AngelScriptEngine* g_engine = NULL;
ScriptMgr* g_scriptMgr = NULL;

asIScriptEngine* g_asEngine = NULL;
asIScriptContext* g_asEventCtx = NULL;
asIScriptContext* g_asCurrentCtx = NULL;

// -------------------

void RegisterUnitEvent( asIScriptGeneric* angel );
void RegisterGameObjectEvent( asIScriptGeneric* angel );
void RegisterQuestEvent( asIScriptGeneric* angel );

// -------------------

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

// -------------------

CreatureAIScript* CreateAngelScriptCreature( Creature* src )
{
	//LuaUnitBinding * pBinding = g_luaMgr.GetUnitBinding( src->GetEntry() );
	//if( pBinding == NULL )
	//	return NULL;

	//LuaCreature * pLua = new LuaCreature( src );
	//pLua->m_binding = pBinding;
	//return pLua;
}

GameObjectAIScript* CreateAngelScriptGameObject( GameObject* src )
{
	//LuaGameObjectBinding * pBinding = g_luaMgr.GetGameObjectBinding( src->GetEntry() );
	//if( pBinding == NULL )
	//	return NULL;

	//LuaGameObject * pLua = new LuaGameObject( src );
	//pLua->m_binding = pBinding;
	//return pLua;
}

QuestScript* CreateAngelScriptQuestScript( uint32 id )
{
	//LuaQuestBinding* pBinding = g_luaMgr.GetQuestBinding( id );
	//if( pBinding == NULL )
	//	return NULL;

	//LuaQuest * pLua = new LuaQuest();
	//pLua->m_binding = pBinding;
	//return pLua;
}

// -------------------

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

	// Register global functions
	g_asEngine->RegisterGlobalFunction("void RegisterUnitEvent( uint32 id, uint32 event_id )", asFUNCTION(RegisterUnitEvent), asCALL_GENERIC);
	//g_asEngine->RegisterGlobalFunction("uint GetSystemTime()", asFUNCTION(timeGetTime_Generic), asCALL_GENERIC);

	// Register class object types
	g_asEngine->RegisterObjectType( "Unit", sizeof( Unit ), 0 );
	g_asEngine->RegisterObjectType( "Object", sizeof( Object ), 0 );
	g_asEngine->RegisterObjectType( "MapMgr", sizeof( MapMgr ), 0 );
	g_asEngine->RegisterObjectType( "Player", sizeof( Player ), 0 );
	g_asEngine->RegisterObjectType( "Creature", sizeof( Creature ), 0 );
	g_asEngine->RegisterObjectType( "GameObject", sizeof( GameObject ), 0 );
	g_asEngine->RegisterObjectType( "AIInterface", sizeof( AIInterface ), 0 );
	g_asEngine->RegisterObjectType( "WayPointMap", sizeof( WayPointMap ), 0 );
	g_asEngine->RegisterObjectType( "CreatureAIScript", sizeof( CreatureAIScript ), 0 );
	g_asEngine->RegisterObjectType( "GameObjectAIScript", sizeof( GameObjectAIScript ), 0 );
	g_asEngine->RegisterObjectType( "QuestScript", sizeof( QuestScript ), 0 );
	g_asEngine->RegisterObjectType( "QuestLogEntry", sizeof( QuestLogEntry ), 0 );

	// Register Player class methods
	g_asEngine->RegisterObjectMethod( "Player", "void GetName()", asMETHOD( Player, GetName ), asCALL_THISCALL );
	g_asEngine->RegisterObjectMethod( "Player", "QuestLogEntry* GetQuestLogForEntry( uint32 quest )", asMETHOD( Player, GetQuestLogForEntry ), asCALL_THISCALL );

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

	// Register QuestLogEntry class methods
	g_asEngine->RegisterObjectMethod( "QuestLogEntry", "void SetMobCount( uint32 i, uint32 count )", asMETHOD( QuestLogEntry, SetMobCount ), asCALL_THISCALL );
	g_asEngine->RegisterObjectMethod( "QuestLogEntry", "void SendUpdateAddKill( uint32 i )", asMETHOD( QuestLogEntry, SendUpdateAddKill ), asCALL_THISCALL );
	g_asEngine->RegisterObjectMethod( "QuestLogEntry", "bool CanBeFinished()", asMETHOD( QuestLogEntry, CanBeFinished ), asCALL_THISCALL );
	g_asEngine->RegisterObjectMethod( "QuestLogEntry", "void SendQuestComplete()", asMETHOD( QuestLogEntry, SendQuestComplete ), asCALL_THISCALL );

	// Register Unit Hooks
	for( UnitBindingMap::iterator itr = m_unitBinding.begin(); itr != m_unitBinding.end(); ++itr )
	{
		//g_scriptMgr->register_creature_script( itr->first, CreateLuaCreature );
	}

	// Register Game Object Hooks
	for( GameObjectBindingMap::iterator itr = m_gameobjectBinding.begin(); itr != m_gameobjectBinding.end(); ++itr )
	{
		//g_scriptMgr->register_gameobject_script( itr->first, CreateLuaGameObject );
	}

	// Register Quest Hooks
	for( QuestBindingMap::iterator itr = m_questBinding.begin(); itr != m_questBinding.end(); ++itr )
	{
		//QuestScript* qs = CreateLuaQuestScript( itr->first );
		//if( qs != NULL )
		//	g_scriptMgr->register_quest_script( itr->first, qs );
	}

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

// -------------------

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

	int32 function_count = g_asEngine->GetFunctionCount( filename.c_str() );

	for( int32 i = 0; i < function_count; ++i )
	{
		int32 id = g_asEngine->GetFunctionIDByIndex( filename.c_str(), i );
		Log.Debug( "AngelScriptEngine", " - %s", g_asEngine->GetFunctionName( id ) );
	}

	//asIScriptContext* m_pContext = g_asEngine->CreateContext();
	//if( m_pContext == NULL )
	//{
	//	Log.Warning( "AngelScriptEngine", "Could not create context for %s!!!", filename.c_str() );
	//	return;
	//}
}

// -------------------

void RegisterUnitEvent( asIScriptGeneric* angel )
{
	uint32* unit_id = (uint32*)angel->GetArgAddress( 0 );
	uint32* event_id = (uint32*)angel->GetArgAddress( 1 );
}

void RegisterGameObjectEvent( asIScriptGeneric* angel )
{
	uint32* unit_id = (uint32*)angel->GetArgAddress( 0 );
	uint32* event_id = (uint32*)angel->GetArgAddress( 1 );
}

void RegisterQuestEvent( asIScriptGeneric* angel )
{
	uint32* unit_id = (uint32*)angel->GetArgAddress( 0 );
	uint32* event_id = (uint32*)angel->GetArgAddress( 1 );
}
