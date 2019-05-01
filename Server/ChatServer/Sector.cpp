#include "pch.h"
#include "Define.h"
#include "ServerDefine.h"

#include "ClientContUnit.h"
#include "MemoryUnit.h"
#include "UserData.h"


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
void Sector::Join(SocketInfo* pOutNewClient)
{
	sectorContUnit->wrlock.lock(); //+++++++++++++++++++++++++++++++++++++1
	sectorContUnit->clientCont.emplace(pOutNewClient->clientKey);
	sectorContUnit->wrlock.unlock(); //-----------------------------------0

	pOutNewClient->sectorIndexX = indexX;
	pOutNewClient->sectorIndexY = indexY;
}

/*
	Exit
		- ���Ϳ� �ִ� Ŭ���̾�Ʈ�� �����ϴ�.
*/
void Sector::Exit(SocketInfo* pInClient)
{
	// ���� �������� ���� �����ְ�.
	sectorContUnit->wrlock.lock(); //+++++++++++++++++++++++++++++++++++++1

	for (auto iter = sectorContUnit->clientCont.begin()
		; iter != sectorContUnit->clientCont.end()
		; ++iter)
	{
		if (pInClient->clientKey == *iter)
		{
			sectorContUnit->clientCont.erase(iter);
			break;
		}
	}

	sectorContUnit->wrlock.unlock(); //-----------------------------------0
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
	// �̺κ� ���߿� ���� ������ ������� �� �����ϴ�. �ٸ� ����� �����ؾ��մϴ�.
	pZoneContUnit->wrlock.lock_shared();	//++++++++++++++++++++++++++++1 Zone : Read Lock!
	auto oldZoneCont = pZoneContUnit->clientCont;
	pZoneContUnit->wrlock.unlock_shared();	//----------------------------0 Zone : Read unLock!

	sectorContUnit->wrlock.lock_shared();	//++++++++++++++++++++++++++++1	Sector : Read Lock!
	for (auto& otherKey : sectorContUnit->clientCont)
	{
		if (otherKey == pClient->clientKey) continue;

		if (IsSeeEachOther(pClient->userData->GetPosition(), oldZoneCont[otherKey].second->userData->GetPosition()))
		{
			// ���� ���Դϴ�.
			if (pClient->viewList.find(otherKey) == pClient->viewList.end())
			{
				// ���� ���̰�, ���� �𸣴� ���̿��� ��.
				SendPutPlayer(pClient, oldZoneCont[otherKey].second);
				SendPutPlayer(oldZoneCont[otherKey].second, pClient);

				// ������ �� ����Ʈ�� �߰��� ��... ������ �� �� �ֽ��ϴ�.
				pClient->viewList.insert(otherKey);
				oldZoneCont[otherKey].second->viewList.insert(pClient->clientKey);
			}
			else
			{
				//���� ���̰�, ���� �ƴ� ���̿��� ��,

				// ���� ���� �˾Ƽ� �Ұ� �� �� �ٲ�� �޾ƶ� �丶!
				SendMovePlayer(pClient, oldZoneCont[otherKey].second);
			}
		}
		else
		{
			// ���� �Ⱥ��Դϴ�.
			if (pClient->viewList.find(otherKey) != pClient->viewList.end())
			{
				// ���� �� ���̰�, ���� ���� �˴� Ŭ���̾�Ʈ���� ��.
				SendRemovePlayer(pClient->clientKey, oldZoneCont[otherKey].second);
				SendRemovePlayer(otherKey, pClient);

				// ������ �� ����Ʈ���� ������ ��... ������ �� �� �ֽ��ϴ�.
				// ������ ������?? ����ϴٻ����
				pClient->viewList.unsafe_erase(otherKey);
				oldZoneCont[otherKey].second->viewList.unsafe_erase(pClient->clientKey);
			}
			//else
			//{
			//	// ���� �� ���̰�, ���� ���� �𸣴� Ŭ���̾�Ʈ���� ��.
			//}
		}
	}
	sectorContUnit->wrlock.unlock_shared();	//----------------------------0	Sector : Read Lock!
}

/*
	IsSeeEachOther
		- ��! ���� ���� �ֳ� ����! ��ġ�� �Ѵ� ���� ���� ��!
*/
bool Sector::IsSeeEachOther(const Position2D& inAPosition, const Position2D& inBPosition) noexcept
{
	if (GLOBAL_DEFINE::VIEW_DISTANCE < abs(inAPosition.x - inBPosition.x)) return false;
	if (GLOBAL_DEFINE::VIEW_DISTANCE < abs(inAPosition.y - inBPosition.y)) return false;
	return true;
}

void Sector::SendPutPlayer(SocketInfo* pPutClient, SocketInfo* pRecvClient)
{
	PACKET_DATA::SC::PutPlayer packet(
		pPutClient->clientKey,
		pPutClient->userData->GetPosition().x,
		pPutClient->userData->GetPosition().y
	);

	NETWORK_UTIL::SendPacket(pRecvClient, reinterpret_cast<char*>(&packet));
}

void Sector::SendRemovePlayer(const _ClientKeyType pRemoveClientID, SocketInfo* pRecvClient)
{
	PACKET_DATA::SC::RemovePlayer packet(pRemoveClientID);

	NETWORK_UTIL::SendPacket(pRecvClient, reinterpret_cast<char*>(&packet));
}

void Sector::SendMovePlayer(SocketInfo* pMovedClientKey, SocketInfo* pRecvClient) 
{
	PACKET_DATA::SC::Position packet(
		pMovedClientKey->clientKey,
		pMovedClientKey->userData->GetPosition().x,
		pMovedClientKey->userData->GetPosition().y
	);

	NETWORK_UTIL::SendPacket(pRecvClient, reinterpret_cast<char*>(&packet));
}