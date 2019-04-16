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

#include "Zone.h"

Zone::Zone() : 
	moveManager(nullptr),
	connectManager(nullptr),
	zoneContUnit(nullptr),
	recvFunctionArr()
{
	InitManagers();
	InitClientCont();
	InitSector();
	InitFunctions();
}

Zone::~Zone()
{
	delete[] recvFunctionArr;
	delete[] zoneContUnit;
}

/*
	Zone::InitManagers()
		- GamsServer�� �����ڿ��� ȣ��Ǹ�, �� �Ŵ������� �ʱ�ȭ�� ����մϴ�.
*/
void Zone::InitManagers()
{
	moveManager = std::make_unique<MoveManager>();
	connectManager = std::make_unique<ConnectManager>();
}

/*
	Zone::InitClientCont()
		- GamsServer�� �����ڿ��� ȣ��Ǹ�, Ŭ���̾�Ʈ �����̳ʵ��� �ʱ�ȭ�մϴ�.
*/
void Zone::InitClientCont()
{
	zoneContUnit = new ZoneContUnit;
	zoneContUnit->clientCont.reserve(GLOBAL_DEFINE::MAX_CLIENT);

	for (int i = 0; i < GLOBAL_DEFINE::MAX_CLIENT; ++i)
	{
		zoneContUnit->clientCont.emplace_back(false, nullptr);
	}
}

/*
	Zone::InitFunctions()
		- GamsServer�� �����ڿ��� ȣ��Ǹ�, ���Ӱ� ���õ� �����͵��� �ʱ�ȭ�� ����մϴ�.
*/
void Zone::InitFunctions()
{
	recvFunctionArr = new std::function<void(Zone&, SocketInfo*)>[PACKET_TYPE::CS::ENUM_SIZE];
	recvFunctionArr[PACKET_TYPE::CS::MOVE] = &Zone::RecvCharacterMove;
}

