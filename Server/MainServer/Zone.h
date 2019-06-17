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

class BaseMonster;

/*
	Zone
		- GameServer(GameWorld)�� �����ϴ� ���� ��ü�Դϴ�.
	
	#0. ���� ������ Client�� ��ü���� �����̳ʸ� ����, �����մϴ�.
	#1. ��Ʈ��ũ �Լ����� GameServer�� �Լ��� �ƴ�, ���⼭ ȣ���մϴ�.
	#2. ���� ������ ����, ������ ��� ������ Sector�Դϴ�.
*/

class Zone
{
public:
	void ProcessTimerUnit(const int timerManagerContIndex);

	Zone();
	~Zone();

public: // ConnectManager
	//std::pair<bool, SocketInfo*> /*std::optional<SocketInfo*>*/ TryToEnter();
	//std::pair<bool, SocketInfo*> OnlyGetUniqueKeyAndMallocSocketInfo();
	void Enter(SocketInfo*);
	void Exit(SocketInfo*);

	void Death(SocketInfo*);
	void DeathForNpc(BaseMonster*);

private:
	void InitViewAndSector(SocketInfo* );

private:
	void InitManagers();
	void InitZoneCont();
	void InitSector();

private:
	void RenewSelfSector(SocketInfo* pClient);
	void RenewSelfSectorForNpc(BaseMonster* pClient);

	void RenewPossibleSectors(ObjectInfo* pObjectInfo);
	void RenewViewListInSectors(SocketInfo* pClient);

	bool RenewViewListInSectorsForNpc(const std::unordered_set<_KeyType>&, BaseMonster* pClient);
	void MakeOldViewListForNpc(std::unordered_set<_KeyType>&, BaseMonster*);
public:
	void RecvCharacterMove(SocketInfo* pClient);
	//void RecvLogin(SocketInfo* pClient);
	void RecvChat(SocketInfo* pClient);

private:
	//std::unique_ptr<ConnectManager> connectManager;
	std::unique_ptr<MonsterModelManager> monsterModelManager;

	std::vector<std::vector<Sector>> sectorCont;

	//std::function<void(Zone&, SocketInfo*)>* recvFunctionArr;
public:
	std::unique_ptr<MoveManager> moveManager;	// �̰͵� ��Ƌ�����;;;
	const std::vector<std::vector<bool>>& GetMapData();
	// ����, public�ϰ� ������ �� �ֵ��� ����.
	ZoneContUnit* zoneContUnit;
};
