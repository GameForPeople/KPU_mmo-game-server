#include "pch.h"
#include "../Define.h"
#include "ServerDefine.h"

#include "ClientContUnit.h"
#include "MemoryUnit.h"
#include "UserData.h"

#include "ObjectInfo.h"
#include "BaseMonster.h"

#include "Sector.h"

Sector::Sector(const BYTE inX, const BYTE inY)
	: indexX(inX)
	, indexY(inY)
	, centerX(inX * GLOBAL_DEFINE::SECTOR_DISTANCE + GLOBAL_DEFINE::SECTOR_HALF_DISTANCE)
	, centerY(inY* GLOBAL_DEFINE::SECTOR_DISTANCE + GLOBAL_DEFINE::SECTOR_HALF_DISTANCE)
	, sectorContUnit(new SectorContUnit)
{
}

Sector::~Sector()
{
	delete sectorContUnit;
}

/*
	Join
		- ���Ϳ� ���ο� Ŭ���̾�Ʈ�� ���ɴϴ�.
*/
void Sector::Join(ObjectInfo* pClientObject)
{
	sectorContUnit->wrlock.lock(); //+++++++++++++++++++++++++++++++++++++1
	sectorContUnit->clientCont.emplace(pClientObject->key);
	sectorContUnit->wrlock.unlock(); //-----------------------------------0

	pClientObject->sectorIndexX = indexX;
	pClientObject->sectorIndexY = indexY;
}

/*
	Exit
		- ���Ϳ� �ִ� Ŭ���̾�Ʈ�� �����ϴ�.
*/
void Sector::Exit(ObjectInfo* pInClient)
{
	// ���� �������� ���� �����ְ�.
	sectorContUnit->wrlock.lock(); //+++++++++++++++++++++++++++++++++++++1

	for (auto iter = sectorContUnit->clientCont.begin()
		; iter != sectorContUnit->clientCont.end()
		; ++iter)
	{
		if (pInClient->key == *iter)
		{
			sectorContUnit->clientCont.erase(iter);
			break;
		}
	}

	sectorContUnit->wrlock.unlock(); //-----------------------------------0
}

void Sector::JoinForNpc(ObjectInfo* pClientObject)
{
	//sectorContUnit->monsterlock.lock(); //+++++++++++++++++++++++++++++++++++++1
	sectorContUnit->monsterCont.emplace(pClientObject->key);
	//sectorContUnit->monsterlock.unlock(); //-----------------------------------0

	pClientObject->sectorIndexX = indexX;
	pClientObject->sectorIndexY = indexY;
}

void Sector::ExitForNpc(ObjectInfo* pInClient)
{
	//sectorContUnit->monsterlock.lock(); //+++++++++++++++++++++++++++++++++++++1

	for (auto iter = sectorContUnit->monsterCont.begin()
		; iter != sectorContUnit->monsterCont.end()
		; ++iter)
	{
		if (pInClient->key == *iter)
		{
			sectorContUnit->monsterCont.erase(iter);
			break;
		}
	}

	//sectorContUnit->monsterlock.unlock(); //-----------------------------------0
}

/*
	JudgeClientInViewList
	 - Sector�� �����̳ʿ��� ���� �ִ� Ŭ���̾�Ʈ�� �Ǵ��ϰ�, viewList�� ���ο� ���� ó���մϴ�..

	 !0. ��ģ �̻��� ���� ����, ZoneContUnit�� ���� �ɴϴ�... �߸����� ���� ���ٰ��� �Ű���!
	 !1. ������ viewList�� ���� ��, ����ȭ ������ ������?? ConCurrency�� �ƴ� Lock�� ��� �ɲ� ����.
	 !2. ���ο��� ��Ʈ��ũ �Լ��� ȣ��˴ϴ�.
*/
void Sector::JudgeClientWithViewList(SocketInfo* pClient, ZoneContUnit* pZoneContUnit)
{
	sectorContUnit->wrlock.lock_shared();	//++++++++++++++++++++++++++++1	Sector : Read Lock!
	for (auto& otherKey : sectorContUnit->clientCont)
	{
		if (otherKey == pClient->objectInfo->key) continue;

		SocketInfo* pOtherClient = pZoneContUnit->clientCont[otherKey];

		if (IsSeeEachOther(pClient->objectInfo, pOtherClient->objectInfo))
		{
			// ���� ���Դϴ�.
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
		}
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
	}
	sectorContUnit->wrlock.unlock_shared();	//----------------------------0	Sector : Read Lock!

	sectorContUnit->monsterlock.lock_shared();
	for (auto& otherKey : sectorContUnit->monsterCont)
	{
		auto pMonster = pZoneContUnit->monsterCont[otherKey - BIT_CONVERTER::NOT_PLAYER_INT];

		if (IsSeeEachOther(pClient->objectInfo, pMonster->objectInfo))
		{
			// ���� ���Դϴ�.
			if (pClient->monsterViewList.find(otherKey) == pClient->monsterViewList.end())
			{
				// ���� ���̰�, ���� �𸣴� ���̿��� ��.
				SendPutPlayer(pMonster->objectInfo, pClient);

				// ������ �� ����Ʈ�� �߰��� ��... ������ �� �� �ֽ��ϴ�.
				pClient->monsterViewList.insert(otherKey);
			}
			//else
			//{
			//	//���� ���̰�, ���� �ƴ� ���̿��� ��,
			//
			//	// ���� ���� �˾Ƽ� �Ұ� �� �� �ٲ�� �޾ƶ� �丶!
			//	SendMovePlayer(pClient->objectInfo, pOtherClient);
			//}
		}
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
	}
	sectorContUnit->monsterlock.unlock_shared();
}

