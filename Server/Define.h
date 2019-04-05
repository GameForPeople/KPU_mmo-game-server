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

namespace NETWORK_TYPE
{
	enum /*class NETWORK_TYPE : BYTE */
	{
		RECV /* = 0*/,
		SEND,
		ENUM_SIZE
	};
}

namespace PACKET_TYPE
{
	namespace CLIENT_TO_SERVER
	{
		enum
		{
			LEFT,
			UP,
			RIGHT,
			DOWN,
			ENUM_SIZE
		};
	}

	namespace SERVER_TO_CLIENT
	{
		enum
		{
			LOGIN_OK,
			PUT_PLAYER,
			REMOVE_PLAYER,
			MOVE,
			ENUM_SIZE
		};
	}

	namespace CS = CLIENT_TO_SERVER;
	namespace SC = SERVER_TO_CLIENT;
}

namespace PACKET_DATA
{
#pragma pack(push, 1)

	namespace CLIENT_TO_SERVER
	{
		struct Up
		{
			char size;
			char type;
		};

		struct Down
		{
			char size;
			char type;
		};

		struct Left
		{
			char size;
			char type;
		};

		struct Right
		{
			char size;
			char type;
		};
	}

	namespace SERVER_TO_CLIENT
	{
		struct LoginOk
		{
			char size;
			char type;
			char id;
		};

		struct PutPlayer
		{
			char size;
			char type;
			char id;
			char x;
			char y;
		};

		struct RemovePlayer
		{
			char size;
			char type;
			char id;
		};

		struct Position
		{
			char size;
			char type;
			char id;
			char x;
			char y;
		};
	}

#pragma pack(pop)

	namespace CS = CLIENT_TO_SERVER;
	namespace SC = SERVER_TO_CLIENT;
}

namespace DIRECTION
{
	enum /* class DIRECTION : BYTE */
	{
		LEFT /*= 0*/,
		UP /*= 1*/,
		RIGHT /*= 2*/,
		DOWN /*= 3*/,
		ENUM_SIZE
	};
}

namespace CHARACTER_CONVERTER {
	void SetLocaleToKorea();
}

namespace GLOBAL_DEFINE
{
	constexpr USHORT SERVER_PORT = 9000;
	constexpr BYTE MAX_HEIGHT = 7;
	constexpr BYTE MAX_WIDTH = 7;
	constexpr BYTE MAX_CLIENT = 10;

	// �Ʒ� ������� ���� �ش� ������� �߹�, SERVER_DEFINE���� ��ġ ���� �ʿ��մϴ�.
	constexpr USHORT MAX_SIZE_OF_RECV = 100;		//Recv �ѹ��� ���� �� �ִ� �ִ� ������
	constexpr USHORT MAX_SIZE_OF_RECV_PACKET = sizeof(PACKET_DATA::CLIENT_TO_SERVER::Down);	// (2) Recv ��, ó���ؾ��ϴ� ��Ŷ �� ���� ū ������
	constexpr USHORT MAX_SIZE_OF_SEND = sizeof(PACKET_DATA::SERVER_TO_CLIENT::Position);	// (5) Send ��, ó���ؾ��ϴ� ��Ŷ �� ���� ū ������
	constexpr USHORT MAX_NUMBER_OF_SEND_POOL = 100;
}