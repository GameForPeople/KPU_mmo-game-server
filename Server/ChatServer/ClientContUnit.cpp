#include "pch.h"

#include "MemoryUnit.h"
#include "ClientContUnit.h"

ZoneContUnit::ZoneContUnit()
{
	for (auto& iter : clientContArr)
	{
		iter.reserve(1000);
		iter.push_back(nullptr);
	}

	for (auto& iter : indexArr)
	{
		iter = 0;
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

	// ��¥�� ���̾� ~~~ �ФФФФФФФФФж��Ⱦ����������� �ФФФФ�
	clientContArr[contIndex][indexArr[contIndex]/*.load()*/] = pClient;	// Ŭ���̾�Ʈ �����̳ʿ� ������ Ŭ���̾�Ʈ ����ü�� �־���.
	pClient->contIndex = indexArr[contIndex];

	indexArr[contIndex].fetch_add(1);	//�ε��� ���� �ϳ� ������Ŵ.

	wrLockArr[contIndex].unlock();	// -------------------------------- 0
}

void ZoneContUnit::Exit(SocketInfo* pClient)
{
	const BYTE contIndex = GetContHashKey(pClient->nickname[0]);

	wrLockArr[contIndex].lock();	//++++++++++++++++++++++++++++++++ 1

	const USHORT contEndIndex = indexArr[contIndex].load();

	// �����̳� �� ���� �����, �������� ����� �ε����� ����
	clientContArr[contIndex][pClient->contIndex] = clientContArr[contIndex][contEndIndex];

	// �ǵ��� �ε������� �ֿ���, ���ο� �ε����� �˷���.
	clientContArr[contIndex][pClient->contIndex]->contIndex = pClient->contIndex;

	// �ǵ��� �����̳� ���� -> �ش� �����̳��� ��� �ε��� ����.
	//clientContArr[contIndex].pop_back();
	indexArr[contIndex].fetch_sub(1);

	wrLockArr[contIndex].unlock();	// -------------------------------- 0
}

