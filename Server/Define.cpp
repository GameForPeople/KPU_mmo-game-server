#include "pch.h"

#include "Define.h"

namespace CHARACTER_CONVERTER
{
	void SetLocaleToKorea()
	{
		_wsetlocale(LC_ALL, L"korean");

		//26444 �� ������, ���� �ʿ� ����, L-Value�� �������ϴ°�;
		auto oldLocale = std::wcout.imbue(std::locale("koeran"));
	}
}