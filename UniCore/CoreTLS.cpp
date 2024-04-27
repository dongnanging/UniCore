#include "stdafx.h"
#include "CoreTLS.h"

thread_local std::stack<std::pair<int64, int32>> TLS_LockStack;

thread_local int32 TLS_ThreadID = 0;

thread_local std::shared_ptr<SendBuffer> TLS_SendBuffer;