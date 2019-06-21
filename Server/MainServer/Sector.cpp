#include "pch.h"
#include "../Define.h"
#include "ServerDefine.h"

#include "ClientContUnit.h"
#include "MemoryUnit.h"
#include "UserData.h"

#include "ObjectInfo.h"
#include "BaseMonster.h"

#include "Sector.h"

#include "TimerManager.h"

Sector::Sector(const BYTE inX, const BYTE inY)
	: indexX(inX)
	, indexY(inY)
	, centerX(inX * GLOBAL_DEFINE::SECTOR_DISTANCE + GLOBAL_DEFINE::SECTOR_HALF_DISTANCE)
	, centerY(inY * GLOBAL_DEFINE::SECTOR_DISTANCE + GLOBAL_DEFINE::SECTOR_HALF_DISTANCE)
	, sectorContUnit(new SectorContUnit)
{
	//std::cout << "indexX : " << (int)indexX << ", centerX : " << (int)centerX << ", indexY : " << (int)indexY << ", centerY : " << (int)centerY << std::endl;
}

Sector::~Sector()
{
	delete sectorContUnit;
}

/*
	Join
		- ���Ϳ� ���ο� Ŭ���̾�Ʈ�� ���ɴϴ�.
*/
void Sector::Join(SocketInfo* pClient)
{
	sectorContUnit->clientLock.lock(); //+++++++++++++++++++++++++++++++++++++1
	sectorContUnit->clientCont.emplace(pClient->key);
	sectorContUnit->clientLock.unlock(); //-----------------------------------0

	pClient->objectInfo->sectorIndexX = indexX;
	pClient->objectInfo->sectorIndexY = indexY;
}

/*
	Exit
		- ���Ϳ� �ִ� Ŭ���̾�Ʈ�� �����ϴ�.
*/
void Sector::Exit(SocketInfo* pInClient)
{
	// ���� �������� ���� �����ְ�.
	sectorContUnit->clientLock.lock(); //+++++++++++++++++++++++++++++++++++++1
	
	// �̰� ���� ��ģ���� �ڵ��;
	//for (auto iter = sectorContUnit->clientCont.begin()
	//	; iter != sectorContUnit->clientCont.end()
	//	; ++iter)
	//
	//	if (pInClient->key == *iter)
	//	{
	//		sectorContUnit->clientCont.erase(iter);
	//		break;
	//	}
	//

	sectorContUnit->clientCont.erase(pInClient->key);

	sectorContUnit->clientLock.unlock(); //-----------------------------------0
}

void Sector::JoinForNpc(BaseMonster* pClientObject)
{
	sectorContUnit->monsterLock.lock(); //+++++++++++++++++++++++++++++++++++++1
	sectorContUnit->monsterCont.emplace(pClientObject->key);
	sectorContUnit->monsterLock.unlock(); //-----------------------------------0

	pClientObject->objectInfo->sectorIndexX = indexX;
	pClientObject->objectInfo->sectorIndexY = indexY;
}

void Sector::ExitForNpc(BaseMonster* pMonster)
{
	sectorContUnit->monsterLock.lock(); //+++++++++++++++++++++++++++++++++++++1
	
	// �ϵ� �̰� ���� �ڵ��;
	//for (auto iter = sectorContUnit->monsterCont.begin()
	//	; iter != sectorContUnit->monsterCont.end()
	//	; ++iter)
	//{
	//	if (pInClient->key == *iter)
	//	{
	//		sectorContUnit->monsterCont.erase(iter);
	//		break;
	//	}
	//}
	sectorContUnit->monsterCont.erase(pMonster->key);

	sectorContUnit->monsterLock.unlock(); //-----------------------------------0
}

