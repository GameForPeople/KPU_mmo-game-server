#include "pch.h"
#include "../Define.h"

#include "MemoryUnit.h"
#include "SendMemoryPool.h"
#include "Zone.h"
#include "ObjectInfo.h"
#include "ServerDefine.h"
#include "BaseMonster.h"
#include "LuaManager.h"

namespace NETWORK_UTIL
{
	extern SOCKET querySocket;
	extern QueryMemoryUnit* queryMemoryUnit;
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
		//std::cout << "���� : " << sendMemoryUnit->memoryUnit.wsaBuf.len << "Ÿ�� : " << (int)packetData[1] << "���� : " << (int)packetData[2];
#endif

		ZeroMemory(&(sendMemoryUnit->memoryUnit.overlapped), sizeof(sendMemoryUnit->memoryUnit.overlapped));

        //ERROR_HANDLING::errorRecvOrSendArr[
		//	static_cast<bool>(
				//1 + 
		if (SOCKET_ERROR == WSASend(pClient->sock, &(sendMemoryUnit->memoryUnit.wsaBuf), 1, NULL, 0, &(sendMemoryUnit->memoryUnit.overlapped), NULL) )
		{
			if (ERROR_HANDLING::HandleSendError())
			{
				LuaManager::GetInstance()->pZone->Death(pClient);
				closesocket(pClient->sock);
				SendMemoryPool::GetInstance()->PushMemory(sendMemoryUnit);
			}
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
			if (auto retValue = WSAGetLastError(); 
				retValue == ERROR_IO_PENDING)
			{
				return;
			}
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
			ERROR_HANDLING::HandleRecvError();
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
			ERROR_HANDLING::HandleRecvError();
			//ERROR_HANDLING::ERROR_DISPLAY("���޾Ҿ��....");
		}
	}

	namespace SEND
	{
		void SendRemovePlayer(const _KeyType pRemoveClientID, SocketInfo* pRecvClient)
		{
			PACKET_DATA::MAIN_TO_CLIENT::RemovePlayer packet(pRemoveClientID);

			NETWORK_UTIL::SendPacket(pRecvClient, reinterpret_cast<char*>(&packet));
		}

		void SendChatMessage(const WCHAR* message, const _KeyType senderKey, SocketInfo* pRecvClient)
		{
			PACKET_DATA::MAIN_TO_CLIENT::Chat packet(senderKey, message);
			NETWORK_UTIL::SendPacket(pRecvClient, reinterpret_cast<char*>(&packet));
		}

		void SendStatChange(const char inStatChangeType, const int inNewValue, SocketInfo* pRecvClient)
		{
			PACKET_DATA::MAIN_TO_CLIENT::StatChange packet(inStatChangeType, inNewValue);
			NETWORK_UTIL::SendPacket(pRecvClient, reinterpret_cast<char*>(&packet));
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

//	void LogOutProcess(LPVOID pClient)
//	{
//
//		//if (reinterpret_cast<MemoryUnit*>(pClient)->memoryUnitType == MEMORY_UNIT_TYPE::RECV_FROM_CLIENT)
//		//{
//		SocketInfo* pOutClient = reinterpret_cast<SocketInfo*>(pClient);
//
//		SOCKADDR_IN clientAddr;
//
//		int addrLength = sizeof(clientAddr);
//
//		getpeername(pOutClient->sock, (SOCKADDR*)& clientAddr, &addrLength);
//		std::cout << " [GOODBYE] Ŭ���̾�Ʈ (" << inet_ntoa(clientAddr.sin_addr) << ") �� �����߽��ϴ�. \n";
//
//		// ���ʿ� ���� ���ӵ� ���ߴµ�, �α׾ƿ� �� ��츦 ����.
//		if (pOutClient->key != -1) pOutClient->Exit(pOutClient);
//
//		closesocket(pOutClient->sock);
//		delete pOutClient;
//		//}
//		//else if (reinterpret_cast<MemoryUnit*>(pClient)->memoryUnitType == MEMORY_UNIT_TYPE::SEND_TO_CLIENT)
//		//{
//		//	SendMemoryUnit* pMemoryUnit = (reinterpret_cast<SendMemoryUnit*>(pClient));
//		//	SendMemoryPool::GetInstance()->PushMemory(pMemoryUnit);
//		//}
//#ifndef DISABLED_UNALLOCATED_MEMORY_SEND
//		else if (reinterpret_cast<MemoryUnit*>(pClient)->memoryUnitType == MEMORY_UNIT_TYPE::SEND)
//		{
//			UnallocatedMemoryUnit* pMemoryUnit = (reinterpret_cast<UnallocatedMemoryUnit*>(pClient));
//
//			SendMemoryPool::GetInstance()->PushUnallocatedMemory(pMemoryUnit);
//		}
//#endif
//	}

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
			FORMAT_MESSAGE_ALLOCATE_BUFFER |
			FORMAT_MESSAGE_FROM_SYSTEM,
			NULL, WSAGetLastError(),
			MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
			(LPTSTR)&lpMsgBuf, 0, NULL);
		wprintf(L"[%s] %s", msg, (WCHAR*)lpMsgBuf);
		LocalFree(lpMsgBuf);
	};

	/*
		HandleRecvOrSendError
			- Send Or Recv ���� �� ��µǴ� �Լ���, ������ ����մϴ�.
	*/
	void HandleRecvError()
	{
		auto retValue = WSAGetLastError();

		if (retValue == ERROR_IO_PENDING)
		{
			return;
		}
		if (retValue == WSAECONNRESET)
		{
			return;
		}
		if (retValue == WSAENOTSOCK)
		{
			return;
		}

		ERROR_DISPLAY(((L"HandleRecvError() : " + std::to_wstring(retValue)).c_str()));
	}

	bool HandleSendError()
	{
		auto retValue = WSAGetLastError();

		if (retValue == ERROR_IO_PENDING)
		{
			return false;
		}
		if (retValue == WSAECONNRESET)
		{
			return true;
		}
		if (retValue == WSAENOTSOCK)
		{
			return true;
		}
		
		ERROR_DISPLAY(((L"HandleSendError() : " + std::to_wstring(retValue)).c_str()));
	}
}

