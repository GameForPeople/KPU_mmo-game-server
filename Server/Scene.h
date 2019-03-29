#pragma once

class MoveManager;
struct SocketInfo;

class Scene
{
public:
	void ProcessRecvData(SocketInfo* pClient);
	bool AddNewClient(SocketInfo* pClient);

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
	std::function<void(Scene&, SocketInfo*)> recvFunctionArr[PACKET_TYPE::ENUM_SIZE];
	
	// ���߿� �̺κ� �迭 ����, ����Ʈ�� �� �� ����.
	// ������ ���� ����Ʈ + ���� ���� ť
	std::array<std::pair<bool, SocketInfo*>, GLOBAL_DEFINE::MAX_CLIENT>	clientArr;
};