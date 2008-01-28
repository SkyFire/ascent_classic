#include "StdAfx.h"
#include "LUAEngine.h"
#include "LUAFunctions.h"
#include <CoreMemoryAllocator.cpp>

#if PLATFORM != PLATFORM_WIN32
#include <dirent.h>
#endif

bool lua_is_starting_up = false;
ScriptMgr * m_scriptMgr;
LuaEngineMgr g_luaMgr;
LuaEngine * g_engine;

extern "C" SCRIPT_DECL uint32 _exp_get_version()
{
	return MAKE_SCRIPT_VERSION(SCRIPTLIB_VERSION_MAJOR, SCRIPTLIB_VERSION_MINOR);
}

extern "C" SCRIPT_DECL uint32 _exp_get_script_type()
{
	return SCRIPT_TYPE_SCRIPT_ENGINE | SCRIPT_TYPE_SCRIPT_ENGINE_LUA;
}

extern "C" SCRIPT_DECL void _exp_script_register(ScriptMgr* mgr)
{
	m_scriptMgr = mgr;
	g_luaMgr.Startup();
}

template<typename T> const char * GetTClassName() { return "UNKNOWN"; }

template<>
const char * GetTClassName<Unit>()
{
	return "Unit";
}

template<>
const char * GetTClassName<GameObject>()
{
	return "GameObject";
}

template<typename T>
struct RegType
{
	const char * name;
	int(*mfunc)(lua_State*,T*);
};


/************************************************************************/
/* SCRIPT FUNCTION TABLES                                               */
/************************************************************************/
RegType<Unit> UnitMethods[] = {
	{ "GetName", &luaUnit_GetName },
	{ "SendChatMessage", &luaUnit_SendChatMessage },
	{ "MoveTo", &luaUnit_MoveTo },
	{ "SetMovementType", &luaUnit_SetMovementType },
	{ "CastSpell", &luaUnit_CastSpell },
	{ "FullCastSpell", &luaUnit_FullCastSpell },
	{ "CastSpellOnTarget", &luaUnit_CastSpell },
	{ "FullCastSpellOnTarget", &luaUnit_FullCastSpellOnTarget },
	{ "SpawnCreature", &luaUnit_SpawnCreature },
	{ "SpawnGameObject", &luaUnit_SpawnGameObject },
	{ "GetX", &luaUnit_GetX },
	{ "GetY", &luaUnit_GetY },
	{ "GetZ", &luaUnit_GetZ },
	{ "GetO", &luaUnit_GetO },
	{ "IsPlayer", &luaUnit_IsPlayer },
	{ "IsCreature", &luaUnit_IsCreature },
	{ "RegisterEvent", &luaUnit_RegisterEvent },
	{ "RemoveEvents", &luaUnit_RemoveEvents },
	{ "SendBroadcastMessage", &luaUnit_SendBroadcastMessage },
	{ "SendAreaTriggerMessage", &luaUnit_SendAreaTriggerMessage },
	{ "KnockBack", &luaUnit_KnockBack },
	{ "MarkQuestObjectiveAsComplete", &luaUnit_MarkQuestObjectiveAsComplete },
	{ "LearnSpell", &luaUnit_LearnSpell },
	{ "UnlearnSpell", &luaUnit_UnlearnSpell },
	{ "HasFinishedQuest", &luaUnit_HasFinishedQuest },
	{ "GetItemCount", &luaUnit_GetItemCount },
	{ "IsInCombat", &luaUnit_IsInCombat },
	{ "GetMainTank", &luaUnit_GetMainTank },
	{ "GetAddTank", &luaUnit_GetAddTank },
	{ "ClearThreatList", &luaUnit_ClearThreatList },
	{ "GetTauntedBy", &luaUnit_GetTauntedBy },
	{ "SetTauntedBy", &luaUnit_SetTauntedBy },
	{ "SetSoulLinkedWith", &luaUnit_SetSoulLinkedWith },
	{ "GetSoulLinkedWith", &luaUnit_GetSoulLinkedWith },
	{ "ChangeTarget", &luaUnit_ChangeTarget },
	{ "GetHealthPct", &luaUnit_GetHealthPct },
	{ "SetHealthPct", &luaUnit_SetHealthPct },
	{ "GetManaPct", &luaUnit_GetManaPct },
	{ "Despawn", &luaUnit_Despawn },
	{ "GetUnitBySqlId", &luaUnit_GetUnitBySqlId },
	{ "PlaySoundToSet", &luaUnit_PlaySoundToSet },
	{ "RemoveAura", &luaUnit_RemoveAura },
	{ "StopMovement", &luaUnit_StopMovement },
	{ "Emote", &luaUnit_Emote },
	{ "GetInstanceID", &luaUnit_GetInstanceID },
	{ "GetClosestPlayer", &luaUnit_GetClosestPlayer },
	{ "GetRandomPlayer", &luaUnit_GetRandomPlayer },
	{ "GetRandomFriend", &luaUnit_GetRandomFriend },
	{ "AddItem", &luaUnit_AddItem },
	{ "RemoveItem", &luaUnit_RemoveItem },
	{ "CreateCustomWaypointMap", &luaUnit_CreateCustomWaypointMap },
	{ "CreateWaypoint", &luaUnit_CreateWaypoint },
	{ "MoveToWaypoint", &luaUnit_MoveToWaypoint },
	{ "DestroyCustomWaypointMap", &luaUnit_DestroyCustomWaypointMap },
	{ "SetCombatCapable", &luaUnit_SetCombatCapable },
	{ "SetCombatMeleeCapable", &luaUnit_SetCombatMeleeCapable },
	{ "SetCombatRangedCapable", &luaUnit_SetCombatRangedCapable },
	{ "SetCombatSpellCapable", &luaUnit_SetCombatSpellCapable },
	{ "SetCombatTargetingCapable", &luaUnit_SetCombatTargetingCapable },
	{ "SetNPCFlags", &luaUnit_SetNPCFlags },
	{ "SetModel", &luaUnit_SetModel },
	{ "SetScale", &luaUnit_SetScale },
	{ "SetFaction", &luaUnit_SetFaction },
	{ "SetStandState",&luaUnit_SetStandState },
	{ "Teleport" , &luaUnit_TeleportUnit },
	{ NULL, NULL },
};

RegType<GameObject> GOMethods[] = {
	{ "GetName", &luaGameObject_GetName },
	{ "Teleport" , &luaGameObject_Teleport },
	{ NULL, NULL },
};

template<typename T> RegType<T>* GetMethodTable() { return NULL; }
template<>
RegType<Unit>* GetMethodTable<Unit>() { return UnitMethods; }

template<>
RegType<GameObject>* GetMethodTable<GameObject>() { return GOMethods; }

