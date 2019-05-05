#pragma once

#include "InHeaderDefine.hh"

struct ObjectInfo
{
public:
	ObjectInfo(_KeyType,  _PosType,  _PosType);
	~ObjectInfo() = default;

public:	// �̷����� �� �� �̰� class�߾��? -> ��������

	const _KeyType key;	// �̰� �ʿ��Ѱ� �¾�? �ε��� ��Ī�Ұǵ�...?

	_PosType posX;
	_PosType posY;

	BYTE sectorIndexX;	// �ڽ��� ���ͷ� ��~
	BYTE sectorIndexY;	// �ڽ��� ���ͷ� ��~

	BYTE possibleSectorCount;	// �˻��ؾ��ϴ� ���� ����, �ִ� 3�� �ʰ��� �� ����.
	std::array<std::pair<BYTE, BYTE>, 3> sectorArr;
};
