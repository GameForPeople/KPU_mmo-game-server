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

#include "BaseMonster.h"
#include "MonsterLoader.h" 
#include "MonsterModelManager.h" 

#include "Zone.h"

Zone::Zone() : 
	//connectManager(nullptr),
	moveManager(nullptr),
	monsterModelManager(nullptr),
	sectorCont(),
	//recvFunctionArr(nullptr),
	zoneContUnit(nullptr)
{
	// �� ������ ����� ���, ������ �������� �ʽ��ϴ�.
	InitManagers();
	InitSector();
	InitZoneCont();
}

Zone::~Zone()
{
	//delete[] recvFunctionArr;
	delete[] zoneContUnit;
}

/*
	Zone::InitManagers()
		- GamsServer�� �����ڿ��� ȣ��Ǹ�, �� �Ŵ������� �ʱ�ȭ�� ����մϴ�.
*/
void Zone::InitManagers()
{
	moveManager = std::make_unique<MoveManager>();
	//connectManager = std::make_unique<ConnectManager>();
	monsterModelManager = std::make_unique<MonsterModelManager>();
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

/*
	Zone::InitZoneCont()
		- GamsServer�� �����ڿ��� ȣ��Ǹ�, Ŭ���̾�Ʈ �����̳ʵ��� �ʱ�ȭ�մϴ�.

	!0. InitSector�� ȣ����� ȣ��Ǿ�� �մϴ�.
*/
void Zone::InitZoneCont()
{
	zoneContUnit = new ZoneContUnit;

	_KeyType tempIndex = BIT_CONVERTER::NOT_PLAYER_INT;

	std::cout << "#. ���� "<< zoneContUnit->monsterCont.size() << " �� �Ҵ����Դϴ�...";

	auto tempMapDate = moveManager->GetMapData();

	// ���� ������ ����
	for (auto& monster : zoneContUnit->monsterCont)
	{
		_PosType tempPosX;
		_PosType tempPosY;
		
		do 
		{
			tempPosX = rand() % GLOBAL_DEFINE::MAX_WIDTH;
			tempPosY = rand() % GLOBAL_DEFINE::MAX_HEIGHT;
		} 
		while (tempMapDate[tempPosY][tempPosX] == false);

		monster = new BaseMonster(tempIndex++, tempPosX, tempPosY, monsterModelManager->GetMonsterModel(MONSTER_TYPE::SLIME));

		sectorCont[tempPosY / GLOBAL_DEFINE::SECTOR_DISTANCE][tempPosX / GLOBAL_DEFINE::SECTOR_DISTANCE].JoinForNpc(monster);
		RenewPossibleSectors(monster->objectInfo);
	}

	std::cout << "    �Ҵ��� ����Ǿ����ϴ�." << std::endl;
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
			switch (pUnit->timerType)
			{
				case (TIMER_TYPE::PUSH_OLD_KEY):
				{
					ConnectManager::GetInstance()->PushOldKey(pUnit->objectKey);
					TimerManager::GetInstance()->PushTimerUnit(pUnit);
					break;
				}
				case (TIMER_TYPE::CC_NODAMAGE):
				{
					//�̰Ŵ� �׻� �����. if - CAS �� �Ȱɾ��.
					zoneContUnit->clientContArr[index]->objectInfo->noDamageFlag = false;
					TimerManager::GetInstance()->PushTimerUnit(pUnit);
					break;
				}
				case (TIMER_TYPE::PLAYER_MOVE):
				{

					break;
				}
				case (TIMER_TYPE::PLAYER_ATTACK):
				{

					break;
				}
				case (TIMER_TYPE::SELF_HEAL):
				{
					auto playerObjectInfo = zoneContUnit->clientContArr[index]->objectInfo;
					if (zoneContUnit->clientContArr[index]->objectInfo->hp > 0)
					{
						const int maxHp = JOB::GetMaxHP(playerObjectInfo->job, playerObjectInfo->level);

						while (7) 
						{
							unsigned short oldHp = zoneContUnit->clientContArr[index]->objectInfo->hp;
							unsigned short newHp = zoneContUnit->clientContArr[index]->objectInfo->hp + (maxHp / 10);

							if (oldHp == 0)
							{
								TimerManager::GetInstance()->PushTimerUnit(pUnit);
								break;
							}
							else if (oldHp == maxHp) 
							{
								TimerManager::GetInstance()->PushTimerUnit(pUnit);
								break;
							}
							else if (newHp > maxHp) newHp = maxHp;

							if (ATOMIC_UTIL::T_CAS(&(zoneContUnit->clientContArr[index]->objectInfo->hp), oldHp, newHp))
							{
								// ��ȸ���Ѱ� ��������.
								TimerManager::GetInstance()->AddTimerEvent(pUnit, TIME::SELF_HEAL);
								break;
							}
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
					zoneContUnit->clientContArr[index]->objectInfo->redTickCount.fetch_sub(1);

					auto playerObjectInfo = zoneContUnit->clientContArr[index]->objectInfo;
					if (zoneContUnit->clientContArr[index]->objectInfo->hp > 0)
					{
						const int maxHp = JOB::GetMaxHP(playerObjectInfo->job, playerObjectInfo->level);

						while (7)
						{
							unsigned short oldHp = zoneContUnit->clientContArr[index]->objectInfo->hp;
							unsigned short newHp = zoneContUnit->clientContArr[index]->objectInfo->hp + ITEM::RED_PER_TICK;

							if (oldHp == 0)
							{
								TimerManager::GetInstance()->PushTimerUnit(pUnit);
								zoneContUnit->clientContArr[index]->objectInfo->redTickCount = 0;
								break;
							}
							else if (oldHp == maxHp)
							{
								TimerManager::GetInstance()->PushTimerUnit(pUnit);
								zoneContUnit->clientContArr[index]->objectInfo->redTickCount = 0;
								break;
							}
							else if (newHp > maxHp) newHp = maxHp;

							if (ATOMIC_UTIL::T_CAS(&(zoneContUnit->clientContArr[index]->objectInfo->hp), oldHp, newHp))
							{
								// ��ȸ���Ѱ� ��������.
								TimerManager::GetInstance()->AddTimerEvent(pUnit, TIME::ITEM_HP);
								break;
							}
						}
					}
					else
					{
						TimerManager::GetInstance()->PushTimerUnit(pUnit);
						zoneContUnit->clientContArr[index]->objectInfo->redTickCount = 0;
					}
					break;
				}
				case (TIMER_TYPE::ITEM_MP):
				{
					zoneContUnit->clientContArr[index]->objectInfo->redTickCount.fetch_sub(1);

					auto playerObjectInfo = zoneContUnit->clientContArr[index]->objectInfo;

					const int maxMp = JOB::GetMaxMP(playerObjectInfo->job, playerObjectInfo->level);

					while (7)
					{
						unsigned short oldMp = zoneContUnit->clientContArr[index]->objectInfo->mp;
						unsigned short newMp = zoneContUnit->clientContArr[index]->objectInfo->mp + ITEM::BLUE_PER_TICK;

						if (oldMp == maxMp)
						{
							TimerManager::GetInstance()->PushTimerUnit(pUnit);
							zoneContUnit->clientContArr[index]->objectInfo->blueTickCount = 0;
							break;
						}
						else if (newMp > maxMp) newMp = maxMp;

						if (ATOMIC_UTIL::T_CAS(&(zoneContUnit->clientContArr[index]->objectInfo->hp), oldMp, newMp))
						{
							// ����ȸ���Ѱ� ��������.
							TimerManager::GetInstance()->AddTimerEvent(pUnit, TIME::ITEM_MP);
							break;
						}
					}

					break;
				}
			}
			break;
		case BIT_CONVERTER::OBJECT_TYPE::MONSTER:
			switch (pUnit->timerType)
			{
				case (TIMER_TYPE::NPC_MOVE):
				{
					BaseMonster* tempBaseMonster = zoneContUnit->monsterCont[index];

					//���� PossibleSector�� ��쿡��, ���� �������� PossibleSector�̹Ƿ� ��ȿ��
					std::unordered_set<_KeyType> oldViewList;
					MakeOldViewListForNpc(oldViewList, zoneContUnit->monsterCont[index]);

					moveManager->MoveRandom(tempBaseMonster->objectInfo);	// �������� �����̰�
					RenewSelfSectorForNpc(tempBaseMonster);					// Ȥ�� �������� ���Ͱ� �ٲ���ϸ� �ٲ� ���ͷ� �������ְ�
					RenewPossibleSectors(tempBaseMonster->objectInfo);		// ���� ������ ��ġ����, Ž���ؾ��ϴ� ���͵��� �����ְ�

					// �ֺ��� Ŭ���̾�Ʈ�� �����ϴ�. �̵��� �����մϴ�.
					if (RenewViewListInSectorsForNpc(oldViewList, zoneContUnit->monsterCont[index]))
					{
						TimerManager::GetInstance()->AddTimerEvent(pUnit, TIME::SLIME_MOVE);
					}
					else
					{
						ATOMIC_UTIL::T_CAS(&(tempBaseMonster->isSleep), true, false); // ����� ���� ���߿���!
						TimerManager::GetInstance()->PushTimerUnit(pUnit);
					}
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
				case (TIMER_TYPE::REVIVAL):
				{
					break;
				}
				case (TIMER_TYPE::CC_FAINT):
				{
					zoneContUnit->monsterCont[index]->objectInfo->faintTick.fetch_sub(1);
					TimerManager::GetInstance()->PushTimerUnit(pUnit);
					break;
				}
				case (TIMER_TYPE::CC_FREEZE):
				{
					zoneContUnit->monsterCont[index]->freezeTick.fetch_sub(1);
					TimerManager::GetInstance()->PushTimerUnit(pUnit);
					break;
				}
				case (TIMER_TYPE::CC_ELECTRIC):
				{
					zoneContUnit->monsterCont[index]->electricTick.fetch_sub(1);
					TimerManager::GetInstance()->PushTimerUnit(pUnit);
					break;
				}
				case (TIMER_TYPE::CC_BURN_3):
				{
					while (7) 
					{
						unsigned short oldHp = zoneContUnit->monsterCont[index]->objectInfo->hp;
						unsigned short newHp = oldHp - (oldHp / 100);

						if (oldHp > 0)
						{
							if (ATOMIC_UTIL::T_CAS(&(zoneContUnit->monsterCont[index]->objectInfo->hp), oldHp, newHp))
							{
								pUnit->timerType = TIMER_TYPE::CC_BURN_2;
								TimerManager::GetInstance()->AddTimerEvent(pUnit, TIME::SECOND);
								break;
							}
						}
						else
						{
							TimerManager::GetInstance()->PushTimerUnit(pUnit);
							break;
						}
					}
					break;
				}
				case (TIMER_TYPE::CC_BURN_2):
				{
					while (7)
					{
						unsigned short oldHp = zoneContUnit->monsterCont[index]->objectInfo->hp;
						unsigned short newHp = oldHp - (oldHp / 100);

						if (oldHp > 0)
						{
							if (ATOMIC_UTIL::T_CAS(&(zoneContUnit->monsterCont[index]->objectInfo->hp), oldHp, newHp))
							{
								pUnit->timerType = TIMER_TYPE::CC_BURN_1;
								TimerManager::GetInstance()->AddTimerEvent(pUnit, TIME::SECOND);
								break;
							}
						}
						else
						{
							TimerManager::GetInstance()->PushTimerUnit(pUnit);
							break;
						}
					}
					break;
				}
				case (TIMER_TYPE::CC_BURN_1):
				{
					while (7)
					{
						unsigned short oldHp = zoneContUnit->monsterCont[index]->objectInfo->hp;
						unsigned short newHp = oldHp - (oldHp / 100);

						if (oldHp > 0)
						{
							if (ATOMIC_UTIL::T_CAS(&(zoneContUnit->monsterCont[index]->objectInfo->hp), oldHp, newHp))
							{
								TimerManager::GetInstance()->PushTimerUnit(pUnit);
								break;
							}
						}
						else
						{
							TimerManager::GetInstance()->PushTimerUnit(pUnit);
							break;
						}
					}
					break;
				}
				default:
					assert(false, L"[ERROR] ���ǵ��� ���� Tmier Unit�� ����Ǿ����ϴ�. ������ �����մϴ�.");

					// ����װ� �ƴ� ���! ������ �׳� �ݳ�.
					TimerManager::GetInstance()->PushTimerUnit(pUnit);

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
//std::pair<bool, SocketInfo*> /* == std::pair<bool, SocketInfo*>*/ Zone::TryToEnter()
//{
//	if (auto retNode = connectManager->LogInToZone(zoneContUnit, this)
//		; retNode.first)
//	{
//		//���� Sector�� Ŭ���̾�Ʈ ����.
//		sectorCont[GLOBAL_DEFINE::START_SECTOR_Y][GLOBAL_DEFINE::START_SECTOR_X].Join(retNode.second->objectInfo);
//
//		// InitViewAndSector���� ���εǸ�, Accept Process���� �ش� Ŭ���̾�Ʈ ������ IOCP ��� ��, ȣ����
//		//{
//			// �ѷ��� ���� �����ϰ� -> ������ ��Ʈ�� ��� ��, ���߿� ��в��ϴ�.
//			//RenewPossibleSectors(retNode.second);
//
//		// ģ���� ���� ��Ͱ� -> ������ ��Ʈ�� ��� ��, ���߿� ��в��ϴ�.
//			//RenewViewListInSectors(retNode.second);
//		//}
//
//		return retNode;
//	}
//	else return retNode;
//}

/*
	Zone::Enter()
		- �ش� �� �� ���Ϳ� ����, ���̴� �ֵ�ְ� �˸��ϴ�.

	#0.connectManager���� �ش� �Լ� ó���ϵ��� ����.�� ���ΰ� ������~
*/
void Zone::Enter(SocketInfo* pEnteredClient)
{
	// ���� �����̳ʿ��� �� ������ ���� �־��ְ�.
	sectorCont[pEnteredClient->objectInfo->posY / GLOBAL_DEFINE::SECTOR_DISTANCE][pEnteredClient->objectInfo->posX / GLOBAL_DEFINE::SECTOR_DISTANCE].Join(pEnteredClient);

	// PossibleSector�� View ó��
	InitViewAndSector(pEnteredClient);
}

/*
	Zone::Exit()
		- �ش� ������ �����ϴ�.

	#0.connectManager���� �ش� �Լ� ó���ϵ��� ����.
*/
void Zone::Exit(SocketInfo* pOutClient)
{
	// ���� ���ԵǾ� �ִ�, ���� �����̳ʿ��� �� ������ �����ְ�
	sectorCont[pOutClient->objectInfo->sectorIndexY][pOutClient->objectInfo->sectorIndexX].Exit(pOutClient);
	
	// �� ViewList�� Client���� �� �����ٰ� �˷��ְ�.
	//connectManager->LogOutToZone(pOutClient, zoneContUnit);

	for (auto iter : pOutClient->viewList)
	{
		zoneContUnit->clientContArr[iter]->viewList.erase(pOutClient->key);
		NETWORK_UTIL::SEND::SendRemovePlayer(pOutClient->key, zoneContUnit->clientContArr[iter]);
	}
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
	FindPossibleSectors? CheckPossibleSectors?
		- ���� ĳ������ ���Ϳ� ��ġ�� �˻��Ͽ�, �þ� üũ�� �ؾ��ϴ� ���͸� �˻��մϴ�.

	?0. ������ ���������� �����Ͽ�, �����ϴ� ��Ŀ���, SocketInfo�� ��������� �δ� ������� �����ϴ°� ���� ������? -> �׷��� �ߴٰ� �̰� ���� ��
*/
void Zone::RenewPossibleSectors(ObjectInfo* pClient)
{
	// ���� ������ �����ϴ� �ڵ忡��, ��� ������ Ȱ���Ͽ� �����ϴ� ������� ����.

	const _PosType nowSectorCenterX = sectorCont[pClient->sectorIndexY][pClient->sectorIndexX].GetCenterX();
	const _PosType nowSectorCenterY = sectorCont[pClient->sectorIndexY][pClient->sectorIndexX].GetCenterY();

	char xDir = 0;	// x ������ �Ǵ� ����
	char yDir = 0;	// y ������ �Ǵ� ����

	/*
		Sector's Size = 10 example
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
	pClient->viewListLock.lock_shared();
	auto oldViewList = pClient->viewList;
	pClient->viewListLock.unlock_shared();

	std::unordered_set<_ClientKeyType> newViewList;

	pClient->monsterViewListLock.lock_shared();
	auto oldMonsterViewList = pClient->monsterViewList;
	pClient->monsterViewListLock.unlock_shared();

	std::unordered_set<_ClientKeyType> newMonsterViewList;
	
	// Make NewViewList
	sectorCont[pClient->objectInfo->sectorIndexY][pClient->objectInfo->sectorIndexX].MakeNewViewList(newViewList, newMonsterViewList, pClient, zoneContUnit);

	for (int i = 0; i < pClient->objectInfo->possibleSectorCount; ++i)
	{
		sectorCont[pClient->objectInfo->sectorArr[i].second][pClient->objectInfo->sectorArr[i].first].MakeNewViewList(newViewList, newMonsterViewList, pClient, zoneContUnit);
	}

	// Client
	for (auto otherClientKey : newViewList)
	{
		if (SocketInfo* pOtherClient = zoneContUnit->clientContArr[otherClientKey];
			oldViewList.count(otherClientKey) != 0)
		{
			pOtherClient->viewListLock.lock();
			if (0 != pOtherClient->viewList.count(pClient->key))
			{
				pOtherClient->viewListLock.unlock();
				NETWORK_UTIL::SEND::SendMovePlayer<SocketInfo, PACKET_DATA::MAIN_TO_CLIENT::Position>(pClient, pOtherClient);
			}
			else
			{
				pOtherClient->viewList.insert(pClient->key);
				pOtherClient->viewListLock.unlock();
				NETWORK_UTIL::SEND::SendPutPlayer<SocketInfo, PACKET_DATA::MAIN_TO_CLIENT::PutPlayer>(pClient, pOtherClient);
			}
		}
		else
			// ���� �þ߿� ����.
		{
			pClient->viewListLock.lock();
			pClient->viewList.insert(otherClientKey);
			pClient->viewListLock.unlock();

			NETWORK_UTIL::SEND::SendPutPlayer<SocketInfo, PACKET_DATA::MAIN_TO_CLIENT::PutPlayer>(pClient, pOtherClient);

			pOtherClient->viewListLock.lock();
			if (0 != pOtherClient->viewList.count(pClient->key)) {
				pOtherClient->viewListLock.unlock();
				NETWORK_UTIL::SEND::SendMovePlayer<SocketInfo, PACKET_DATA::MAIN_TO_CLIENT::Position>(pOtherClient, pClient);
			}
			else {
				pOtherClient->viewList.insert(pClient->key);
				pOtherClient->viewListLock.unlock();
				NETWORK_UTIL::SEND::SendPutPlayer<SocketInfo, PACKET_DATA::MAIN_TO_CLIENT::PutPlayer>(pOtherClient, pClient);
			}
		}
	}

	for (auto otherClientKey : oldViewList)
	{
		if (newViewList.count(otherClientKey) != 0) continue;

		pClient->viewListLock.lock();
		pClient->viewList.erase(otherClientKey);
		pClient->viewListLock.unlock();

		NETWORK_UTIL::SEND::SendRemovePlayer(otherClientKey, pClient);

		SocketInfo* pOtherClient = zoneContUnit->clientContArr[otherClientKey];

		pOtherClient->viewListLock.lock();
		if (0 != pOtherClient->viewList.count(pClient->key))
		{
			pOtherClient->viewList.erase(pClient->key);
			 pOtherClient->viewListLock.unlock();
			NETWORK_UTIL::SEND::SendRemovePlayer(pClient->key, pOtherClient);
		}
		else pOtherClient->viewListLock.unlock();
	}

	// Monster

	for (auto otherMonsterKey : newMonsterViewList)
	{
		if (oldMonsterViewList.count(otherMonsterKey) != 0)
		{
			// ������ �ִ� ģ����.
			// ���� ģ���� �ȳ��! �� �����δپ�~~~!!
		}
		else
		{
			// ���� �þ߿� ����.
			auto pMonster = zoneContUnit->monsterCont[otherMonsterKey - BIT_CONVERTER::NOT_PLAYER_INT];
			pClient->monsterViewListLock.lock();
			pClient->monsterViewList.insert(otherMonsterKey);
			pClient->monsterViewListLock.unlock();

			NETWORK_UTIL::SEND::SendPutPlayer<BaseMonster, PACKET_DATA::MAIN_TO_CLIENT::PutPlayer>(pMonster, pClient);

			if (ATOMIC_UTIL::T_CAS(&(pMonster->isSleep), false, true))
			{
				// �̵� Ÿ�̸Ӹ� �������.
				auto timerUnit = TimerManager::GetInstance()->PopTimerUnit();
				timerUnit->timerType = TIMER_TYPE::NPC_MOVE;
				timerUnit->objectKey = pMonster->key;
				TimerManager::GetInstance()->AddTimerEvent(timerUnit, TIME::SLIME_MOVE);
			}
			//else  // �̹� true�� ��� �� �� ����!
		}
	}

	for (auto otherMonsterKey : oldMonsterViewList)
	{
		auto pMonster = zoneContUnit->monsterCont[otherMonsterKey - BIT_CONVERTER::NOT_PLAYER_INT];

		if (0 != newMonsterViewList.count(otherMonsterKey)) continue;

		pClient->monsterViewListLock.lock();
		pClient->monsterViewList.erase(otherMonsterKey);
		pClient->monsterViewListLock.unlock();

		NETWORK_UTIL::SEND::SendRemovePlayer(otherMonsterKey, pClient);
	}
}

/*
	RenewViewListInSectorsForNPC
		- RenewPossibleSectors���� �ֽ�ȭ�� ���͵鿡��, �丮��Ʈ�� �����Ѵ�.

	!0. �ݵ�� �� �Լ��� ȣ��Ǳ� ����, RenewPossibleSectors�� ����Ǿ�� ���� ViewList�� ȹ���� �� �ֽ��ϴ�.
*/
bool Zone::RenewViewListInSectorsForNpc(const std::unordered_set<_KeyType>& oldViewList, BaseMonster* pMonster)
{
	bool retValue = false;
	auto pObjectInfo = pMonster->objectInfo;
	std::unordered_set<_KeyType> newViewList;

	// make newViewList

	sectorCont[pObjectInfo->sectorIndexY][pObjectInfo->sectorIndexX].MakeNewViewListForNpc(newViewList, pMonster, zoneContUnit);

	for (int i = 0; i < pObjectInfo->possibleSectorCount; ++i)
	{
		sectorCont[pObjectInfo->sectorArr[i].second][pObjectInfo->sectorArr[i].first].MakeNewViewListForNpc(newViewList, pMonster, zoneContUnit);
	}

	// 

	_KeyType monsterKey = pMonster->key;
	for (auto clientKey : oldViewList)
	{
		if (auto pOtherClient = zoneContUnit->clientContArr[clientKey]; 
			0 != newViewList.count(clientKey)) {
			pOtherClient->monsterViewListLock.lock();
			
			if (pOtherClient->monsterViewList.count(monsterKey)) {
				pOtherClient->monsterViewListLock.unlock();
				NETWORK_UTIL::SEND::SendMovePlayer<BaseMonster, PACKET_DATA::MAIN_TO_CLIENT::Position>(pMonster, pOtherClient);
			}
			else {
				pOtherClient->monsterViewList.insert(monsterKey);
				pOtherClient->monsterViewListLock.unlock();
				NETWORK_UTIL::SEND::SendPutPlayer< BaseMonster, PACKET_DATA::MAIN_TO_CLIENT::PutPlayer>(pMonster, pOtherClient);
			}
		}
		else {
			pOtherClient->monsterViewListLock.lock();

			if (0 < pOtherClient->monsterViewList.count(monsterKey)) {
				pOtherClient->monsterViewList.erase(monsterKey);
				pOtherClient->monsterViewListLock.unlock();
				NETWORK_UTIL::SEND::SendRemovePlayer(monsterKey, pOtherClient);
			}
			else pOtherClient->monsterViewListLock.unlock();
		}
	}
	for (auto clientKey : newViewList)
	{
		if (oldViewList.count(clientKey) == 0)
		{
			auto pOtherClient = zoneContUnit->clientContArr[clientKey];

			pOtherClient->monsterViewListLock.lock();

			if (0 == pOtherClient->monsterViewList.count(monsterKey)) {
				pOtherClient->monsterViewList.insert(monsterKey);
				pOtherClient->monsterViewListLock.unlock();
				NETWORK_UTIL::SEND::SendPutPlayer<BaseMonster, PACKET_DATA::MAIN_TO_CLIENT::PutPlayer>(pMonster, zoneContUnit->clientContArr[clientKey]);
			}
			else {
				pOtherClient->monsterViewListLock.unlock();
				NETWORK_UTIL::SEND::SendMovePlayer<BaseMonster, PACKET_DATA::MAIN_TO_CLIENT::Position>(pMonster, zoneContUnit->clientContArr[clientKey]);
			}
		}
	}
	// �� �� �ֺ��� ���� �ƹ��� ����! ���� ����߾� ��
	return static_cast<bool>(newViewList.size());
}

void Zone::MakeOldViewListForNpc(std::unordered_set<_KeyType>& retVewList, BaseMonster* pMonster)
{
	sectorCont[pMonster->objectInfo->sectorIndexY][pMonster->objectInfo->sectorIndexX].MakeOldViewListForNpc(retVewList, pMonster, zoneContUnit);

	for (int i = 0; i < pMonster->objectInfo->possibleSectorCount; ++i)
	{
		sectorCont[pMonster->objectInfo->sectorArr[i].second][pMonster->objectInfo->sectorArr[i].first].MakeOldViewListForNpc(retVewList, pMonster, zoneContUnit);
	}
}


/*
	RenewClientSector
		- Ŭ���̾�Ʈ�� �̵��� ��, �ٸ� ���ͷ��� �̵� ���θ� �Ǵ��մϴ�.

	!0. �ݵ�� �� �Լ��� ȣ��Ǳ� ����, RenewPossibleSectors�� ����Ǿ�� ���� ViewList�� ȹ���� �� �ֽ��ϴ�.
*/
void Zone::RenewSelfSector(SocketInfo* pClient)
{
	ObjectInfo* tempObjectInfo = pClient->objectInfo;

	_SectorIndexType tempX = static_cast<_SectorIndexType>(tempObjectInfo->posX / GLOBAL_DEFINE::SECTOR_DISTANCE);
	_SectorIndexType tempY = static_cast<_SectorIndexType>(tempObjectInfo->posY / GLOBAL_DEFINE::SECTOR_DISTANCE);

	if (tempX != tempObjectInfo->sectorIndexX || tempY != tempObjectInfo->sectorIndexY)
	{
		sectorCont[tempObjectInfo->sectorIndexY][tempObjectInfo->sectorIndexX].Exit(pClient);
		//sectorCont[pClient->sectorIndexY][pClient->sectorIndexX].Join(pClient);
		sectorCont[tempY][tempX].Join(pClient);
	}
	else return;
}

void Zone::RenewSelfSectorForNpc(BaseMonster* pMonster)
{
	ObjectInfo* tempObjectInfo = pMonster->objectInfo;

	_SectorIndexType tempX = static_cast<_SectorIndexType>(tempObjectInfo->posX / GLOBAL_DEFINE::SECTOR_DISTANCE);
	_SectorIndexType tempY = static_cast<_SectorIndexType>(tempObjectInfo->posY / GLOBAL_DEFINE::SECTOR_DISTANCE);

	if (tempX != tempObjectInfo->sectorIndexX || tempY != tempObjectInfo->sectorIndexY)
	{
		sectorCont[tempObjectInfo->sectorIndexY][tempObjectInfo->sectorIndexX].ExitForNpc(pMonster);
		sectorCont[tempY][tempX].JoinForNpc(pMonster);
		//sectorCont[pClient->sectorIndexY][pClient->sectorIndexX].JoinForNpc(pClient);
	}
	else return;
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
	//std::cout << "[AfterRecv] ���� ���۴�" << int(pClient->loadedBuf[1]) << "����ϴ� ������ : " << int(pClient->loadedBuf[2]) << "\n";
#endif

	if (moveManager->MoveCharacter(pClient))
	{
		// �����ο��� ����.
		NETWORK_UTIL::SEND::SendMovePlayer<SocketInfo, PACKET_DATA::MAIN_TO_CLIENT::Position>(pClient, pClient);
		//PACKET_DATA::MAIN_TO_CLIENT::Position packet(
		//	pClient->key,
		//	pClient->objectInfo->posX,
		//	pClient->objectInfo->posY
		//);
		//NETWORK_UTIL::SendPacket(pClient, reinterpret_cast<char*>(&packet));

		RenewSelfSector(pClient);
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


const std::vector<std::vector<bool>>& Zone::GetMapData()
{
	return moveManager->GetMapData();
}