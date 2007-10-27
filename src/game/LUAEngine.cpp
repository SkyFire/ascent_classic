#include "StdAfx.h"

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

LuaEngine::LuaEngine()
{
	this->L = lua_open();
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
#endif

	// we prefer precompiled code.
	for(set<string>::iterator itr = luaBytecodeFiles.begin(); itr != luaBytecodeFiles.end(); ++itr)
	{
		set<string>::iterator it2 = luaFiles.find(*itr);
		if(it2 == luaFiles.end())
			luaFiles.erase(it2);
	}

	printf("LUA Loader: Loading LUA Libraries...\n");
	luaopen_base(L);
	luaopen_table(L);
	//luaopen_io(L);
	luaopen_string(L);
	luaopen_math(L);
	luaopen_debug(L);

	printf("LUA Loader: Registering Core Functions...\n");
	RegisterCoreFunctions();

	printf("LUA Loader: Loading LUA Scripts...\n");
	char filename[200];

	for(set<string>::iterator itr = luaFiles.begin(); itr != luaFiles.end(); ++itr)
	{
		snprintf(filename, 200, "scripts\\%s", itr->c_str());
		printf("\t%s...", (*itr).c_str());
		if(luaL_loadfile(L, filename) != 0)
		{
			printf("failed. (could not load)\n");
			const char * msg = lua_tostring(L, -1);
			if(msg!=NULL)
				printf("%s\n",msg);
		}
		else
		{
			if(lua_pcall(L, 0, LUA_MULTRET, 0) != 0)
			{
				printf("failed. (could not run)\n");
				//report(L);
			}
			else
				printf("ok!.\n");
		}
	}
}

/************************************************************************/
/* SCRIPT FUNCTIONS DECLARATIONS                                        */
/************************************************************************/
int luaUnit_GetName(lua_State * L, Unit * ptr);
int luaUnit_SendChatMessage(lua_State * L, Unit * ptr);
int luaUnit_MoveTo(lua_State * L, Unit * ptr);
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

/************************************************************************/
/* SCRIPT FUNCTION TABLES                                               */
/************************************************************************/
const char Unit::className[] = "Unit";
Unit::RegType Unit::methods[] = {
	{ "GetName", &luaUnit_GetName },
	{ "SendChatMessage", &luaUnit_SendChatMessage },
	{ "MoveTo", &luaUnit_GetName },
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
	{ NULL, NULL },
};

void LuaEngine::RegisterCoreFunctions()
{
	Lunar<Unit>::Register(L);
}

/************************************************************************/
/* SCRIPT FUNCTION IMPLEMENTATION                                       */
/************************************************************************/
#define CHECK_TYPEID(expected_type) if(!ptr || !ptr->IsInWorld() || ptr->GetTypeId() != expected_type) { return 0; }

int luaUnit_GetName(lua_State * L, Unit * ptr)
{
	if(!ptr)
		return 0;

	switch(ptr->GetTypeId())
	{
	case TYPEID_UNIT:
		lua_pushstring(L, ((Creature*)ptr)->GetCreatureName()->Name);
		break;

	case TYPEID_PLAYER:
		lua_pushstring(L, ((Player*)ptr)->GetName());
		break;

	default:
		return 0;
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

	if(x == 0 || y == 0 || z == 0 || o == 0)
		return 0;

	ptr->GetAIInterface()->MoveTo( (float)x, (float)y, (float)z, (float)o );
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
	
	if(!entry_id || !faction || !duration)
		return 0;

	CreatureProto * proto = CreatureProtoStorage.LookupEntry(entry_id);
	CreatureInfo * inf = CreatureNameStorage.LookupEntry(entry_id);
	if(!proto || !inf)
		return 0;

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
		return 0;

	GameObjectInfo * goi = GameObjectNameStorage.LookupEntry(entry_id);
	if(!goi)
		return 0;

	GameObject * pC = ptr->GetMapMgr()->CreateGameObject();
	pC->spawnid=0;
	pC->m_spawn=0;
	pC->CreateFromProto(entry_id, ptr->GetMapId(), (float)x, (float)y, (float)z, (float)o);
	pC->SetMapId(ptr->GetMapId());
	pC->SetInstanceID(ptr->GetInstanceID());
	pC->PushToWorld(ptr->GetMapMgr());
	if(duration)
		sEventMgr.AddEvent(pC, &GameObject::ExpireAndDelete, EVENT_GAMEOBJECT_EXPIRE, duration, 1, EVENT_FLAG_DO_NOT_EXECUTE_IN_WORLD_CONTEXT);

	//Lunar<GameObject>::push(L, pC, false);
	return 1;
}
