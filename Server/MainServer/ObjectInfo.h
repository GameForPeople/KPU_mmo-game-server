#pragma once

#include "InHeaderDefine.hh"

/*
	���Ϳ�, NPC�� �������� ����� Object�� ������ ��� ����.
*/
struct ObjectInfo
{
public:
	ObjectInfo(_PosType,  _PosType);
	virtual ~ObjectInfo() = default;

public:	// �̷����� �� �� �̰� class�߾��? -> �������� -> �ᱹ Struct...��

	_PosType posX;
	_PosType posY;

	_SectorIndexType sectorIndexX;
	_SectorIndexType sectorIndexY;

	BYTE possibleSectorCount;
	std::array<std::pair<_SectorIndexType, _SectorIndexType>, 3> sectorArr;
};

/*
	�ᱹ ����� ����մϴ�..
*/
struct PlayerObjectInfo : public ObjectInfo
{
	_NicknameType nickname;

public:
	PlayerObjectInfo(_NicknameType&, _PosType, _PosType);
	~PlayerObjectInfo() final = default;
};