#include "pch.h"
#include "../Define.h"
#include "ServerDefine.h"

#include "MemoryUnit.h"
#include "UserData.h"
#include "Zone.h"

#include "ClientContUnit.h"

#include "ObjectInfo.h"

#include "ConnectManager.h"

ConnectManager::ConnectManager()
{
	USHORT tempPushKey{ 0 };
	while (tempPushKey < GLOBAL_DEFINE::MAX_CLIENT)
	{
		uniqueKeyPool.push(tempPushKey++);
	}
}

std::pair<bool, _ClientKeyType> ConnectManager::GetUniqueKey()
{
	if (USHORT retClientKey; uniqueKeyPool.try_pop(retClientKey))
	{
		return std::make_pair(true, static_cast<_ClientKeyType>(retClientKey));
	}
	else
	{
		assert(false, L"����? ������ ��ã�ٴµ���?");
		//�̰� ��� �ؾ�����;
		return std::make_pair(false, -1);
	}
}

inline void ConnectManager::PushOldKey(USHORT oldKey)
{
	uniqueKeyPool.push(oldKey);
}

///*
//	ConnectManager::LogInToZone()
//		- ���ο� Ŭ���̾�Ʈ�� �������� ��, �̸� �����̳ʿ� �־��ݴϴ�.
//
//	#!?0. �ϳ��� ���� �������� �ϳ��� ���� ���� ���, ����ó���ϴ°� ����.
//	#!?1. �ٸ� �ϳ��� �������� ���� ���� ���� ���, ���ʿ� SocketInfo�� ���� �ְ�, InNewCliet�� ���ڷ� �־��ִ� �Ը���.
//*/
//std::pair<bool, SocketInfo*> ConnectManager::LogInToZone(ZoneContUnit* inClientContUnit, Zone* zone)
//{
//	if (USHORT retClientKey; uniqueKeyPool.try_pop(retClientKey))
//	{
//		// ���� ���� ����ü �Ҵ�
//		SocketInfo* pInClient = new SocketInfo(retClientKey);
//		if (pInClient == nullptr)
//		{
//			ERROR_HANDLING::ERROR_QUIT(TEXT("Make_SocketInfo()"));
//		}
//		inClientContUnit->Enter(pInClient);
//
//		pInClient->pZone = zone;
//
//		return std::make_pair(true, pInClient);
//	}
//	//-------------------------------------------------------------------0
//	return std::make_pair(false, nullptr);
//}
//
//
//
//
///*
//	ConnectManager::OutClient()
//		- Ŭ���̾�Ʈ�� �ٸ� ������ �̵��ϰų�, �α׾ƿ� �ɶ�, �ش� Ŭ���̾�Ʈ�� �������� ���ݴϴ�.
//
//	#!?0. ����ü ���⼭ ������ ������ ������ϴ°���. �� ������ ������ ���װ� �� �� ���� ������.
//*/
//void ConnectManager::LogOutToZone(SocketInfo* pOutClient, ZoneContUnit* inClientContUnit)
//{
//	// ��� ���͸� ���� Lock �� �ʿ� ���� �ʳ�. -> �׷��� ����........���� ����ϱ�...
//	SendRemovePlayerInOuttedClientViewList(pOutClient, inClientContUnit);
//
//	inClientContUnit->Exit(pOutClient);
//	// second�� �ʱ�ȭ �� �ʿ� ����.
//
//	// �ٸ� �̺κп���, ����� ���� �� ��������, �������� �����ϱ� ���� ó�����ֵ��� �սô�.
//	// ��� pZone�� nullptr�ϰ�, LogOutProcess����, �ش� ���θ� �˻��ϴ� �͵� �� �� �ѵ�;
//	pOutClient->pZone = nullptr;
//}

// �� �̻� �ش� �Լ��� ConnectManager���� ������� �ʽ��ϴ�. Sector�� ����.
//void ConnectManager::SendPutPlayer(SocketInfo* pPutClient, ZoneContUnit* inClientCont)
//{
//	PACKET_DATA::SC::PutPlayer packet(
//		pPutClient->clientKey,
//		pPutClient->userData->GetPosition().x,
//		pPutClient->userData->GetPosition().y
//	);
//
//	for (std::pair<bool, SocketInfo*>& pRecvedClient : inClientCont->clientCont)
//	{
//		if (pRecvedClient.first)
//		{
//			NETWORK_UTIL::SendPacket(pRecvedClient.second, reinterpret_cast<char*>(&packet));
//		}
//	}
//}
//
void ConnectManager::SendRemovePlayerInOuttedClientViewList(SocketInfo* pOutClient, ZoneContUnit* inClientCont)
{
	PACKET_DATA::MAIN_TO_CLIENT::RemovePlayer packet(
		pOutClient->objectInfo->key
	);

	//inClientCont->wrlock.lock_shared(); // +++++++++++++++++++++++++++++++++++++++++++++++++++1 read
	for (auto otherKey : pOutClient->viewList)
	{
		if (auto [isConnect, pOtherClient] = inClientCont->FindClient(otherKey); isConnect)
		{
			NETWORK_UTIL::SendPacket(pOtherClient, reinterpret_cast<char*>(&packet));

			// ���� viewList ����.
			pOtherClient->viewList.unsafe_erase(pOutClient->objectInfo->key);
		}
	}
	//inClientCont->wrlock.unlock_shared(); //--------------------------------------------------0 read

	//for (std::pair<bool, SocketInfo*>& pRecvedClient : inClientCont)
	//{
	//	if (pRecvedClient.first)
	//	{
	//		NETWORK_UTIL::SendPacket(pRecvedClient.second, reinterpret_cast<char*>(&packet));
	//	}
	//}
}

std::pair<bool, SocketInfo*> ConnectManager::OnlyGetUniqueKeyAndMallocSocketInfo()
{
	if (USHORT retClientKey; uniqueKeyPool.try_pop(retClientKey))
	{
		// ���� ���� ����ü �Ҵ�
		SocketInfo* pInClient = new SocketInfo(retClientKey);
		if (pInClient == nullptr)
		{
			ERROR_HANDLING::ERROR_QUIT(TEXT("Make_SocketInfo()"));
		}
		return std::make_pair(true, pInClient);
	}
	//-------------------------------------------------------------------0
	return std::make_pair(false, nullptr);
}