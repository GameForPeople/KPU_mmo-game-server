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
	void InNewClient(SocketInfo*);
	void OutClient(SocketInfo*);

	void CheckViewList(SocketInfo*, ZoneContUnit*);

	void SendPutPlayer(SocketInfo* pPutClient, SocketInfo* pRecvClient);
	void SendRemovePlayer(const _ClientKeyType pRemoveClient, SocketInfo* pRecvClient);

	void SendMovePlayer(SocketInfo* pPutClient, SocketInfo* pRecvClient);

private:
	bool IsSeeEachOther(const Position2D&, const Position2D&) noexcept;

public:
	Sector(const BYTE X, const BYTE Y);
	~Sector();

private:
	const BYTE indexX;
	const BYTE indexY;
	const BYTE centerX;
	const BYTE centerY;

	SectorContUnit* sectorContUnit;

public:
	inline constexpr BYTE GetCenterX() noexcept { return centerX; }
	inline constexpr BYTE GetCenterY() noexcept { return centerY; }
};