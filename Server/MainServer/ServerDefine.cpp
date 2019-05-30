#include "pch.h"
#include "../Define.h"

#include "MemoryUnit.h"
#include "SendMemoryPool.h"
#include "Zone.h"
#include "ObjectInfo.h"
#include "ServerDefine.h"

namespace NETWORK_UTIL
{
	/*
		SendPacket()
			- WSASend!�� ���⿡���� ������ �� �ֽ��ϴ�.

		!0. �ܼ��� WSA Send�� �ִ� �Լ��Դϴ�. �����ʹ� �غ����ּ���.
		!1. �� �Լ��� ȣ���ϱ� ����, wsaBuf�� len�� �������ּ���.

		?0. wsaBuf�� buf�� ���������� �ٲ���� �ұ��?
	*/
	void SendPacket(SocketInfo* pClient, char* packetData)
	{
		SendMemoryUnit* sendMemoryUnit = SendMemoryPool::GetInstance()->PopMemory();
		memcpy(sendMemoryUnit->memoryUnit.dataBuf, packetData, packetData[0]);
		
		sendMemoryUnit->memoryUnit.wsaBuf.len = static_cast<ULONG>(packetData[0]);

#ifdef _DEV_MODE_
		std::cout << "���� : " << sendMemoryUnit->memoryUnit.wsaBuf.len << "Ÿ�� : " << (int)packetData[1] << "���� : " << (int)packetData[2];
#endif

		ZeroMemory(&(sendMemoryUnit->memoryUnit.overlapped), sizeof(sendMemoryUnit->memoryUnit.overlapped));

        //ERROR_HANDLING::errorRecvOrSendArr[
		//	static_cast<bool>(
				//1 + 
		if (SOCKET_ERROR ==
			WSASend(pClient->sock, &(sendMemoryUnit->memoryUnit.wsaBuf), 1, NULL, 0, &(sendMemoryUnit->memoryUnit.overlapped), NULL)
			)
		{
			ERROR_HANDLING::ERROR_DISPLAY(L"SendPacket()");
		}
			//]();
	}

	void SendQueryPacket(char* packetData)
	{
		SendMemoryUnit* sendMemoryUnit = SendMemoryPool::GetInstance()->PopMemory();
		memcpy(sendMemoryUnit->memoryUnit.dataBuf, packetData, packetData[0]);

		sendMemoryUnit->memoryUnit.wsaBuf.len = static_cast<ULONG>(packetData[0]);
		
		ZeroMemory(&(sendMemoryUnit->memoryUnit.overlapped), sizeof(sendMemoryUnit->memoryUnit.overlapped));

		if (SOCKET_ERROR ==
			WSASend(querySocket, &(sendMemoryUnit->memoryUnit.wsaBuf), 1, NULL, 0, &(sendMemoryUnit->memoryUnit.overlapped), NULL)
			)
		{
			ERROR_HANDLING::ERROR_DISPLAY(L"SendQuery()");
		}
	}

#ifndef DISABLED_UNALLOCATED_MEMORY_SEND
	void SendUnallocatedPacket(SocketInfo* pClient, char* pOriginData)
	{
		UnallocatedMemoryUnit* pUnallocatedMemoryUnit
			= SendMemoryPool::GetInstance()->PopUnallocatedMemory();

		pUnallocatedMemoryUnit->wsaBuf.buf = pOriginData;
		pUnallocatedMemoryUnit->wsaBuf.len = static_cast<ULONG>(pOriginData[0]);

		ZeroMemory(&(pUnallocatedMemoryUnit->overlapped), sizeof(pUnallocatedMemoryUnit->overlapped));

		if (SOCKET_ERROR ==
			WSASend(pClient->sock, &(pUnallocatedMemoryUnit->wsaBuf), 1, NULL, 0, &(pUnallocatedMemoryUnit->overlapped), NULL)
			)
		{
			ERROR_HANDLING::ERROR_DISPLAY("SendUnallocatedPacket()");
		}
	}
#endif

	/*
		RecvPacket()
			- WSA Recv�� ���⼭�� �����մϴ�.

		!0. SocketInfo�� �ִ� wsaBuf -> buf �� ���ú긦 �մϴ�. 
		!1. len�� ������ ���� ����մϴ�. MAX_SIZE_OF_RECV_BUFFER!
	*/
	void RecvPacket(SocketInfo* pClient)
	{
		// ���� �����Ϳ� ���� ó���� ������ �ٷ� �ٽ� ���� �غ�.
		DWORD flag{};

		ZeroMemory(&(pClient->memoryUnit.overlapped), sizeof(pClient->memoryUnit.overlapped));

		//ERROR_HANDLING::errorRecvOrSendArr[
		//	static_cast<bool>(
				//1 + 
		if (SOCKET_ERROR == WSARecv(pClient->sock, &(pClient->memoryUnit.wsaBuf), 1, NULL, &flag /* NULL*/, &(pClient->memoryUnit.overlapped), NULL))
		{
			ERROR_HANDLING::HandleRecvOrSendError();
			//ERROR_HANDLING::ERROR_DISPLAY("���޾Ҿ��....");
		}
		//		)
		//]();
	}