template <typename T> class Lunar {
	typedef struct { T *pT; } userdataType;
public:
	typedef int (*mfp)(lua_State *L, T* ptr);
	typedef struct { const char *name; mfp mfunc; } RegType;

	static void Register(lua_State *L) {
		lua_newtable(L);
		int methods = lua_gettop(L);

		luaL_newmetatable(L, GetTClassName<T>());
		int metatable = lua_gettop(L);

		// store method table in globals so that
		// scripts can add functions written in Lua.
		lua_pushvalue(L, methods);
		set(L, LUA_GLOBALSINDEX, GetTClassName<T>());

		// hide metatable from Lua getmetatable()
		lua_pushvalue(L, methods);
		set(L, metatable, "__metatable");

		lua_pushvalue(L, methods);
		set(L, metatable, "__index");

		lua_pushcfunction(L, tostring_T);
		set(L, metatable, "__tostring");

		lua_pushcfunction(L, gc_T);
		set(L, metatable, "__gc");

		lua_newtable(L);                // mt for method table
		lua_pushcfunction(L, new_T);
		lua_pushvalue(L, -1);           // dup new_T function
		set(L, methods, "new");         // add new_T to method table
		set(L, -3, "__call");           // mt.__call = new_T
		lua_setmetatable(L, methods);

		// fill method table with methods from class T
		for (RegType *l = ((RegType*)GetMethodTable<T>()); l->name; l++) {
			lua_pushstring(L, l->name);
			lua_pushlightuserdata(L, (void*)l);
			lua_pushcclosure(L, thunk, 1);
			lua_settable(L, methods);
		}

		lua_pop(L, 2);  // drop metatable and method table
	}

	// call named lua method from userdata method table
	static int call(lua_State *L, const char *method,
		int nargs=0, int nresults=LUA_MULTRET, int errfunc=0)
	{
		int base = lua_gettop(L) - nargs;  // userdata index
		if (!luaL_checkudata(L, base, T::className)) {
			lua_settop(L, base-1);           // drop userdata and args
			lua_pushfstring(L, "not a valid %s userdata", T::className);
			return -1;
		}

		lua_pushstring(L, method);         // method name
		lua_gettable(L, base);             // get method from userdata
		if (lua_isnil(L, -1)) {            // no method?
			lua_settop(L, base-1);           // drop userdata and args
			lua_pushfstring(L, "%s missing method '%s'", T::className, method);
			return -1;
		}
		lua_insert(L, base);               // put method under userdata, args

		int status = lua_pcall(L, 1+nargs, nresults, errfunc);  // call method
		if (status) {
			const char *msg = lua_tostring(L, -1);
			if (msg == NULL) msg = "(error with no message)";
			lua_pushfstring(L, "%s:%s status = %d\n%s",
				T::className, method, status, msg);
			lua_remove(L, base);             // remove old message
			return -1;
		}
		return lua_gettop(L) - base + 1;   // number of results
	}

	// push onto the Lua stack a userdata containing a pointer to T object
	static int push(lua_State *L, T *obj, bool gc=false) {
		if (!obj) { lua_pushnil(L); return 0; }
		luaL_getmetatable(L, GetTClassName<T>());  // lookup metatable in Lua registry
		if (lua_isnil(L, -1)) luaL_error(L, "%s missing metatable", GetTClassName<T>());
		int mt = lua_gettop(L);
		subtable(L, mt, "userdata", "v");
		userdataType *ud =
			static_cast<userdataType*>(pushuserdata(L, obj, sizeof(userdataType)));
		if (ud) {
			ud->pT = obj;  // store pointer to object in userdata
			lua_pushvalue(L, mt);
			lua_setmetatable(L, -2);
			if (gc == false) {
				lua_checkstack(L, 3);
				subtable(L, mt, "do not trash", "k");
				lua_pushvalue(L, -2);
				lua_pushboolean(L, 1);
				lua_settable(L, -3);
				lua_pop(L, 1);
			}
		}
		lua_replace(L, mt);
		lua_settop(L, mt);
		return mt;  // index of userdata containing pointer to T object
	}

	// get userdata from Lua stack and return pointer to T object
	static T *check(lua_State *L, int narg) {
		userdataType *ud =
			static_cast<userdataType*>(luaL_checkudata(L, narg, GetTClassName<T>()));
		if(!ud) { luaL_typerror(L, narg, GetTClassName<T>()); return NULL; }
		return ud->pT;  // pointer to T object
	}

private:
	Lunar();  // hide default constructor

	// member function dispatcher
	static int thunk(lua_State *L) {
		// stack has userdata, followed by method args
		T *obj = check(L, 1);  // get 'self', or if you prefer, 'this'
		lua_remove(L, 1);  // remove self so member function args start at index 1
		// get member function from upvalue
		RegType *l = static_cast<RegType*>(lua_touserdata(L, lua_upvalueindex(1)));
		//return (obj->*(l->mfunc))(L);  // call member function
		return l->mfunc(L,obj);
	}

	// create a new T object and
	// push onto the Lua stack a userdata containing a pointer to T object
	static int new_T(lua_State *L) {
		lua_remove(L, 1);   // use classname:new(), instead of classname.new()
		T *obj = NULL/*new T(L)*/;  // call constructor for T objects
		assert(false);
		push(L, obj, true); // gc_T will delete this object
		return 1;           // userdata containing pointer to T object
	}

	// garbage collection metamethod
	static int gc_T(lua_State *L) {
		if (luaL_getmetafield(L, 1, "do not trash")) {
			lua_pushvalue(L, 1);  // dup userdata
			lua_gettable(L, -2);
			if (!lua_isnil(L, -1)) return 0;  // do not delete object
		}
		userdataType *ud = static_cast<userdataType*>(lua_touserdata(L, 1));
		T *obj = ud->pT;
		if (obj) delete obj;  // call destructor for T objects
		return 0;
	}

	static int tostring_T (lua_State *L) {
		char buff[32];
		userdataType *ud = static_cast<userdataType*>(lua_touserdata(L, 1));
		T *obj = ud->pT;
		sprintf(buff, "%p", obj);
		lua_pushfstring(L, "%s (%s)", GetTClassName<T>(), buff);
		return 1;
	}

	static void set(lua_State *L, int table_index, const char *key) {
		lua_pushstring(L, key);
		lua_insert(L, -2);  // swap value and key
		lua_settable(L, table_index);
	}

	static void weaktable(lua_State *L, const char *mode) {
		lua_newtable(L);
		lua_pushvalue(L, -1);  // table is its own metatable
		lua_setmetatable(L, -2);
		lua_pushliteral(L, "__mode");
		lua_pushstring(L, mode);
		lua_settable(L, -3);   // metatable.__mode = mode
	}

	static void subtable(lua_State *L, int tindex, const char *name, const char *mode) {
		lua_pushstring(L, name);
		lua_gettable(L, tindex);
		if (lua_isnil(L, -1)) {
			lua_pop(L, 1);
			lua_checkstack(L, 3);
			weaktable(L, mode);
			lua_pushstring(L, name);
			lua_pushvalue(L, -2);
			lua_settable(L, tindex);
		}
	}

	static void *pushuserdata(lua_State *L, void *key, size_t sz) {
		void *ud = 0;
		lua_pushlightuserdata(L, key);
		lua_gettable(L, -2);     // lookup[key]
		if (lua_isnil(L, -1)) {
			lua_pop(L, 1);         // drop nil
			lua_checkstack(L, 3);
			ud = lua_newuserdata(L, sz);  // create new userdata
			lua_pushlightuserdata(L, key);
			lua_pushvalue(L, -2);  // dup userdata
			lua_settable(L, -4);   // lookup[key] = userdata
		}
		return ud;
	}
};

void report(lua_State * L)
{
	const char * msg= lua_tostring(L,-1);
	while(msg)
	{
		lua_pop(L,-1);
		printf("\t%s\n", msg);
		msg=lua_tostring(L,-1);
	}
}

LuaEngine::LuaEngine()
{
	this->L = lua_open();
}

LuaEngine::~LuaEngine()
{
	lua_close(L);
}

void LuaEngine::LoadScripts()
{
	set<string> luaFiles;
	set<string> luaBytecodeFiles;

#ifdef WIN32
	WIN32_FIND_DATA fd;
	HANDLE h;

	h = FindFirstFile("scripts\\*.*", &fd);
	if(h == INVALID_HANDLE_VALUE)
		return;

	do 
	{
		char * fn = strrchr(fd.cFileName, '\\');
		if(!fn)
			fn=fd.cFileName;
        char * ext = strrchr(fd.cFileName, '.');
		if(!stricmp(ext, ".lua"))
			luaFiles.insert(string(fn));
		else if(!stricmp(ext, ".luc"))
			luaBytecodeFiles.insert(string(fn));
	} while(FindNextFile(h, &fd));
	FindClose(h);
#else
	struct dirent ** list;
	int filecount = scandir("./scripts", &list, 0, 0);
	if(filecount <= 0 || !list)
		return;

	while(filecount--)
	{
		char* ext = strrchr(list[filecount]->d_name, '.');
		if(ext != NULL && !strcmp(ext, ".lua"))
			{
				string full_path = string(list[filecount]->d_name);
				luaFiles.insert(string(full_path.c_str()));
		}
		else if(!stricmp(ext, ".luc"))
		{
		string full_path = string(list[filecount]->d_name);
		luaBytecodeFiles.insert(string(full_path.c_str()));
		}

		free(list[filecount]);
	}
	free(list);
#endif

	// we prefer precompiled code.
	for(set<string>::iterator itr = luaBytecodeFiles.begin(); itr != luaBytecodeFiles.end(); ++itr)
	{
		set<string>::iterator it2 = luaFiles.find(*itr);
		if(it2 == luaFiles.end())
			luaFiles.erase(it2);
	}

	luaL_openlibs(L);
	RegisterCoreFunctions();

	if(lua_is_starting_up)
		Log.Notice("LuaEngine", "Loading Scripts...");

	char filename[200];

	for(set<string>::iterator itr = luaFiles.begin(); itr != luaFiles.end(); ++itr)
	{
#ifdef WIN32
			snprintf(filename, 200, "scripts\\%s", itr->c_str());
#else
			snprintf(filename, 200, "scripts/%s", itr->c_str());
#endif
		if(lua_is_starting_up)
			Log.Notice("LuaEngine", "%s...", itr->c_str());

		if(luaL_loadfile(L, filename) != 0)
		{
			printf("failed. (could not load)\n");
			const char * msg = lua_tostring(L, -1);
			if(msg!=NULL&&lua_is_starting_up)
				printf("\t%s\n",msg);
		}
		else
		{
			if(lua_pcall(L, 0, LUA_MULTRET, 0) != 0)
			{
				printf("failed. (could not run)\n");
				const char * msg = lua_tostring(L, -1);
				if(msg!=NULL&&lua_is_starting_up)
					printf("\t%s\n",msg);
			}
		}
	}
}

