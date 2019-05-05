#pragma once

#include "InHeaderDefine.hh"

struct SocketInfo;
struct ZoneContUnit;

class Zone;

class ConnectManager
{
public:
	ConnectManager();
	~ConnectManager() = default;

	ConnectManager(const ConnectManager&) = delete;
	ConnectManager& operator=(const ConnectManager&) = delete;

public:
	std::pair<bool, SocketInfo*> LogInToZone(ZoneContUnit* inClientContUnit, Zone*);
	void LogOutToZone(SocketInfo*, ZoneContUnit* inClientContUnit);

private:
	void SendRemovePlayerInOuttedClientViewList(SocketInfo* pOutClient, ZoneContUnit* inClientCont);
	
	// Sector�� �̻簬���ϴ�.
	//void SendPutPlayer(SocketInfo* pPutClient, ZoneContUnit* inClientCont);
private:
	//std::shared_mutex connectLock;
	concurrency::concurrent_queue<USHORT> uniqueKeyPool;
};