#pragma once

#include "InHeaderDefine.hh"

enum class MEMORY_UNIT_TYPE : int		/*int*/
{
	RECV_FROM_CLIENT = 0x00,
	SEND_TO_CLIENT = 0x01,

	TIMER_FUNCTION = 0x02,
	RECV_FROM_COMMAND = 0x03,
	//UNALLOCATED_SEND = 0x04
	ENUM_SIZE
};

#ifndef DISABLED_UNALLOCATED_MEMORY_SEND
/*
	UnallocatedMemoryUnit
		- �Ҵ����� �ʰ�, ���� ���۸� Ȱ���Ͽ� Send�ϴ� �޸𸮿� ���˴ϴ�.
*/
struct UnallocatedMemoryUnit
{
	OVERLAPPED overlapped;
	WSABUF wsaBuf;

	const MEMORY_UNIT_TYPE memoryUnitType;	// �ش� ������ ���� �ÿ� ���ǵǰ� ������ ����.
};
#endif

/*
	MemoryUnit
		- ������ ���� Send, Recv ��, �ش� ó������ �ʿ��� ����� �����س��� ����ü�Դϴ�.
	
	#0. �ش� ����ü�� �뵵�� (isRecv)�� ���� �� �� �ֽ��ϴ�.

	#1. �ش� ����ü��, ���Ǵ� ������ ����,
		- Recv����, SocketInfo���� ���� �������� ����Ǵ� ���� ��� ������ ���˴ϴ�.
		- Send����, SendMemoryUnit���� ���� �������� ����Ǵ� ���� ��� ������ ���˴ϴ�.

	#2. dataBuf�� �������, ���Ǵ� ������ ����,
		- Recv����, GLOBAL_DEFINE::MAX_SIZE_OF_RECV ��ŭ
		- Send����, GLOBAL_DEFINE::MAX_SIZE_OF_SEND ��ŭ �Ҵ�޽��ϴ�.

	!0. #2�� �����Ͽ�, MAX_SIZE_OF_SEND���� ū ��쿡 ���Ͽ� ������ ��û�� ���, ������ �߻��մϴ�.
		- ���� �׻�, MAX_SIZE_OF_SEND�� ���۵Ǵ� ��Ŷ�� �� ���� ū ������� ���ǵǾ� �մϴ�.

	!1. �ش� ����ü�� �ܵ����� ������ �ʽ��ϴ�.

	!2. [MAIN] 19/05/03 ���ķ�, �ش� ����ü ��� ������ ������ ����Ǿ����ϴ�.
*/
struct MemoryUnit
{
	OVERLAPPED overlapped;
	WSABUF wsaBuf;

	const MEMORY_UNIT_TYPE memoryUnitType;	// �ش� ������ ���� �ÿ� ���ǵǰ� ������ ����.

	_BufferType *dataBuf;
public:
	MemoryUnit(const MEMORY_UNIT_TYPE InMemoryUnitType);
	~MemoryUnit();

	MemoryUnit(const MemoryUnit& other);
	MemoryUnit(MemoryUnit&& other) noexcept;
	MemoryUnit& operator=(MemoryUnit&& other) noexcept;
};

/*
	SendMemoryUnit
		- Send ��, ���Ǵ� MemoryUnit�Դϴ�.

	#0. pOwner�� �ش� Send�� ����� �Ǵ� Socket Info ����ü�Դϴ�.
		- �̴� Send ���� ��, ����ó���� ���� ���˴ϴ�.

	!0. ��� ���� ���� �������� MemoryUnit�� �־���մϴ�. ����� ����Ǿ��մϴ�.

	?0. POwner�� ���ӿ� �ǹ��� ������ϴ�. �̰� �ʿ䰡 ���� ��?
		- ���� �ڵ� ��� �ּ� ó���ϰ�, Ȯ�� ��, ���� ����.
*/
//struct SocketInfo;

struct SendMemoryUnit
{
	MemoryUnit memoryUnit;
	//SocketInfo* pOwner;	// Send ����ó�� ���� -> ���� ���� Ȯ�� �� pOwner�� Search!

	SendMemoryUnit();
	~SendMemoryUnit();

	SendMemoryUnit(const SendMemoryUnit& other);
	SendMemoryUnit(SendMemoryUnit&& other) noexcept;
	SendMemoryUnit& operator=(SendMemoryUnit&& other) noexcept;
};

/*
	SocketInfo
		- ������������ü �Դϴ�.

	!0. ��� ���� ���� �������� MemoryUnit�� �־���մϴ�. ����� ����Ǿ��մϴ�.
*/
class Zone;
struct ObjectInfo;
/*
	4����Ʈ ���� �� �ؾ��մϴ� ����.
*/

struct SocketInfo
{
public:
	SocketInfo(_KeyType) /*noexcept*/;
	~SocketInfo();

public:
	MemoryUnit memoryUnit;

	_BufferType loadedBuf[GLOBAL_DEFINE::MAX_SIZE_OF_RECV_PACKET];
	int loadedSize;

	SOCKET sock;
	_NicknameType nickname;
	BYTE contIndex;

	Zone* pZone;		// ���� ������ ��.

	Concurrency::concurrent_unordered_set<_ClientKeyType> viewList;
	Concurrency::concurrent_unordered_set<_MonsterKeyType> monsterViewList;

	ObjectInfo* objectInfo;
};

/*
	TimerUnit
		- Ÿ�̸ӿ��� ���Ǵ� �޸� �����Դϴ�.
*/

struct TimerUnit
{
	MemoryUnit memoryUnit;
	BYTE commandType;
	_KeyType objectKey;

public:
	TimerUnit();
	~TimerUnit();
};