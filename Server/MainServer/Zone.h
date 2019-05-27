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
		- GameServer(GameWorld)�� �����ϴ� ���� ��ü�Դϴ�.
	
	#0. ���� ������ Client�� ��ü���� �����̳ʸ� ����, �����մϴ�.
	#1. ��Ʈ��ũ �Լ����� GameServer�� �Լ��� �ƴ�, ���⼭ ȣ���մϴ�.
	#2. ���� ������ ����, ������ ��� ������ Sector�Դϴ�.
*/

class Zone
{
public:
	void ProcessPacket(SocketInfo* pClient);
	void ProcessTimerUnit(const int timerManagerContIndex);

	Zone();
	~Zone();

public: // ConnectManager
	std::pair<bool, SocketInfo*> /*std::optional<SocketInfo*>*/ TryToEnter();
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

	// MoveManager
	void RecvCharacterMove(SocketInfo* pClient);
	void RecvChat(SocketInfo* pClient);

private:
	std::unique_ptr<ConnectManager> connectManager;
	std::unique_ptr<MoveManager> moveManager;
	std::unique_ptr<MonsterModelManager> monsterModelManager;

	std::vector<std::vector<Sector>> sectorCont;

	ZoneContUnit* zoneContUnit;
	std::function<void(Zone&, SocketInfo*)>* recvFunctionArr;
};