void LuaEngine::OnUnitEvent(Unit * pUnit, const char * FunctionName, uint32 EventType, Unit * pMiscUnit, uint32 Misc)
{
	if(FunctionName==NULL)
		return;

	m_Lock.Acquire();
	lua_pushstring(L, FunctionName);
	lua_gettable(L, LUA_GLOBALSINDEX);
	if(lua_isnil(L,-1))
	{
		printf("Tried to call invalid LUA function '%s' from Ascent (Unit)!\n", FunctionName);
		m_Lock.Release();
		return;
	}

	Lunar<Unit>::push(L, pUnit);
	lua_pushinteger(L,EventType);
	if(pMiscUnit!=NULL)
		Lunar<Unit>::push(L, pMiscUnit);
	else
		lua_pushnil(L);
	lua_pushinteger(L,Misc);
	
	int r = lua_pcall(L,4,LUA_MULTRET,0);
	if(r)
		report(L);

	m_Lock.Release();
}

void LuaEngine::OnQuestEvent(Player * QuestOwner, const char * FunctionName, uint32 QuestID, uint32 EventType, Object * QuestStarter)
{
	if(FunctionName==NULL)
		return;

	m_Lock.Acquire();
	lua_pushstring(L, FunctionName);
	lua_gettable(L, LUA_GLOBALSINDEX);
	if(lua_isnil(L,-1))
	{
		printf("Tried to call invalid LUA function '%s' from Ascent (Quest)!\n", FunctionName);
		m_Lock.Release();
		return;
	}

	if (QuestOwner)
		Lunar<Unit>::push(L, (Unit*)QuestOwner);
	else
		lua_pushnil(L);

	lua_pushinteger(L,EventType);

	if(QuestStarter!=NULL && QuestStarter->GetTypeId() == TYPEID_UNIT)
		Lunar<Unit>::push(L, (Unit*)QuestStarter);
	else
		lua_pushnil(L);

	int r = lua_pcall(L,3,LUA_MULTRET,0);
	if(r)
		report(L);

	m_Lock.Release();
	
}
void LuaEngine::CallFunction(Unit * pUnit, const char * FuncName)
{
	m_Lock.Acquire();
	lua_pushstring(L, FuncName);
	lua_gettable(L, LUA_GLOBALSINDEX);
	if(lua_isnil(L,-1))
	{
		printf("Tried to call invalid LUA function '%s' from Ascent (Unit)!\n", FuncName);
		m_Lock.Release();
		return;
	}

	Lunar<Unit>::push(L, pUnit);
	int r = lua_pcall(L,1,LUA_MULTRET,0);
	if(r)
		report(L);

	m_Lock.Release();
}

void LuaEngine::OnGameObjectEvent(GameObject * pGameObject, const char * FunctionName, uint32 EventType, Unit * pMiscUnit)
{
	if(FunctionName==NULL)
		return;

	m_Lock.Acquire();
	lua_pushstring(L, FunctionName);
	lua_gettable(L, LUA_GLOBALSINDEX);
	if(lua_isnil(L,-1))
	{
		printf("Tried to call invalid LUA function '%s' from Ascent! (GO)\n", FunctionName);
		m_Lock.Release();
		return;
	}

	Lunar<GameObject>::push(L, pGameObject);
	lua_pushinteger(L,EventType);
	if(!pMiscUnit)
		lua_pushnil(L);
	else
		Lunar<Unit>::push(L, pMiscUnit);

	int r = lua_pcall(L,3,LUA_MULTRET,0);
	if(r)
		report(L);

	m_Lock.Release();
}

static int RegisterUnitEvent(lua_State * L);
static int RegisterQuestEvent(lua_State * L);
static int RegisterGameObjectEvent(lua_State * L);

void LuaEngine::RegisterCoreFunctions()
{
	lua_pushcfunction(L, &RegisterUnitEvent);
	lua_setglobal(L, "RegisterUnitEvent");

	lua_pushcfunction(L, &RegisterGameObjectEvent);
	lua_setglobal(L, "RegisterGameObjectEvent");

	lua_pushcfunction(L, &RegisterQuestEvent);
	lua_setglobal(L, "RegisterQuestEvent");

	Lunar<Unit>::Register(L);
	Lunar<GameObject>::Register(L);
	//Lunar<Quest>::Register(L); quest isn't a class
}

static int RegisterUnitEvent(lua_State * L)
{
	int entry = luaL_checkint(L, 1);
	int ev = luaL_checkint(L, 2);
	const char * str = luaL_checkstring(L, 3);

	if(!entry || !ev || !str || !lua_is_starting_up)
		return 0;

	g_luaMgr.RegisterUnitEvent(entry,ev,str);
	return 0;
}

static int RegisterQuestEvent(lua_State * L)
{
	int entry = luaL_checkint(L, 1);
	int ev = luaL_checkint(L, 2);
	const char * str = luaL_checkstring(L, 3);

	if(!entry || !ev || !str || !lua_is_starting_up)
		return 0;

	g_luaMgr.RegisterQuestEvent(entry,ev,str);
	return 0;
}

static int RegisterGameObjectEvent(lua_State * L)
{
	int entry = luaL_checkint(L, 1);
	int ev = luaL_checkint(L, 2);
	const char * str = luaL_checkstring(L, 3);

	if(!entry || !ev || !str || !lua_is_starting_up)
		return 0;

	g_luaMgr.RegisterGameObjectEvent(entry,ev,str);
	return 0;
}

/************************************************************************/
/* Manager Stuff                                                        */
/************************************************************************/

class LuaCreature : public CreatureAIScript
{
public:
	LuaCreature(Creature* creature) : CreatureAIScript(creature) {};
	~LuaCreature() {};

	void OnCombatStart(Unit* mTarget)
	{
		if( m_binding->Functions[CREATURE_EVENT_ON_ENTER_COMBAT] != NULL )
			g_engine->OnUnitEvent( _unit, m_binding->Functions[CREATURE_EVENT_ON_ENTER_COMBAT], CREATURE_EVENT_ON_ENTER_COMBAT, mTarget, 0 );
	}

	void OnCombatStop(Unit* mTarget)
	{
		if( m_binding->Functions[CREATURE_EVENT_ON_LEAVE_COMBAT] != NULL )
			g_engine->OnUnitEvent( _unit, m_binding->Functions[CREATURE_EVENT_ON_LEAVE_COMBAT], CREATURE_EVENT_ON_LEAVE_COMBAT, mTarget, 0 );
	}

	void OnTargetDied(Unit* mTarget)
	{
		if( m_binding->Functions[CREATURE_EVENT_ON_KILLED_TARGET] != NULL )
			g_engine->OnUnitEvent( _unit, m_binding->Functions[CREATURE_EVENT_ON_KILLED_TARGET], CREATURE_EVENT_ON_KILLED_TARGET, mTarget, 0 );
	}

	void OnDied(Unit *mKiller)
	{
		if( m_binding->Functions[CREATURE_EVENT_ON_DIED] != NULL )
			g_engine->OnUnitEvent( _unit, m_binding->Functions[CREATURE_EVENT_ON_DIED], CREATURE_EVENT_ON_DIED, mKiller, 0 );
	}

	void OnLoad()
	{
		if( m_binding->Functions[CREATURE_EVENT_ON_SPAWN] != NULL )
			g_engine->OnUnitEvent( _unit, m_binding->Functions[CREATURE_EVENT_ON_SPAWN], CREATURE_EVENT_ON_SPAWN, NULL, 0 );
	}

	void OnReachWP(uint32 iWaypointId, bool bForwards)
	{
		if( m_binding->Functions[CREATURE_EVENT_ON_REACH_WP] != NULL )
			g_engine->OnUnitEvent( _unit, m_binding->Functions[CREATURE_EVENT_ON_REACH_WP], CREATURE_EVENT_ON_REACH_WP, NULL, iWaypointId );
	}

	void AIUpdate()
	{
		if( m_binding->Functions[CREATURE_EVENT_AI_TICK] != NULL )
			g_engine->OnUnitEvent( _unit, m_binding->Functions[CREATURE_EVENT_AI_TICK], CREATURE_EVENT_AI_TICK, NULL, 0 );
	}

	void StringFunctionCall(const char * pFunction)
	{
		g_engine->CallFunction( _unit, pFunction );
	}

	void Destroy()
	{
		delete this;
	}

	LuaUnitBinding * m_binding;
};

class LuaGameObject : public GameObjectAIScript
{
public:
	LuaGameObject(GameObject * go) : GameObjectAIScript(go) {}
	~LuaGameObject() {}

	void OnSpawn()
	{
		if( m_binding->Functions[GAMEOBJECT_EVENT_ON_SPAWN] != NULL )
			g_engine->OnGameObjectEvent( _gameobject, m_binding->Functions[GAMEOBJECT_EVENT_ON_SPAWN], GAMEOBJECT_EVENT_ON_SPAWN, NULL );
	}

