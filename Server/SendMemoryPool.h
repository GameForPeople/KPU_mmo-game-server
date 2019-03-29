#pragma once

struct SendMemoryUnit
{
	WSABUF wsaBuf;
	char *buf;

public:
	SendMemoryUnit()
	{
#ifdef _DEV_MODE_
		std::cout << " SendMemoryPoolUnit�� �⺻�����ڰ� ȣ��Ǿ����ϴ�. \n";
#endif
		buf = new char[GLOBAL_DEFINE::MAX_SIZE_OF_BUFFER];
		wsaBuf.buf = buf;
	}
	~SendMemoryUnit() {
#ifdef _DEV_MODE_
		std::cout << " SendMemoryPoolUnit�� �Ҹ��ڰ� ȣ��Ǿ����ϴ�. \n";
#endif
		delete[] buf; 
	}

	SendMemoryUnit(const SendMemoryUnit& other)
		: wsaBuf(other.wsaBuf), buf(other.buf)
	{
#ifdef _DEV_MODE_
		std::cout << " SendMemoryPoolUnit�� ��������ڰ� ȣ��Ǿ����ϴ�. \n";
#endif
	}

	SendMemoryUnit(SendMemoryUnit&& other)
		: wsaBuf(), buf(nullptr)
	{
		*this = std::move(other);
	}

	SendMemoryUnit& operator=(SendMemoryUnit&& other)
	{
#ifdef _DEV_MODE_
		std::cout << " SendMemoryPoolUnit�� �̵� �Ҵ� ������(Ȥ�� �̵� ������)�� ȣ��Ǿ����ϴ�. \n";
#endif
		if (this != &other)
		{
			delete[] buf;
			buf = other.buf;
			wsaBuf = other.wsaBuf;
			other.buf = nullptr;
		}
		return *this;
	}
};

class SendMemoryPool // Singleton
{
public:
	_NODISCARD static inline SendMemoryPool* GetInstance() noexcept { return SendMemoryPool::instance; };
	
	// �ش� �Լ��� GameServer.cpp�� �����ڿ��� �ѹ� ȣ��Ǿ���մϴ�.
	/*_NODISCARD*/ static void MakeInstance() { SendMemoryPool::instance = new SendMemoryPool(); /*return SendMemoryPool::instance;*/ };
	
	~SendMemoryPool();

	void PopMemory(SendMemoryUnit&);	// �޸� ����
	void PushMemory(SendMemoryUnit&&);	// �޸� �ݳ�

private:
	static SendMemoryPool* instance;
	SendMemoryPool();

	Concurrency::concurrent_queue<SendMemoryUnit> sendMemoryPool;
};

