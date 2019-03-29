#include "pch.h"
#include "Define.h"
#include "ServerDefine.h"

#include "SendMemoryPool.h"

SendMemoryPool* SendMemoryPool::instance = nullptr;

SendMemoryUnit::SendMemoryUnit() :
	overlapped(),
	wsaBuf(),
	sendBuf(nullptr)
{
#ifdef _DEV_MODE_
	std::cout << " SendMemoryPoolUnit�� �⺻�����ڰ� ȣ��Ǿ����ϴ�. \n";
#endif
	sendBuf = new char[GLOBAL_DEFINE::MAX_SIZE_OF_SEND];
	wsaBuf.buf = sendBuf;
}

SendMemoryUnit::~SendMemoryUnit()
{
#ifdef _DEV_MODE_
	std::cout << "�׷����� ���µ�?? SendMemoryPoolUnit�� �Ҹ��ڰ� ȣ��Ǿ����ϴ�. \n";
#endif
	delete[] sendBuf;
}

SendMemoryUnit::SendMemoryUnit(const SendMemoryUnit& other)
	: overlapped(), wsaBuf(other.wsaBuf), sendBuf(other.sendBuf)
{
#ifdef _DEV_MODE_
	std::cout << " SendMemoryPoolUnit�� ��������ڰ� ȣ��Ǿ����ϴ�. \n";
#endif
}

SendMemoryUnit::SendMemoryUnit(SendMemoryUnit&& other) noexcept
	: overlapped(), wsaBuf(), sendBuf(nullptr)
{
	*this = std::move(other);
}

SendMemoryUnit& SendMemoryUnit::operator=(SendMemoryUnit&& other) noexcept
{
#ifdef _DEV_MODE_
	std::cout << " SendMemoryPoolUnit�� �̵� �Ҵ� ������(Ȥ�� �̵� ������)�� ȣ��Ǿ����ϴ�. \n";
#endif
	if (this != &other)
	{
		// �ƴ� ��������, ������ �غ� �̰� ������ �Ǵ°� �¾ƿ�? �������𸣰ھ�G��������
		delete[] sendBuf;
		sendBuf = other.sendBuf;
		wsaBuf = other.wsaBuf;
		other.sendBuf = nullptr;
	}
	return *this;
}

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
		- Send�� �ʿ��� SendMemoryUnit�� ��û�մϴ�.

	?0. ������ �ʱ� �޸� Ǯ ����� �˱� ����, ���� SendǮ�� ���� ���, �α׸� ����մϴ�.
*/
SendMemoryUnit* SendMemoryPool::PopMemory()
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

