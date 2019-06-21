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

	_TickCountType faintTick;	// ���� ��, �� ������������ ������, �ش� �ð��� ��� ����ؾ���

	_TickCountType burnTick;	//�� ī��Ʈ 

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
	_CountType redCount;
	_CountType blueCount;
	_TreeCountType treeCount;

	_FlagType noDamageFlag;	// �ѹ� �����ϸ� �� ����, off�� �� on�Ǵ� ���� Ȯ����.
	_FlagType selfHealFlag;	// �� ȸ���� onoFF�� ó���� Ȯ����.
	_FlagType moveFlag;
	_FlagType attackFlag;

	_FlagType skill1Flag;
	_FlagType skill2Flag;

	_TickCountType redTickCount;
	_TickCountType blueTickCount;

public:
	PlayerObjectInfo();
	PlayerObjectInfo(_NicknameType*, _PosType, _PosType);
	~PlayerObjectInfo() final = default;
};