/*
	JudgeClientInViewList
	 - Sector�� �����̳ʿ��� ���� �ִ� Ŭ���̾�Ʈ�� �Ǵ��ϰ�, viewList�� ���ο� ���� ó���մϴ�..

	 !0. ��ģ �̻��� ���� ����, ZoneContUnit�� ���� �ɴϴ�... �߸����� ���� ���ٰ��� �Ű���!
	 !1. ������ viewList�� ���� ��, ����ȭ ������ ������?? ConCurrency�� �ƴ� Lock�� ��� �ɲ� ����.
	 !2. ���ο��� ��Ʈ��ũ �Լ��� ȣ��˴ϴ�.
*/
void Sector::MakeNewViewList(std::unordered_set<_KeyType>& newViewList, std::unordered_set<_KeyType>& newMonsterViewList, SocketInfo* pClient, ZoneContUnit* pZoneContUnit)
{
	if (sectorContUnit->clientCont.size() > 0)
	{
		sectorContUnit->clientLock.lock_shared();	//++++++++++++++++++++++++++++1	Sector : Read Lock!
		for (auto/*&*/ otherKey : sectorContUnit->clientCont)
		{
			if (otherKey == pClient->key) continue;

			SocketInfo* pOtherClient = pZoneContUnit->clientContArr[otherKey];

			//if (!isOn) continue; // �̷��� ��������.

			if (IsSeeEachOther(pClient->objectInfo, pOtherClient->objectInfo))
			{
				// ���� ���Դϴ�.
				newViewList.insert(otherKey);
				/*
				if (pClient->viewList.find(otherKey) == pClient->viewList.end())
				{
					// ���� ���̰�, ���� �𸣴� ���̿��� ��.
					SendPutPlayer(pClient->objectInfo, pOtherClient);
					SendPutPlayer(pOtherClient->objectInfo, pClient);

					// ������ �� ����Ʈ�� �߰��� ��... ������ �� �� �ֽ��ϴ�.
					pClient->viewList.insert(otherKey);
					pOtherClient->viewList.insert(pClient->objectInfo->key);
				}
				else
				{
					//���� ���̰�, ���� �ƴ� ���̿��� ��,

					// ���� ���� �˾Ƽ� �Ұ� �� �� �ٲ�� �޾ƶ� �丶!
					SendMovePlayer(pClient->objectInfo, pOtherClient);
				}
				*/
			}
			/*
			else
			{
				// ���� �Ⱥ��Դϴ�.
				if (pClient->viewList.find(otherKey) != pClient->viewList.end())
				{
					// ���� �� ���̰�, ���� ���� �˴� Ŭ���̾�Ʈ���� ��.
					SendRemovePlayer(pClient->objectInfo->key, pOtherClient);
					SendRemovePlayer(otherKey, pClient);

					// ������ �� ����Ʈ���� ������ ��... ������ �� �� �ֽ��ϴ�.
					// ������ ������?? ����ϴٻ����
					pClient->viewList.unsafe_erase(otherKey);
					pOtherClient->viewList.unsafe_erase(pClient->objectInfo->key);
				}
				//else
				//{
				//	// ���� �� ���̰�, ���� ���� �𸣴� Ŭ���̾�Ʈ���� ��.
				//}
			}
			*/

		}
		sectorContUnit->clientLock.unlock_shared();	//----------------------------0	Sector : Read Lock!
	}

	if (sectorContUnit->monsterCont.size() > 0)
	{
		sectorContUnit->monsterLock.lock_shared(); //++++++++++++++++++++++++++++1	Sector : Read Lock!
		for (auto/*&*/ otherKey : sectorContUnit->monsterCont)
		{
			auto pMonster = pZoneContUnit->monsterCont[otherKey - BIT_CONVERTER::NOT_PLAYER_INT];
			if (IsSeeEachOther(pClient->objectInfo, pMonster->objectInfo))
			{
				// ���� ���Դϴ�.
				newMonsterViewList.insert(otherKey);
				/*
				if (pClient->monsterViewList.find(otherKey) == pClient->monsterViewList.end())
				{
					// ���� ���̰�, ���� �𸣴� ���̿��� ��.
					SendPutPlayer(pMonster->objectInfo, pClient);

					// ������ �� ����Ʈ�� �߰��� ��... ������ �� �� �ֽ��ϴ�.
					pClient->monsterViewList.insert(otherKey);
				}
				else
				{
					//���� ���̰�, ���� �ƴ� ���̿��� ��,

					// ���� ���� �˾Ƽ� �Ұ� �� �� �ٲ�� �޾ƶ� �丶!
					SendMovePlayer(pMonster->objectInfo, pClient);
				}
				*/
			}
			/*
			else
			{
				// ���� �Ⱥ��Դϴ�.
				if (pClient->monsterViewList.find(otherKey) != pClient->monsterViewList.end())
				{
					// ���� �� ���̰�, ���� ���� �˴� Ŭ���̾�Ʈ���� ��.
					//SendRemovePlayer(pClient->objectInfo->key, pOtherClient);
					SendRemovePlayer(otherKey, pClient);

					// ������ �� ����Ʈ���� ������ ��... ������ �� �� �ֽ��ϴ�.
					// ������ ������?? ����ϴٻ����
					pClient->monsterViewList.unsafe_erase(otherKey);
				}
				//else
				//{
				//	// ���� �� ���̰�, ���� ���� �𸣴� Ŭ���̾�Ʈ���� ��.
				//}
			}
			*/
		}
		sectorContUnit->monsterLock.unlock_shared(); //----------------------------0	Sector : Read Lock!
	}
}