	void OnActivate(Player * pPlayer)
	{
		if( m_binding->Functions[GAMEOBJECT_EVENT_ON_USE] != NULL )
			g_engine->OnGameObjectEvent( _gameobject, m_binding->Functions[GAMEOBJECT_EVENT_ON_USE], GAMEOBJECT_EVENT_ON_USE, pPlayer );
	}

	LuaGameObjectBinding * m_binding;
};

class LuaQuest : public QuestScript
{
public:
	LuaQuest() : QuestScript() {}
	~LuaQuest() {}

	void OnQuestStart(Player* mTarget, QuestLogEntry *qLogEntry)
	{
		if( m_binding->Functions[QUEST_EVENT_ON_ACCEPT] != NULL )
			g_engine->OnQuestEvent( mTarget, m_binding->Functions[QUEST_EVENT_ON_ACCEPT], qLogEntry->GetQuest()->id, QUEST_EVENT_ON_ACCEPT, mTarget );
	}

	void OnQuestComplete(Player* mTarget, QuestLogEntry *qLogEntry)
	{
		if( m_binding->Functions[QUEST_EVENT_ON_COMPLETE] != NULL )
			g_engine->OnQuestEvent( mTarget, m_binding->Functions[QUEST_EVENT_ON_COMPLETE], qLogEntry->GetQuest()->id, QUEST_EVENT_ON_COMPLETE, mTarget );
	}

	LuaQuestBinding * m_binding;
};

CreatureAIScript * CreateLuaCreature(Creature * src)
{
	LuaUnitBinding * pBinding = g_luaMgr.GetUnitBinding( src->GetEntry() );
	if( pBinding == NULL )
		return NULL;

	LuaCreature * pLua = new LuaCreature( src );
	pLua->m_binding = pBinding;
	return pLua;
}

GameObjectAIScript * CreateLuaGameObject(GameObject * src)
{
	LuaGameObjectBinding * pBinding = g_luaMgr.GetGameObjectBinding( src->GetEntry() );
	if( pBinding == NULL )
		return NULL;

	LuaGameObject * pLua = new LuaGameObject( src );
	pLua->m_binding = pBinding;
	return pLua;
}

QuestScript * CreateLuaQuestScript(uint32 id)
{
	LuaQuestBinding * pBinding = g_luaMgr.GetQuestBinding( id );
	if( pBinding == NULL )
		return NULL;

	LuaQuest * pLua = new LuaQuest();
	pLua->m_binding = pBinding;
	return pLua;
}

void LuaEngineMgr::Startup()
{
	Log.Notice("LuaEngineMgr", "Spawning Lua Engine...");
	m_engine = new LuaEngine();
	lua_is_starting_up = true;
	m_engine->LoadScripts();
	g_engine = m_engine;
	lua_is_starting_up = false;

	// stuff is registered, so lets go ahead and make our emulated C++ scripted lua classes.
	for(UnitBindingMap::iterator itr = m_unitBinding.begin(); itr != m_unitBinding.end(); ++itr)
	{
		m_scriptMgr->register_creature_script( itr->first, CreateLuaCreature );
	}

	for(GameObjectBindingMap::iterator itr = m_gameobjectBinding.begin(); itr != m_gameobjectBinding.end(); ++itr)
	{
		m_scriptMgr->register_gameobject_script( itr->first, CreateLuaGameObject );
	}

	for(QuestBindingMap::iterator itr = m_questBinding.begin(); itr != m_questBinding.end(); ++itr)
	{
		QuestScript * qs = CreateLuaQuestScript( itr->first );
		if( qs != NULL )
			m_scriptMgr->register_quest_script( itr->first, qs );
	}
}

void LuaEngineMgr::RegisterUnitEvent(uint32 Id, uint32 Event, const char * FunctionName)
{
	UnitBindingMap::iterator itr = m_unitBinding.find(Id);
	if(itr == m_unitBinding.end())
	{
		LuaUnitBinding ub;
		memset(&ub,0,sizeof(LuaUnitBinding));
		ub.Functions[Event] = strdup(FunctionName);
		m_unitBinding.insert(make_pair(Id,ub));
	}
	else
	{
		if(itr->second.Functions[Event]!=NULL)
			free((void*)itr->second.Functions[Event]);

		itr->second.Functions[Event]=strdup(FunctionName);
	}
}

void LuaEngineMgr::RegisterQuestEvent(uint32 Id, uint32 Event, const char * FunctionName)
{
	QuestBindingMap::iterator itr = m_questBinding.find(Id);
	if(itr == m_questBinding.end())
	{
		LuaQuestBinding qb;
		memset(&qb,0,sizeof(LuaQuestBinding));
		qb.Functions[Event] = strdup(FunctionName);
		m_questBinding.insert(make_pair(Id,qb));
	}
	else
	{
		if(itr->second.Functions[Event]!=NULL)
			free((void*)itr->second.Functions[Event]);

		itr->second.Functions[Event]=strdup(FunctionName);
	}
}
void LuaEngineMgr::RegisterGameObjectEvent(uint32 Id, uint32 Event, const char * FunctionName)
{
	GameObjectBindingMap::iterator itr = m_gameobjectBinding.find(Id);
	if(itr == m_gameobjectBinding.end())
	{
		LuaGameObjectBinding ub;
		memset(&ub,0,sizeof(LuaGameObjectBinding));
		ub.Functions[Event] = strdup(FunctionName);
		m_gameobjectBinding.insert(make_pair(Id,ub));
	}
	else
	{
		if(itr->second.Functions[Event]!=NULL)
			free((void*)itr->second.Functions[Event]);

		itr->second.Functions[Event]=strdup(FunctionName);
	}
}

/*void LuaEngineMgr::ReloadScripts()
{
	m_lock.Acquire();

	// acquire the locks on all the luaengines so they don't do anything.
	for(LuaEngineMap::iterator itr = m_engines.begin(); itr != m_engines.end(); ++itr)
		itr->first->GetLock().Acquire();

	// remove all the function name bindings
	for(UnitBindingMap::iterator itr = m_unitBinding.begin(); itr != m_unitBinding.end(); ++itr)
	{
		for(uint32 i = 0; i < CREATURE_EVENT_COUNT; ++i)
			if(itr->second.Functions[i] != NULL)
				free((void*)itr->second.Functions[i]);
	}
	
	for(GameObjectBindingMap::iterator itr = m_gameobjectBinding.begin(); itr != m_gameobjectBinding.end(); ++itr)
	{
		for(uint32 i = 0; i < GAMEOBJECT_EVENT_COUNT; ++i)
			if(itr->second.Functions[i] != NULL)
				free((void*)itr->second.Functions[i]);
	}

	// clear the maps
	m_gameobjectBinding.clear();
	m_unitBinding.clear();

	// grab the first lua engine in the list, use it to re-create all the binding names.
	LuaEngine * l = m_engines.begin()->first;
	lua_is_starting_up = true;
	l->Restart();
	lua_is_starting_up = false;

	// all our bindings have been re-created, go through the lua engines and restart them all, and then release their locks.
	for(LuaEngineMap::iterator itr = m_engines.begin(); itr != m_engines.end(); ++itr)
	{
		if(itr->first != l)		// this one is already done
		{
			itr->first->Restart();
			itr->first->GetLock().Release();
		}
	}

	// release the big lock
	m_lock.Release();
}*/

void LuaEngineMgr::Unload()
{
	/*m_lock.Acquire();
	for(LuaEngineMap::iterator itr = m_engines.begin(); itr != m_engines.end(); ++itr)
	{
		delete itr->first;
	}
	m_lock.Release();*/
}

void LuaEngine::Restart()
{
	m_Lock.Acquire();
	lua_close(L);
	L = lua_open();
	LoadScripts();
	m_Lock.Release();
}




/************************************************************************/
/* SCRIPT FUNCTION IMPLEMENTATION                                       */
/************************************************************************/
#define CHECK_TYPEID(expected_type) if(!ptr || !ptr->IsInWorld() || ptr->GetTypeId() != expected_type) { return 0; }
#define CHECK_TYPEID_RET(expected_type) if(!ptr || !ptr->IsInWorld() || ptr->GetTypeId() != expected_type) { lua_pushboolean(L,0); return 0; }
#define CHECK_TYPEID_RET_INT(expected_type) if(!ptr || !ptr->IsInWorld() || ptr->GetTypeId() != expected_type) { lua_pushinteger(L,0); return 0; }

int luaUnit_IsPlayer(lua_State * L, Unit * ptr)
{
	if(!ptr)
	{
		lua_pushboolean(L, 0);
		return 1;
	}

	if(ptr->IsPlayer())
		lua_pushboolean(L, 1);
	else
		lua_pushboolean(L, 0);

	return 1;
}

int luaUnit_IsCreature(lua_State * L, Unit * ptr)
{
	if(!ptr)
	{
		lua_pushboolean(L, 0);
		return 1;
	}

	if(ptr->GetTypeId()==TYPEID_UNIT)
		lua_pushboolean(L, 1);
	else
		lua_pushboolean(L, 0);

	return 1;
}

