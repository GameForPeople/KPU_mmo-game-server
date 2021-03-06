#include "pch.h"
#include "../Define.h"
#include "ServerDefine.h"
#include "InHeaderDefine.hh"
#include "GameServer.h"

/*
	게임 서버 프레임워크 프로젝트 (2019/03/05)
			- 한국산업기술대 게임공학과 13학번 원성연

	https://github.com/GameForPeople/mmo-game-server-framework
*/

int main(int argc, char * argv[])
{
	std::cout << "#. 게임서버를 실행합니다. :  " << TIME_UTIL::GetCurrentDateTime() << "\n";

#pragma region [ CHECK ]
#ifdef UNICODE
	UNICODE_UTIL::SetLocaleToKorean();
#else
	ERROR_HANDLING::ERROR_QUIT("문자 집합을 UNICODE로 변경해주세요. 서버를 종료합니다.");
#endif

#ifndef _WIN64
	ERROR_HANDLING::ERROR_QUIT(L"솔루션 플랫폼을 x64로 변경해주세요. 서버를 종료합니다.");
#endif

#ifdef _DEV_MODE_
	std::wcout << L"!. 주의하세요. _DEV_MODE_가 활성화 되어 있습니다. \n";
#endif

#pragma endregion

	std::unique_ptr<GameServer> gameServer
		= std::make_unique<GameServer>
		(
			[/* void */]() noexcept(false) -> bool
			{
				return true;
			}()
		);

	// 추후 커맨더 서버에서 명령.
	gameServer->Run();
}
