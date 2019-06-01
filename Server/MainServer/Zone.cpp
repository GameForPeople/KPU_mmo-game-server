#include "pch.h"
#include "../Define.h"
#include "ServerDefine.h"

#include "TimerManager.h"
#include "ConnectManager.h"
#include "MoveManager.h"
#include "ChatManager.h"

#include "ClientContUnit.h"

#include "UserData.h"

#include "BaseMonster.h"

#include "MemoryUnit.h"
#include "SendMemoryPool.h"

#include "Sector.h"

#include "ObjectInfo.h"

#include "MonsterLoader.h" 
#include "MonsterModelManager.h" 

#include "Zone.h"

Zone::Zone() : 
	connectManager(nullptr),
	moveManager(nullptr),
	monsterModelManager(nullptr),
	sectorCont(),
	zoneContUnit(nullptr),
	recvFunctionArr(nullptr)
{
	InitManagers();
	InitSector();
	InitClientCont();
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
	monsterModelManager = std::make_unique<MonsterModelManager>();
}

/*
	Zone::InitClientCont()
		- GamsServer�� �����ڿ��� ȣ��Ǹ�, Ŭ���̾�Ʈ �����̳ʵ��� �ʱ�ȭ�մϴ�.
*/
void Zone::InitClientCont()
{
	zoneContUnit = new ZoneContUnit;
	_KeyType tempIndex = BIT_CONVERTER::NOT_PLAYER_INT;

	std::cout << "\n#. ���� �Ҵ����Դϴ�." << std::endl;

	//����
	for (auto& monster : zoneContUnit->monsterCont)
	{

		if((tempIndex - BIT_CONVERTER::NOT_PLAYER_INT) % 1000 == 0)
			std::cout << tempIndex - BIT_CONVERTER::NOT_PLAYER_INT << " ";
		
		const _PosType tempPosX = rand() % GLOBAL_DEFINE::MAX_WIDTH;
		const _PosType tempPosY = rand() % GLOBAL_DEFINE::MAX_HEIGHT;

		monster = new BaseMonster(tempIndex++, tempPosX, tempPosY, monsterModelManager->GetRenderModel(MONSTER_TYPE::SLIME));
		
		RenewSelfSectorForNpc(monster->objectInfo); // ����� �ʹ� ŭ.
		//sectorCont[tempPosY / GLOBAL_DEFINE::SECTOR_DISTANCE][tempPosX / GLOBAL_DEFINE::SECTOR_DISTANCE].JoinForNpc(monster->objectInfo);

		// �̵� Ÿ�̸Ӹ� �������.
		//auto timerUnit = TimerManager::GetInstance()->PopTimerUnit();
		//timerUnit->timerType = TIMER_TYPE::NPC_MOVE;
		//timerUnit->objectKey = monster->objectInfo->key;
		//TimerManager::GetInstance()->AddTimerEvent(timerUnit, TIME::SECOND);
	}

	std::cout << "\n#. ���� �Ҵ��� ����Ǿ����ϴ�." << std::endl;
}

/*
	Zone::InitFunctions()
		- GamsServer�� �����ڿ��� ȣ��Ǹ�, ���Ӱ� ���õ� �����͵��� �ʱ�ȭ�� ����մϴ�.
*/
void Zone::InitFunctions()
{
	recvFunctionArr = new std::function<void(Zone&, SocketInfo*)>[PACKET_TYPE::CLIENT_TO_MAIN::ENUM_SIZE];
	recvFunctionArr[PACKET_TYPE::CLIENT_TO_MAIN::MOVE] = &Zone::RecvCharacterMove;
}