int luaUnit_Emote(lua_State * L, Unit * ptr)
{
	if(ptr==NULL) return 0;
	uint32 emote_id = luaL_checkint(L, 1);
	uint32 time = luaL_checkint(L, 1);
	if(emote_id==NULL) 
		return 0;
	if (time)
		ptr->EventAddEmote((EmoteType)emote_id,time);
	else
		ptr->Emote((EmoteType)emote_id);
	return 1;
}

int luaUnit_GetManaPct(lua_State * L, Unit * ptr)
{
	if(!ptr) 
		return 0;
	if (ptr->GetPowerType() == POWER_TYPE_MANA)
		lua_pushnumber(L, (int)(ptr->GetUInt32Value(UNIT_FIELD_POWER1) * 100.0f / ptr->GetUInt32Value(UNIT_FIELD_MAXPOWER1)));
	else
		lua_pushnil(L);
	return 1;
}

int luaUnit_GetName(lua_State * L, Unit * ptr)
{
	if(!ptr)
		return 0;

	switch(ptr->GetTypeId())
	{
	case TYPEID_UNIT:
		lua_pushstring(L, ((Creature*)ptr)->GetCreatureName() ? ((Creature*)ptr)->GetCreatureName()->Name : "Unknown");
		break;

	case TYPEID_PLAYER:
		lua_pushstring(L, ((Player*)ptr)->GetName());
		break;

	default:
		lua_pushstring(L, "Unknown");
		break;
	}

	return 1;
}

int luaUnit_SendChatMessage(lua_State * L, Unit * ptr)
{
	CHECK_TYPEID(TYPEID_UNIT);
	uint32 typ = luaL_checkint(L, 1);
	uint32 lang = luaL_checkint(L, 2);
	const char * message = luaL_checklstring(L, 3, NULL);
	if(message == NULL)
		return 0;

	ptr->SendChatMessage(typ, lang, message);
	return 0;
}

int luaUnit_MoveTo(lua_State * L, Unit * ptr)
{
	CHECK_TYPEID(TYPEID_UNIT);
	double x = luaL_checknumber(L, 1);
	double y = luaL_checknumber(L, 2);
	double z = luaL_checknumber(L, 3);
	double o = luaL_checknumber(L, 4);

	if(x == 0 || y == 0 || z == 0)
		return 0;

	ptr->GetAIInterface()->MoveTo( (float)x, (float)y, (float)z, (float)o );
	return 0;
}

int luaUnit_SetMovementType(lua_State * L, Unit * ptr)
{
	CHECK_TYPEID(TYPEID_UNIT);
	uint32 typ = luaL_checkint(L, 1);
	ptr->GetAIInterface()->setMoveType(typ);
	return 0;
}

int luaUnit_GetX(lua_State * L, Unit * ptr)
{
	if(ptr==NULL) return 0;
	lua_pushnumber(L, (double)ptr->GetPositionX());
	return 1;
}

int luaUnit_GetY(lua_State * L, Unit * ptr)
{
	if(ptr==NULL) return 0;
	lua_pushnumber(L, (double)ptr->GetPositionY());
	return 1;
}

int luaUnit_GetZ(lua_State * L, Unit * ptr)
{
	if(ptr==NULL) return 0;
	lua_pushnumber(L, (double)ptr->GetPositionZ());
	return 1;
}

int luaUnit_GetO(lua_State * L, Unit * ptr)
{
	if(ptr==NULL) return 0;
	lua_pushnumber(L, (double)ptr->GetOrientation());
	return 1;
}

int luaUnit_CastSpell(lua_State * L, Unit * ptr)
{
	CHECK_TYPEID(TYPEID_UNIT);
	uint32 sp = luaL_checkint(L, 1);

	if(sp==0)
		return 0;

	Spell * spp = new Spell(ptr, dbcSpell.LookupEntry(sp), true, NULL);
	SpellCastTargets tar(ptr->GetGUID());
	spp->prepare(&tar);
	return 0;
}

int luaUnit_FullCastSpell(lua_State * L, Unit * ptr)
{
	CHECK_TYPEID(TYPEID_UNIT);
	uint32 sp = luaL_checkint(L, 1);

	if(sp==0)
		return 0;

	Spell * spp = new Spell(ptr, dbcSpell.LookupEntry(sp), false, NULL);
	SpellCastTargets tar(ptr->GetGUID());
	spp->prepare(&tar);
	return 0;
}

int luaUnit_CastSpellOnTarget(lua_State * L, Unit * ptr)
{
	CHECK_TYPEID(TYPEID_UNIT);
	uint32 sp = luaL_checkint(L, 1);
	Unit * target = Lunar<Unit>::check(L, 2);

	if(sp==0 || target==NULL)
		return 0;

	Spell * spp = new Spell(ptr, dbcSpell.LookupEntry(sp), false, NULL);
	SpellCastTargets tar(target->GetGUID());
	spp->prepare(&tar);
	return 0;
}

int luaUnit_FullCastSpellOnTarget(lua_State * L, Unit * ptr)
{
	CHECK_TYPEID(TYPEID_UNIT);
	uint32 sp = luaL_checkint(L, 1);
	Unit * target = Lunar<Unit>::check(L, 2);

	if(sp==0 || target==NULL)
		return 0;

	Spell * spp = new Spell(ptr, dbcSpell.LookupEntry(sp), false, NULL);
	SpellCastTargets tar(target->GetGUID());
	spp->prepare(&tar);
	return 0;
}

int luaUnit_SpawnCreature(lua_State * L, Unit * ptr)
{
	if(ptr == NULL) return 0;
	uint32 entry_id = luaL_checkint(L, 1);
	double x = luaL_checknumber(L, 2);
	double y = luaL_checknumber(L, 3);
	double z = luaL_checknumber(L, 4);
	double o = luaL_checknumber(L, 5);
	uint32 faction = luaL_checkint(L, 6);
	uint32 duration = luaL_checkint(L, 7);

	if( !x || !y || !z || !entry_id || !faction /*|| !duration*/) //Shady: is it really required?
	{
		lua_pushnil(L);
		return 1;
	}

	CreatureProto * proto = CreatureProtoStorage.LookupEntry(entry_id);
	CreatureInfo * inf = CreatureNameStorage.LookupEntry(entry_id);
	if(!proto || !inf)
	{
		lua_pushnil(L);
		return 1;
	}

	Creature * pC = ptr->GetMapMgr()->CreateCreature();
	pC->spawnid=0;
	pC->m_spawn=0;
	pC->Load(proto, (float)x, (float)y, (float)z);
	float fo = (float)o;
	pC->SetOrientation(fo);
	pC->SetUInt32Value(UNIT_FIELD_FACTIONTEMPLATE, faction);
	pC->_setFaction();
	pC->SetMapId(ptr->GetMapId());
	pC->SetInstanceID(ptr->GetInstanceID());
	pC->PushToWorld(ptr->GetMapMgr());
	if(duration)
		pC->Despawn(duration, 0);

	Lunar<Unit>::push(L, pC, false);
	return 1;
}

int luaUnit_SpawnGameObject(lua_State * L, Unit * ptr)
{
	if(ptr == NULL) return 0;
	uint32 entry_id = luaL_checkint(L, 1);
	double x = luaL_checknumber(L, 2);
	double y = luaL_checknumber(L, 3);
	double z = luaL_checknumber(L, 4);
	double o = luaL_checknumber(L, 5);
	uint32 duration = luaL_checkint(L, 6);

	if(!entry_id || !duration)
	{
		lua_pushnil(L);
		return 1;
	}

	GameObjectInfo * goi = GameObjectNameStorage.LookupEntry(entry_id);
	if(!goi)
	{
		lua_pushnil(L);
		return 1;
	}

	GameObject * pC = ptr->GetMapMgr()->CreateGameObject();
	pC->spawnid=0;
	pC->m_spawn=0;
	pC->CreateFromProto(entry_id, ptr->GetMapId(), (float)x, (float)y, (float)z, (float)o);
	pC->SetMapId(ptr->GetMapId());
	pC->SetInstanceID(ptr->GetInstanceID());
	pC->PushToWorld(ptr->GetMapMgr());
	if(duration)
		sEventMgr.AddEvent(pC, &GameObject::ExpireAndDelete, EVENT_GAMEOBJECT_EXPIRE, duration, 1, EVENT_FLAG_DO_NOT_EXECUTE_IN_WORLD_CONTEXT);

	Lunar<GameObject>::push(L, pC, false);
	return 1;
}

int luaGameObject_GetName(lua_State * L, GameObject * ptr)
{
	if(!ptr||ptr->GetTypeId()!=TYPEID_GAMEOBJECT||!ptr->GetInfo())
	{
		lua_pushstring(L,"Unknown");
		return 1;
	}

	lua_pushstring(L,ptr->GetInfo()->Name);
	return 1;
}

