#pragma once

#include "InHeaderDefine.hh"

/*
	MonsterLoader
	
		- ������ �ε��� ��, File I/O�� ���� ���� ������ �ε��ϰ�,
			�ٷ� ���ŵǴ�(�ʿ䰡 ����) ��ü�Դϴ�.
*/

class MonsterLoader
{
	using _PosContType = std::vector<std::pair<_PosType, _PosType>>;

public:
	MonsterLoader();

	_NODISCARD inline _PosContType& GetSlimePosCont() noexcept { return slimePosCont; };
	_NODISCARD inline _PosContType& GetGolemPosCont() noexcept { return golemPosCont; };

private:
	_PosContType slimePosCont;
	_PosContType golemPosCont;
};