#pragma once
#include "DeadLockProfiler.h"
#include "ThreadManager.h"


class CoreGlobal
{
public:
	CoreGlobal();

public:
	std::shared_ptr<DeadLockProfiler> deadLockProfiler;
	// std::shared_ptr<ThreadManager> threadManager;
	// std::shared_ptr<SendBufferSlicer> sendBufferSlicer;
};

extern CoreGlobal GlobalHandler;