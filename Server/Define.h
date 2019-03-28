#pragma once

/*
	Define.h
		- �ش� ��� ������, ������ Ŭ���̾�Ʈ�� �������� ����մϴ�. 
		(Ŭ���̾�Ʈ���� ���� ������Ʈ �ڵ� ����)

	#0. �ش� �����, Enum, Static Constexp
*/

namespace GLOBAL_DEFINE
{
	constexpr USHORT SERVER_PORT = 9000;
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