	void RecvQueryPacket()
	{
		DWORD flag{};
		ZeroMemory(&(queryMemoryUnit->memoryUnit.overlapped), sizeof(queryMemoryUnit->memoryUnit.overlapped));
		if (SOCKET_ERROR == WSARecv(querySocket, &(queryMemoryUnit->memoryUnit.wsaBuf), 1, NULL, &flag /* NULL*/, &(queryMemoryUnit->memoryUnit.overlapped), NULL))
		{
			ERROR_HANDLING::HandleRecvOrSendError();
			//ERROR_HANDLING::ERROR_DISPLAY("���޾Ҿ��....");
		}
	}

	/*
		LogOutProcess()
			- Ŭ���̾�Ʈ�� �α׾ƿ� ó���� �մϴ�.

			#0. OutClient �Լ�����, pZone�� nullptr�� clientKey�� -1�� �����մϴ�.

			!0. �ٸ�, Socket�� ���� ��� �ش� �Լ� ȣ�� ��, ������ �� �� �ֽ��ϴ�. (Accept ���� �� 
			!1. ��, nullptr�� ���ڷ� ���� ���, nullptr ���� ������ �߻��մϴ�.
			!2. pClient�� pZone�� nullptr�� ���, nullptr ���� ������ �߻��մϴ�. 

			#0. ���ɻ��� �̽���, !0, !1, !2�� nullptr���θ� �������� �ʽ��ϴ�. ( ������ ������ ���, nullptr������ �߻��ϱ� ����� )
	*/
	void LogOutProcess(LPVOID pClient)
	{
		if (reinterpret_cast<MemoryUnit*>(pClient)->memoryUnitType == MEMORY_UNIT_TYPE::RECV_FROM_CLIENT)
		{
			SocketInfo* pOutClient = reinterpret_cast<SocketInfo*>(pClient);

			SOCKADDR_IN clientAddr;

			int addrLength = sizeof(clientAddr);

			getpeername(pOutClient->sock, (SOCKADDR*)& clientAddr, &addrLength);
			std::cout << " [GOODBYE] Ŭ���̾�Ʈ (" << inet_ntoa(clientAddr.sin_addr) << ") �� �����߽��ϴ�. \n";
			
			// ���ʿ� ���� ���ӵ� ���ߴµ�, �α׾ƿ� �� ��츦 ����.
			if (pOutClient->objectInfo->key != -1) pOutClient->pZone->Exit(pOutClient);

			closesocket(pOutClient->sock);
			delete pOutClient;
		}
		else if (reinterpret_cast<MemoryUnit*>(pClient)->memoryUnitType == MEMORY_UNIT_TYPE::SEND_TO_CLIENT)
		{
			SendMemoryUnit* pMemoryUnit = (reinterpret_cast<SendMemoryUnit*>(pClient));
			SendMemoryPool::GetInstance()->PushMemory(pMemoryUnit);
		}
#ifndef DISABLED_UNALLOCATED_MEMORY_SEND
		else if (reinterpret_cast<MemoryUnit*>(pClient)->memoryUnitType == MEMORY_UNIT_TYPE::SEND)
		{
			UnallocatedMemoryUnit* pMemoryUnit = (reinterpret_cast<UnallocatedMemoryUnit*>(pClient));

			SendMemoryPool::GetInstance()->PushUnallocatedMemory(pMemoryUnit);
		}
#endif
	}
}

namespace ERROR_HANDLING
{
	/*
		ERROR_QUIT
			- ������ �ɰ��� ������ �߻��� ���, �޼��� �ڽ��� Ȱ���� ������ ����ϰ�, ������ �����մϴ�.
	*/
	_NORETURN void ERROR_QUIT(const WCHAR *msg)
	{
		LPVOID lpMsgBuf;
		FormatMessage(
			FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
			NULL,
			WSAGetLastError(),
			MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
			(LPTSTR)&lpMsgBuf,
			0,
			NULL
		);

		MessageBox(NULL, (LPTSTR)lpMsgBuf, msg, MB_ICONERROR);
		LocalFree(lpMsgBuf);
		exit(1);
	};

	/*
		ERROR_DISPLAY
			- ������ ������ �߻��� ���, ���� �α׸� ����մϴ�.
	*/
	/*_DEPRECATED*/ void ERROR_DISPLAY(const WCHAR *msg)
	{
		LPVOID lpMsgBuf;
		FormatMessage(
			FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
			NULL,
			WSAGetLastError(),
			MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
			(LPTSTR)&lpMsgBuf,
			0,
			NULL
		);

		//C603 ���� ���ڿ��� ��ġ���� �ʽ��ϴ�. ���̵� ���ڿ��� _Param_(3)���� ���޵Ǿ����ϴ�.
		//printf(" [%s]  %s", msg, (LPTSTR)&lpMsgBuf);
		std::wcout << L" Error no." << msg << L" - " << lpMsgBuf;
		LocalFree(lpMsgBuf);
	};

	/*
		HandleRecvOrSendError
			- Send Or Recv ���� �� ��µǴ� �Լ���, ������ ����մϴ�.
	*/
	void HandleRecvOrSendError()
	{
		if (WSAGetLastError() != ERROR_IO_PENDING)
		{
			ERROR_DISPLAY((L"RecvOrSend()"));
		}
	}
}

