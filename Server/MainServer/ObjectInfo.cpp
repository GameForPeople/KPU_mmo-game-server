#include "pch.h"

#include "../Define.h"
#include "ServerDefine.h"

#include "ObjectInfo.h"

ObjectInfo::ObjectInfo()
	:
	posX(),
	posY(),
	sectorIndexX(),
	sectorIndexY(),
	possibleSectorCount(),
	sectorArr()
{
	// ��¥�� ���ŵ˴ϴ�.
}

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
//
//-----------

PlayerObjectInfo::PlayerObjectInfo() 
	: ObjectInfo()
	, nickname()
{

}

PlayerObjectInfo::PlayerObjectInfo(_NicknameType* inNickname, _PosType inX, _PosType inY)
	: ObjectInfo(inX, inY)
	, nickname()
{
	memcpy(nickname, inNickname, 20);
}