#include "stdafx.h"
#include "CoreGlobal.h"

CoreGlobal GlobalHandler;

CoreGlobal::CoreGlobal()
{
	deadLockProfiler = stdex::pmake_shared<DeadLockProfiler>();
	// threadManager = stdex::pmake_shared<ThreadManager>();
	// sendBufferSlicer = stdex::pmake_shared<SendBufferSlicer>();
}