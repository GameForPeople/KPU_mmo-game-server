#pragma once

/*
	SendMemoryPool
		- Send�� �� ���� , ������ ����ü, wsaBuf, Buf�� �̸� �Ҵ��� ���� Ŭ�����Դϴ�.

	!0. �ش� ��ü�� ���� - �̱����Դϴ�.
		- �� ������ Send�� ��û�ϴ� ��ġ�� �ٸ� �� �ֱ� �����Դϴ�.
	
	!1. Send ��û�� ���� ���, �ش� Pool�� Empty ������ �� �ֽ��ϴ�. �׷� ���, ���ѷ����� ����մϴ�.
		- ���� �̸� �����ϱ� ����, ������ Pool ũ�⸦ ���ؾ��մϴ�?

	?0. !1�� ��Ȳ�� Ǯ ����� �ٲٴ� ������ �ذ��ϱ� ����� ���,
		Ǯ�� ���� á�� ��, �������� �޸𸮸� �Ҵ�޾� ��ȯ���ִ� ������ ������ �ʿ��մϴ�.
		- �ٸ� �����, ������ Ǯ ����� �˱� ���Ͽ�, �����մϴ�.
*/
class SendMemoryPool // Singleton
{
public:
	_NODISCARD static inline SendMemoryPool* GetInstance() noexcept { return SendMemoryPool::instance; };
	
	// �ش� �Լ��� GameServer.cpp�� �����ڿ��� �ѹ� ȣ��Ǿ���մϴ�.
	/*_NODISCARD*/ static void MakeInstance() { SendMemoryPool::instance = new SendMemoryPool(); /*return SendMemoryPool::instance;*/ };
	
	// �ش� �Լ��� GameServer.cpp�� �Ҹ��ڿ��� �ѹ� ȣ��Ǿ���մϴ�.
	static void DeleteInstance() { delete instance; }

	~SendMemoryPool();

	SendMemoryUnit* PopMemory(SocketInfo*);	// �޸� ����
	void PushMemory(SendMemoryUnit*);	// �޸� �ݳ�

private:
	static SendMemoryPool* instance;
	SendMemoryPool();

	Concurrency::concurrent_queue<SendMemoryUnit*> sendMemoryPool;
};

