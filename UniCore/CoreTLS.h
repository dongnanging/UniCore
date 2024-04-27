#pragma once
#include <stack>

extern thread_local std::stack<std::pair<int64, int32>> TLS_LockStack;

extern thread_local int32 TLS_ThreadID;

extern thread_local std::shared_ptr<SendBuffer> TLS_SendBuffer;