int luaUnit_RegisterEvent(lua_State * L, Unit * ptr)
{
	if(!ptr||ptr->GetTypeId()!=TYPEID_UNIT)
		return 0;

	const char * func_to_call = luaL_checkstring(L,1);
	int delay=luaL_checkint(L,2);
	int repeats=luaL_checkint(L,3);

	if(!func_to_call||!delay)
		return 0;

	Creature * pCreature = ((Creature*)ptr);
	string strFunc = string(func_to_call);
	sEventMgr.AddEvent(pCreature, &Creature::TriggerScriptEvent, strFunc, EVENT_CREATURE_UPDATE, (uint32)delay, (uint32)repeats, EVENT_FLAG_DO_NOT_EXECUTE_IN_WORLD_CONTEXT);
	return 0;
}
int luaUnit_RemoveEvents(lua_State * L, Unit * ptr)
{
	if(!ptr||ptr->GetTypeId()!=TYPEID_UNIT)
		return 0;

	Creature * pCreature = ((Creature*)ptr);
	sEventMgr.RemoveEvents(pCreature);
	return 0;
}

int luaUnit_SetFaction(lua_State * L, Unit * ptr)
{
	CHECK_TYPEID(TYPEID_UNIT);
	int faction = luaL_checkint(L,1);
	if(!faction)
		return 0;

	ptr->SetUInt32Value(UNIT_FIELD_FACTIONTEMPLATE,faction);
	ptr->_setFaction();
	return 0;
}
int luaUnit_SetStandState(lua_State * L, Unit * ptr) //states 0..8
{
	if (!ptr)
		return 0;
	int state = luaL_checkint(L,1);
	if(state<=0)
		return 0;
	ptr->SetStandState(state);
	return 0;
}
int luaUnit_IsInCombat(lua_State * L, Unit * ptr)
{
	if (!ptr)
		return 0;
	lua_pushboolean(L,(ptr->CombatStatus.IsInCombat())?1:0);
	return 1;
}

int luaUnit_SetScale(lua_State * L, Unit * ptr)
{
	CHECK_TYPEID(TYPEID_UNIT);
	double faction = luaL_checknumber(L,1);
	if(faction==0.0)
		return 0;

	ptr->SetFloatValue(OBJECT_FIELD_SCALE_X,(float)faction);
	return 0;
}

int luaUnit_SetModel(lua_State * L, Unit * ptr)
{
	if(!ptr||!ptr->IsUnit())
		return 0;

	int modelid = luaL_checkint(L,1);
	if(modelid==0)
		return 0;

	ptr->SetUInt32Value(UNIT_FIELD_DISPLAYID,modelid);
	return 0;
}

int luaUnit_SetNPCFlags(lua_State * L, Unit * ptr)
{
	CHECK_TYPEID(TYPEID_UNIT);
	int flags = luaL_checkint(L,1);
	if(!flags)
		return 0;

	ptr->SetUInt32Value(UNIT_NPC_FLAGS,flags);
	return 0;
}

int luaUnit_SetCombatCapable(lua_State * L, Unit * ptr)
{
	CHECK_TYPEID(TYPEID_UNIT);
	int enabled = luaL_checkint(L,1);
	ptr->GetAIInterface()->disable_combat = (enabled > 0) ? true : false;
	return 0;
}

int luaUnit_SetCombatMeleeCapable(lua_State * L, Unit * ptr)
{
	CHECK_TYPEID(TYPEID_UNIT);
	int enabled = luaL_checkint(L,1);
	ptr->GetAIInterface()->disable_melee = (enabled > 0) ? true : false;
	return 0;
}

int luaUnit_SetCombatRangedCapable(lua_State * L, Unit * ptr)
{
	CHECK_TYPEID(TYPEID_UNIT);
	int enabled = luaL_checkint(L,1);
	ptr->GetAIInterface()->disable_ranged = (enabled > 0) ? true : false;
	return 0;
}

int luaUnit_SetCombatSpellCapable(lua_State * L, Unit * ptr)
{
	CHECK_TYPEID(TYPEID_UNIT);
	int enabled = luaL_checkint(L,1);
	ptr->GetAIInterface()->disable_spell = (enabled > 0) ? true : false;
	return 0;
}

int luaUnit_SetCombatTargetingCapable(lua_State * L, Unit * ptr)
{
	CHECK_TYPEID(TYPEID_UNIT);
	int enabled = luaL_checkint(L,1);
	ptr->GetAIInterface()->disable_targeting = (enabled > 0) ? true : false;
	return 0;
}

int luaUnit_DestroyCustomWaypointMap(lua_State * L, Unit * ptr)
{
	CHECK_TYPEID(TYPEID_UNIT);
	((Creature*)ptr)->DestroyCustomWaypointMap();
	return 0;
}

int luaUnit_CreateCustomWaypointMap(lua_State * L, Unit * ptr)
{
	CHECK_TYPEID(TYPEID_UNIT);
	Creature * pCreature = (Creature*)ptr;
	if(pCreature->m_custom_waypoint_map)
	{
		for(WayPointMap::iterator itr = pCreature->m_custom_waypoint_map->begin(); itr != pCreature->m_custom_waypoint_map->end(); ++itr)
			delete (*itr);
		delete pCreature->m_custom_waypoint_map;
	}

	pCreature->m_custom_waypoint_map = new WayPointMap;
	pCreature->GetAIInterface()->SetWaypointMap(pCreature->m_custom_waypoint_map);
	return 0;
}

int luaUnit_CreateWaypoint(lua_State * L, Unit * ptr)
{
	CHECK_TYPEID(TYPEID_UNIT);
	float x = (float)luaL_checknumber(L,1);
	float y = (float)luaL_checknumber(L,2);
	float z = (float)luaL_checknumber(L,3);
	float o = (float)luaL_checknumber(L,4);
	int waittime = luaL_checkint(L,5);
	int flags = luaL_checkint(L,6);
	int modelid = luaL_checkint(L,7);

	Creature * pCreature = ((Creature*)ptr);
	if(!pCreature->m_custom_waypoint_map)
	{
		pCreature->m_custom_waypoint_map = new WayPointMap;
		pCreature->GetAIInterface()->SetWaypointMap(pCreature->m_custom_waypoint_map);
	}

	if(!modelid)
		modelid = pCreature->GetUInt32Value(UNIT_FIELD_DISPLAYID);

	WayPoint * wp = new WayPoint;
	wp->id = (uint32)pCreature->m_custom_waypoint_map->size() + 1;
	wp->x = x;
	wp->y = y;
	wp->z = z;
	wp->o = o;
	wp->flags = flags;
	wp->backwardskinid = modelid;
	wp->forwardskinid = modelid;
	wp->backwardemoteid = wp->forwardemoteid = 0;
	wp->backwardemoteoneshot = wp->forwardemoteoneshot = false;
	wp->waittime = waittime;
	pCreature->m_custom_waypoint_map->push_back(wp);

	return 0;
}

int luaUnit_MoveToWaypoint(lua_State * L, Unit * ptr)
{
	CHECK_TYPEID(TYPEID_UNIT);
	int id = luaL_checkint(L,1);
	//if(!id)
	//	return 0;

	ptr->MoveToWaypoint(id);
	return 0;
}

int luaUnit_RemoveItem(lua_State * L, Unit * ptr)
{
	CHECK_TYPEID(TYPEID_PLAYER);
	int id = luaL_checkint(L,1);
	int count = luaL_checkint(L,2);

	((Player*)ptr)->GetItemInterface()->RemoveItemAmt(id,count);
	return 0;
}

int luaUnit_AddItem(lua_State * L, Unit * ptr)
{
	CHECK_TYPEID(TYPEID_PLAYER);
	int id = luaL_checkint(L,1);
	int count = luaL_checkint(L,2);

	Player * plr = (Player*)ptr;
	ItemPrototype * proto = ItemPrototypeStorage.LookupEntry(id);
	if(proto==NULL)
		return 0;

	Item * add = plr->GetItemInterface()->FindItemLessMax(id,count,false);
	if(add==NULL)
	{
		add=objmgr.CreateItem(id,plr);
		add->SetUInt32Value(ITEM_FIELD_STACK_COUNT,count);
		if(plr->GetItemInterface()->AddItemToFreeSlot(add))
			plr->GetSession()->SendItemPushResult(add,false,true,false,true,plr->GetItemInterface()->LastSearchItemBagSlot(),plr->GetItemInterface()->LastSearchItemSlot(),count);
		else
			delete add;
	}
	else
	{
		add->ModUInt32Value(ITEM_FIELD_STACK_COUNT,count);
		plr->GetSession()->SendItemPushResult(add,false,true,false,false,plr->GetItemInterface()->GetBagSlotByGuid(add->GetGUID()),0xFFFFFFFF,count);
	}

	return 0;
}
int luaUnit_GetInstanceID(lua_State * L, Unit * ptr)
{
	CHECK_TYPEID(TYPEID_UNIT);
	/*lua_pushinteger(L,ptr->GetInstanceID());*///Commented cause don't work :)
	return 1;
}

