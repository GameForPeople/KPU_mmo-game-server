#include "pch.h"
#include "MemoryUnit.h"
#include "SendMemoryPool.h"
#include "Scene.h"
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
	void SendPacket(SocketInfo* pClient, SendMemoryUnit* pMemoryUnit)
	{
		// ���� �����Ϳ� ���� ó���� ������ �ٷ� �ٽ� ���� �غ�.
		DWORD flag{};

		ZeroMemory(&pMemoryUnit->memoryUnit.overlapped, sizeof(pMemoryUnit->memoryUnit.overlapped));

		ERROR_HANDLING::errorRecvOrSendArr[
			static_cast<bool>(
				1 + WSASend(pClient->sock, &pMemoryUnit->memoryUnit.wsaBuf, 1, NULL, 0, &pMemoryUnit->memoryUnit.overlapped, NULL)
				)
		]();
	}

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

		ZeroMemory(&pClient->memoryUnit.overlapped, sizeof(pClient->memoryUnit.overlapped));

		ERROR_HANDLING::errorRecvOrSendArr[
			static_cast<bool>(
				1 + WSARecv(pClient->sock, &pClient->memoryUnit.wsaBuf, 1, NULL, &flag /* NULL*/, &pClient->memoryUnit.overlapped, NULL)
				)
		]();
	}

	/*
		LogOutProcess()
			- Ŭ���̾�Ʈ�� �α׾ƿ� ó���� �մϴ�.

			#0. OutClient �Լ�����, pScene�� nullptr�� clientContIndex�� -1�� �����մϴ�.

			!0. �ٸ�, Socket�� ���� ��� �ش� �Լ� ȣ�� ��, ������ �� �� �ֽ��ϴ�. (Accept ���� �� 
			!1. ��, nullptr�� ���ڷ� ���� ���, nullptr ���� ������ �߻��մϴ�.
			!2. pClient�� pScene�� nullptr�� ���, nullptr ���� ������ �߻��մϴ�. 

			#0. ���ɻ��� �̽���, !0, !1, !2�� nullptr���θ� �������� �ʽ��ϴ�. ( ������ ������ ���, nullptr������ �߻��ϱ� ����� )
	*/
	void LogOutProcess(MemoryUnit* pClient)
	{
		SocketInfo* pOutClient;
		if (pClient->isRecv)
		{
			pOutClient = reinterpret_cast<SocketInfo*>(pClient);
		}
		else
		{
			SendMemoryUnit* pMemoryUnit = (reinterpret_cast<SendMemoryUnit*>(pClient));
			pOutClient = pMemoryUnit->pOwner;
			SendMemoryPool::GetInstance()->PushMemory(pMemoryUnit);
		}

		SOCKADDR_IN clientAddr;

		int addrLength = sizeof(clientAddr);

		getpeername(pOutClient->sock, (SOCKADDR *)&clientAddr, &addrLength);
		std::cout << " [GOODBYE] Ŭ���̾�Ʈ (" << inet_ntoa(clientAddr.sin_addr) << ") �� �����߽��ϴ�. \n";
		if (pOutClient->clientContIndex != -1) pOutClient->pScene->OutClient(pOutClient);

		closesocket(pOutClient->sock);
		delete pOutClient;
	}

}

namespace BIT_CONVERTER
{
	/*
		MakeByteFromLeftAndRightByte()
			- ���ڷ� ���� ��Ŷ Ÿ��(����Ʈ)�� �ֻ��� ��Ʈ�� 1�� �ٲ��� ��ȯ�մϴ�.

		!0. ��Ŷ Ÿ�� ������, ox7f���� Ŭ ���, �ش� �Լ� �� ���� ���� ������ ������ �߻��մϴ�.
	*/
	_NODISCARD BYTE MakeSendPacket(const BYTE inPacketType) noexcept { return inPacketType | SEND_BYTE; }

	/*
		MakeByteFromLeftAndRightByte()
			- ���ڷ� ���� ��Ŷ Ÿ��(����Ʈ)�� �ֻ��� ��Ʈ�� �˻��մϴ�.

		#0. �ֻ��� ��Ʈ�� 1�� ���, true��, 0�� ��� false�� ��ȯ�մϴ�. (����ȯ�� ���� Array Overflow ���� )

		!0. ��Ŷ Ÿ�� ������, ox7f���� Ŭ ���, �ش� �Լ� �� ���� ���� ������ ������ �߻��մϴ�.
	*/
	_NODISCARD bool GetRecvOrSend(const char inChar) noexcept { return (inChar >> 7) & (0x01); }
	
	/*
		MakeByteFromLeftAndRightByte
			- 0x10���� ���� ����Ʈ �ΰ��� ���ڷ� �޾�(Left, Right) ���� 4����Ʈ�� Left, ���� 4�� ��Ʈ�� Right�� ��ƹ�ȯ�մϴ�.

		!0. ���ڷ� ������ �� ����Ʈ ���, 0x10���� ���� ���� ���;��մϴ�.
			- ���ڷ� ������ Left ����Ʈ�� 0x0f���� ū ���� ���� ���, �����÷ο�Ǿ� ���������� ���� ��Ⱥ�� �� ����.
			- ���ڷ� ������ Right ����Ʈ�� 0x0f���� ū ���� ���� ���, LeftByte�� | ���꿡�� ���������� ���� ��ȯ�� �� ����.
	*/
	_NODISCARD BYTE MakeByteFromLeftAndRightByte(const BYTE inLeftByte, const BYTE inRightByte) noexcept
	{
		return (inLeftByte << 4) | (inRightByte);
	}

	/*
		GetLeft4Bit
			- HIWORD(?)�� �����ϰ� �����մϴ�. �ϳ��� ����Ʈ�� �޾Ƽ� ����(����) 4���� ��Ʈ�� ����Ʈ�� ��ȯ�ؼ� ��ȯ�մϴ�.
	*/
	_NODISCARD BYTE GetLeft4Bit(const BYTE inByte) noexcept
	{
		return (inByte >> 4) & (0x0f);
	}

	/*
		GetRight4Bit
			- LOWORD(?)�� �����ϰ� �����մϴ�. �ϳ��� ����Ʈ�� �޾Ƽ� ����(����) 4���� ��Ʈ�� ����Ʈ�� ��ȯ�ؼ� ��ȯ�մϴ�.
	*/
	_NODISCARD BYTE GetRight4Bit(const BYTE inByte) noexcept
	{
		return (inByte) & (0x0f);
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
	/*_DEPRECATED*/ void ERROR_DISPLAY(const CHAR *msg)
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
		std::wcout << L" Error no.%s" << msg << L" - " << lpMsgBuf;
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
			ERROR_DISPLAY(("RecvOrSend()"));
		}
	}
}

