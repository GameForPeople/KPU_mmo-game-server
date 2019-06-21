#include "pch.h"
#include "../Define.h"
#include "ServerDefine.h"

#include "MemoryUnit.h"
#include "SendMemoryPool.h"

SendMemoryPool* SendMemoryPool::instance = nullptr;

//---------------------------------------------------------------------------
// SendMemoryPool
//---------------------------------------------------------------------------

SendMemoryPool::SendMemoryPool()
{
	for(int i = 0 ; i <  GLOBAL_DEFINE::MAX_NUMBER_OF_SEND_POOL; ++i)
		sendMemoryPool.push(/*new SendMemoryUnit()*/ std::move(new SendMemoryUnit()));

	std::cout << "!. sendMemoryPool�� �ʱ� �Ҵ� ������� " << sendMemoryPool.unsafe_size() << " �Դϴ�." << std::endl;
}

SendMemoryPool::~SendMemoryPool()
{
	sendMemoryPool.clear();
	delete SendMemoryPool::instance;
}

/*
	PopMemory()
		- Send�� �ʿ��� MemoryUnit�� ��û�մϴ�.

	?0. ������ �ʱ� �޸� Ǯ ����� �˱� ����, ���� SendǮ�� ���� ���, �α׸� ����մϴ�.
*/
SendMemoryUnit* SendMemoryPool::PopMemory()
{
	SendMemoryUnit* retMemoryUnit{nullptr};

	while (!sendMemoryPool.try_pop(retMemoryUnit))
	{
		for (int i = 0; i < GLOBAL_DEFINE::ALLOCATE_COUNT_OF_SEND_POOL; ++i)
			sendMemoryPool.push(/*new SendMemoryUnit()*/ std::move(new SendMemoryUnit()));
		
		ERROR_HANDLING::ERROR_DISPLAY(L"[ERROR] SendPool�� ���� �޸𸮰� ������ �߰��Ҵ��Ͽ����ϴ�.");
		/*
			���� �޸� Ǯ ������ ����.
		*/
	}

	//assert(retMemoryUnit != nullptr, "retMemoryUnit�� try_pop�� nullptr�� ��ȯ�߽��ϴ�.");
	return retMemoryUnit;
}

/*
	PushMemory()
		- Send�� ���� �޸𸮸� �޾Ƽ� Ǯ�� �ٽ� ����ֽ��ϴ�. ��!

	?0. ��� �� ������ �ѹ� Ȯ���� �ʿ��մϴ�.
	�̰� �� �̻��Ѱ� ������;;;;;;;;;; �����Բ� ���庸�� ����!
*/
void SendMemoryPool::PushMemory(SendMemoryUnit* inMemoryUnit)
{
	//std::cout << "packetData : " << (int)inMemoryUnit->memoryUnit.dataBuf[1] << " \n";
	sendMemoryPool.push(std::move(inMemoryUnit));
}

#ifndef DISABLED_UNALLOCATED_MEMORY_SEND
UnallocatedMemoryUnit* SendMemoryPool::PopUnallocatedMemory()
{
	UnallocatedMemoryUnit* retMemoryUnit{ nullptr };

	// C6013 : NULL ������ 'retMemoryUnit'�� �������ϰ� �ֽ��ϴ�.
	// �������Ǿ����� ������ NULL ������ �� �� �ֽ��ϴ�.
	while (!sendUnallocatedMemoryPool.try_pop(retMemoryUnit))
	{
		ERROR_HANDLING::ERROR_DISPLAY("[ERROR]PopUnallocatedMemory�� �޸𸮰� �����մϴ�.");
		/*
			������ ���⼭ �޸� �߰��� �Ҵ��ؼ�, �Ѱ������. ��ٸ��� �ȵ�.
		*/
	}
	return retMemoryUnit;
}

void SendMemoryPool::PushUnallocatedMemory(UnallocatedMemoryUnit* inMemoryUnit)
{
	sendUnallocatedMemoryPool.push(/*std::move(*/inMemoryUnit/*)*/);
}
#endif
