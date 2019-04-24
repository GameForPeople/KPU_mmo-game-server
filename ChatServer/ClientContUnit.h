#pragma once

#include "InHeaderDefine.hh"

struct SocketInfo;

struct ZoneContUnit
{
	std::vector<_ClientNode> clientCont;
	std::shared_mutex wrlock;
};

struct SectorContUnit
{
	std::unordered_set<_ClientKeyType> clientCont;
	//Concurrency::concurrent_unordered_set<_ClientKeyType> clientCont;
	//std::list<_ClientKeyType> clientCont;
	std::shared_mutex wrlock;
};

struct UrbanSectorContUnit
{
	
};

struct RuralSectorContUnit
{
	Concurrency::concurrent_vector <std::pair<std::atomic<bool>, _ClientKeyType>> keyCont;
	std::mutex onlyWriteLock;
};