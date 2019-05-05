#include "pch.h"

#include "MemoryUnit.h"
#include "ServerDefine.h"
#include "ObjectInfo.h"
#include "BaseMonster.h"
#include "BaseNPC.h"

#include "ClientContUnit.h"

ZoneContUnit::ZoneContUnit() :
	clientCont(),
	//lockArr(),
	//indexArr(),
	monsterCont(),
	npcCont()
{
	int contKey = 0;
	for (auto& iter : clientCont)
	{
		iter = new SocketInfo(contKey++);
		//iter.reserve(CONT_INIT_SIZE);
		//
		//for (int i = 0; i < CONT_INIT_SIZE; ++i)
		//{
		//	iter.emplace_back(nullptr);
		//}
	}

}

ZoneContUnit::~ZoneContUnit()
{
	//for (auto& iter : lockArr)
	//{
	//	iter.clear(std::memory_order_release);
	//}

	for (auto& iter : clientCont)
		delete iter;

	for (auto& iter : monsterCont)
		delete iter;

	for (auto& iter : npcCont)
		delete iter;
}

//void ZoneContUnit::Enter(SocketInfo* pClient)
//{
	//const BYTE contHashIndex = GetContHashKey(pClient->objectInfo->key);

	//while (lockArr[contHashIndex].test_and_set(std::memory_order_acquire))  // acquire lock //++++++++++++++++++++++++++++++++ 1
	//	; // spin

	//clientContArr[contHashIndex][indexArr[contHashIndex]/*.load()*/] = pClient;	// Ŭ���̾�Ʈ �����̳ʿ� ������ Ŭ���̾�Ʈ ����ü�� �־���.
	//pClient->contIndex = indexArr[contHashIndex];
	//
	//indexArr[contHashIndex].fetch_add(1);	//�ε��� ���� �ϳ� ������Ŵ.
	//
	//lockArr[contHashIndex].clear(std::memory_order_release);               // release lock //+-------------------------------- 0
//}

//void ZoneContUnit::Exit(SocketInfo* pClient)
//{
	//const BYTE contHashIndex = GetContHashKey(pClient->objectInfo->key);
	//
	//while (lockArr[contHashIndex].test_and_set(std::memory_order_acquire))  // acquire lock //++++++++++++++++++++++++++++++++ 1
	//	; // spin
	//
	//const USHORT contEndIndex = indexArr[contHashIndex].load() - 1;
	//
	//// �����̳� �� ���� �����, �������� ����� �ε����� ����
	//clientContArr[contHashIndex][pClient->contIndex] = clientContArr[contHashIndex][contEndIndex];
	//
	//// �ǵ��� �ε������� �ֿ���, ���ο� �ε����� �˷���.
	//clientContArr[contHashIndex][pClient->contIndex]->contIndex = pClient->contIndex;
	//
	//// �ǵ��� �����̳� ���� -> �ش� �����̳��� ��� �ε��� ����.
	////clientContArr[contIndex].pop_back();
	//indexArr[contHashIndex].fetch_sub(1);
	//
	//lockArr[contHashIndex].clear(std::memory_order_release);               // release lock //+-------------------------------- 0
//}

//std::pair<bool, SocketInfo*> ZoneContUnit::FindClient(_ClientKeyType inClientKey)
//{
//	const BYTE contHashIndex = GetContHashKey(inClientKey);
//	
//	for (auto iter : clientContArr[contHashIndex])
//	{
//		if (iter->objectInfo->key == inClientKey)
//		{
//			return std::make_pair(true, iter);
//		}
//	}
//
//	return std::make_pair(false, nullptr);
//}
