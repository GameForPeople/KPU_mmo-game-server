#pragma once

#include "InHeaderDefine.hh"

/*
	���Ϳ�, NPC�� �������� ����� Object�� ������ ��� ����.
*/
struct ObjectInfo
{
public:
	ObjectInfo();
	ObjectInfo(_PosType,  _PosType);
	virtual ~ObjectInfo() = default;

public:	// �̷����� �� �� �̰� class�߾��? -> �������� -> �ᱹ Struct...��
	_PosType posX;
	_PosType posY;

	_LevelType level;
	
	_JobType job;

	_HpType hp;

	_DamageType damage;

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
	_NicknameType nickname[10];
	_MpType mp;
	_ExpType exp;
	_MoneyType money;
	_RedCountType redCount;
	_BlueCountType blueCount;
	_TreeCountType treeCount;

public:
	PlayerObjectInfo();
	PlayerObjectInfo(_NicknameType*, _PosType, _PosType);
	~PlayerObjectInfo() final = default;
};