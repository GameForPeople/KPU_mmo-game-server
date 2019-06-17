#pragma once

/*
	InHeaderDefine

	!0. �ش� �����, �����ϰ� �ٸ� ��� ���Ͽ� ���ԵǴ� ����Դϴ�.
*/
#ifdef _DEBUG
#define _DEV_MODE_
#endif

namespace std {
	template <class _Ty1, class _Ty2> struct pair;
}

//struct SocketInfo;
//using _ClientNode = std::pair<bool, SocketInfo*>;
//using _ClientCont = std::vector<_ClientNode>;

using _BufferType = char;

using _KeyType = unsigned int;
using _ClientKeyType = _KeyType;
using _MonsterKeyType = _KeyType;
using _NpcKeyType = _KeyType;

using _NicknameType = WCHAR;

using _PosType = unsigned short;

using _HpType = std::atomic<unsigned short>;
using _HpType_T = unsigned short;
using _MpType = std::atomic<unsigned short>;
using _MpType_T = unsigned short;

using _LevelType = std::atomic<unsigned char>;
using _LevelType_T = unsigned char;

using _DamageType = unsigned short;
using _StateType = unsigned char;
using _SectorIndexType = unsigned char;

using _FlagType = std::atomic<bool>;
using _FlagType_T = bool;

using _TickCountType = std::atomic<unsigned char>;
using _TickCountType_T = unsigned char;

using _ExpType = std::atomic<unsigned int>;
using _ExpType_T = unsigned int;

using _JobType = unsigned int;
using _MoneyType = std::atomic<unsigned int>;
using _MoneyType_T = unsigned int;

using _CountType = std::atomic<unsigned int>;
using _CountType_T = unsigned int;

using _TreeCountType = unsigned int;

enum class OBJECT_TYPE : unsigned char
{
	PLAYER,
	MONSTER,
	NPC
};

namespace GLOBAL_DEFINE
{
	//---------
	constexpr unsigned short MAX_SIZE_OF_RECV{ 100 };			//Recv �ѹ��� ���� �� �ִ� �ִ� ������
	constexpr unsigned short MAX_SIZE_OF_RECV_PACKET{ 100 };		//sizeof(PACKET_DATA::CLIENT_TO_SERVER::Chat);	// (2) Recv ��, ó���ؾ��ϴ� ��Ŷ �� ���� ū ������
	constexpr unsigned short MAX_SIZE_OF_SEND{ 100 };				// sizeof(PACKET_DATA::SERVER_TO_CLIENT::Position);	// (5) Send ��, ó���ؾ��ϴ� ��Ŷ �� ���� ū ������
	constexpr unsigned int MAX_NUMBER_OF_SEND_POOL{ 100000 };
	//---

	constexpr unsigned char MAX_CHAT_MASSAGE_SIZE { 80 };
}
