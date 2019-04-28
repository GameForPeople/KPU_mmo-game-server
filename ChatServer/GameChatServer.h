#pragma once

struct MemoryUnit;
struct SendMemoryUnit;
struct SocketInfo;

class ZoneContUnit;
class ChatManager;

/*
	GameChatServer
		- ���� ä�� �����Դϴ�.
*/
class GameChatServer
{
public:
	void Run();

public:
	GameChatServer(bool);
	~GameChatServer();

	// �� ���� ������ ��� �����ϴ�!
	GameChatServer(const GameChatServer&) = delete;
	GameChatServer& operator=(const GameChatServer&) = delete;

private:	// for Init
	void ServerIntegrityCheck();
	void PrintServerInfoUI();
	void InitNetwork();

private:	// for Worker Thread
	static DWORD WINAPI StartWorkerThread(LPVOID arg);
	void WorkerThreadFunction();

private:	// for Aceept Thread
	static DWORD WINAPI StartAcceptThread(LPVOID arg);
	void AcceptThreadFunction();

private:
	void AfterRecv(SocketInfo* pClient, int cbTransferred);
	void AfterSend(SendMemoryUnit* pUnit);

	void ProcessRecvData(SocketInfo* pClient, int restSize);
	void ProcessPacket(SocketInfo* pClient);

	void ProcessChat(SocketInfo* pClient);
	void ProcessConnect(SocketInfo* pClient);
	void ProcessChange(SocketInfo* pClient);

private:
	WSADATA								wsa;
	HANDLE								hIOCP;
	SOCKET								listenSocket;

	SOCKADDR_IN							serverAddr;

	std::vector<std::thread>			workerThreadCont;

	ZoneContUnit*						zoneCont;	// ���߿��� �� ������ ���� Ȯ���ؾ���

	std::unique_ptr<ChatManager>		chatManager;	
};
