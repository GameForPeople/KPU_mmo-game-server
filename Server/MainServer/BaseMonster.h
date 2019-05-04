#pragma once

#include "InHeaderDefine.hh"

class BaseMonster 
{
public:
	BaseMonster( _MonsterKeyType,  _PosType,  _PosType);
	~BaseMonster() = default;

public:	// �̷����� �� �� �̰� class�߾��? -> ��������

	const _MonsterKeyType key;	// �̰� �ʿ��Ѱ� �¾�? �ε��� ��Ī�Ұǵ�...?

	_PosType posX;
	_PosType posY;

	BYTE sectorIndexX;	// �ڽ��� ���ͷ� ��~
	BYTE sectorIndexY;	// �ڽ��� ���ͷ� ��~

	BYTE possibleSectorCount;	// �˻��ؾ��ϴ� ���� ����, �ִ� 3�� �ʰ��� �� ����.
	std::array<std::pair<BYTE, BYTE>, 3> sectorArr;
};