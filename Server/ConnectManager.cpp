#include "pch.h"
#include "Define.h"
#include "ServerDefine.h"

#include "MemoryUnit.h"
#include "UserData.h"
#include "Scene.h"

#include "ConnectManager.h"

/*
	ConnectManager::InNewClient()
		- ���ο� Ŭ���̾�Ʈ�� �������� ��, �̸� �����̳ʿ� �־��ݴϴ�.

	#!?0. �ϳ��� ���� �������� �ϳ��� ���� ���� ���, ����ó���ϴ°� ����.
	#!?1. �ٸ� �ϳ��� �������� ���� ���� ���� ���, ���ʿ� SocketInfo�� ���� �ְ�, InNewCliet�� ���ڷ� �־��ִ� �Ը���.
*/
ConnectManager::_ClientNode ConnectManager::InNewClient(std::vector< _ClientNode>&  inClientCont, Scene* scene)
{
	//std::lock_guard<std::mutex> localLock(addLock);
	
	connectLock.lock();
	//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++1

	for (int index = 0; index < inClientCont.size(); ++index)
	{
		if (inClientCont[index].first == false)
		{
			inClientCont[index].first = true;

			connectLock.unlock();
			//------------------------------------------------------------0

			// ���� ���� ����ü �Ҵ�
			SocketInfo* pInClient = new SocketInfo;
			if (pInClient == nullptr)
			{
				ERROR_HANDLING::ERROR_QUIT(TEXT("Make_SocketInfo()"));
				break;
			}

			inClientCont[index].second = pInClient;
			pInClient->clientContIndex = index;
			pInClient->pScene = scene;

			SendPutPlayer(pInClient, inClientCont);

			return std::make_pair(true, pInClient);
			//return pClient;
		}
	}

	connectLock.unlock();
	//-------------------------------------------------------------------0
	return std::make_pair(false, nullptr);
	//return {};
}

/*
	ConnectManager::OutClient()
		- Ŭ���̾�Ʈ�� �ٸ� ������ �̵��ϰų�, �α׾ƿ� �ɶ�, �ش� Ŭ���̾�Ʈ�� �������� ���ݴϴ�.

	#!?0. ����ü ���⼭ ������ ������ ������ϴ°���. �� ������ ������ ���װ� �� �� ���� ������.
*/
void ConnectManager::OutClient(SocketInfo* pOutClient, std::vector< _ClientNode>& inClientCont)
{
	// ���� Lock �� �ʿ� ���� �ʳ�.
	SendRemovePlayer(inClientCont[pOutClient->clientContIndex].first, inClientCont);

	inClientCont[pOutClient->clientContIndex].first = false;
	// second�� �ʱ�ȭ �� �ʿ� ����.

	// �ٸ� �̺κп���, ����� ���� �� ��������, �������� �����ϱ� ���� ó�����ֵ��� �սô�.

	// ��� pScene�� nullptr�ϰ�, LogOutProcess����, �ش� ���θ� �˻��ϴ� �͵� �� �� �ѵ�;
	pOutClient->pScene = nullptr;
	pOutClient->clientContIndex = -1;
}

void ConnectManager::SendPutPlayer(SocketInfo* pPutClient, std::vector<_ClientNode>& inClientCont)
{
	PACKET_DATA::SC::PutPlayer packet(
		pPutClient->clientContIndex,
		pPutClient->userData->GetPosition().x,
		pPutClient->userData->GetPosition().y
	);

	for (std::pair<bool, SocketInfo*>& pRecvedClient : inClientCont)
	{
		if (pRecvedClient.first)
		{
			NETWORK_UTIL::SendPacket(pRecvedClient.second, reinterpret_cast<char*>(&packet));
		}
	}
}

void ConnectManager::SendRemovePlayer(const char outClientKey, std::vector<_ClientNode>& inClientCont)
{
	PACKET_DATA::SC::RemovePlayer packet(
		outClientKey
	);

	for (int i = 0; i < inClientCont.size(); ++i)
	{
		if (inClientCont[i].first && i != outClientKey) 
		{
			NETWORK_UTIL::SendPacket(inClientCont[i].second, reinterpret_cast<char*>(&packet));
		}
	}

	//for (std::pair<bool, SocketInfo*>& pRecvedClient : inClientCont)
	//{
	//	if (pRecvedClient.first)
	//	{
	//		NETWORK_UTIL::SendPacket(pRecvedClient.second, reinterpret_cast<char*>(&packet));
	//	}
	//}
}