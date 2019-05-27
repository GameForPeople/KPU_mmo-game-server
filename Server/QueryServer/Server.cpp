#include "pch.h"
#include "../Define.h"
#include "GameQueryServer.h"

/*
	���� ���� �����ӿ�ũ ������Ʈ
			- �ѱ��������� ���Ӱ��а� 13�й� ������ (2013182027)

	https://github.com/GameForPeople/mmo-game-server-framework
*/

int main(int argc, char * argv[])
{
#ifdef UNICODE
	UNICODE_UTIL::SetLocaleToKorean();
#endif

	std::unique_ptr<GameQueryServer> gameQueryServer
		= std::make_unique<GameQueryServer>
		(
			[/* void */]() noexcept(false) -> bool
			{

#ifdef _DEV_MODE_
				std::cout << " ����!! _DEV_MODE_�� Ȱ��ȭ �Ǿ� �ֽ��ϴ�. " << std::endl;
#endif
				return true;
			}()
				);

	gameQueryServer->Run();
}
