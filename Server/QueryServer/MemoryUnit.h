#pragma once

#include "InHeaderDefine.hh"

enum class MEMORY_UNIT_TYPE : BYTE
{
	RECV = 0x00,
	SEND = 0x01,
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
*/
struct MemoryUnit
{
	OVERLAPPED overlapped;
	WSABUF wsaBuf;

	const MEMORY_UNIT_TYPE memoryUnitType;	// �ش� ������ ���� �ÿ� ���ǵǰ� ������ ����.

	char dataBuf[GLOBAL_DEFINE::MAX_SIZE_OF_RECV_PACKET];

public:
	MemoryUnit(const MEMORY_UNIT_TYPE InMemoryUnitType);
	~MemoryUnit();

	MemoryUnit(const MemoryUnit& other);
	MemoryUnit(MemoryUnit&& other) noexcept;
	MemoryUnit& operator=(MemoryUnit&& other) noexcept;
};


