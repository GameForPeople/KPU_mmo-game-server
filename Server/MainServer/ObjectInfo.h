#pragma once

#include "InHeaderDefine.hh"

/*
	Object�� ������ ��� ����.

*/
struct ObjectInfo
{
public:
	ObjectInfo(_NicknameType,  _PosType,  _PosType);
	~ObjectInfo() = default;

public:	// �̷����� �� �� �̰� class�߾��? -> �������� -> �ᱹ Struct...��

	_PosType posX;
	_PosType posY;

	_NicknameType nickname;

	_SectorIndexType sectorIndexX;	// �ڽ��� ���ͷ� ��~
	_SectorIndexType sectorIndexY;	// �ڽ��� ���ͷ� ��~

	BYTE possibleSectorCount;
	std::array<std::pair<_SectorIndexType, _SectorIndexType>, 3> sectorArr;
};
