#include "pch.h"

#include "../Define.h"
#include "ServerDefine.h"

#include "ObjectInfo.h"

ObjectInfo::ObjectInfo(_PosType inX, _PosType inY)
	: 
	posX(inX),
	posY(inY),
	sectorIndexX(GLOBAL_DEFINE::START_SECTOR_X),
	sectorIndexY(GLOBAL_DEFINE::START_SECTOR_Y)
{
	// ��¥�� ���ŵ˴ϴ�.
}

//-----------
//-----------

PlayerObjectInfo::PlayerObjectInfo(_NicknameType& inNickname, _PosType inX, _PosType inY)
	: ObjectInfo(inX, inY),
	nickname(inNickname)
{
}