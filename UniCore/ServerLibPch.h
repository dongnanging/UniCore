#pragma once
#include <iostream>

#include <type_traits>
#include <vector>
#include <deque>
#include <queue>
#include <stack>
#include <functional>
#include <map>
#include <set>


#if defined(_WIN64)
#if defined(_DEBUG) && defined(_WIN64)
#pragma comment(lib, "lib\\x64\\Debug\\libprotobufd.lib")
#pragma comment(lib, "lib\\x64\\Debug\\cryptlib.lib")
#elif defined(_DEBUG) && defined(_WIN32)
//#pragma comment(lib, "ServerCore\\Debug\\ServerCore.lib")
//#pragma comment(lib, "ProtocolBuffers\\Debug\\libprotobuf.lib")
#elif defined(_WIN64)
//#pragma comment(lib, "ServerCore\\Release\\ServerCore.lib")
//#pragma comment(lib, "ProtocolBuffers\\Release\\libprotobuf.lib")
#elif defined(_WIN32)
//#pragma comment(lib, "ServerCore\\Release\\ServerCore.lib")
//#pragma comment(lib, "ProtocolBuffers\\Release\\libprotobuf.lib")
#endif
#endif

//#define custom_malloc(size) ::malloc(size)
//#define custom_free(vptr) ::free(vptr)

#include <csignal>
#include <cstdlib>

// 커스텀 terminate 핸들러 함수
inline void customTerminateHandler() {
    std::cerr << "Unhandled exception! Terminating program safely..." << std::endl;
    std::exit(EXIT_FAILURE); // 비정상 종료를 피하기 위해 abort 대신 적절한 종료 방법 선택
}

// 시스템 신호 처리기 함수
inline void signalHandler(int signal) {
    std::cerr << "Received signal " << signal << ". Terminating program safely..." << std::endl;
    std::exit(EXIT_FAILURE);
}

inline void _enroll_exception_handler() {
    // 커스텀 terminate 핸들러를 설정
    std::set_terminate(customTerminateHandler);
    std::signal(SIGSEGV, signalHandler);
    std::signal(SIGABRT, signalHandler);
}


#define __BOOST_ASIO
#if defined(__BOOST_ASIO)	//Boost ASIO사용 => Linux, Window 크로스 가능
#include <boost/asio.hpp>
#define GETTICKCOUNT64() ::GetTickCount64()
#elif defined(_WIN64)		//winsock (IORP 혹은 RIO) 사용
#include<windows.h>
#define GETTICKCOUNT64() ::GetTickCount64()
#endif



#include "Base_Types.h"
#include "stdExtension.h"
#include "LockCsec.h"
#include "MemoryPool.h"
#include "Allocator.h"
#include "CoreMacro.h"
template<typename _Ty>
using now_allocator = PoolAllocator<_Ty>;
#include "AdvancedSTL.h"
#include "ObjectPool.h"
#include "Instantiator.h"

#include "NetBuffer.h"
#include "CoreGlobal.h"
#include "CoreTLS.h"

#if defined(__SQL_ODBC)
#include "DBConnector.h"
#elif defined(__SQL_ADO) || defined(__SQL_OLEDB)
#import "msado15.dll" no_namespace rename("EOF", "EndOfFile")
#include "ADODBCon.h"
#endif

#include "Service.h"
#include "Session.h"

#include "Cryptonizer.h"
#include "DataObjects.h"
#include "OnOffFlags.h"




//class SQL_Sender
//{
//public:
//	template<typename... _InputParams, typename... _OutputParams, typename... _Memory>
//	SQL_Sender(SQLWCHAR* proc, SQL_Binder<_InputParams...>& input,SQL_Binder<_OutputParams...>& output, SQL_Binder<_Memory...>& mem)
//	{
//		_sender = [&]() mutable { return GlobalHandler.dbConnector->ProcedureCast(proc, input.tie(), output.tie(), mem.tie()); };
//	}
//	void Send()
//	{
//		_ret = _sender();
//	}
//
//protected:
//	std::function<int32()> _sender;
//	int32 _ret = SQL_NO_RETURN;
//};
