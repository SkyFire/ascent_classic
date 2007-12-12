#include "StdAfx.h"

#ifdef ENABLE_LUA_SCRIPTING

bool lua_is_starting_up = false;
initialiseSingleton(LuaEngineMgr);

template <typename T> class Lunar {
	typedef struct { T *pT; } userdataType;
public:
	typedef int (*mfp)(lua_State *L, T* ptr);
	typedef struct { const char *name; mfp mfunc; } RegType;

	static void Register(lua_State *L) {
		lua_newtable(L);
		int methods = lua_gettop(L);

		luaL_newmetatable(L, T::className);
		int metatable = lua_gettop(L);

		// store method table in globals so that
		// scripts can add functions written in Lua.
		lua_pushvalue(L, methods);
		set(L, LUA_GLOBALSINDEX, T::className);

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
		for (RegType *l = ((RegType*)T::methods); l->name; l++) {
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
		luaL_getmetatable(L, T::className);  // lookup metatable in Lua registry
		if (lua_isnil(L, -1)) luaL_error(L, "%s missing metatable", T::className);
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
			static_cast<userdataType*>(luaL_checkudata(L, narg, T::className));
		if(!ud) { luaL_typerror(L, narg, T::className); return NULL; }
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
		T *obj = new T(L);  // call constructor for T objects
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
		lua_pushfstring(L, "%s (%s)", T::className, buff);
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
		char * fn = strrchr(list[filecount]->d_name, '\\');
		if(!fn)
			fn=list[filecount]->d_name;
		char * ext = strrchr(list[filecount]->d_name, '.');
		if(!stricmp(ext, ".lua"))
			luaFiles.insert(string(fn));
		else if(!stricmp(ext, ".luc"))
			luaBytecodeFiles.insert(string(fn));

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

	luaopen_base(L);
	luaopen_table(L);
	//luaopen_io(L);
	luaopen_string(L);
	luaopen_math(L);
	luaopen_debug(L);
	RegisterCoreFunctions();

	if(lua_is_starting_up)
		Log.Notice("LuaEngine", "Loading Scripts...");

	char filename[200];

	for(set<string>::iterator itr = luaFiles.begin(); itr != luaFiles.end(); ++itr)
	{
		snprintf(filename, 200, "scripts\\%s", itr->c_str());
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

void LuaEngine::OnUnitEvent(Unit * pUnit, uint32 EventType, Unit * pMiscUnit, uint32 Misc)
{
	const char * FuncName = LuaEngineMgr::getSingleton().GetUnitEvent(pUnit->GetEntry(), EventType);
	if(FuncName==NULL)
		return;

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

void LuaEngine::OnGameObjectEvent(GameObject * pGameObject, uint32 EventType, Unit * pMiscUnit)
{
	const char * FuncName = LuaEngineMgr::getSingleton().GetGameObjectEvent(pGameObject->GetEntry(),EventType);
	if(FuncName==NULL)
		return;

	m_Lock.Acquire();
	lua_pushstring(L, FuncName);
	lua_gettable(L, LUA_GLOBALSINDEX);
	if(lua_isnil(L,-1))
	{
		printf("Tried to call invalid LUA function '%s' from Ascent! (GO)\n", FuncName);
		m_Lock.Release();
		return;
	}

	Lunar<GameObject>::push(L, pGameObject);
	lua_pushinteger(L,EventType);
	if(pMiscUnit)
		lua_pushnil(L);
	else
		Lunar<Unit>::push(L, pMiscUnit);

	int r = lua_pcall(L,3,LUA_MULTRET,0);
	if(r)
		report(L);

	m_Lock.Release();
}

/************************************************************************/
/* SCRIPT FUNCTIONS DECLARATIONS                                        */
/************************************************************************/
int luaUnit_GetName(lua_State * L, Unit * ptr);
int luaUnit_SendChatMessage(lua_State * L, Unit * ptr);
int luaUnit_MoveTo(lua_State * L, Unit * ptr);
int luaUnit_SetMovementType(lua_State * L, Unit * ptr);
int luaUnit_CastSpell(lua_State * L, Unit * ptr);
int luaUnit_CastSpellOnTarget(lua_State * L, Unit * ptr);
int luaUnit_FullCastSpell(lua_State * L, Unit * ptr);
int luaUnit_FullCastSpellOnTarget(lua_State * L, Unit * ptr);
int luaUnit_SpawnGameObject(lua_State * L, Unit * ptr);
int luaUnit_SpawnCreature(lua_State * L, Unit * ptr);
int luaUnit_GetX(lua_State * L, Unit * ptr);
int luaUnit_GetY(lua_State * L, Unit * ptr);
int luaUnit_GetZ(lua_State * L, Unit * ptr);
int luaUnit_GetO(lua_State * L, Unit * ptr);
int luaUnit_IsPlayer(lua_State * L, Unit * ptr);
int luaUnit_IsCreature(lua_State * L, Unit * ptr);
int luaUnit_RegisterEvent(lua_State * L, Unit * ptr);
int luaUnit_RemoveEvents(lua_State * L, Unit * ptr);
int luaUnit_SendBroadcastMessage(lua_State * L, Unit * ptr);
int luaUnit_SendAreaTriggerMessage(lua_State * L, Unit * ptr);
int luaUnit_KnockBack(lua_State * L, Unit * ptr);
int luaUnit_MarkQuestObjectiveAsComplete(lua_State * L, Unit * ptr);
int luaUnit_LearnSpell(lua_State * L, Unit* ptr);
int luaUnit_UnlearnSpell(lua_State * L, Unit * ptr);
int luaUnit_HasFinishedQuest(lua_State * L, Unit * ptr);
int luaUnit_GetItemCount(lua_State * L, Unit * ptr);
int luaUnit_IsInCombat(lua_State * L, Unit * ptr);
int luaUnit_GetMainTank(lua_State * L, Unit * ptr);
int luaUnit_GetAddTank(lua_State * L, Unit * ptr);
int luaUnit_ClearThreatList(lua_State * L, Unit * ptr);
int luaUnit_GetTauntedBy(lua_State * L, Unit * ptr);
int luaUnit_SetTauntedBy(lua_State * L, Unit * ptr);
int luaUnit_GetSoulLinkedWith(lua_State * L, Unit * ptr);
int luaUnit_SetSoulLinkedWith(lua_State * L, Unit * ptr);
int luaUnit_ChangeTarget(lua_State * L, Unit * ptr);
int luaUnit_GetHealthPct(lua_State * L, Unit * ptr);
int luaUnit_SetHealthPct(lua_State * L, Unit * ptr);
int luaUnit_Despawn(lua_State * L, Unit * ptr);
int luaUnit_GetUnitBySqlId(lua_State * L, Unit * ptr);
int luaUnit_PlaySoundToSet(lua_State * L, Unit * ptr);
int luaUnit_RemoveAura(lua_State * L, Unit * ptr);
int luaUnit_StopMovement(lua_State * L, Unit * ptr);
int luaUnit_Emote(lua_State * L, Unit * ptr);
int luaUnit_GetInstanceID(lua_State * L, Unit * ptr);
int luaUnit_GetClosestPlayer(lua_State * L, Unit * ptr);
int luaUnit_GetRandomPlayer(lua_State * L, Unit * ptr);
int luaUnit_GetRandomFriend(lua_State * L, Unit * ptr);
int luaUnit_AddItem(lua_State * L, Unit * ptr);
int luaUnit_RemoveItem(lua_State * L, Unit * ptr);
int luaUnit_CreateCustomWaypointMap(lua_State * L, Unit * ptr);
int luaUnit_CreateWaypoint(lua_State * L, Unit * ptr);
int luaUnit_DestroyCustomWaypointMap(lua_State * L, Unit * ptr);
int luaUnit_MoveToWaypoint(lua_State * L, Unit * ptr);
int luaUnit_SetCombatCapable(lua_State * L, Unit * ptr);
int luaUnit_SetNPCFlags(lua_State * L, Unit * ptr);
int luaUnit_SetModel(lua_State * L, Unit * ptr);
int luaUnit_SetScale(lua_State * L, Unit * ptr);
int luaUnit_SetFaction(lua_State * L, Unit * ptr);
int luaUnit_SetStandState(lua_State * L, Unit * ptr);

int luaGameObject_GetName(lua_State * L, GameObject * ptr);
int luaGameObject_SpawnCreature(lua_State * L, GameObject * ptr);
int luaGameObject_PlayCustomAnim(lua_State * L, GameObject * ptr);
int luaGameObject_PlaySoundToSet(lua_State * L, GameObject * ptr);

/************************************************************************/
/* SCRIPT FUNCTION TABLES                                               */
/************************************************************************/
const char Unit::className[] = "Unit";
Unit::RegType Unit::methods[] = {
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
	{ "SetNPCFlags", &luaUnit_SetNPCFlags },
	{ "SetModel", &luaUnit_SetModel },
	{ "SetScale", &luaUnit_SetScale },
	{ "SetFaction", &luaUnit_SetFaction },
	{ "SetStandState",&luaUnit_SetStandState },
	{ NULL, NULL },
};

const char GameObject::className[] = "GameObject";
GameObject::RegType GameObject::methods[] = {
	{ "GetName", &luaGameObject_GetName },
	{ NULL, NULL },
};

static int RegisterUnitEvent(lua_State * L);
static int RegisterGameObjectEvent(lua_State * L);

void LuaEngine::RegisterCoreFunctions()
{
	lua_pushcfunction(L, &RegisterUnitEvent);
	lua_setglobal(L, "RegisterUnitEvent");

	lua_pushcfunction(L, &RegisterGameObjectEvent);
	lua_setglobal(L, "RegisterGameObjectEvent");

	Lunar<Unit>::Register(L);
}

static int RegisterUnitEvent(lua_State * L)
{
	int entry = luaL_checkint(L, 1);
	int ev = luaL_checkint(L, 2);
	const char * str = luaL_checkstring(L, 3);

	if(!entry || !ev || !str || !lua_is_starting_up)
		return 0;

	LuaEngineMgr::getSingleton().RegisterUnitEvent(entry,ev,str);
	return 0;
}

static int RegisterGameObjectEvent(lua_State * L)
{
	int entry = luaL_checkint(L, 0);
	int ev = luaL_checkint(L, 1);
	const char * str = luaL_checkstring(L, 2);

	if(!entry || !ev || !str || !lua_is_starting_up)
		return 0;

	LuaEngineMgr::getSingleton().RegisterGameObjectEvent(entry,ev,str);
	return 0;
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
	lua_pushboolean(L,ptr->IsInCombat());
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
	ptr->GetAIInterface()->disable_melee = (enabled > 0) ? true : false;
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
	if(!ptr)
		return 0;

	int flag = luaL_checkint(L,1);

	Player * ret=NULL;

	switch (flag)
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
int luaUnit_Emote(lua_State * L, Unit * ptr)
{
	if(!ptr)
		return 0;

	int emote = luaL_checkint(L,1);
	ptr->Emote((EmoteType)emote);
	return 0;
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
	((Player*)ptr)->BroadcastMessage(msg);
	return 0;
}

int luaUnit_SendBroadcastMessage(lua_State * L, Unit * ptr)
{
	CHECK_TYPEID(TYPEID_PLAYER);
	const char * msg = luaL_checkstring(L,1);
	if(!msg) return 0;
	((Player*)ptr)->SendAreaTriggerMessage(msg);
	return 0;
}

/************************************************************************/
/* Manager Stuff                                                        */
/************************************************************************/

void LuaEngineMgr::Startup()
{
	// create 3
	uint32 c = 5;
	Log.Notice("LuaEngineMgr", "Spawning %u Lua Engines...", c);
	for(uint32 i = 0; i < c; ++i)
	{
		if(i==0)
			lua_is_starting_up=true;

		LuaEngine * l = new LuaEngine();
		l->LoadScripts();
		m_engines.insert(make_pair(l,(uint32)0));

		if(i==0)
			lua_is_starting_up=false;
	}
}

LuaEngine* LuaEngineMgr::GetLuaEngine()
{
	m_lock.Acquire();
	uint32 lowest_c = 0;
	LuaEngine * lowest=NULL;
	LuaEngineMap::iterator ditr;
	for(LuaEngineMap::iterator itr = m_engines.begin(); itr != m_engines.end(); ++itr)
	{
		if(itr->second < lowest_c || lowest==NULL)
		{
			lowest_c = itr->second;
			lowest =itr->first;
			ditr=itr;
		}
	}

	if(lowest_c > 20||lowest==NULL)
	{
		// perferably we don't want any more than 3 instances sharing the same lua engine
		lowest = new LuaEngine();
		m_engines.insert(make_pair(lowest,(uint32)1));
		m_lock.Release();
		return lowest;
	}
	
	// return that engine, after incrementing its used count.
	ditr->second++;
	m_lock.Release();
	return lowest;
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

void LuaEngineMgr::FinishedWithLuaEngine(LuaEngine * engine)
{
	m_lock.Acquire();
	LuaEngineMap::iterator itr = m_engines.find(engine);
	ASSERT(itr!=m_engines.end());
	itr->second--;
	m_lock.Release();
}

void LuaEngineMgr::ReloadScripts()
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
}

void LuaEngineMgr::Unload()
{
	m_lock.Acquire();
	for(LuaEngineMap::iterator itr = m_engines.begin(); itr != m_engines.end(); ++itr)
	{
		delete itr->first;
	}
	m_lock.Release();
}

void LuaEngine::Restart()
{
	m_Lock.Acquire();
	lua_close(L);
	L = lua_open();
	LoadScripts();
	m_Lock.Release();
}

#endif		// ENABLE_LUA_SCRIPTING
