#pragma once

struct UnallocatedMemoryUnit;
struct MemoryUnit;

struct SendMemoryUnit;
struct SocketInfo;
struct TimerMemoryHead;

class Zone;

/*
	GameServer
		- ���Ӽ����Դϴ�.
		
		&0. Zone�� Vector�� ������ �����ϴ� ��Ȳ����, �� ���� ������ �� ���� Zone�� �� �����ϵ��� ����(19/05/17)
			- �Լ� : InitZones 
			- ���� : zoneCont
		&1. �Լ� �迭�� ���� �ؽ� ��ɿ���, Switch-case�� ����, ���ʿ� ����(19/05/18)
			- ��ũ�� : DISABLED_FUNCTION_POINTER 
			- �Լ� : InitFunctions
			- ���� : recvOrSendArr
		&2. ������ �Լ��� ������ ����(19/05/18)
			- static DWORD WINAPI -> static void
		&3. ���ʿ��� �Լ� �ܰ�, ���Ž÷� ���� ��, ���� �ζ���. 
			- �Լ��� : AfterSend, AfterRecv, ProcessTimerUnit
*/
class GameServer
{
public:
	void Run();

public:
	GameServer(bool);
	~GameServer();

	// �� ���� ������ ��� �����ϴ�!
	GameServer(const GameServer&) = delete;
	GameServer& operator=(const GameServer&) = delete;

private:	// for Init
	void ServerIntegrityCheck();
	void PrintServerInfoUI();
	void InitNetwork();

private:	// for Worker Thread
	static void StartWorkerThread(LPVOID arg);
	void WorkerThreadFunction();

private:	// for Aceept Thread
	static void StartAcceptThread(LPVOID arg);
	void AcceptThreadFunction();

private:
	void ProcessRecvData(SocketInfo* pClient, int restSize);


private:
	WSADATA								wsa;
	HANDLE								hIOCP;
	SOCKET								listenSocket;

	SOCKADDR_IN							serverAddr;

	std::vector<std::thread>			workerThreadCont;
	std::unique_ptr<Zone>				zone;
};

#pragma region [Legacy Code]
	//void AfterRecv(SocketInfo* pClient, int cbTransferred);
	//void AfterSend(SendMemoryUnit* pUnit);
	//void ProcessTimerUnit(const int timerManagerContIndex);
#pragma endregion