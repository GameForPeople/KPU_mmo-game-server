#include "pch.h"
#include "Define.h"
#include "ServerDefine.h"

#include "ConnectManager.h"
#include "MoveManager.h"

#include "ClientContUnit.h"

#include "UserData.h"

#include "MemoryUnit.h"
#include "SendMemoryPool.h"

#include "Sector.h"

#include "Scene.h"

Scene::Scene() : 
	moveManager(nullptr),
	connectManager(nullptr),
	sceneContUnit(nullptr),
	recvFunctionArr()
{
	InitManagers();
	InitClientCont();
	InitSector();
	InitFunctions();
}

Scene::~Scene()
{
	delete[] recvFunctionArr;
	delete[] sceneContUnit;
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
	sceneContUnit = new SceneContUnit;
	sceneContUnit->clientCont.reserve(GLOBAL_DEFINE::MAX_CLIENT);

	for (int i = 0; i < GLOBAL_DEFINE::MAX_CLIENT; ++i)
	{
		sceneContUnit->clientCont.emplace_back(false, nullptr);
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
	Scene::InitSectorCont()
		- GamsServer�� �����ڿ��� ȣ��Ǹ�, ���͵��� �ʱ�ȭ�մϴ�.
*/
void Scene::InitSector()
{
	// ��� ���Ἲ �˻�
	if(GLOBAL_DEFINE::MAX_HEIGHT != GLOBAL_DEFINE::MAX_WIDTH) std::cout << "### Scene�� ����� ���������Դϴ�. Ȯ�����ּ���.";
	if((int)((GLOBAL_DEFINE::MAX_HEIGHT - 1) / GLOBAL_DEFINE::SECTOR_DISTANCE) 
		== (int)((GLOBAL_DEFINE::MAX_HEIGHT + 1) / GLOBAL_DEFINE::SECTOR_DISTANCE)) std::cout << "### Scene�� ����� ���������Դϴ�. Ȯ�����ּ���.";

	constexpr BYTE sectorContCount = GLOBAL_DEFINE::MAX_HEIGHT / GLOBAL_DEFINE::SECTOR_DISTANCE;
	
	//X�࿡ ����, Sector ����
	sectorCont.reserve(sectorContCount);
	for (int i = 0; i < sectorContCount; ++i)
	{
		sectorCont.emplace_back(0, i);
	}

	//Y�࿡ ����, Sector ����
	for (int i = 0; i < sectorContCount; ++i)
	{
		sectorCont[i].reserve(sectorContCount);

		for (int j = 0; j < sectorContCount; ++j)
		{
			sectorCont[i].emplace_back(j, i);
		}
	}
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
_ClientNode /* == std::pair<bool, SocketInfo*>*/ Scene::InNewClient()
{
	if (auto retNode = connectManager->InNewClient(clientContUnit, this)
		; retNode.first)
	{
		sectorCont[5][5].InNewClient(retNode.second);
	}
	else return retNode;
}

/*
	Scene::OutClient()
		- connectManager���� �ش� �Լ� ó���ϵ��� ����.
*/
void Scene::OutClient(SocketInfo* pOutClient)
{
	sectorCont[pOutClient->sectorIndexY][pOutClient->sectorIndexX].OutClient(pOutClient);
	connectManager->OutClient(pOutClient, clientContUnit);
}


std::vector<std::pair<BYTE, BYTE>> Scene::FindPossibleSectors(SocketInfo* pClient)
{
	std::vector<std::pair<BYTE, BYTE>> retVector;
	//retVector.reserve(4);	// Max Possible Sector!

	// ���� ���� ���� �翬�� �˻��ؾ���.
	retVector.emplace_back(std::make_pair(pClient->sectorIndexX, pClient->sectorIndexY));

	const BYTE tempX = sectorCont[pClient->sectorIndexX][pClient->sectorIndexY].GetCenterX();
	const BYTE tempY = sectorCont[pClient->sectorIndexX][pClient->sectorIndexY].GetCenterY();

	UserData* pTempUserData = pClient->userData;

	char xDir = 0;
	char yDir = 0;

	if (pTempUserData->GetPosition().x > tempX) { xDir = 1; }
	else if (pTempUserData->GetPosition().x < tempX - 1) { xDir = -1; }

	if (pTempUserData->GetPosition().y > tempY) { yDir = 1; }
	else if (pTempUserData->GetPosition().y < tempY - 1) { yDir = -1; }

	const bool isYZero = pClient->sectorIndexY == 0 ? true : false;
	const bool isYMax = pClient->sectorIndexY == 9 ? true : false;

	if (xDir == 0)
	{
		retVector.reserve(2);

		if (yDir == -1) { if (!isYZero) retVector.emplace_back(std::make_pair(pClient->sectorIndexX, pClient->sectorIndexY - 1)); }
		else if (yDir == 1) { if (!isYMax) retVector.emplace_back(std::make_pair(pClient->sectorIndexX, pClient->sectorIndexY + 1)); }
		//else if (yDir == 0) 
		
		return retVector;
	}

	if (xDir == 1)
	{
		if (pClient->sectorIndexX == 9)
		{
			retVector.reserve(2);

			if (yDir == -1) { if (!isYZero) retVector.emplace_back(std::make_pair(pClient->sectorIndexX, pClient->sectorIndexY - 1)); }
			else if (yDir == 1) { if (!isYMax) retVector.emplace_back(std::make_pair(pClient->sectorIndexX, pClient->sectorIndexY + 1)); }
		}
		else
		{
			retVector.reserve(4);
			retVector.emplace_back(std::make_pair(pClient->sectorIndexX + 1, pClient->sectorIndexY));

			if (yDir == -1) 
			{ 
				if (!isYZero)
				{
					retVector.emplace_back(std::make_pair(pClient->sectorIndexX, pClient->sectorIndexY - 1));
					retVector.emplace_back(std::make_pair(pClient->sectorIndexX + 1, pClient->sectorIndexY - 1));
				}
			}
			else if (yDir == 1)
			{
				if (!isYMax)
				{
					retVector.emplace_back(std::make_pair(pClient->sectorIndexX, pClient->sectorIndexY + 1));
					retVector.emplace_back(std::make_pair(pClient->sectorIndexX + 1, pClient->sectorIndexY + 1));
				}
			}
		}
		return retVector;
	}

	if (xDir == -1)
	{
		if (pClient->sectorIndexX == 0)
		{
			retVector.reserve(2);

			if (yDir == -1) { if (!isYZero) retVector.emplace_back(std::make_pair(pClient->sectorIndexX, pClient->sectorIndexY - 1)); }
			else if (yDir == 1) { if (!isYMax) retVector.emplace_back(std::make_pair(pClient->sectorIndexX, pClient->sectorIndexY + 1)); }
		}
		else
		{
			retVector.reserve(4);
			retVector.emplace_back(std::make_pair(pClient->sectorIndexX - 1, pClient->sectorIndexY));

			if (yDir == -1)
			{
				if (!isYZero)
				{
					retVector.emplace_back(std::make_pair(pClient->sectorIndexX, pClient->sectorIndexY - 1));
					retVector.emplace_back(std::make_pair(pClient->sectorIndexX - 1, pClient->sectorIndexY - 1));
				}
			}
			else if (yDir == 1)
			{
				if (!isYMax)
				{
					retVector.emplace_back(std::make_pair(pClient->sectorIndexX, pClient->sectorIndexY + 1));
					retVector.emplace_back(std::make_pair(pClient->sectorIndexX - 1, pClient->sectorIndexY + 1));
				}
			}
		}
		return retVector;
	}

	std::cout << "FindPossibleSectors : ���⿡ �ɸ����� ���µ�??";
	return retVector;
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
	moveManager->SendMoveCharacter(pClient, clientContUnit);
}
