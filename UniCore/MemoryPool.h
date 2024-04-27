#pragma once
#include "boost/lockfree/queue.hpp"
#define MAX_ARR_SIZE 1024 * 32
#define MAX_MERMORY_SIZE MAX_ARR_SIZE * 4


class MemoryPool
{
public:
	template <typename _Ty>
	struct raw_lock_queue
	{
		template <typename _Uy, typename = std::enable_if_t<std::is_same_v<_Ty, stdex::pure_type_t<_Uy>>,_Uy>>
		void push(_Uy&& item)
		{
			LockSecScope scopelock(csec.cs());
			//std::lock_guard<std::mutex> scopelock(lock);
			_queue.push(std::forward<_Uy>(item));
		}

		void pop(_Ty& ret)
		{
			LockSecScope scopelock(csec.cs());
			//std::lock_guard<std::mutex> scopelock(lock);
			if (_queue.empty())
				return;

			ret = std::move(_queue.front());
			_queue.pop();
		}

	private:
		CriticalSection csec;
		//std::mutex lock;
		std::queue<_Ty> _queue;
	};

public:
	static void* pop(const std::size_t& size);
	static void push(void* ptr, const std::size_t& size);

	static void Monitor();

private:
	static void _clear_all();

private:
	inline static std::unordered_map<std::size_t, std::atomic<std::size_t>> _monitor;
	inline static raw_lock_queue<void*> _pool[MAX_ARR_SIZE];
};