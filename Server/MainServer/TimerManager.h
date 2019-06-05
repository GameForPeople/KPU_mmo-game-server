#pragma once

#include "InHeaderDefine.hh"

struct TimerMemoryHead;

/*
	TimerUnit
		- Ÿ�̸ӿ��� ���Ǵ� �޸� �����Դϴ�.
*/


enum class TIME : /*unsigned short*/ int
{
	MAX_TIME = 600,

	SECOND = 10,
	MINUTE = 600,

	SLIME_MOVE = 10,
	SLIME_ATTACK = 5,

	GOLEM_ATTACK = 20,

	MONSTER_REVIVAL = 300,

	// ��Ÿ��
	KNIGHT_SKILL_1 = 300,
	KNIGHT_SKILL_2 = 600,

	ARCHER_SKILL_1 = 200,
	ARCHER_SKILL_2 = 500,

	WITCH_SKILL_1 = 150,
	WITCH_SKILL_2 = 400,

	// Tick�� ����.
	ITEM_HP = 10,
	ITEM_MP = 10,

	// �̺�Ʈ Ȯ�� �ֱ�
	CC_NODAMAGE = 50,
	CC_FAINT = 20,
	CC_FREEZE = 30,
	CC_ELECTRIC = 30,
	CC_BURN = 50

	//KNIGHT_CC_NODAMAGE_COOLTIME = 50,
	//KNIGHT_CC_FAINT_COOLTIME = 20,
	//ARCHER_CC_FREEZE_COOLTIME = 30,
	//WITCH_CC_ELECTRIC_COOLTIME = 15
};

enum class TIMER_TYPE
{
	NONE_TYPE,
	NPC_MOVE,
	NPC_ATTACK,

	SKILL_1_COOLTIME,
	SKILL_2_COOLTIME,

	SELF_HEAL,
	REVIVAL,

	CC_NODAMAGE,
	CC_FAINT,
	CC_FREEZE,
	CC_ELECTRIC,
	CC_BURN,

	ITEM_HP,
	ITEM_MP,

	PUSH_OLD_KEY
};

struct TimerUnit
{
	TIMER_TYPE timerType;
	_KeyType objectKey;

public:
	TimerUnit();
	~TimerUnit();
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
	static DWORD WINAPI StartTimerThread();

	void AddTimerEvent(TimerUnit*, TIME);

	_NODISCARD TimerUnit* PopTimerUnit();
	void PushTimerUnit(TimerUnit*);

private:
	HANDLE hIOCP;
	static TimerManager* instance;

	TimerManager(HANDLE hIOCP);
	~TimerManager();

	std::atomic<int> nowTime;	// 10���� 1�ʸ� ������ ����մϴ�. 10 = 1��, 100 = 10��
	std::vector<concurrency::concurrent_queue<TimerUnit*>> timerCont;

	concurrency::concurrent_queue<TimerUnit*> timerMemoryPool;
	std::vector<std::array<TimerMemoryHead, 10>> timerMemoryHeadCont;

	BYTE postQueuedFunctionCallCount;
public:
	void SetPostQueuedFunctionCallCountAndTimerMemoryHeadCont(const BYTE inCallCount);
	concurrency::concurrent_queue<TimerUnit*>* GetTimerContWithIndex(const int inTimerContIndex);
};