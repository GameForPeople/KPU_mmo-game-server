#pragma once

#include "InHeaderDefine.hh"

struct SocketInfo;
struct MemoryUnit;

class MoveManager;
class ConnectManager;
class Sector;

struct SceneContUnit;

/*
	Scene
		- GameServer(GameWorld)�� �����ϴ� ���� ��ü�Դϴ�.
	
	#0. ���� ������ Client�� ��ü���� �����̳ʸ� ����, �����մϴ�.
	#1. ��Ʈ��ũ �Լ����� GameServer�� �Լ��� �ƴ�, ���⼭ ȣ���մϴ�.
	#2. ���� ������ ����, ������ Sector�Դϴ�.
*/

class Scene
{
public:
	void ProcessPacket(SocketInfo* pClient);

	Scene();
	~Scene();

public:
	// ConnectManager
	_ClientNode /*std::optional<SocketInfo*>*/ InNewClient();
	void OutClient(SocketInfo*);

private:
	void InitManagers();
	void InitClientCont();
	void InitFunctions();
	void InitSector();

private:
	void RenewPossibleSectors(SocketInfo* pClient);
	void RenewViewListInSectors(SocketInfo* pClient);

	// MoveManager
	void RecvCharacterMove(SocketInfo* pClient);

private:
	std::unique_ptr<MoveManager> moveManager;
	std::unique_ptr<ConnectManager> connectManager;
	
	std::vector<std::vector<Sector>> sectorCont;

	SceneContUnit* sceneContUnit;
	std::function<void(Scene&, SocketInfo*)>* recvFunctionArr;
};
