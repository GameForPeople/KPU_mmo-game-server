#pragma once

#include "InHeaderDefine.hh"

struct SocketInfo;
struct MemoryUnit;
struct TimerMemoryHead;
struct TimerUnit;

class ConnectManager;
class MoveManager;
class MonsterModelManager;

class Sector;

struct ZoneContUnit;
struct ObjectInfo;

/*
	Zone
		- GameServer(GameWorld)를 구성하는 단위 객체입니다.
	
	#0. 씐에 접속한 Client의 객체들의 컨테이너를 갖고, 관리합니다.
	#1. 네트워크 함수들은 GameServer의 함수가 아닌, 여기서 호출합니다.
	#2. 공간 분할의 단위, 기준은 멤버 변수인 Sector입니다.
*/

class Zone
{
public:
	void ProcessTimerUnit(const int timerManagerContIndex);

	Zone();
	~Zone();

public: // ConnectManager
	std::pair<bool, SocketInfo*> /*std::optional<SocketInfo*>*/ TryToEnter();
	//std::pair<bool, SocketInfo*> OnlyGetUniqueKeyAndMallocSocketInfo();
	void Exit(SocketInfo*);
	void InitViewAndSector(SocketInfo* );

private:
	void InitManagers();
	void InitClientCont();
	void InitFunctions();
	void InitSector();

private:
	void RenewSelfSector(ObjectInfo* pClient);
	void RenewSelfSectorForNpc(ObjectInfo* pClient);

	void RenewPossibleSectors(ObjectInfo* pObjectInfo);
	void RenewViewListInSectors(SocketInfo* pClient);
	bool RenewViewListInSectorsForNpc(ObjectInfo* pClient);

public:
	void RecvCharacterMove(SocketInfo* pClient);
	//void RecvLogin(SocketInfo* pClient);
	void RecvChat(SocketInfo* pClient);

private:
	//std::unique_ptr<ConnectManager> connectManager;
	std::unique_ptr<MoveManager> moveManager;
	std::unique_ptr<MonsterModelManager> monsterModelManager;

	std::vector<std::vector<Sector>> sectorCont;

	std::function<void(Zone&, SocketInfo*)>* recvFunctionArr;
public:
	// 성능, public하게 접근할 수 있도록 변경.
	ZoneContUnit* zoneContUnit;
};
