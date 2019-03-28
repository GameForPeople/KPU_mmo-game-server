#include "pch.h"
#include "Define.h"

#include "SocketInfo.h"
#include "MoveManager.h"

#include "Scene.h"

Scene::Scene()
	: moveManager()
	, recvFunctionArr()
	, clientArr()
{
	InitManagers();
	InitClientCont();
	InitFunctions();
}

/*
	GameServer::InitManagers()
		- GamsServer�� �����ڿ��� ȣ��Ǹ�, �� �Ŵ������� �ʱ�ȭ�� ����մϴ�.
*/
void Scene::ProcessRecvData(SocketInfo* pClient)
{
	recvFunctionArr[(pClient->buf[0]) % (PACKET_TYPE::ENUM_SIZE)](*this, pClient);
}

/*
	Scene::InitManagers()
		- GamsServer�� �����ڿ��� ȣ��Ǹ�, �� �Ŵ������� �ʱ�ȭ�� ����մϴ�.
*/
void Scene::InitManagers()
{
	moveManager = std::make_unique<MoveManager>();
}


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
	pClient->buf[0] = MakeSendPacket(PACKET_TYPE::MOVE);
}
