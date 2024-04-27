#include "stdafx.h"
#include "CoreGlobal.h"

CoreGlobal GlobalHandler;

CoreGlobal::CoreGlobal()
{
	deadLockProfiler = J_MakeShared<DeadLockProfiler>();
	threadManager = J_MakeShared<ThreadManager>();
	sendBufferSlicer = J_MakeShared<SendBufferSlicer>();
}