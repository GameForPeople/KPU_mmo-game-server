#include "pch.h"

#include "MemoryUnit.h"

#include "ClientContUnit.h"

ZoneContUnit::ZoneContUnit()
{
	for (auto& iter : clientContArr)
	{
		iter.reserve(1000);
	}
}

ZoneContUnit::~ZoneContUnit()
{
	for (auto& iter : clientContArr)
	{
		for (auto& pSocketInfo : iter)
		{
			delete pSocketInfo;
		}
	}
}


void ZoneContUnit::Enter(SocketInfo* pClient)
{
	const BYTE contIndex = GetContHashKey(pClient->nickname[0]);

	wrLockArr[contIndex].lock();	//++++++++++++++++++++++++++++++++ 1

	clientContArr[contIndex].emplace_back(pClient);
	pClient->contIndex = clientContArr[contIndex].size() - 1;

	wrLockArr[contIndex].unlock();	// -------------------------------- 0
}

void ZoneContUnit::Exit(SocketInfo* pClient)
{
	const BYTE contIndex = GetContHashKey(pClient->nickname[0]);

	wrLockArr[contIndex].lock();	//++++++++++++++++++++++++++++++++ 1

	const USHORT contEndIndex = clientContArr[contIndex].size() - 1;

	// �����̳� �� ���� �����, �������� ����� �ε����� ����
	clientContArr[contIndex][pClient->contIndex] = clientContArr[contIndex][contEndIndex];

	// �ǵ��� �ε������� �ֿ���, ���ο� �ε����� �˷���.
	clientContArr[contIndex][pClient->contIndex]->contIndex = pClient->contIndex;

	// �ǵ��� �����̳� ����
	clientContArr[contIndex].pop_back();

	wrLockArr[contIndex].unlock();	// -------------------------------- 0
}

