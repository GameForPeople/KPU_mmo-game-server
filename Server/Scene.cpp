#include "pch.h"
#include "Define.h"
#include "ServerDefine.h"

#include "MoveManager.h"
#include "UserData.h"

#include "MemoryUnit.h"
#include "SendMemoryPool.h"

#include "Scene.h"

Scene::Scene() : 
	moveManager(nullptr),
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
	recvFunctionArr = new std::function<void(Scene&, SocketInfo*)>[PACKET_TYPE::ENUM_SIZE];
	recvFunctionArr[PACKET_TYPE::MOVE] = &Scene::RecvCharacterMove;
}

/*
	Scene::ProcessRecvData()
		- ���� �����͵��� �Լ��� �������ݴϴ�.
*/
void Scene::ProcessPacket(SocketInfo* pClient)
{
	recvFunctionArr[(pClient->loadedBuf[1]) % (PACKET_TYPE::ENUM_SIZE)](*this, pClient);
}

/*
	Scene::InNewClient()
		- ���ο� Ŭ���̾�Ʈ�� �������� ��, �̸� �����̳ʿ� �־��ݴϴ�.

	#!?0. �ϳ��� ���� �������� �ϳ��� ���� ���� ���, ����ó���ϴ°� ����.
	#!?1. �ٸ� �ϳ��� �������� ���� ���� ���� ���, ���ʿ� SocketInfo�� ���� �ְ�, InNewCliet�� ���ڷ� �־��ִ� �Ը���.
*/
std::pair<bool, SocketInfo*> Scene::InNewClient()
{
	//std::lock_guard<std::mutex> localLock(addLock);
	addLock.lock();

	for (int index = 0; index < clientCont.size(); ++index)
	{
		if (clientCont[index].first == false)
		{
			clientCont[index].first = true;

			addLock.unlock();

			// ���� ���� ����ü �Ҵ�
			SocketInfo* pClient = new SocketInfo;
			if (pClient == nullptr)
			{
				ERROR_HANDLING::ERROR_QUIT(TEXT("Make_SocketInfo()"));
				break;
			}

			clientCont[index].second = pClient;
			pClient->clientContIndex = index;
			pClient->pScene = this;
			return std::make_pair(true, pClient);
		}
	}
	addLock.unlock();
	return std::make_pair(false, nullptr);
}

/*
	Scene::OutClient()
		- Ŭ���̾�Ʈ�� �ٸ� ������ �̵��ϰų�, �α׾ƿ� �ɶ�, �ش� Ŭ���̾�Ʈ�� �������� ���ݴϴ�.

	#!?0. ����ü ���⼭ ������ ������ ������ϴ°���. �� ������ ������ ���װ� �� �� ���� ������.
*/
void Scene::OutClient(SocketInfo* pClient)
{
	// ���� Lock �� �ʿ� ����.
	clientCont[pClient->clientContIndex].first = false;
	// second�� �ʱ�ȭ �� �ʿ� ����.

	// �ٸ� �̺κп���, ����� ���� �� ��������, �������� �����ϱ� ���� ó�����ֵ��� �սô�.

	// ��� pScene�� nullptr�ϰ�, LogOutProcess����, �ش� ���θ� �˻��ϴ� �͵� �� �� �ѵ�;
	pClient->pScene = nullptr;
	pClient->clientContIndex = -1;
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
	std::cout << "[AfterRecv] ���� ���۴�" << int(pClient->loadedBuf[0]) << "����ϴ� ������ : " << int(pClient->loadedBuf[1]) << "\n";
#endif
	moveManager->MoveCharacter(pClient);
	//moveManager->SendMoveCharacter(pClient);
	const BYTE clientPositionByte 
		= BIT_CONVERTER::MakeByteFromLeftAndRightByte(pClient->userData->GetPosition().x, pClient->userData->GetPosition().y);

	for (std::pair<bool, SocketInfo*> retClient : clientCont)
	{
		if (retClient.first) 
		{
			SendMemoryUnit* sendMemoryUnit = SendMemoryPool::GetInstance()->PopMemory(retClient.second);

			sendMemoryUnit->memoryUnit.dataBuf[0] = PACKET_TYPE::MOVE;
			sendMemoryUnit->memoryUnit.dataBuf[1] = pClient->clientContIndex;
			sendMemoryUnit->memoryUnit.dataBuf[2] = clientPositionByte;

			sendMemoryUnit->memoryUnit.wsaBuf.len = 3;

			NETWORK_UTIL::SendPacket(pClient, sendMemoryUnit);
		}
	}
}
