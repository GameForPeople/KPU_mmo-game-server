#pragma once

#include "InHeaderDefine.hh"

struct ObjectInfo
{
public:
	ObjectInfo(_KeyType,  _PosType,  _PosType);
	~ObjectInfo() = default;

public:	// �̷����� �� �� �̰� class�߾��? -> �������� -> �ᱹ Struct...��

	const _KeyType key;	// �̰� �ʿ��Ѱ� �¾�? �ε��� ��Ī�Ұǵ�...?

	_PosType posX;
	_PosType posY;

	_SectorIndexType sectorIndexX;	// �ڽ��� ���ͷ� ��~
	_SectorIndexType sectorIndexY;	// �ڽ��� ���ͷ� ��~

	BYTE possibleSectorCount;	// �˻��ؾ��ϴ� ���� ����, �ִ� 3�� �ʰ��� �� ����.
	std::array<std::pair<_SectorIndexType, _SectorIndexType>, 3> sectorArr;
};
