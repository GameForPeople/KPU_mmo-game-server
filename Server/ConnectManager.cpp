#include "pch.h"
#include "Define.h"
#include "ServerDefine.h"

#include "MemoryUnit.h"
#include "UserData.h"
#include "Zone.h"

#include "ClientContUnit.h"

#include "ConnectManager.h"

/*
	ConnectManager::InNewClient()
		- ���ο� Ŭ���̾�Ʈ�� �������� ��, �̸� �����̳ʿ� �־��ݴϴ�.

	#!?0. �ϳ��� ���� �������� �ϳ��� ���� ���� ���, ����ó���ϴ°� ����.
	#!?1. �ٸ� �ϳ��� �������� ���� ���� ���� ���, ���ʿ� SocketInfo�� ���� �ְ�, InNewCliet�� ���ڷ� �־��ִ� �Ը���.
*/
_ClientNode ConnectManager::InNewClient(ZoneContUnit* inClientContUnit, Zone* zone)
{
	//std::lock_guard<std::mutex> localLock(addLock);
	//connectLock.lock();
	inClientContUnit->wrlock.lock();
	//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++1

	for (int index = 0; index < inClientContUnit->clientCont.size(); ++index)
	{
		if (inClientContUnit->clientCont[index].first == false)
		{
			inClientContUnit->clientCont[index].first = true;

			inClientContUnit->wrlock.unlock();
			//------------------------------------------------------------0

			// ���� ���� ����ü �Ҵ�
			SocketInfo* pInClient = new SocketInfo;
			if (pInClient == nullptr)
			{
				ERROR_HANDLING::ERROR_QUIT(TEXT("Make_SocketInfo()"));
				break;
			}

			inClientContUnit->clientCont[index].second = pInClient;
			pInClient->clientKey = index;
			pInClient->pZone = zone;

			//SendPutPlayer(pInClient, inClientContUnit);

			return std::make_pair(true, pInClient);
			//return pClient;
		}
	}

	inClientContUnit->wrlock.unlock();
	//-------------------------------------------------------------------0
	return std::make_pair(false, nullptr);
	//return {};
}

/*
	ConnectManager::OutClient()
		- Ŭ���̾�Ʈ�� �ٸ� ������ �̵��ϰų�, �α׾ƿ� �ɶ�, �ش� Ŭ���̾�Ʈ�� �������� ���ݴϴ�.

	#!?0. ����ü ���⼭ ������ ������ ������ϴ°���. �� ������ ������ ���װ� �� �� ���� ������.
*/
void ConnectManager::OutClient(SocketInfo* pOutClient, ZoneContUnit* inClientContUnit)
{
	// ��� ���͸� ���� Lock �� �ʿ� ���� �ʳ�. -> �׷��� ����........���� ����ϱ�...
	SendRemovePlayer(pOutClient, inClientContUnit);

	inClientContUnit->clientCont[pOutClient->clientKey].first = false;
	// second�� �ʱ�ȭ �� �ʿ� ����.

	// �ٸ� �̺κп���, ����� ���� �� ��������, �������� �����ϱ� ���� ó�����ֵ��� �սô�.
	// ��� pZone�� nullptr�ϰ�, LogOutProcess����, �ش� ���θ� �˻��ϴ� �͵� �� �� �ѵ�;
	pOutClient->pZone = nullptr;
	pOutClient->clientKey = -1;
}

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
void ConnectManager::SendRemovePlayer(SocketInfo* pOutClient, ZoneContUnit* inClientCont)
{
	PACKET_DATA::SC::RemovePlayer packet(
		pOutClient->clientKey
	);

	inClientCont->wrlock.lock_shared(); // +++++++++++++++++++++++++++++++++++++++++++++++++++1 read
	for (auto otherKey : pOutClient->viewList)
	{
		NETWORK_UTIL::SendPacket(inClientCont->clientCont[otherKey].second, reinterpret_cast<char*>(&packet));
		
		// ���� viewList ����.
		inClientCont->clientCont[otherKey].second->viewList.unsafe_erase(pOutClient->clientKey);
		// ���� viewList�� �ǵ��ʿ� ����.
	}
	inClientCont->wrlock.unlock_shared(); //--------------------------------------------------0 read

	//for (std::pair<bool, SocketInfo*>& pRecvedClient : inClientCont)
	//{
	//	if (pRecvedClient.first)
	//	{
	//		NETWORK_UTIL::SendPacket(pRecvedClient.second, reinterpret_cast<char*>(&packet));
	//	}
	//}
}