/*bool*/ void Sector::MakeNewViewListForNpc(std::unordered_set<_KeyType>& newViewList, BaseMonster* pMonster, ZoneContUnit* pZoneContUnit)
{
	sectorContUnit->clientLock.lock_shared();	//++++++++++++++++++++++++++++1	Sector : Read Lock!
	// ���� 0���̴��� �ƴϴ��� �˻�
	
	if (sectorContUnit->clientCont.size() == 0) {
		/*std::cout << "�ֺ���ֽ��ϴ�!" << std::endl; */ 
		sectorContUnit->clientLock.unlock_shared();	//----------------------------0	Sector : Read Lock!
		return; //�̰� ���ƾ����ٵ�? }
	}

	for (auto/*&*/ otherKey : sectorContUnit->clientCont)
	{
		//auto[isOn, pOtherClient] = pZoneContUnit->FindClient(otherKey /*- BIT_CONVERTER::NOT_PLAYER_INT*/);
		//if (!isOn) continue; // ? �� ����
		if (IsSeeEachOther(pMonster->objectInfo, pZoneContUnit->clientContArr[otherKey]->objectInfo))
		{
			newViewList.emplace(otherKey);
			// ���� ���Դϴ�.
			//if (pOtherClient->monsterViewList.find(pClient->key /* - BIT_CONVERTER::NOT_PLAYER_INT*/) == pOtherClient->monsterViewList.end())
			//{
			//	// ���� ���̰�, ���� �𸣴� ���̿��� ��.
			//	SendPutPlayer(pClient, pOtherClient);
			//	//SendPutPlayer(pOtherClient->objectInfo, pClient);
			//
			//	// ������ �� ����Ʈ�� �߰��� ��... ������ �� �� �ֽ��ϴ�.
			//	//pClient->viewList.insert(otherKey);
			//	pOtherClient->monsterViewList.insert(pClient->key);
			//}
			//else
			//{
			//	//���� ���̰�, ���� �ƴ� ���̿��� ��,
			//
			//	// ���� �̹� ��������, �� �� �ٲ�� �޾ƶ� �丶!
			//	SendMovePlayer(pClient, pOtherClient);
			//}
		}
		//else
		//{
		//	// ���� �Ⱥ��Դϴ�.
		//	if (pOtherClient->monsterViewList.find(pClient->key) != pOtherClient->monsterViewList.end())
		//	{
		//		// ���� �� ���̰�, ���� ���� �˴� Ŭ���̾�Ʈ���� ��.
		//
		//		SendRemovePlayer(pClient->key, pOtherClient);
		//		//SendRemovePlayer(otherKey, pClient);
		//
		//		// ������ �� ����Ʈ���� ������ ��... ������ �� �� �ֽ��ϴ�.
		//		// ������ ������?? ����ϴٻ����
		//		pOtherClient->monsterViewList.unsafe_erase(pClient->key);
		//	}
		//	//else
		//	//{
		//	//	// ���� �� ���̰�, ���� ���� �𸣴� Ŭ���̾�Ʈ���� ��.
		//	//}
		//}
	}
	sectorContUnit->clientLock.unlock_shared();	//----------------------------0	Sector : Read Lock!
}

void Sector::MakeOldViewListForNpc(std::unordered_set<_KeyType>& retOldViewList, BaseMonster* pMonster, ZoneContUnit* pZoneContUnit)
{
	sectorContUnit->clientLock.lock_shared();
	for (auto/*&*/ otherKey : sectorContUnit->clientCont)
	{
		if (IsSeeEachOther(pMonster->objectInfo, pZoneContUnit->clientContArr[otherKey]->objectInfo))
		{
			retOldViewList.emplace(otherKey);
		}
	}
	sectorContUnit->clientLock.unlock_shared();
}
/*
	IsSeeEachOther
		- ��! ���� ���� �ֳ� ����! ��ġ�� �Ѵ� ���� ���� ��!
*/
//bool Sector::IsSeeEachOther(const _PosType aPosX, const _PosType aPosY, const _PosType bPosX, const _PosType bPosY) const noexcept
//{
//	if (GLOBAL_DEFINE::VIEW_DISTANCE < abs(aPosX - bPosX)) return false;
//	if (GLOBAL_DEFINE::VIEW_DISTANCE < abs(aPosY - bPosY)) return false;
//	return true;
//}
//
//bool Sector::IsSeeEachOther(const std::pair<_PosType, _PosType>& inAPosition, const std::pair<_PosType, _PosType>& inBPosition) const noexcept
//{
//	if (GLOBAL_DEFINE::VIEW_DISTANCE < abs(inAPosition.first - inBPosition.first)) return false;
//	if (GLOBAL_DEFINE::VIEW_DISTANCE < abs(inAPosition.second - inBPosition.second)) return false;
//	return true;
//}

bool Sector::IsSeeEachOther(const ObjectInfo* const objectA, const ObjectInfo* const objectB) const noexcept
{
	if (GLOBAL_DEFINE::VIEW_DISTANCE < abs(objectA->posX - objectB->posX)) return false;
	if (GLOBAL_DEFINE::VIEW_DISTANCE < abs(objectA->posY - objectB->posY)) return false;
	return true;
}

