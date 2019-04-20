#include "pch.h"
#include "Define.h"
#include "ServerDefine.h"

#include "UserData.h"
#include "Zone.h"

#include "MemoryUnit.h"

//---------------------------------------------------------------------------
// MemoryUnit
//---------------------------------------------------------------------------

MemoryUnit::MemoryUnit(const MEMORY_UNIT_TYPE inMemoryUnitType) :
	overlapped(),
	wsaBuf(),
	memoryUnitType(inMemoryUnitType),
	dataBuf(nullptr)
{
#ifdef _DEV_MODE_
	std::cout << " MemoryUnit�� �⺻�����ڰ� ȣ��Ǿ����ϴ�. \n";
#endif
	if (MEMORY_UNIT_TYPE::RECV == inMemoryUnitType)
	{
		dataBuf = new char[GLOBAL_DEFINE::MAX_SIZE_OF_RECV];
		wsaBuf.len = GLOBAL_DEFINE::MAX_SIZE_OF_RECV;
	}
	else dataBuf = new char[GLOBAL_DEFINE::MAX_SIZE_OF_SEND];

	wsaBuf.buf = dataBuf;
}

MemoryUnit::~MemoryUnit()
{
#ifdef _DEV_MODE_
	//std::cout << "MemoryUnit�� �Ҹ��ڰ� ȣ��Ǿ����ϴ�. \n";
#endif
	delete[] dataBuf;
}

MemoryUnit::MemoryUnit(const MemoryUnit& other) :
	overlapped(),
	wsaBuf(other.wsaBuf),
	dataBuf(other.dataBuf),
	memoryUnitType(other.memoryUnitType)
{
#ifdef _DEV_MODE_
	std::cout << " MemoryUnit�� ��������ڰ� ȣ��Ǿ����ϴ�. \n";
#endif
}

MemoryUnit::MemoryUnit(MemoryUnit&& other) noexcept :
	overlapped(),
	wsaBuf(),
	dataBuf(nullptr),
	memoryUnitType(other.memoryUnitType)
{
	*this = std::move(other);
}

MemoryUnit& MemoryUnit::operator=(MemoryUnit&& other) noexcept
{
#ifdef _DEV_MODE_
	std::cout << " MemoryUnit�� �̵� �Ҵ� ������(Ȥ�� �̵� ������)�� ȣ��Ǿ����ϴ�. \n";
#endif
	if (this != &other)
	{
		// �ƴ� ��������, ������ �غ� �̰� ������ �Ǵ°� �¾ƿ�? �������𸣰ھ�G��������
		delete[] dataBuf;

		dataBuf = other.dataBuf;
		wsaBuf = other.wsaBuf;
		other.dataBuf = nullptr;
	}
	return *this;
}

//---------------------------------------------------------------------------
// SendMemoryUnit
//---------------------------------------------------------------------------

SendMemoryUnit::SendMemoryUnit() 
	: memoryUnit(MEMORY_UNIT_TYPE::SEND)
	//, pOwner(nullptr)
{
}

SendMemoryUnit::~SendMemoryUnit()
{
}

SendMemoryUnit::SendMemoryUnit(const SendMemoryUnit& other) 
	: memoryUnit(other.memoryUnit)
	//, pOwner(other.pOwner)
{
#ifdef _DEV_MODE_
	std::cout << " SendMemoryUnit�� ��������ڰ� ȣ��Ǿ����ϴ�. \n";
#endif
}

SendMemoryUnit::SendMemoryUnit(SendMemoryUnit&& other) noexcept 
	: memoryUnit(other.memoryUnit)
{
	*this = std::move(other);
}

SendMemoryUnit& SendMemoryUnit::operator=(SendMemoryUnit&& other) noexcept
{
#ifdef _DEV_MODE_
	std::cout << " SendMemoryUnit�� �̵� �Ҵ� ������(Ȥ�� �̵� ������)�� ȣ��Ǿ����ϴ�. \n";
#endif
	if (this != &other)
	{
		memoryUnit = std::move(other.memoryUnit);
	}
	return *this;
}

//---------------------------------------------------------------------------
// SocketInfo
//---------------------------------------------------------------------------
SocketInfo::SocketInfo() /*noexcept*/ :
	memoryUnit(MEMORY_UNIT_TYPE::RECV),
	sock(),
	loadedSize(),
	loadedBuf(),
	userData(new UserData(GLOBAL_DEFINE::START_POSITION_X, GLOBAL_DEFINE::START_POSITION_Y)/*std::make_unique<UserData>(0, 0)*/),
	clientKey(-1),
	pZone(nullptr),
	viewList(),
	sectorArr(),
	sectorIndexX(),
	sectorIndexY(),
	possibleSectorCount()
{
	//loadedBuf = new char[GLOBAL_DEFINE::MAX_SIZE_OF_RECV_PACKET];
	viewList.clear();
}

SocketInfo::~SocketInfo()
{
	viewList.clear();

	//delete[] loadedBuf;
	delete userData;
}