#ifndef PCH_H
#define PCH_H

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

#define _WINSOCK_DEPRECATED_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS
#define _SILENCE_CXX17_OLD_ALLOCATOR_MEMBERS_DEPRECATION_WARNING
// <concurrent_unordered_set.h>
//오류	C4996 'std::allocator<int>::rebind<int>::other'
//: warning STL4010: Various members of std::allocator are deprecated in C++17. 
//Use std::allocator_traits instead of accessing these members directly.
//You can define _SILENCE_CXX17_OLD_ALLOCATOR_MEMBERS_DEPRECATION_WARNING or _SILENCE_ALL_CXX17_DEPRECATION_WARNINGS 
//to acknowledge that you have received this warning.	Server	C:\Program Files (x86)\Microsoft Visual Studio\2019\Community\VC\Tools\MSVC\14.20.27508\include\concurrent_unordered_set.h	45	

#pragma comment(lib, "ws2_32")
#pragma comment(lib, "wininet.lib")
#include <WinSock2.h>

#include <iostream>
#include <fstream>

#include <functional>
#include <optional>
#include <cassert>

#include <bitset>

#include <thread>
#include <mutex>
#include <shared_mutex>

#include <array>
#include <vector>
#include <list>
#include <unordered_set>	// by SectorCont

//#include "../include/tbb/concurrent_hash_map.h" // tbb 제거.

#include <concurrent_unordered_set.h>	// by ViewList, 
#include <concurrent_queue.h>			// by SendMemoryPool, 
#include <concurrent_vector.h>			// by ZoneCont!

#include <cwchar>
#include <string> 
#include <string_view>

#include <typeinfo>

extern "C" {
#include "include/lua.h"
#include "include/lauxlib.h"
#include "include/lualib.h"
}

#define		_NORETURN			[[noreturn]]
#define		_NODISCARD			[[nodiscard]]
#define		_DEPRECATED			[[deprecated]]
#define		_MAYBE_UNUSED		[[maybe_unused]]
#define		_FALLTHROUGH		[[fallthrough]]

#endif //PCH_H