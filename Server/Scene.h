#pragma once

class MoveManager;
struct SocketInfo;
struct MemoryUnit;

/*
	Scene
		- GameServer(GameWorld)�� �����ϴ� ���� ��ü�Դϴ�.
	
	#0. ���� ������ Client�� ��ü���� �����̳ʸ� ����, �����մϴ�.
	#1. ��Ʈ��ũ �Լ����� GameServer�� �Լ��� �ƴ�, ���⼭ ȣ���մϴ�.
	#2. ���� ������ ����, �����Դϴ�.
*/
class Scene
{
public:
	void ProcessPacket(SocketInfo* pClient);
	std::pair<bool, SocketInfo*> /*std::optional<SocketInfo*>*/ InNewClient();
	void OutClient(SocketInfo*);

	Scene();
	~Scene();

private:
	void InitManagers();
	void InitClientCont();
	void InitFunctions();

	void RecvCharacterMove(SocketInfo* pClient);

private:
	std::unique_ptr<MoveManager> moveManager;

	std::mutex addLock;
	std::function<void(Scene&, SocketInfo*)>* recvFunctionArr;
	
	// ���߿� �̺κ� Array ����, ����Ʈ�� �� �� ����.
	// ������ ���� ����Ʈ + ���� ���� ť
	std::vector<std::pair<bool, SocketInfo*>>	clientCont;
};