int luaUnit_GetClosestPlayer(lua_State * L, Unit * ptr)
{
	if(!ptr)
		return 0;

	float dist, d2;
	Player * ret=NULL;

	for(set<Player*>::iterator itr = ptr->GetInRangePlayerSetBegin(); itr != ptr->GetInRangePlayerSetEnd(); ++itr)
	{
		d2=(*itr)->GetDistanceSq(ptr);
		if(!ret||d2<dist)
		{
			dist=d2;
			ret=*itr;
		}
	}

	if(ret==NULL)
		lua_pushnil(L);
	else
		Lunar<Unit>::push(L,((Unit*)ret),false);

	return 1;
}
int luaUnit_GetRandomPlayer(lua_State * L, Unit * ptr)
{
	if( ptr == NULL )
		return 0;

	int flag = luaL_checkint( L, 1 );

	Player* ret = NULL;

	switch( flag )
	{
	case RANDOM_ANY:
		{
			uint32 count = (uint32)ptr->GetInRangePlayersCount();
			uint32 r = RandomUInt(count-1);
			count=0;

			for(set<Player*>::iterator itr = ptr->GetInRangePlayerSetBegin(); itr != ptr->GetInRangePlayerSetEnd(); ++itr)
			{
				if (count==r)
				{
					ret=*itr;
					break;
				}
				++count;
			}
		}
		break;
	case RANDOM_IN_SHORTRANGE:
		{
			uint32 count = 0;
			for(set<Player*>::iterator itr = ptr->GetInRangePlayerSetBegin(); itr != ptr->GetInRangePlayerSetEnd(); ++itr)
			{
				Player* obj = (Player*)(*itr);
				if (obj && obj->CalcDistance(obj,ptr)<=8)
					++count;
			}
			if (count)
			{
				uint32 r = RandomUInt(count-1);
				count=0;
				for(set<Player*>::iterator itr = ptr->GetInRangePlayerSetBegin(); itr != ptr->GetInRangePlayerSetEnd(); ++itr)
				{
					Player* obj = (Player*)(*itr);
					if (obj && obj->CalcDistance(obj,ptr)<=8 && count==r)
					{
						ret=obj;
						break;
					}
					++count;
				}
			}
		}
		break;
	case RANDOM_IN_MIDRANGE:
		{
			uint32 count = 0;
			for(set<Player*>::iterator itr = ptr->GetInRangePlayerSetBegin(); itr != ptr->GetInRangePlayerSetEnd(); ++itr)
			{
				Player* obj = (Player*)(*itr);
				if (!obj)
					continue;
				float distance = obj->CalcDistance(obj,ptr);
				if (distance<20 && distance>8)
					++count;
			}
			if (count)
			{
				uint32 r = RandomUInt(count-1);
				count=0;
				for(set<Player*>::iterator itr = ptr->GetInRangePlayerSetBegin(); itr != ptr->GetInRangePlayerSetEnd(); ++itr)
				{
					Player* obj = (Player*)(*itr);
					if (!obj)
						continue;
					float distance = obj->CalcDistance(obj,ptr);
					if (distance<20 && distance>8 && count==r)
					{
						ret=obj;
						break;
					}
					++count;
				}
			}
		}
		break;
	case RANDOM_IN_LONGRANGE:
		{
			uint32 count = 0;
			for(set<Player*>::iterator itr = ptr->GetInRangePlayerSetBegin(); itr != ptr->GetInRangePlayerSetEnd(); ++itr)
			{
				Player* obj = (Player*)(*itr);
				if (obj && obj->CalcDistance(obj,ptr)>=20)
					++count;
			}
			if (count)
			{
				uint32 r = RandomUInt(count-1);
				count=0;
				for(set<Player*>::iterator itr = ptr->GetInRangePlayerSetBegin(); itr != ptr->GetInRangePlayerSetEnd(); ++itr)
				{
					Player* obj = (Player*)(*itr);
					if (obj && obj->CalcDistance(obj,ptr)>=20 && count==r)
					{
						ret=obj;
						break;
					}
					++count;
				}
			}
		}
		break;
	case RANDOM_WITH_MANA:
		{
			uint32 count = 0;
			for(set<Player*>::iterator itr = ptr->GetInRangePlayerSetBegin(); itr != ptr->GetInRangePlayerSetEnd(); ++itr)
			{
				Player* obj = (Player*)(*itr);
				if (obj && obj->GetPowerType() == POWER_TYPE_MANA)
					++count;
			}
			if (count)
			{
				uint32 r = RandomUInt(count-1);
				count=0;
				for(set<Player*>::iterator itr = ptr->GetInRangePlayerSetBegin(); itr != ptr->GetInRangePlayerSetEnd(); ++itr)
				{
					Player* obj = (Player*)(*itr);
					if (obj && obj->GetPowerType() == POWER_TYPE_MANA && count==r)
					{
						ret=obj;
						break;
					}
					++count;
				}
			}
		}
		break;
	case RANDOM_WITH_ENERGY:
		{
			uint32 count = 0;
			for(set<Player*>::iterator itr = ptr->GetInRangePlayerSetBegin(); itr != ptr->GetInRangePlayerSetEnd(); ++itr)
			{
				Player* obj = (Player*)(*itr);
				if (obj && obj->GetPowerType() == POWER_TYPE_ENERGY)
					++count;
			}
			if (count)
			{
				uint32 r = RandomUInt(count-1);
				count=0;
				for(set<Player*>::iterator itr = ptr->GetInRangePlayerSetBegin(); itr != ptr->GetInRangePlayerSetEnd(); ++itr)
				{
					Player* obj = (Player*)(*itr);
					if (obj && obj->GetPowerType() == POWER_TYPE_ENERGY && count==r)
					{
						ret=obj;
						break;
					}
					++count;
				}
			}
		}
		break;
	case RANDOM_WITH_RAGE:
		{
			uint32 count = 0;
			for(set<Player*>::iterator itr = ptr->GetInRangePlayerSetBegin(); itr != ptr->GetInRangePlayerSetEnd(); ++itr)
			{
				Player* obj = (Player*)(*itr);
				if (obj && obj->GetPowerType() == POWER_TYPE_RAGE)
					++count;
			}
			if (count)
			{
				uint32 r = RandomUInt(count-1);
				count=0;
				for(set<Player*>::iterator itr = ptr->GetInRangePlayerSetBegin(); itr != ptr->GetInRangePlayerSetEnd(); ++itr)
				{
					Player* obj = (Player*)(*itr);
					if (obj && obj->GetPowerType() == POWER_TYPE_RAGE && count==r)
					{
						ret=obj;
						break;
					}
					++count;
				}
			}
		}
		break;
	case RANDOM_NOT_MAINTANK:
		{
			uint32 count = 0;
			Unit* mt = ptr->GetAIInterface()->GetMostHated();
			if (!mt->IsPlayer())
				return 0;

			for(set<Player*>::iterator itr = ptr->GetInRangePlayerSetBegin(); itr != ptr->GetInRangePlayerSetEnd(); ++itr)
			{
				Player* obj = (Player*)(*itr);
				if (obj != mt)
					++count;
			}
			if (count)
			{
				uint32 r = RandomUInt(count-1);
				count=0;
				for(set<Player*>::iterator itr = ptr->GetInRangePlayerSetBegin(); itr != ptr->GetInRangePlayerSetEnd(); ++itr)
				{
					Player* obj = (Player*)(*itr);
					if (obj && obj != mt && count==r)
					{
						ret=obj;
						break;
					}
					++count;
				}
			}
		}
		break;
	}

	if(ret==NULL)
		lua_pushnil(L);
	else
		Lunar<Unit>::push(L,((Unit*)ret),false);

	return 1;
}
int luaUnit_GetRandomFriend(lua_State * L, Unit * ptr)
{
	if(!ptr)
		return 0;

	Unit * ret=NULL;
	uint32 count = 0;

	for(set<Object*>::iterator itr = ptr->GetInRangeSetBegin(); itr != ptr->GetInRangeSetEnd(); ++itr)
	{
		Object* obj = (Object*)(*itr);
		if (obj->IsUnit() && isFriendly(obj,ptr))
			++count;
	}

	if (count)
	{
		uint32 r = RandomUInt(count-1);
		count=0;
		for(set<Object*>::iterator itr = ptr->GetInRangeSetBegin(); itr != ptr->GetInRangeSetEnd(); ++itr)
		{
			Object* obj = (Object*)(*itr);
			if (obj->IsUnit() && isFriendly(obj,ptr) && count==r)
			{
				ret=(Unit*)obj;
				break;
			}
			++count;
		}
	}
	if(ret==NULL)
		lua_pushnil(L);
	else
		Lunar<Unit>::push(L,(ret),false);
	return 1;
}

