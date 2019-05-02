#pragma once

#include "InHeaderDefine.hh"

struct TimerUnit
{
	_KeyType objectKey;
	BYTE commandType;
	OBJECT_TYPE objectType;
};

class TimerManager /*: �̱��� */
{
	const int MAX_COOL_TIME;

public:
	_NODISCARD static inline TimerManager* GetInstance() noexcept { return TimerManager::instance; };

	// �ش� �Լ��� GameServer.cpp�� �����ڿ��� �ѹ� ȣ��Ǿ���մϴ�.
	/*_NODISCARD*/ static void MakeInstance(HANDLE hIOCP) { TimerManager::instance = new TimerManager(hIOCP); /*return SendMemoryPool::instance;*/ };

	// �ش� �Լ��� GameServer.cpp�� �Ҹ��ڿ��� �ѹ� ȣ��Ǿ���մϴ�.
	static void DeleteInstance() { delete instance; }

public:
	void TimerThread();
	void AddTimerEvent(TimerUnit*, int);

	_NODISCARD TimerUnit* GetTimerUnit();

private:
	HANDLE hIOCP;
	static TimerManager* instance;
	TimerManager(HANDLE hIOCP);
	~TimerManager();

	std::atomic<int> nowTime;	// 10���� 1�ʸ� ������ ����մϴ�. 10 = 1��, 100 = 10��
	std::vector<concurrency::concurrent_queue<TimerUnit*>> timerCont;
	concurrency::concurrent_queue<TimerUnit*> timerMemoryPool;
};