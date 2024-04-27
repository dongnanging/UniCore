#pragma once
struct CriticalSection
{
public:
	CriticalSection()
	{
		InitializeCriticalSection(&_cs);
	}

	~CriticalSection()
	{
		DeleteCriticalSection(&_cs);
	}

	inline auto& cs() { return _cs; }

	CriticalSection(const CriticalSection& other) = delete;
	CriticalSection(CriticalSection&& right) = delete;
	CriticalSection& operator=(const CriticalSection& other) = delete;
	CriticalSection& operator=(CriticalSection&& right) = delete;

private:
	CRITICAL_SECTION _cs;
};

class LockSecScope
{
public:
	LockSecScope(CRITICAL_SECTION& cs)
		: _cs(cs)
	{
		EnterCriticalSection(&_cs);
	}

	~LockSecScope()
	{
		LeaveCriticalSection(&_cs);
	}

	LockSecScope(const LockSecScope& other) = delete;
	LockSecScope(LockSecScope&& right) = delete;
	LockSecScope& operator=(const LockSecScope& other) = delete;
	LockSecScope& operator=(LockSecScope&& right) = delete;

private:
	CRITICAL_SECTION& _cs;
};