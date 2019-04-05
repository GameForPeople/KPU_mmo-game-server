#pragma once

#define DISABLED_FUNCTION_POINTER

struct MemoryUnit;
struct SocketInfo;
class Scene;

/*
	GameServer
		- ���Ӽ����Դϴ�.
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
	void PrintServerInfoUI();
	void InitScenes();
	void InitFunctions();
	void InitNetwork();

private:	// for Worker Thread
	static DWORD WINAPI StartWorkerThread(LPVOID arg);
	void WorkerThreadFunction();

private:	// for Aceept Thread
	static DWORD WINAPI StartAcceptThread(LPVOID arg);
	void AcceptThreadFunction();

private:
#ifdef DISABLED_FUNCTION_POINTER
	void AfterRecv(MemoryUnit* pClient, int cbTransferred);
	void AfterSend(MemoryUnit* pClient);
#else
	std::function <void(GameServer&, LPVOID)>* recvOrSendArr;
	void AfterRecv(LPVOID pClient);
	void AfterSend(LPVOID pClient);
#endif
	void ProcessRecvData(SocketInfo* pClient, int restSize);

private:
	WSADATA								wsa;
	HANDLE								hIOCP;
	SOCKET								listenSocket;

	SOCKADDR_IN							serverAddr;

	std::vector<std::thread>			workerThreadCont;
	std::vector<std::unique_ptr<Scene>>	sceneCont;
};

#pragma region [Legacy]
/*

struct ServerInitInfomation 
{
	enum class SERVER_IP_TYPE : BYTE
	{
		LOCAL_HOST = 1
		, INPUTTED_IP = 2
		//,	AWS_PUBLIC = 3
	};

	SERVER_IP_TYPE serverIPType;
	std::string IPAddress;

public:
	ServerInitInfomation(const SERVER_IP_TYPE inServerIpType, const std::string_view& inIPAddress) noexcept
		: serverIPType(inServerIpType)
		, IPAddress(inIPAddress)
	{};

	~ServerInitInfomation() = default;
};
*/
#pragma endregion