#pragma once

/*
	InHeaderDefine

	!0. �ش� �����, �����ϰ� �ٸ� ��� ���Ͽ� ���ԵǴ� ����Դϴ�.
*/

struct SocketInfo;

namespace std {
	template <class _Ty1, class _Ty2> struct pair;
}

using _ClientNode = std::pair<bool, SocketInfo*>;
using _ClientCont = std::vector<_ClientNode>;
using _ClientKeyType = int;