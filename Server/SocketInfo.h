#pragma once

class UserData;
class Scene;

/*
	SocketInfo
		- ������������ü �Դϴ�.

	!0. ���� ��� ���� ���� ������ ����� ����Ǿ��մϴ�.
		- ���� ���� overlapped ����ü, WSABUF, char*�� ������ �׻� ����Ǿ� �մϴ�.
*/
struct SocketInfo
{
public:
	SocketInfo() /*noexcept*/;
	~SocketInfo();

public:
	OVERLAPPED overlapped;	// OVERLAPPED ����ü
	WSABUF wsaBuf;
	char* recvBuf;
	int loadedSize;
	char *loadedBuf; 

	SOCKET sock;
	UserData* userData;
	//std::unique_ptr<UserData> userData;

	int clientContIndex;
	Scene* pScene;
};