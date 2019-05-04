#pragma once

#include "InHeaderDefine.hh"

enum class MEMORY_UNIT_TYPE : BYTE
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
	const MEMORY_UNIT_TYPE memoryUnitType;	// �ش� ������ ���� �ÿ� ���ǵǰ� ������ ����.
	
	OVERLAPPED overlapped;
	WSABUF wsaBuf;
	
	char *dataBuf;

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
/*
	4����Ʈ ���� �� �ؾ��մϴ� ����.
*/

struct SocketInfo
{
public:
	SocketInfo() /*noexcept*/;
	~SocketInfo();

public:
	MemoryUnit memoryUnit;

	char loadedBuf[GLOBAL_DEFINE::MAX_SIZE_OF_RECV_PACKET];
	int loadedSize;

	SOCKET sock;
	std::wstring nickname;
	_ClientKeyType clientKey;
	BYTE contIndex;

	// UserData* userdata
	_PosType posX;	// ���ɻ��� �̽��� ���������� �ᱹ �и�. posX, posY ��� socketInfo�� ����
	_PosType posY;

	Zone* pZone;		// ���� ������ ��.

	BYTE sectorIndexX;	// �ڽ��� ���ͷ� ��~
	BYTE sectorIndexY;	// �ڽ��� ���ͷ� ��~

	BYTE possibleSectorCount;	// �˻��ؾ��ϴ� ���� ����, �ִ� 3�� �ʰ��� �� ����.
	std::array<std::pair<BYTE, BYTE>, 3> sectorArr;
	Concurrency::concurrent_unordered_set<_ClientKeyType> viewList;
};

/*
	TimerUnit
		- Ÿ�̸ӿ��� ���Ǵ� �޸� �����Դϴ�.
*/

struct TimerUnit
{
	const MEMORY_UNIT_TYPE memoryUnitType;	// �ش� ������ ���� �ÿ� ���ǵǰ� ������ ����.
	OBJECT_TYPE objectType;
	BYTE commandType;
	int objectKey;

public:
	TimerUnit();
	~TimerUnit();
};