/*
	Zone::InitSectorCont()
		- GamsServer�� �����ڿ��� ȣ��Ǹ�, ���͵��� �ʱ�ȭ�մϴ�.
*/
void Zone::InitSector()
{
	// ��� ���Ἲ �˻�
	if(GLOBAL_DEFINE::MAX_HEIGHT != GLOBAL_DEFINE::MAX_WIDTH) std::cout << "### Zone�� ����� ���������Դϴ�. Ȯ�����ּ���.";
	if((int)((GLOBAL_DEFINE::MAX_HEIGHT - 1) / GLOBAL_DEFINE::SECTOR_DISTANCE) 
		== (int)((GLOBAL_DEFINE::MAX_HEIGHT + 1) / GLOBAL_DEFINE::SECTOR_DISTANCE)) std::cout << "### Zone�� ����� ���������Դϴ�. Ȯ�����ּ���.";

	constexpr BYTE sectorContCount = GLOBAL_DEFINE::MAX_HEIGHT / GLOBAL_DEFINE::SECTOR_DISTANCE;
	
	//X�࿡ ����, Sector ����
	sectorCont.reserve(sectorContCount);
	for (int i = 0; i < sectorContCount; ++i)
	{
		sectorCont.emplace_back();
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
	Zone::ProcessRecvData()
		- ���� �����͵��� �Լ��� �������ݴϴ�.
*/
void Zone::ProcessPacket(SocketInfo* pClient)
{
	recvFunctionArr[(pClient->loadedBuf[1]) % (PACKET_TYPE::CS::ENUM_SIZE)](*this, pClient);
}

/*
	Zone::InNewClient()
		- connectManager���� �ش� �Լ� ó���ϵ��� ����.
*/
/*std::optional<SocketInfo*>*/ 
_ClientNode /* == std::pair<bool, SocketInfo*>*/ Zone::InNewClient()
{
	if (auto retNode = connectManager->InNewClient(zoneContUnit, this)
		; retNode.first)
	{
		//���� Sector�� Ŭ���̾�Ʈ ����.
		sectorCont[5][5].InNewClient(retNode.second);

		// InitViewAndSector���� ���ε�.

		// �ѷ��� ���� �����ϰ� -> ������ ��Ʈ�� ��� ��, ���߿� ��в��ϴ�.
		//RenewPossibleSectors(retNode.second);

		// ģ���� ���� ��Ͱ� -> ������ ��Ʈ�� ��� ��, ���߿� ��в��ϴ�.
		//RenewViewListInSectors(retNode.second);

		return retNode;
	}
	else return retNode;
}

void Zone::InitViewAndSector(SocketInfo* pClient)
{
	RenewPossibleSectors(pClient);
	RenewViewListInSectors(pClient);
}

/*
	Zone::OutClient()
		- connectManager���� �ش� �Լ� ó���ϵ��� ����.
*/
void Zone::OutClient(SocketInfo* pOutClient)
{
	// ���� �����̳ʿ��� �� ������ �����ְ�
	sectorCont[pOutClient->sectorIndexY][pOutClient->sectorIndexX].OutClient(pOutClient);
	
	// �� ViewList�� Client���� �� �����ٰ� �˷��ְ�.
	connectManager->OutClient(pOutClient, zoneContUnit);
}

/*
	FindPossibleSectors? CheckPossibleSectors?
		- ���� ĳ������ ���Ϳ� ��ġ�� �˻��Ͽ�, �þ� üũ�� �ؾ��ϴ� ���͸� �˻��մϴ�.

	?0. ������ ���������� �����Ͽ�, �����ϴ� ��Ŀ���, SocketInfo�� ��������� �δ� ������� �����ϴ°� ���� ������?
*/
void Zone::RenewPossibleSectors(SocketInfo* pClient)
{
	// ���� ������ �����ϴ� �ڵ忡��, ��� ������ �����ϴ� ������� ����.
	//std::vector<std::pair<BYTE, BYTE>> retVector;
	//retVector.reserve(4);	// Max Possible Sector!
	//retVector.emplace_back(std::make_pair(pClient->sectorIndexX, pClient->sectorIndexY));

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

	pClient->possibleSectorCount = 0;

	if (xDir == 0)
	{
		if (yDir == -1)
		{
			if (!isYZero)
			{
				pClient->possibleSectorCount = 1;
				pClient->sectorArr[0] = { pClient->sectorIndexX, pClient->sectorIndexY - 1 };
			}
		}
		else if (yDir == 1)
		{
			if (!isYMax)
			{
				pClient->possibleSectorCount = 1;
				pClient->sectorArr[0] = { pClient->sectorIndexX, pClient->sectorIndexY + 1 };
				
			}
		}
		/* else if (yDir == 0) */
		
		return; //return retVector;
	}
	else if (xDir == 1)
	{
		if (pClient->sectorIndexX == 9)	// X�� ���� ��,
		{
			if (yDir == -1) 
			{ 
				if (!isYZero)
				{
					pClient->possibleSectorCount = 1;
					pClient->sectorArr[0] = { pClient->sectorIndexX, pClient->sectorIndexY - 1 };
					//retVector.emplace_back(std::make_pair(pClient->sectorIndexX, pClient->sectorIndexY - 1));
				}
			}
			else if (yDir == 1)
			{
				if (!isYMax)
				{
					pClient->possibleSectorCount = 1;
					pClient->sectorArr[0] = { pClient->sectorIndexX, pClient->sectorIndexY + 1 };
					//retVector.emplace_back(std::make_pair(pClient->sectorIndexX, pClient->sectorIndexY + 1));

				}
			}
		}
		else
		{
			pClient->possibleSectorCount = 1;
			pClient->sectorArr[0] = { pClient->sectorIndexX + 1, pClient->sectorIndexY};
			//retVector.reserve(4);
			//retVector.emplace_back(std::make_pair(pClient->sectorIndexX + 1, pClient->sectorIndexY));

			if (yDir == -1) 
			{ 
				if (!isYZero)
				{
					pClient->sectorArr[1] = { pClient->sectorIndexX, pClient->sectorIndexY - 1 };
					pClient->sectorArr[2] = { pClient->sectorIndexX + 1, pClient->sectorIndexY - 1};
					pClient->possibleSectorCount = 3;

					//retVector.emplace_back(std::make_pair(pClient->sectorIndexX, pClient->sectorIndexY - 1));
					//retVector.emplace_back(std::make_pair(pClient->sectorIndexX + 1, pClient->sectorIndexY - 1));
				}
			}
			else if (yDir == 1)
			{
				if (!isYMax)
				{
					pClient->sectorArr[1] = { pClient->sectorIndexX, pClient->sectorIndexY + 1 };
					pClient->sectorArr[2] = { pClient->sectorIndexX + 1, pClient->sectorIndexY + 1 };
					pClient->possibleSectorCount = 3;

					//retVector.emplace_back(std::make_pair(pClient->sectorIndexX, pClient->sectorIndexY + 1));
					//retVector.emplace_back(std::make_pair(pClient->sectorIndexX + 1, pClient->sectorIndexY + 1));
				}
			}
		}
		
		return;
	}
	else if (xDir == -1)
	{
		if (pClient->sectorIndexX == 0)
		{
			if (yDir == -1) 
			{ 
				if (!isYZero)
				{
					pClient->possibleSectorCount = 1;
					pClient->sectorArr[0] = { pClient->sectorIndexX, pClient->sectorIndexY - 1 };
					//retVector.emplace_back(std::make_pair(pClient->sectorIndexX, pClient->sectorIndexY - 1));
				}
			}
			else if (yDir == 1)
			{
				if (!isYMax)
				{
					pClient->possibleSectorCount = 1;
					pClient->sectorArr[0] = { pClient->sectorIndexX, pClient->sectorIndexY + 1 };

					//retVector.emplace_back(std::make_pair(pClient->sectorIndexX, pClient->sectorIndexY + 1));
				}
			}
		}
		else
		{
			pClient->possibleSectorCount = 1;
			pClient->sectorArr[0] = { pClient->sectorIndexX - 1, pClient->sectorIndexY };
			//retVector.reserve(4);
			//retVector.emplace_back(std::make_pair(pClient->sectorIndexX - 1, pClient->sectorIndexY));

			if (yDir == -1)
			{
				if (!isYZero)
				{
					pClient->possibleSectorCount = 3;
					pClient->sectorArr[1] = { pClient->sectorIndexX, pClient->sectorIndexY - 1 };
					pClient->sectorArr[2] = { pClient->sectorIndexX - 1, pClient->sectorIndexY - 1 };

					//retVector.emplace_back(std::make_pair(pClient->sectorIndexX, pClient->sectorIndexY - 1));
					//retVector.emplace_back(std::make_pair(pClient->sectorIndexX - 1, pClient->sectorIndexY - 1));
				}
			}
			else if (yDir == 1)
			{
				if (!isYMax)
				{
					pClient->possibleSectorCount = 3;
					pClient->sectorArr[1] = { pClient->sectorIndexX, pClient->sectorIndexY + 1 };
					pClient->sectorArr[2] = { pClient->sectorIndexX - 1, pClient->sectorIndexY + 1 };

					//retVector.emplace_back(std::make_pair(pClient->sectorIndexX, pClient->sectorIndexY + 1));
					//retVector.emplace_back(std::make_pair(pClient->sectorIndexX - 1, pClient->sectorIndexY + 1));
				}
			}
		}
		return ;
	}

	assert(false, "FindPossibleSectors : ���⿡ �ɸ����� ���µ�??");
	return;
}

/*
	RenewViewListInSectors
		- �ֽ�ȭ�� ���Ϳ���, �丮��Ʈ�� �����Ѵ�.

	!0. �ݵ�� �� �Լ��� ȣ��Ǳ� ����, RenewPossibleSectors�� ����Ǿ�� ���� ViewList�� ȹ���� �� �ֽ��ϴ�.
*/
void Zone::RenewViewListInSectors(SocketInfo* pClient)
{
	sectorCont[pClient->sectorIndexY][pClient->sectorIndexX].CheckViewList(pClient, zoneContUnit);

	for (int i = 0; i < pClient->possibleSectorCount; ++i)
	{
		sectorCont[pClient->sectorArr[i].second][pClient->sectorArr[i].first].CheckViewList(pClient, zoneContUnit);
	}
}

void Zone::RenewClientSector(SocketInfo* pClient)
{
	bool isNeedToChangeSector{ false };

	if (static_cast<BYTE>(pClient->userData->GetPosition().x / 10) != pClient->sectorIndexX) isNeedToChangeSector = true;
	if (static_cast<BYTE>(pClient->userData->GetPosition().y / 10) != pClient->sectorIndexY) isNeedToChangeSector = true;

	if (isNeedToChangeSector == false) 	return;
	else
	{
		sectorCont[pClient->sectorIndexY][pClient->sectorIndexX].OutClient(pClient);
		sectorCont[pClient->sectorIndexY][pClient->sectorIndexX].InNewClient(pClient);
	}
}

/*
	Zone::RecvCharacterMove(SocketInfo* pClient)
		- Ŭ���̾�Ʈ�κ��� CharacterMove�� �޾��� ���, ȣ��Ǵ� �Լ�.

	#0. �ش� Ŭ���̾�Ʈ�� Move�� ���� ó���մϴ�.
	#1. Zone�� �����ϴ� �ڽ��� ������ ��� Ŭ���̾�Ʈ���� ID���� ��ġ���� �����մϴ�.
*/
void Zone::RecvCharacterMove(SocketInfo* pClient)
{
#ifdef _DEV_MODE_
	std::cout << "[AfterRecv] ���� ���۴�" << int(pClient->loadedBuf[1]) << "����ϴ� ������ : " << int(pClient->loadedBuf[2]) << "\n";
#endif
	moveManager->MoveCharacter(pClient);

	// �����ο��� ����.
	PACKET_DATA::SC::Position packet(
		pClient->clientKey,
		pClient->userData->GetPosition().x,
		pClient->userData->GetPosition().y
	);
	NETWORK_UTIL::SendPacket(pClient, reinterpret_cast<char*>(&packet));

	RenewClientSector(pClient);
	RenewPossibleSectors(pClient);
	RenewViewListInSectors(pClient);

	//moveManager->SendMoveCharacter(pClient, zoneContUnit);
	//sectorCont[pClient->sectorIndexY][pClient->sectorIndexX].RecvCharacterMove(pClient);
}
