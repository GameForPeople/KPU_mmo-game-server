#pragma once

/*
	Define.h
		- �ش� ��� ������, ������ Ŭ���̾�Ʈ�� �������� ����մϴ�. 
		(Ŭ���̾�Ʈ���� ���� ������Ʈ �ڵ� ����)

	#0. �ش� �����, Enum class, enum, Constexpr ��� ���� �����ؾ��մϴ�.
	
	!0. enum, ���� ����� ���, �ݵ�� namespace�ȿ� ����Ǿ�� �մϴ�.
	!1. enum class, enum�� ��� �������� ENUM_SIZE�� �����ؾ��մϴ�.
*/

namespace GLOBAL_DEFINE
{
	constexpr USHORT SERVER_PORT = 9000;

	// �Ʒ� ������� ���� �ش� ������� �߹�, SERVER_DEFINE���� ��ġ ���� �ʿ��մϴ�.
	constexpr BYTE MAX_CLIENT = 10;
	constexpr USHORT MAX_SIZE_OF_BUFFER = 50;
	constexpr USHORT MAX_SIZE_OF_SEND_POOL = 100;
}

namespace PACKET_TYPE
{
	enum /* class PACKET_TYPE : BYTE */
	{
		MOVE /* = 0*/,
		ENUM_SIZE
	};
}

namespace NETWORK_TYPE
{
	enum NETWORK_TYPE /*class NETWORK_TYPE : BYTE */
	{
		RECV /* = 0*/,
		SEND,
		ENUM_SIZE
	};
}