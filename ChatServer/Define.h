#pragma once

//--------------------- for DEV_MODE
//#define _DEV_MODE_
//---------------------


/*
	Define.h
		- �ش� ��� ������, ������ Ŭ���̾�Ʈ�� �������� ����մϴ�.
		(Ŭ���̾�Ʈ���� ���� ������Ʈ �ڵ� ����)

	#0. �ش� �����, ������ Ŭ���̾�Ʈ

	!0. enum, ���� ���, ���� �Լ��� ���, �ݵ�� namespace�ȿ� ����Ǿ�� �մϴ�.
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
			MOVE, 	//LEFT, //UP, //RIGHT, //DOWN,
			CHAT_SERVER_CHAT,	// CS::CHAT�� SC::CHAT�� �����ؾ��մϴ�.
			CHAT_SERVER_CONNECT,	// ä�� ������ �ش� Ŭ���̾�Ʈ�� ����մϴ�.
			CHAT_SERVER_CHANGE,		// �ش� Ŭ���̾�Ʈ�� ���� ����Ǿ����ϴ�.
			ENUM_SIZE
		};
	}

	namespace SERVER_TO_CLIENT
	{
		enum
		{
			POSITION,
			CHAT_SERVER_CHAT,	// CS::CHAT�� SC::CHAT�� �����ؾ��մϴ�.
			LOGIN_OK,
			PUT_PLAYER,
			REMOVE_PLAYER,
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
		struct Move {
			const char size;
			const char type;
			char direction;

			Move(char inDirection) noexcept;
		};

		struct Chat {
#ifdef NOT_USE
			char size;	// Fixed - 1	0
			const char type;	// Fixed - 1	1
			char nickNameLength;	// 1	2
			char messageLength;	// 1	2	// �е���Ʈ��Ȱ������ ����1����Ʈ���ڰ��
			std::wstring nickName;	// 1	
			std::wstring message;

			//message[0] = Length;				//Fixed
			//message[1] = type;					//Fixed
			//message[2] = nickNameLength;
			//message[3] = messageLength;

			//message[4] ~message[4 + nickNameLength * 2] = Nickname;
			//message[5 + nickNameLength * 2 + 1] ~message[Length] = ChatMessage;

			Chat(const char* pBufferStart);	
#endif
			char message[80];
			Chat(const std::wstring& inNickName, const std::wstring& inMessage);	// ���۷����� �ƴ�, Copy�մϴ�.
		};
	}

	namespace MAIN_SERVER_TO_CLIENT
	{
		struct LoginOk
		{
			const char size;
			const char type;
			char id;

			LoginOk(const char inNewId) noexcept;
		};

		struct PutPlayer
		{
			const char size;
			const char type;
			char id;
			char x;
			char y;

			PutPlayer(const char inMovedClientId, const char inX, const char inY) noexcept;
		};

		struct RemovePlayer
		{
			const char size;
			const char type;
			char id;

			RemovePlayer(const char inRemovedClientID) noexcept;
		};

		struct Position
		{
			const char size;
			const char type;
			char id;
			char x;
			char y;

			Position(const char inMovedClientId, const char inX, const char inY) noexcept;
		};

	}

	namespace CHAT_SERVER_TO_CLIENT
	{
		struct Chat 
		{
			// ���ϸ� ���̱� ����, ä���� ������ ������� Ȱ��
			char message[80];
			Chat(char* );
		};
	}

#pragma pack(pop)

	namespace CS = CLIENT_TO_SERVER;	// �� ���� ��� ������.
	// namespace SC = SERVER_TO_CLIENT;	// �̰Ŵ� �� �������� �ڽ��� ��Ŷ�����͸� ����ϴ� ������ ����.
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

namespace UNICODE_UTIL
{
	void SetLocaleToKorean();

	_NODISCARD std::string WStringToString(const std::wstring& InWstring);
	_NODISCARD std::wstring StringToWString(const std::string& InString);
}

namespace GLOBAL_DEFINE
{
	constexpr USHORT SERVER_PORT = 9000;
	constexpr USHORT CHAT_SERVER_PORT = 9001;

	constexpr BYTE MAX_HEIGHT = 100;
	constexpr BYTE MAX_WIDTH = 100;
}