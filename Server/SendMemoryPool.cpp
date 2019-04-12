#include "pch.h"
#include "Define.h"
#include "ServerDefine.h"

#include "MemoryUnit.h"
#include "SendMemoryPool.h"

SendMemoryPool* SendMemoryPool::instance = nullptr;

//---------------------------------------------------------------------------
// SendMemoryPool
//---------------------------------------------------------------------------

SendMemoryPool::SendMemoryPool()
{
	for(int i = 0 ; i < GLOBAL_DEFINE::MAX_NUMBER_OF_SEND_POOL; ++i)
		sendMemoryPool.push(std::move(SendMemoryUnit()));
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
SendMemoryUnit* SendMemoryPool::PopMemory(SocketInfo* pClient)
{
	SendMemoryUnit* retMemoryUnit{nullptr};
	
	// C6013 : NULL ������ 'retMemoryUnit'�� �������ϰ� �ֽ��ϴ�.
	// �������Ǿ����� ������ NULL ������ �� �� �ֽ��ϴ�.
	while (!sendMemoryPool.try_pop(*retMemoryUnit))
	{
		ERROR_HANDLING::ERROR_DISPLAY("[ERROR]SendPool�� �޸𸮰� �����մϴ�.");
		/*
			������ ���⼭ �޸� �߰��� �Ҵ��ؼ�, �Ѱ������ ��� ��ٷ�!
		*/
	}

	//	retMemoryUnit->pOwner = pClient;
	return retMemoryUnit;
}

/*
	PushMemory()
		- Send�� ���� �޸𸮸� �޾Ƽ� Ǯ�� �ٽ� ����ֽ��ϴ�. ��!

	?0. ��� �� ������ �ѹ� Ȯ���� �ʿ��մϴ�.
*/
void SendMemoryPool::PushMemory(SendMemoryUnit* inMemoryUnit)
{
	sendMemoryPool.push(std::move(*inMemoryUnit));
}

