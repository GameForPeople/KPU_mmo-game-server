#pragma once

#include "InHeaderDefine.hh"

struct SectorContUnit;
struct ZoneContUnit;

struct SocketInfo;
struct ObjectInfo;

/*
	Sector
		- Sector�� �þ� ó���� ���� �⺻ �����Դϴ�.
	
	#0. Sector�� ���Ϳ� ���Ե� Ŭ���̾�Ʈ�� ����� �����ϴ�.
	#1. Zone�̳� Manager�� �ٸ���, �ش� Ŭ������, ���������� ó������ �ʰ�, Client Cont�� ���� �κи� ó���մϴ�!
	#2. #1 �����, ��� Sector���� ó���Ѵ�~
*/

class Sector
{
public:
	void Join(SocketInfo*);
	void Exit(SocketInfo*);

	void JoinForNpc(BaseMonster*);
	void ExitForNpc(BaseMonster*);

	void MakeNewViewList(std::unordered_set<_KeyType>&, std::unordered_set<_KeyType>&, SocketInfo*, ZoneContUnit*);
	void MakeNewViewListForNpc(std::unordered_set<_KeyType>&, BaseMonster*, ZoneContUnit*);

	void MakeOldViewListForNpc(std::unordered_set<_KeyType>&, BaseMonster*, ZoneContUnit*);

private:
	//bool IsSeeEachOther(const _PosType, const _PosType, const _PosType, const _PosType) const noexcept;
	//bool IsSeeEachOther(const std::pair<_PosType, _PosType>&, const std::pair<_PosType, _PosType>&) const noexcept;
	bool IsSeeEachOther(const ObjectInfo* const, const ObjectInfo* const) const noexcept;

public:
	Sector(const BYTE sectorIndeX, const BYTE sectorIndeY);
	~Sector();

private:
	const BYTE indexX;
	const BYTE indexY;
	const _PosType centerX;
	const _PosType centerY;

	SectorContUnit* sectorContUnit;

public:
	inline constexpr _PosType GetCenterX() const noexcept { return centerX; }
	inline constexpr _PosType GetCenterY() const noexcept { return centerY; }
};