namespace JOB
{
	unsigned short GetMaxHP(_JobType inJob, unsigned char inLevel) noexcept
	{
		unsigned short  maxHp = BASE_HP; // �⺻ ü��

		if (inJob == JOB_TYPE::KNIGHT)
		{
			maxHp = maxHp + (inLevel * KNIGHT_HP_PER_LEVEL);
		}
		else if (inJob == JOB_TYPE::ARCHER)
		{
			maxHp = maxHp + (inLevel * ARCHER_HP_PER_LEVEL);
		}
		else if (inJob == JOB_TYPE::WITCH)
		{
			maxHp = maxHp + (inLevel * WITCH_HP_PER_LEVEL);
		}

		return maxHp;
	}

	unsigned short GetMaxMP(_JobType inJob, unsigned char inLevel) noexcept
	{
		unsigned short  maxMp = BASE_MP; // �⺻ ����

		if (inJob == JOB_TYPE::KNIGHT)
		{
			maxMp = maxMp + (inLevel * KNIGHT_MP_PER_LEVEL);
		}
		else if (inJob == JOB_TYPE::ARCHER)
		{
			maxMp = maxMp + (inLevel * ARCHER_MP_PER_LEVEL);
		}
		else if (inJob == JOB_TYPE::WITCH)
		{
			maxMp = maxMp + (inLevel * WITCH_MP_PER_LEVEL);
		}

		return maxMp;
	}

	unsigned short GetDamage(_JobType inJob, /*_LevelType*/ unsigned char inLevel) noexcept
	{
		unsigned short retDamage = BASE_DAMAGE; // �⺻ ����

		if (inJob == JOB_TYPE::KNIGHT)
		{
			retDamage = retDamage + (inLevel * KNIGHT_DAMAGE_PER_LEVEL);
		}
		else if (inJob == JOB_TYPE::ARCHER)
		{
			retDamage = retDamage + (inLevel * ARCHER_DAMAGE_PER_LEVEL);
		}
		else if (inJob == JOB_TYPE::WITCH)
		{
			retDamage = retDamage + (inLevel * WITCH_DAMAGE_PER_LEVEL);
		}
		return retDamage;
	}

	bool IsInAttackRange(int range, SocketInfo* pHitter, BaseMonster* pMonster)
	{
		return (range >= abs(pHitter->objectInfo->posX - pMonster->objectInfo->posX) + abs(pHitter->objectInfo->posY - pMonster->objectInfo->posY));
	}

	bool IsAttack(_JobType inJob, unsigned char inAttackkType, SocketInfo* pClient, BaseMonster* pMonster) noexcept
	{
		if (inJob == JOB_TYPE::KNIGHT)
		{
			if (inAttackkType == 0)
			{
				return IsInAttackRange(KNIGHT_ATTACK_0_RANGE, pClient, pMonster);
			}
			else if (inAttackkType == 2)
			{
				return IsInAttackRange(KNIGHT_ATTACK_2_RANGE, pClient, pMonster);
			}
		}
		else if (inJob == JOB_TYPE::ARCHER)
		{
			// ��� ������
			if (inAttackkType == 0 || inAttackkType == 1 || inAttackkType == 2)
			{
				return IsInAttackRange(ARCHER_ATTACK_0_RANGE, pClient, pMonster);
			}
		}
		else if (inJob == JOB_TYPE::WITCH)
		{
			if (inAttackkType == 0)
			{
				return IsInAttackRange(WITCH_ATTACK_0_RANGE, pClient, pMonster);
			}
			else if (inAttackkType == 1)
			{
				return IsInAttackRange(WITCH_ATTACK_1_RANGE, pClient, pMonster);
			}
			else if (inAttackkType == 2)
			{
				return IsInAttackRange(WITCH_ATTACK_2_RANGE, pClient, pMonster);
			}
		}
		return false;
	}
}

namespace LUA_UTIL
{
	void PrintError(lua_State* L)
	{
		std::cout << lua_tostring(L, -1);
		lua_pop(L, -1);
	}
}

namespace TIME_UTIL {
	const std::string GetCurrentDateTime() {
		time_t     now = time(0); //���� �ð��� time_t Ÿ������ ����
		struct tm  tstruct;
		char       buf[80];
		localtime_s(&tstruct, &now);
		strftime(buf, sizeof(buf), "%Y-%m-%d.%X", &tstruct); // YYYY-MM-DD.HH:mm:ss ������ ��Ʈ��

		return buf;
	}
}
