#include "pch.h"
#include "../Define.h"

#include "QueryDefine.h"
#include "MemoryUnit.h"
#include "SendMemoryPool.h"

SendMemoryPool* SendMemoryPool::instance = nullptr;

//---------------------------------------------------------------------------
// SendMemoryPool
//---------------------------------------------------------------------------

SendMemoryPool::SendMemoryPool()
{
	for (int i = 0; i < GLOBAL_DEFINE::MAX_NUMBER_OF_SEND_POOL; ++i)
		sendMemoryPool.push(new MemoryUnit(MEMORY_UNIT_TYPE::SEND) /* == std::move(SendMemoryUnit())*/);

#ifdef _DEV_MODE_
	std::cout << "sendMemoryPool�� ������� " << sendMemoryPool.unsafe_size() << std::endl;
#endif
}

SendMemoryPool::~SendMemoryPool()
{
	MemoryUnit* tempUnit{ nullptr };
	while (sendMemoryPool.try_pop(tempUnit))
	{
		delete tempUnit;
	}
	delete SendMemoryPool::instance;
}

/*
	PopMemory()
		- Send�� �ʿ��� MemoryUnit�� ��û�մϴ�.

	?0. ������ �ʱ� �޸� Ǯ ����� �˱� ����, ���� SendǮ�� ���� ���, �α׸� ����մϴ�.
*/
MemoryUnit* SendMemoryPool::PopMemory()
{
	MemoryUnit* retMemoryUnit{ nullptr };

	// C6013 : NULL ������ 'retMemoryUnit'�� �������ϰ� �ֽ��ϴ�.
	// �������Ǿ����� ������ NULL ������ �� �� �ֽ��ϴ�.
	while (!sendMemoryPool.try_pop(retMemoryUnit))
	{
		ERROR_HANDLING::ERROR_DISPLAY(L"[ERROR]SendPool�� �޸𸮰� �����մϴ�.");
		/*
			������ ���⼭ �޸� �߰��� �Ҵ��ؼ�, �Ѱ������. ��ٸ��� �ȵ�.
		*/
	}
	return retMemoryUnit;
}

/*
	PushMemory()
		- Send�� ���� �޸𸮸� �޾Ƽ� Ǯ�� �ٽ� ����ֽ��ϴ�. ��!

	?0. ��� �� ������ �ѹ� Ȯ���� �ʿ��մϴ�.
*/
void SendMemoryPool::PushMemory(MemoryUnit* inMemoryUnit)
{
	sendMemoryPool.push(/*std::move(*/inMemoryUnit/*)*/);
}
