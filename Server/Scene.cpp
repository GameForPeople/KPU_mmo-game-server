#include "pch.h"
#include "Define.h"
#include "ServerDefine.h"

#include "ConnectManager.h"
#include "MoveManager.h"
#include "UserData.h"

#include "MemoryUnit.h"
#include "SendMemoryPool.h"

#include "Scene.h"

Scene::Scene() : 
	moveManager(nullptr),
	connectManager(nullptr),
	recvFunctionArr(),
	clientCont()
{
	InitManagers();
	InitClientCont();
	InitFunctions();
}

Scene::~Scene()
{
	delete[] recvFunctionArr;
}

/*
	Scene::InitManagers()
		- GamsServer�� �����ڿ��� ȣ��Ǹ�, �� �Ŵ������� �ʱ�ȭ�� ����մϴ�.
*/
void Scene::InitManagers()
{
	moveManager = std::make_unique<MoveManager>();
	connectManager = std::make_unique<ConnectManager>();
}

/*
	Scene::InitClientCont()
		- GamsServer�� �����ڿ��� ȣ��Ǹ�, Ŭ���̾�Ʈ �����̳ʵ��� �ʱ�ȭ�մϴ�.
*/
void Scene::InitClientCont()
{
	clientCont.reserve(GLOBAL_DEFINE::MAX_CLIENT);
	for (int i = 0; i < GLOBAL_DEFINE::MAX_CLIENT; ++i)
	{
		clientCont.emplace_back(false, nullptr);
	}
}

/*
	Scene::InitFunctions()
		- GamsServer�� �����ڿ��� ȣ��Ǹ�, ���Ӱ� ���õ� �����͵��� �ʱ�ȭ�� ����մϴ�.
*/
void Scene::InitFunctions()
{
	recvFunctionArr = new std::function<void(Scene&, SocketInfo*)>[PACKET_TYPE::CS::ENUM_SIZE];
	recvFunctionArr[PACKET_TYPE::CS::LEFT] = &Scene::RecvCharacterMove;
	recvFunctionArr[PACKET_TYPE::CS::UP] = &Scene::RecvCharacterMove;
	recvFunctionArr[PACKET_TYPE::CS::RIGHT] = &Scene::RecvCharacterMove;
	recvFunctionArr[PACKET_TYPE::CS::DOWN] = &Scene::RecvCharacterMove;
}

/*
	Scene::ProcessRecvData()
		- ���� �����͵��� �Լ��� �������ݴϴ�.
*/
void Scene::ProcessPacket(SocketInfo* pClient)
{
	recvFunctionArr[(pClient->loadedBuf[1]) % (PACKET_TYPE::CS::ENUM_SIZE)](*this, pClient);
}

/*
	Scene::InNewClient()
		- connectManager���� �ش� �Լ� ó���ϵ��� ����.
*/
/*std::optional<SocketInfo*>*/ 
Scene::_ClientNode /* == std::pair<bool, SocketInfo*>*/ Scene::InNewClient()
{
	return connectManager->InNewClient(clientCont, this);
}

/*
	Scene::OutClient()
		- connectManager���� �ش� �Լ� ó���ϵ��� ����.
*/
void Scene::OutClient(SocketInfo* pOutClient)
{
	connectManager->OutClient(pOutClient, clientCont);
}


/*
	Scene::RecvCharacterMove(SocketInfo* pClient)
		- Ŭ���̾�Ʈ�κ��� CharacterMove�� �޾��� ���, ȣ��Ǵ� �Լ�.

	#0. �ش� Ŭ���̾�Ʈ�� Move�� ���� ó���մϴ�.
	#1. Scene�� �����ϴ� �ڽ��� ������ ��� Ŭ���̾�Ʈ���� ID���� ��ġ���� �����մϴ�.
*/
void Scene::RecvCharacterMove(SocketInfo* pClient)
{
#ifdef _DEV_MODE_
	std::cout << "[AfterRecv] ���� ���۴�" << int(pClient->loadedBuf[1]) << "����ϴ� ������ : " << int(pClient->loadedBuf[1]) << "\n";
#endif
	moveManager->MoveCharacter(pClient);
	moveManager->SendMoveCharacter(pClient, clientCont);
}
