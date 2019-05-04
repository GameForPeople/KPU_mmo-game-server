#pragma once

#include "InHeaderDefine.hh"

struct SectorContUnit;
struct ZoneContUnit;

struct SocketInfo;

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

	void JudgeClientWithViewList(SocketInfo*, ZoneContUnit*);

	void SendPutPlayer(SocketInfo* pPutClient, SocketInfo* pRecvClient);
	void SendRemovePlayer(const _ClientKeyType pRemoveClient, SocketInfo* pRecvClient);

	void SendMovePlayer(SocketInfo* pPutClient, SocketInfo* pRecvClient);

private:
	bool IsSeeEachOther(const _PosType, const _PosType, const _PosType, const _PosType) const noexcept;
	bool IsSeeEachOther(const std::pair<_PosType, _PosType>&, const std::pair<_PosType, _PosType>&) const noexcept;

public:
	Sector(const BYTE sectorIndeX, const BYTE sectorIndeY);
	~Sector();

private:
	const BYTE indexX;
	const BYTE indexY;
	const BYTE centerX;
	const BYTE centerY;

	SectorContUnit* sectorContUnit;

public:
	inline constexpr BYTE GetCenterX() const noexcept { return centerX; }
	inline constexpr BYTE GetCenterY() const noexcept { return centerY; }
};