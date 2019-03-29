#pragma once

/*
	Define.h
		- �ش� ��� ������, ������ Ŭ���̾�Ʈ�� �������� ����մϴ�. 
		(Ŭ���̾�Ʈ���� ���� ������Ʈ �ڵ� ����)

	#0. �ش� �����, Enum class, enum, Constexpr ��� ���� �����ؾ��մϴ�.
	
	!0. enum, ���� ����� ���, �ݵ�� namespace�ȿ� ����Ǿ�� �մϴ�.
		- ����� ���ӽ����̽� ����, ���������� ���Ǵ� ��츦 �����մϴ�.
	!1. enum class, enum�� ��� �������� ENUM_SIZE�� �����ؾ��մϴ�.
*/

namespace GLOBAL_DEFINE
{
	constexpr USHORT SERVER_PORT = 9000;
	constexpr BYTE MAX_HEIGHT = 7;
	constexpr BYTE MAX_WIDTH = 7;

	// �Ʒ� ������� ���� �ش� ������� �߹�, SERVER_DEFINE���� ��ġ ���� �ʿ��մϴ�.
	constexpr BYTE MAX_CLIENT = 10;
	constexpr USHORT MAX_SIZE_OF_RECV = 100;	//Recv �ѹ��� ���� �� �ִ� �ִ� ������
	constexpr USHORT MAX_SIZE_OF_RECV_PACKET = 30; //Recv ��, ó���ؾ��ϴ� ��Ŷ �� ���� ū ������
	constexpr USHORT MAX_SIZE_OF_SEND = 10;
	constexpr USHORT MAX_NUMBER_OF_SEND_POOL = 100;
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
	enum /*class NETWORK_TYPE : BYTE */
	{
		RECV /* = 0*/,
		SEND,
		ENUM_SIZE
	};
}

namespace DIRECTION
{
	enum /* class DIRECTION : BYTE */
	{
		LEFT /*= 0*/,
		UP /*= 1*/,
		RIGHT /*= 2*/,
		DOWN /*= 3*/,
		DIRECTION_END
	};
}