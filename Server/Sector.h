#pragma once

struct SectorContUnit;
struct SocketInfo;

/*
	Sector
		- Sector�� �þ� ó���� ���� �⺻ �����Դϴ�.
	
	#0. Sector�� ���Ϳ� ���Ե� Ŭ���̾�Ʈ�� ����� �����ϴ�.
	#1. Scene�̳� Manager�� �ٸ���, �ش� Ŭ������, ���������� ó������ �ʰ�, Client Cont�� ���� �κи� ó���մϴ�!
	#2. #1 �����, ��� Sector���� ó���Ѵ�~
*/

class Sector
{
public:
	SectorContUnit* GetSectorContUnit() noexcept;

	void InNewClient(SocketInfo*);
	void OutClient(SocketInfo*);

public:
	Sector(const BYTE X, const BYTE Y);
	~Sector();

private:
	BYTE indexX;
	BYTE indexY;
	BYTE centerX;
	BYTE centerY;

	SectorContUnit* sectorContUnit;
};