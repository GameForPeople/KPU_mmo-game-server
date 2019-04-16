#pragma once

#include "InHeaderDefine.hh"

struct SocketInfo;
struct MemoryUnit;

class ConnectManager;
class MoveManager;

class Sector;

struct ZoneContUnit;

/*
	Zone
		- GameServer(GameWorld)�� �����ϴ� ���� ��ü�Դϴ�.
	
	#0. ���� ������ Client�� ��ü���� �����̳ʸ� ����, �����մϴ�.
	#1. ��Ʈ��ũ �Լ����� GameServer�� �Լ��� �ƴ�, ���⼭ ȣ���մϴ�.
	#2. ���� ������ ����, ������ Sector�Դϴ�.
*/

class Zone
{
public:
	void ProcessPacket(SocketInfo* pClient);

	Zone();
	~Zone();

public:
	// ConnectManager
	_ClientNode /*std::optional<SocketInfo*>*/ InNewClient();
	void OutClient(SocketInfo*);
	void InitViewAndSector(SocketInfo* );

private:
	void InitManagers();
	void InitClientCont();
	void InitFunctions();
	void InitSector();

private:
	void RenewClientSector(SocketInfo* pClient);
	void RenewPossibleSectors(SocketInfo* pClient);
	void RenewViewListInSectors(SocketInfo* pClient);

	// MoveManager
	void RecvCharacterMove(SocketInfo* pClient);

private:
	std::unique_ptr<ConnectManager> connectManager;
	std::unique_ptr<MoveManager> moveManager;

	std::vector<std::vector<Sector>> sectorCont;

	ZoneContUnit* zoneContUnit;
	std::function<void(Zone&, SocketInfo*)>* recvFunctionArr;
};