int luaUnit_StopMovement(lua_State * L, Unit * ptr)
{
	CHECK_TYPEID(TYPEID_UNIT);
	int tim = luaL_checkint(L,1);
	ptr->GetAIInterface()->StopMovement(tim);
	return 0;
}

int luaUnit_RemoveAura(lua_State * L, Unit * ptr)
{
	if(!ptr)return 0;
	int auraid = luaL_checkint(L,1);
	ptr->RemoveAura(auraid);
	return 0;
}

int luaUnit_PlaySoundToSet(lua_State * L, Unit * ptr)
{
	if(!ptr) return 0;
	int soundid = luaL_checkint(L,1);
	ptr->PlaySoundToSet(soundid);
	return 0;
}

int luaUnit_GetUnitBySqlId(lua_State * L, Unit * ptr)
{
	if(!ptr) return 0;
	int sqlid = luaL_checkint(L,1);
	return 0;
}

int luaUnit_Despawn(lua_State * L, Unit * ptr)
{
	CHECK_TYPEID(TYPEID_UNIT);
	int delay = luaL_checkint(L,1);
	int respawntime = luaL_checkint(L,2);
	((Creature*)ptr)->Despawn(delay,respawntime);
	return 0;
}

int luaUnit_GetHealthPct(lua_State * L, Unit * ptr)
{
	if(!ptr)
		lua_pushinteger(L,0);
	else
		lua_pushinteger(L, ptr->GetHealthPct());

	return 1;
}
int luaUnit_SetHealthPct(lua_State * L, Unit * ptr)
{
	int val = luaL_checkint(L,1);
	if (val>0)
		ptr->SetHealthPct(val);
	return 1;
}
int luaUnit_GetItemCount(lua_State * L, Unit * ptr)
{
	CHECK_TYPEID_RET_INT(TYPEID_PLAYER);
	int itemid = luaL_checkint(L,1);
	lua_pushinteger(L, ((Player*)ptr)->GetItemInterface()->GetItemCount(itemid,false));
	return 1;
}

int luaUnit_GetMainTank(lua_State * L, Unit * ptr)
{
	CHECK_TYPEID_RET_INT(TYPEID_UNIT);
	Unit* ret = ptr->GetAIInterface()->GetMostHated();
	if(ret==NULL)
		lua_pushnil(L);
	else
		Lunar<Unit>::push(L,(ret),false);
	return 1;
}
int luaUnit_GetAddTank(lua_State * L, Unit * ptr)
{
	CHECK_TYPEID_RET_INT(TYPEID_UNIT);
	Unit* ret = ptr->GetAIInterface()->GetSecondHated();
	if(ret==NULL)
		lua_pushnil(L);
	else
		Lunar<Unit>::push(L,(ret),false);
	return 1;
}
int luaUnit_ClearThreatList(lua_State * L, Unit * ptr)
{
	CHECK_TYPEID_RET_INT(TYPEID_UNIT);
	ptr->ClearHateList();
	return 1;
}

int luaUnit_GetTauntedBy(lua_State * L, Unit * ptr)
{
	CHECK_TYPEID(TYPEID_UNIT)

		if (!ptr->GetAIInterface()->getTauntedBy())
			lua_pushnil(L);
		else
			Lunar<Unit>::push(L,ptr->GetAIInterface()->getTauntedBy(),false);
	return 1;
}
int luaUnit_SetTauntedBy(lua_State * L, Unit * ptr)
{
	CHECK_TYPEID(TYPEID_UNIT)
		Unit * target = Lunar<Unit>::check(L, 2);

	if (!target || ptr->GetAIInterface()->GetIsTaunted() || target==ptr)
		return 0;
	else
		ptr->GetAIInterface()->taunt(target);
	return 1;
}

int luaUnit_GetSoulLinkedWith(lua_State * L, Unit * ptr)
{
	CHECK_TYPEID(TYPEID_UNIT)

		if (!ptr->GetAIInterface()->GetIsSoulLinked())
			lua_pushnil(L);
		else
			Lunar<Unit>::push(L,ptr->GetAIInterface()->getSoullinkedWith(),false);
	return 1;
}
int luaUnit_SetSoulLinkedWith(lua_State * L, Unit * ptr)
{
	CHECK_TYPEID(TYPEID_UNIT)
		Unit * target = Lunar<Unit>::check(L, 2);

	if (!target || ptr->GetAIInterface()->GetIsSoulLinked() || target==ptr)
		return 0;
	else
		ptr->GetAIInterface()->SetSoulLinkedWith(ptr);
	return 1;
}
int luaUnit_ChangeTarget(lua_State * L, Unit * ptr)
{
	CHECK_TYPEID(TYPEID_UNIT)
		Unit * target = Lunar<Unit>::check(L, 2);

	if (!target || !isHostile(ptr,target) || ptr==target)
		return 0;
	else
		ptr->GetAIInterface()->SetNextTarget(target);
	return 1;
}

int luaUnit_HasFinishedQuest(lua_State * L, Unit * ptr)
{
	CHECK_TYPEID_RET(TYPEID_PLAYER);
	int questid = luaL_checkint(L,1);
	if(((Player*)ptr)->HasFinishedQuest(questid))
		lua_pushboolean(L,1);
	else
		lua_pushboolean(L,0);

	return 1;
}

int luaUnit_UnlearnSpell(lua_State * L, Unit * ptr)
{
	CHECK_TYPEID(TYPEID_PLAYER);
	int spellid = luaL_checkint(L,1);
	((Player*)ptr)->removeSpell(spellid,false,false,0);
	return 0;
}

int luaUnit_LearnSpell(lua_State * L, Unit* ptr)
{
	CHECK_TYPEID(TYPEID_PLAYER);
	int spellid = luaL_checkint(L,1);
	((Player*)ptr)->addSpell(spellid);
	return 0;
}

int luaUnit_MarkQuestObjectiveAsComplete(lua_State * L, Unit * ptr)
{
	CHECK_TYPEID(TYPEID_PLAYER);
	int questid = luaL_checkint(L,1);
	int objective = luaL_checkint(L,2);
	Player * pl = ((Player*)ptr);
	QuestLogEntry * qle = pl->GetQuestLogForEntry(questid);
	qle->SetMobCount(objective, qle->GetQuest()->required_mobcount[objective]);
	qle->SendUpdateAddKill(objective);
	if(qle->CanBeFinished())
		qle->SendQuestComplete();

	return 0;
}

int luaUnit_KnockBack(lua_State * L, Unit * ptr)
{
	if(!ptr) return 0;
	double dx = luaL_checknumber(L,1);
	double dy = luaL_checknumber(L,2);
	double affect1 = luaL_checknumber(L,3);
	double affect2 = luaL_checknumber(L,4);
	WorldPacket data(SMSG_MOVE_KNOCK_BACK, 30);
	data << ptr->GetNewGUID();
	data << getMSTime();
	data << dx << dy << affect1 << affect2;
	ptr->SendMessageToSet(&data, true);

	return 0;
}

int luaUnit_SendAreaTriggerMessage(lua_State * L, Unit * ptr)
{
	CHECK_TYPEID(TYPEID_PLAYER);
	const char * msg = luaL_checkstring(L,1);
	if(!msg) return 0;
	((Player*)ptr)->SendAreaTriggerMessage(msg);
	return 0;
}

int luaUnit_SendBroadcastMessage(lua_State * L, Unit * ptr)
{
	CHECK_TYPEID(TYPEID_PLAYER);
	const char * msg = luaL_checkstring(L,1);
	if(!msg) return 0;
	((Player*)ptr)->BroadcastMessage(msg);
	return 0;
}

int luaUnit_TeleportUnit(lua_State * L, Unit * ptr)
{
	CHECK_TYPEID(TYPEID_PLAYER);
	int mapId = luaL_checkint(L, 1);
	float posX = (float)luaL_checknumber(L, 2);
	float posY = (float)luaL_checknumber(L, 3);
	float posZ = (float)luaL_checknumber(L, 4);
	//if(!mapId || !posX || !posY || !posZ)
	//	return 0;

	LocationVector vec(posX, posY, posZ);
	((Player*)ptr)->SafeTeleport((uint32)mapId, 0, vec);
	return 0;
}
// Player Teleport (GO)
int luaGameObject_Teleport(lua_State * L, GameObject * ptr)
{
	//CHECK_TYPEID(TYPEID_PLAYER);
	CHECK_TYPEID(TYPEID_GAMEOBJECT);
	Player* target = Lunar<Player>::check(L, 1);
	int mapId = luaL_checkint(L, 2);
	double posX = luaL_checknumber(L, 3);
	double posY = luaL_checknumber(L, 4);
	double posZ = luaL_checknumber(L, 5);
	//if(!mapId || !posX || !posY || !posZ)
	//	return 0;

	LocationVector vec((float)posX, (float)posY, (float)posZ);
	((Player*)target)->SafeTeleport((uint32)mapId, 0, vec);
	return 0;
}

