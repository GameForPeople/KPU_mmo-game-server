#pragma once

#include "InHeaderDefine.hh"

class Zone;

class LuaManager /**/
{
public:
	_NODISCARD static inline LuaManager* GetInstance() noexcept { return LuaManager::instance; };

	// �ش� �Լ��� GameServer.cpp�� �����ڿ��� �ѹ� ȣ��Ǿ���մϴ�.
	/*_NODISCARD*/ static void MakeInstance(Zone* pZone) 
	{ LuaManager::instance = new LuaManager(pZone); /*return SendMemoryPool::instance;*/ };

	// �ش� �Լ��� GameServer.cpp�� �Ҹ��ڿ��� �ѹ� ȣ��Ǿ���մϴ�.
	static void DeleteInstance() { delete instance; }

private:
	static LuaManager* instance;

	LuaManager(Zone*);
	~LuaManager() = default;

	LuaManager(const LuaManager&) = delete;
	LuaManager& operator=(const LuaManager&) = delete;

public:	//For Script Use
	Zone* pZone;

public: //Script function
	static int API_get_x(lua_State *L);
	static int API_get_y(lua_State *L);
	static int API_get_IsLive(lua_State *L);
	static int API_go_SpawnPosition(lua_State *L);
	static int API_do_attack(lua_State *L);
	static int API_do_chase(lua_State *L);
};