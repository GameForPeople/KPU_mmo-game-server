#include "pch.h"
#include "Define.h"
#include "ServerDefine.h"

#include "MoveManager.h"
#include "SocketInfo.h"

#include "Scene.h"

Scene::Scene() : 
	//moveManager(nullptr),
	recvFunctionArr(),
	clientArr()
{
	InitManagers();
	InitClientCont();
	InitFunctions();
}

Scene::~Scene()
{

}

/*
	Scene::ProcessRecvData()
		- ���� �����͵��� �Լ��� �������ݴϴ�.
*/
void Scene::ProcessRecvData(SocketInfo* pClient)
{
	recvFunctionArr[(pClient->buf[0]) % (PACKET_TYPE::ENUM_SIZE)](*this, pClient);
}

/*
	Scene::AddNewClient()
		- ���ο� Ŭ���̾�Ʈ�� �������� ��, �̸� �����̳ʿ� �־��ݴϴ�.
*/
bool Scene::AddNewClient(SocketInfo* pClient)
{
	//std::lock_guard<std::mutex> localLock(addLock);
	addLock.lock();

	for (int index = 0; index < clientArr.size(); ++index)
	{
		if (clientArr[index].first == false)
		{
			clientArr[index].first = true;

			addLock.unlock();

			clientArr[index].second = pClient;
			pClient->clientContIndex = index;
			return true;
		}
	}
	addLock.unlock();
	return false;
}

/*
	Scene::InitManagers()
		- GamsServer�� �����ڿ��� ȣ��Ǹ�, �� �Ŵ������� �ʱ�ȭ�� ����մϴ�.
*/
void Scene::InitManagers()
{
	moveManager = std::make_unique<MoveManager>();
}

/*
	Scene::InitClientCont()
		- GamsServer�� �����ڿ��� ȣ��Ǹ�, Ŭ���̾�Ʈ �����̳ʵ��� �ʱ�ȭ�մϴ�.
*/
void Scene::InitClientCont()
{
	for (auto& client : clientArr)
	{
		client.first = false;
		client.second = nullptr;
	}
}

/*
	Scene::InitFunctions()
		- GamsServer�� �����ڿ��� ȣ��Ǹ�, ���Ӱ� ���õ� �����͵��� �ʱ�ȭ�� ����մϴ�.
*/
void Scene::InitFunctions()
{
	recvFunctionArr[PACKET_TYPE::MOVE] = &Scene::RecvCharacterMove;
}

/*
	Scene::RecvCharacterMove(SocketInfo* pClient)
		- Ŭ���̾�Ʈ�κ��� CharacterMove�� �޾��� ���, ȣ��Ǵ� �Լ�.
*/
void Scene::RecvCharacterMove(SocketInfo* pClient)
{
#ifdef _DEV_MODE_
	std::cout << "[AfterRecv] ���� ���۴�" << int(pClient->buf[0]) << "����ϴ� ������ : " << int(pClient->buf[1]) << "\n";
#endif
	moveManager->MoveCharacter(pClient);

	moveManager->SendMoveCharacter(pClient);
	pClient->buf[0] = GLOBAL_UTIL::BIT_CONVERTER::MakeSendPacket(PACKET_TYPE::MOVE);
}
