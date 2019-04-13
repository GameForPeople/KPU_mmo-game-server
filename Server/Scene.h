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
		- GameServer(GameWorld)를 구성하는 단위 객체입니다.
	
	#0. 씐에 접속한 Client의 객체들의 컨테이너를 갖고, 관리합니다.
	#1. 네트워크 함수들은 GameServer의 함수가 아닌, 여기서 호출합니다.
	#2. 공간 분할의 단위, 기준은 Sector입니다.
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
	std::vector<std::pair<BYTE, BYTE>> FindPossibleSectors(SocketInfo* pClient);

	// MoveManager
	void RecvCharacterMove(SocketInfo* pClient);

private:
	std::unique_ptr<MoveManager> moveManager;
	std::unique_ptr<ConnectManager> connectManager;
	
	std::vector<std::vector<Sector>> sectorCont;

	SceneContUnit* sceneContUnit;
	std::function<void(Scene&, SocketInfo*)>* recvFunctionArr;
};