bool Sector::JudgeClientWithViewListForNpc(ObjectInfo* pClient, ZoneContUnit* pZoneContUnit)
{
	bool retValue = false;

	// ���� 0���̴��� �ƴϴ��� �˻�
	if (sectorContUnit->clientCont.size() == 0) {
		std::cout << "�ֺ���ֽ��ϴ�!" << std::endl;  return false; //�̰� ���ƾ����ٵ�? }
	}

	sectorContUnit->wrlock.lock_shared();	//++++++++++++++++++++++++++++1	Sector : Read Lock!
	for (auto& otherKey : sectorContUnit->clientCont)
	{
		// ���Ϳ� �ִ� Ŭ���̾�Ʈ ���³�
		SocketInfo* pOtherClient = pZoneContUnit->clientCont[otherKey]; //FindClient(otherKey /*- BIT_CONVERTER::NOT_PLAYER_INT*/);
		//if (!isOn) continue; // ? �� ����

		if (IsSeeEachOther(pClient, pOtherClient->objectInfo))
		{
			// ���� ���Դϴ�.
			if (pOtherClient->monsterViewList.find(pClient->key /* - BIT_CONVERTER::NOT_PLAYER_INT*/) == pOtherClient->monsterViewList.end())
			{
				// ���� ���̰�, ���� �𸣴� ���̿��� ��.
				SendPutPlayer(pClient, pOtherClient);
				//SendPutPlayer(pOtherClient->objectInfo, pClient);

				// ������ �� ����Ʈ�� �߰��� ��... ������ �� �� �ֽ��ϴ�.
				//pClient->viewList.insert(otherKey);
				pOtherClient->monsterViewList.insert(pClient->key);
			}
			else
			{
				//���� ���̰�, ���� �ƴ� ���̿��� ��,

				// ���� �̹� ��������, �� �� �ٲ�� �޾ƶ� �丶!
				SendMovePlayer(pClient, pOtherClient);
			}

			retValue = true;
		}
		else
		{
			// ���� �Ⱥ��Դϴ�.
			if (pOtherClient->monsterViewList.find(pClient->key) != pOtherClient->monsterViewList.end())
			{
				// ���� �� ���̰�, ���� ���� �˴� Ŭ���̾�Ʈ���� ��.

				SendRemovePlayer(pClient->key, pOtherClient);
				//SendRemovePlayer(otherKey, pClient);

				// ������ �� ����Ʈ���� ������ ��... ������ �� �� �ֽ��ϴ�.
				// ������ ������?? ����ϴٻ����
				pOtherClient->monsterViewList.unsafe_erase(pClient->key);
			}
			//else
			//{
			//	// ���� �� ���̰�, ���� ���� �𸣴� Ŭ���̾�Ʈ���� ��.
			//}
		}
	}
	sectorContUnit->wrlock.unlock_shared();	//----------------------------0	Sector : Read Lock!

	return retValue;
}

/*
	IsSeeEachOther
		- ��! ���� ���� �ֳ� ����! ��ġ�� �Ѵ� ���� ���� ��!
*/
bool Sector::IsSeeEachOther(const _PosType aPosX, const _PosType aPosY, const _PosType bPosX, const _PosType bPosY) const noexcept
{
	if (GLOBAL_DEFINE::VIEW_DISTANCE < abs(aPosX - bPosX)) return false;
	if (GLOBAL_DEFINE::VIEW_DISTANCE < abs(aPosY - bPosY)) return false;
	return true;
}

bool Sector::IsSeeEachOther(const std::pair<_PosType, _PosType>& inAPosition, const std::pair<_PosType, _PosType>& inBPosition) const noexcept
{
	if (GLOBAL_DEFINE::VIEW_DISTANCE < abs(inAPosition.first - inBPosition.first)) return false;
	if (GLOBAL_DEFINE::VIEW_DISTANCE < abs(inAPosition.second - inBPosition.second)) return false;
	return true;
}

bool Sector::IsSeeEachOther(const ObjectInfo* objectA, const ObjectInfo* objectB) const noexcept
{
	if (GLOBAL_DEFINE::VIEW_DISTANCE < abs(objectA->posX - objectB->posX)) return false;
	if (GLOBAL_DEFINE::VIEW_DISTANCE < abs(objectA->posY - objectB->posY)) return false;
	return true;
}

void Sector::SendPutPlayer(ObjectInfo* pPutClient, SocketInfo* pRecvClient)
{
	PACKET_DATA::MAIN_TO_CLIENT::PutPlayer packet(
		pPutClient->key,
		//pPutClient->userData->GetPosition().x,
		//pPutClient->userData->GetPosition().y
		pPutClient->posX,
		pPutClient->posY
	);

	NETWORK_UTIL::SendPacket(pRecvClient, reinterpret_cast<char*>(&packet));
}

void Sector::SendRemovePlayer(const _ClientKeyType pRemoveClientID, SocketInfo* pRecvClient)
{
	PACKET_DATA::MAIN_TO_CLIENT::RemovePlayer packet(pRemoveClientID);

	NETWORK_UTIL::SendPacket(pRecvClient, reinterpret_cast<char*>(&packet));
}

void Sector::SendMovePlayer(ObjectInfo* pMovedClient, SocketInfo* pRecvClient)
{
	PACKET_DATA::MAIN_TO_CLIENT::Position packet(
		pMovedClient->key,
		//pMovedClientKey->userData->GetPosition().x,
		//pMovedClientKey->userData->GetPosition().y
		pMovedClient->posX,
		pMovedClient->posY
	);

	NETWORK_UTIL::SendPacket(pRecvClient, reinterpret_cast<char*>(&packet));
}