#pragma once

/*
	ServerDefine.h
		- �ش� ��� ������, ���������� ����մϴ�.
*/
struct SendMemoryUnit;
struct SocketInfo;
struct MemoryUnit;

namespace NETWORK_UTIL
{
	void SendPacket(SocketInfo* pClient, char* packetData);
	void RecvPacket(SocketInfo* pClient);
	void LogOutProcess(LPVOID pClient);
	//_NODISCARD const bool GetRecvOrSendPacket(const LPVOID);
}

namespace BIT_CONVERTER
{
	constexpr BYTE SEND_BYTE = (1 << 7);

	/*_NODISCARD*/ BYTE MakeSendPacket(const BYTE inPacketType) noexcept;
	/*_NODISCARD*/ bool GetRecvOrSend(const char inChar) noexcept;
	
	/*_NODISCARD*/ BYTE MakeByteFromLeftAndRightByte(const BYTE inLeftByte, const BYTE inRightByte) noexcept;
	/*_NODISCARD*/ BYTE GetLeft4Bit(const BYTE inByte) noexcept;
	/*_NODISCARD*/ BYTE GetRight4Bit(const BYTE inByte) noexcept;
}

namespace ERROR_HANDLING {
	// �ش� static Function Array�� �ʱ�ȭ�� GameServer�� �����ڿ��� �̷����.
	static std::function<void(void)> errorRecvOrSendArr[2];
	inline void NotError(void) {};
	void HandleRecvOrSendError(void);

	_NORETURN void ERROR_QUIT(const CHAR *msg);
	/*_DEPRECATED*/ void ERROR_DISPLAY(const CHAR *msg);
}

 namespace GLOBAL_DEFINE
{
	constexpr BYTE START_POSITION_X = 50;
	constexpr BYTE START_POSITION_Y = 50;

	constexpr BYTE SECTOR_DISTANCE = 10;	// �� ��ü ũ��� viewDistance�� ����ؾ���!
	constexpr BYTE SECTOR_HALF_DISTANCE = SECTOR_DISTANCE / 2;

	constexpr BYTE VIEW_DISTANCE = 3;
	//---------
}