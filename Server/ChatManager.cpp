#include "pch.h"
#include "ServerDefine.h"
#include "MemoryUnit.h"
#include "ClientContUnit.h"
#include "ChatManager.h"

ChatManager::ChatManager()
	: sendedMessageCont()
{
	sendedMessageCont.clear();

	for(int i = 0 ; i < ChatUnit::chatUnitCount; ++i)
		sendedMessageCont.push(new ChatUnit());
}

ChatManager::~ChatManager()
{
	ChatUnit* pChatUnit = nullptr;
	while (sendedMessageCont.try_pop(pChatUnit))
	{
		delete pChatUnit;
	}
}

/*
	ChatProcess
		- ä�� �޼����� �������, �ش� ���� �����̳ʿ� �����ϰ�, �� �����鿡�� ��ε�ĳ����
	
	!0. 1�� �����Դϴ�. -> PZoneCont���ٰ� ���� �ɾ���Ƚ��ϴ�.
	!1. 2�� �����Դϴ�. -> ���� �� ���·� ��� Ŭ���̾�Ʈ�� ��ȸ�մϴ�.
	!2. 3�� �����Դϴ�. -> �� ���ο��� Send�Լ����� ȣ���߽��ϴ�.

	�������ٷ� �����丵�� ����Ǿ����ϴ�.
*/
void ChatManager::ChatProcess(SocketInfo* pClient, ZoneContUnit* pZoneContUnit)
{
	ChatUnit* retChatUnit{ nullptr };

	while (!sendedMessageCont.try_pop(retChatUnit))
	{
		ERROR_HANDLING::ERROR_DISPLAY("[ERROR]SendPool�� �޸𸮰� �����մϴ�.");
		/*
			������ ���⼭ �޸� �߰��� �Ҵ��ؼ�, �Ѱ������ ��� ��ٷ�!
		*/
	}
	
	// ���� �����Ͱ� �״�� ����մϴ�.
	memcpy(retChatUnit->message, pClient->loadedBuf, pClient->loadedBuf[0]);

	//pZoneContUnit->wrlock.lock_shared(); //++++++++++++++++++++++++++++++++++1
	//for (auto& iter : pZoneContUnit->clientCont)
	//{
	//	NETWORK_UTIL::SendUnallocatedPacket(iter.second, reinterpret_cast<char*>(retChatUnit));
	//}
	//pZoneContUnit->wrlock.unlock_shared(); //--------------------------------0
	//
	//sendedMessageCont.push(retChatUnit);
}