/*
	Zone::InitSectorCont()
		- GamsServer�� �����ڿ��� ȣ��Ǹ�, ���͵��� �ʱ�ȭ�մϴ�.
*/
void Zone::InitSector()
{
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

void Zone::ProcessTimerUnit(const int timerManagerContIndex)
{
	concurrency::concurrent_queue<TimerUnit*>* tempCont = TimerManager::GetInstance()->GetTimerContWithIndex(timerManagerContIndex);

	TimerUnit* pUnit = nullptr;
	while (tempCont->try_pop(pUnit))
	{
		switch (auto[objectType, index] = BIT_CONVERTER::WhatIsYourTypeAndRealKey(pUnit->objectKey); objectType)
		{
		case BIT_CONVERTER::OBJECT_TYPE::PLAYER:
			break;
		case BIT_CONVERTER::OBJECT_TYPE::MONSTER:
			switch (pUnit->timerType)
			{
				case (TIMER_TYPE::NPC_MOVE):
				{
					ObjectInfo* tempObjectInfo = zoneContUnit->monsterCont[index]->objectInfo;

					moveManager->MoveRandom(tempObjectInfo);	// �������� �����̰�
					RenewSelfSectorForNpc(tempObjectInfo);		// Ȥ�� �������� ���Ͱ� �ٲ���ϸ� �ٲ� ���ͷ� �������ְ�
					RenewPossibleSectors(tempObjectInfo);		// ���� ������ ��ġ����, Ž���ؾ��ϴ� ���͵��� �����ְ�

					RenewViewListInSectorsForNpc(tempObjectInfo)
						? TimerManager::GetInstance()->AddTimerEvent(pUnit, TIME::SECOND)
						: TimerManager::GetInstance()->AddTimerEvent(pUnit, TIME::SECOND);
					//����ȭ ���Ұ� �� ��¿��
					
					//TimerManager::GetInstance()->PushTimerUnit(pUnit);
					break;
				}
				case (TIMER_TYPE::NPC_ATTACK):
				{
					break;
				}
				case (TIMER_TYPE::SKILL_1_COOLTIME):
				{
					break;
				}
				case (TIMER_TYPE::SKILL_2_COOLTIME):
				{
					break;
				}
				case (TIMER_TYPE::SELF_HEAL):
				{
					break;
				}
				case (TIMER_TYPE::REVIVAL):
				{
					break;
				}
				case (TIMER_TYPE::CC_NODAMAGE):
				{
					if (zoneContUnit->monsterCont[index]->noDamageTick != 0)
					{
						--(zoneContUnit->monsterCont[index]->noDamageTick);
					}
					
					TimerManager::GetInstance()->PushTimerUnit(pUnit);
					break;
				}
				case (TIMER_TYPE::CC_FAINT):
				{
					if (zoneContUnit->monsterCont[index]->faintTick != 0)
					{
						--(zoneContUnit->monsterCont[index]->faintTick);
					}
					
					TimerManager::GetInstance()->PushTimerUnit(pUnit);
					break;
				}
				case (TIMER_TYPE::CC_FREEZE):
				{
					if (zoneContUnit->monsterCont[index]->freezeTick != 0)
					{
						--(zoneContUnit->monsterCont[index]->freezeTick);
					}
					
					TimerManager::GetInstance()->PushTimerUnit(pUnit);
					break;
				}
				case (TIMER_TYPE::CC_ELECTRIC):
				{
					if (zoneContUnit->monsterCont[index]->electricTick != 0)
					{
						--(zoneContUnit->monsterCont[index]->electricTick);
					}
					TimerManager::GetInstance()->PushTimerUnit(pUnit);
					break;
				}
				case (TIMER_TYPE::CC_BURN):
				{
					if (zoneContUnit->monsterCont[index]->burnTick != 0)
					{
						--(zoneContUnit->monsterCont[index]->burnTick);
						zoneContUnit->monsterCont[index]->hp -= STATE::DAMAGE::BURN_DAMAGE;

						if (zoneContUnit->monsterCont[index]->burnTick != 0)
						{
							TimerManager::GetInstance()->AddTimerEvent(pUnit, TIME::SECOND);
						}
						else
						{
							TimerManager::GetInstance()->PushTimerUnit(pUnit);
						}
					}
					else
					{
						TimerManager::GetInstance()->PushTimerUnit(pUnit);
					}
					break;
				}
				case (TIMER_TYPE::ITEM_HP):
				{
					break;
				}
				case (TIMER_TYPE::ITEM_MP):
				{
					break;
				}
				default:
					break;
			}
			break;
		default:
			break;
		}
	}
}

/*
	Zone::TryToEnter()
		- �ش� �������� ������ �õ��մϴ�.

	#0. Zone�� connectManager���� ���� ���� �Ǵ�
	#1. ���� ���� ��, ���� ��ġ(���� 5, 5)�� Ŭ���̾�Ʈ ����.
	#2.	���� ���� ��, �ƹ��� ���� ���� ����
*/
/*std::optional<SocketInfo*>*/ 
std::pair<bool, SocketInfo*> /* == std::pair<bool, SocketInfo*>*/ Zone::TryToEnter()
{
	if (auto retNode = connectManager->LogInToZone(zoneContUnit, this)
		; retNode.first)
	{
		//���� Sector�� Ŭ���̾�Ʈ ����.
		sectorCont[GLOBAL_DEFINE::START_SECTOR_Y][GLOBAL_DEFINE::START_SECTOR_X].Join(retNode.second->objectInfo);

		// InitViewAndSector���� ���εǸ�, Accept Process���� �ش� Ŭ���̾�Ʈ ������ IOCP ��� ��, ȣ����
		//{
			// �ѷ��� ���� �����ϰ� -> ������ ��Ʈ�� ��� ��, ���߿� ��в��ϴ�.
			//RenewPossibleSectors(retNode.second);

		// ģ���� ���� ��Ͱ� -> ������ ��Ʈ�� ��� ��, ���߿� ��в��ϴ�.
			//RenewViewListInSectors(retNode.second);
		//}

		return retNode;
	}
	else return retNode;
}

std::pair<bool, SocketInfo*> Zone::OnlyGetUniqueKeyAndMallocSocketInfo()
{
	if (auto retNode = connectManager->OnlyGetUniqueKeyAndMallocSocketInfo()
		; retNode.first)
	{
		return retNode;
	}
	else return retNode;
}

/*
	Zone::InitViewAndSector()
		- Zone�� Enter ��, ���ʷ� Sector�� ViewList�� �����մϴ�.

	!0. �ش� �� �Լ����� ��Ʈ��ũ �Լ��� �����ϰ� �ֽ��ϴ�.
		- IOCP�� ������ ����� �Ŀ�, ȣ��Ǿ�� �մϴ�.
*/
void Zone::InitViewAndSector(SocketInfo* pClient)
{
	RenewPossibleSectors(pClient->objectInfo);
	RenewViewListInSectors(pClient);
}

/*
	Zone::Exit()
		- �ش� ������ �����ϴ�.

	#0.connectManager���� �ش� �Լ� ó���ϵ��� ����.
*/
void Zone::Exit(SocketInfo* pOutClient)
{
	// ���� �����̳ʿ��� �� ������ �����ְ�
	sectorCont[pOutClient->objectInfo->sectorIndexY][pOutClient->objectInfo->sectorIndexX].Exit(pOutClient->objectInfo);
	
	// �� ViewList�� Client���� �� �����ٰ� �˷��ְ�.
	connectManager->LogOutToZone(pOutClient, zoneContUnit);
}

/*
	FindPossibleSectors? CheckPossibleSectors?
		- ���� ĳ������ ���Ϳ� ��ġ�� �˻��Ͽ�, �þ� üũ�� �ؾ��ϴ� ���͸� �˻��մϴ�.

	?0. ������ ���������� �����Ͽ�, �����ϴ� ��Ŀ���, SocketInfo�� ��������� �δ� ������� �����ϴ°� ���� ������?
*/
void Zone::RenewPossibleSectors(ObjectInfo* pClient)
{
	// ���� ������ �����ϴ� �ڵ忡��, ��� ������ Ȱ���Ͽ� �����ϴ� ������� ����.

	const _PosType nowSectorCenterX = sectorCont[pClient->sectorIndexY][pClient->sectorIndexX].GetCenterX();
	const _PosType nowSectorCenterY = sectorCont[pClient->sectorIndexY][pClient->sectorIndexX].GetCenterY();

	char xDir = 0;	// x ������ �Ǵ� ����
	char yDir = 0;	// y ������ �Ǵ� ����

	/*
		Sector's Size = 10
		View Length = 3

		possible Other Sector Count

		0   1   2 | 3   4   5   6 | 7   8   9
		1	      |				  |
		 -1,1 = 3 |		0,1 = 1	  |	 1,1 = 3
		2		  |				  |
		--------------------------------------
		3		  |				  |
				  |				  |
		4		  |				  |
		 -1,0 = 1 |		0,0 = 0	  |	 1,0 = 1
		5		  |				  |
				  |				  |
		6		  |				  |
		--------------------------------------
		7		  |				  |
				  |				  |
		8		  |				  |
		 -1,-1 = 3|		0,-1 = 1  |	 1,-1 = 3
		9		  |				  |
	*/

	if (pClient->posX > nowSectorCenterX + GLOBAL_DEFINE::SECTOR_PLUS_LIMIT_DISTANCE) { xDir = 1; }
	else if (pClient->posX < nowSectorCenterX - GLOBAL_DEFINE::SECTOR_MINUS_LIMIT_DISTANCE) { xDir = -1; }

	if (pClient->posY > nowSectorCenterY + GLOBAL_DEFINE::SECTOR_PLUS_LIMIT_DISTANCE) { yDir = 1; }
	else if (pClient->posY < nowSectorCenterY - GLOBAL_DEFINE::SECTOR_MINUS_LIMIT_DISTANCE) { yDir = -1; }

	const bool isYZero = pClient->sectorIndexY == 0 ? true : false;
	const bool isYMax = pClient->sectorIndexY == (GLOBAL_DEFINE::SECTOR_END_POSITION) ? true : false;

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
		if (pClient->sectorIndexX == GLOBAL_DEFINE::SECTOR_END_POSITION)	// X�� ���� ��,
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

	assert(false, L"FindPossibleSectors : ���⿡ �ɸ����� ���µ�??");
	return;
}

/*
	RenewViewListInSectors
		- RenewPossibleSectors���� �ֽ�ȭ�� ���͵鿡��, �丮��Ʈ�� �����Ѵ�.

	!0. �ݵ�� �� �Լ��� ȣ��Ǳ� ����, RenewPossibleSectors�� ����Ǿ�� ���� ViewList�� ȹ���� �� �ֽ��ϴ�.
*/
void Zone::RenewViewListInSectors(SocketInfo* pClient)
{
	sectorCont[pClient->objectInfo->sectorIndexY][pClient->objectInfo->sectorIndexX].JudgeClientWithViewList(pClient, zoneContUnit);

	for (int i = 0; i < pClient->objectInfo->possibleSectorCount; ++i)
	{
		sectorCont[pClient->objectInfo->sectorArr[i].second][pClient->objectInfo->sectorArr[i].first].JudgeClientWithViewList(pClient, zoneContUnit);
	}
}

/*
	RenewViewListInSectorsForNPC
		- RenewPossibleSectors���� �ֽ�ȭ�� ���͵鿡��, �丮��Ʈ�� �����Ѵ�.

	!0. �ݵ�� �� �Լ��� ȣ��Ǳ� ����, RenewPossibleSectors�� ����Ǿ�� ���� ViewList�� ȹ���� �� �ֽ��ϴ�.
*/
bool Zone::RenewViewListInSectorsForNpc(ObjectInfo* pClient)
{
	bool retValue = false;

	if (sectorCont[pClient->sectorIndexY][pClient->sectorIndexX].JudgeClientWithViewListForNpc(pClient, zoneContUnit))
		retValue = true;

	for (int i = 0; i < pClient->possibleSectorCount; ++i)
	{
		if (sectorCont[pClient->sectorArr[i].second][pClient->sectorArr[i].first].JudgeClientWithViewListForNpc(pClient, zoneContUnit))
			retValue = true;
	}

	return retValue;
}


/*
	RenewClientSector
		- Ŭ���̾�Ʈ�� �̵��� ��, �ٸ� ���ͷ��� �̵� ���θ� �Ǵ��մϴ�.

	!0. �ݵ�� �� �Լ��� ȣ��Ǳ� ����, RenewPossibleSectors�� ����Ǿ�� ���� ViewList�� ȹ���� �� �ֽ��ϴ�.
*/
void Zone::RenewSelfSector(ObjectInfo* pClient)
{
	bool isNeedToChangeSector{ false };

	if (static_cast<BYTE>(pClient->posX / GLOBAL_DEFINE::SECTOR_DISTANCE) != pClient->sectorIndexX) isNeedToChangeSector = true;
	if (static_cast<BYTE>(pClient->posY / GLOBAL_DEFINE::SECTOR_DISTANCE) != pClient->sectorIndexY) isNeedToChangeSector = true;

	if (isNeedToChangeSector == false) 	return;
	else
	{
		sectorCont[pClient->sectorIndexY][pClient->sectorIndexX].Exit(pClient);
		//sectorCont[pClient->sectorIndexY][pClient->sectorIndexX].Join(pClient);
		sectorCont[static_cast<BYTE>(pClient->posY / GLOBAL_DEFINE::SECTOR_DISTANCE)][static_cast<BYTE>(pClient->posX / GLOBAL_DEFINE::SECTOR_DISTANCE)].Join(pClient);
	}
}

void Zone::RenewSelfSectorForNpc(ObjectInfo* pClient)
{
	bool isNeedToChangeSector{ false };

	if (static_cast<BYTE>(pClient->posX / GLOBAL_DEFINE::SECTOR_DISTANCE) != pClient->sectorIndexX) isNeedToChangeSector = true;
	else if (static_cast<BYTE>(pClient->posY / GLOBAL_DEFINE::SECTOR_DISTANCE) != pClient->sectorIndexY) isNeedToChangeSector = true;

	if (isNeedToChangeSector == false) 	return;
	else
	{
		sectorCont[pClient->sectorIndexY][pClient->sectorIndexX].ExitForNpc(pClient);
		sectorCont[static_cast<BYTE>(pClient->posY / GLOBAL_DEFINE::SECTOR_DISTANCE)][static_cast<BYTE>(pClient->posX / GLOBAL_DEFINE::SECTOR_DISTANCE)].JoinForNpc(pClient);
		//sectorCont[pClient->sectorIndexY][pClient->sectorIndexX].JoinForNpc(pClient);
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
	const bool tempIsMove = moveManager->MoveCharacter(pClient);

	// �����ο��� ����.
	PACKET_DATA::MAIN_TO_CLIENT::Position packet(
		pClient->objectInfo->key,
		pClient->objectInfo->posX,
		pClient->objectInfo->posY
	);
	NETWORK_UTIL::SendPacket(pClient, reinterpret_cast<char*>(&packet));

	if (tempIsMove)
	{
		RenewSelfSector(pClient->objectInfo);
		RenewPossibleSectors(pClient->objectInfo);
		RenewViewListInSectors(pClient);
	}
}

void Zone::RecvChat(SocketInfo* pClient)
{
#ifdef _DEV_MODE_
	std::cout << "[AfterRecv] ä�� ���۸� �޾ҽ��ϴ�. \n";
#endif
	//chatManager->ChatProcess(pClient, zoneContUnit);
}

SocketInfo* Zone::GetSocektInfoInZoneContUnitWithKey(const _KeyType inClientKey)
{
	return zoneContUnit->clientContArr[inClientKey];
}
