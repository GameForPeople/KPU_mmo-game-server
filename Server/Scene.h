#pragma once

struct SocketInfo;
struct MemoryUnit;

class MoveManager;
class ConnectManager;

/*
	Scene
		- GameServer(GameWorld)�� �����ϴ� ���� ��ü�Դϴ�.
	
	#0. ���� ������ Client�� ��ü���� �����̳ʸ� ����, �����մϴ�.
	#1. ��Ʈ��ũ �Լ����� GameServer�� �Լ��� �ƴ�, ���⼭ ȣ���մϴ�.
	#2. ���� ������ ����, �����Դϴ�.
*/

class Scene
{
	using _ClientNode = std::pair<bool, SocketInfo*>;
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

	// MoveManager
	void RecvCharacterMove(SocketInfo* pClient);

private:
	std::unique_ptr<MoveManager> moveManager;
	std::unique_ptr<ConnectManager> connectManager;

	std::function<void(Scene&, SocketInfo*)>* recvFunctionArr;
	
	// ���߿� �̺κ� Array ����, ����Ʈ�� �� �� ����.
	// ������ ���� ����Ʈ + ���� ���� ť
	std::vector<_ClientNode>	clientCont;
};