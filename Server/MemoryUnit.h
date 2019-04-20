#pragma once

#include "InHeaderDefine.hh"

enum class MEMORY_UNIT_TYPE : BYTE
{
	RECV = 0x00,
	SEND = 0x01,
	UNALLOCATED_SEND = 0x02
};

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
*/
struct MemoryUnit
{
	OVERLAPPED overlapped;
	WSABUF wsaBuf;
	
	const MEMORY_UNIT_TYPE memoryUnitType;	// �ش� ������ ���� �ÿ� ���ǵǰ� ������ ����.
	
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
class UserData;
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

	int loadedSize;
	//char *loadedBuf;
	char loadedBuf[GLOBAL_DEFINE::MAX_SIZE_OF_RECV_PACKET];

	SOCKET sock;
	UserData* userData;
	//std::unique_ptr<UserData> userData;

	_ClientKeyType clientKey;
	
	Zone* pZone;		// ���� ������ ��.

	BYTE sectorIndexX;	// �ڽ��� ���ͷ� ��~
	BYTE sectorIndexY;	// �ڽ��� ���ͷ� ��~

	BYTE possibleSectorCount;	// �˻��ؾ��ϴ� ���� ����, �ִ� 3�� �ʰ��� �� ����.
	std::array<std::pair<BYTE, BYTE>, 3> sectorArr;
	Concurrency::concurrent_unordered_set<_ClientKeyType> viewList;
};