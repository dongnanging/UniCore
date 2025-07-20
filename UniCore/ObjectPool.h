#pragma once
namespace objpool
{
	struct NoPlacemnetNew {};
}

template<typename _Ty>
struct ObjectAllocator
{
	static constexpr auto item_size = sizeof(_Ty);

	inline static void* alloc()
	{
		return MemoryPool::pop(item_size);
	}

	inline static void free(void* _ptr)
	{
		MemoryPool::push(_ptr, item_size);
	}
};


template<typename _Ty>
struct DefaultAllocator {
	// TODO 동시 접근을 개선할 수 있을까?
	static inline std::queue<_Ty*> _queue;
	static inline CriticalSection _csec;

	static constexpr auto _item_size = sizeof(_Ty);

	inline static _Ty* alloc()
	{
		do {
			LockSecScope lock(_csec.cs());
			if (_queue.empty())
				break;

			auto ret = _queue.front();
			_queue.pop();
			return ret;
		} while (false);
		
		return static_cast<_Ty*>(::malloc(_item_size));
	}

	inline static void free(_Ty* _ptr)
	{
		if (!_ptr)
			return;

		LockSecScope lock(_csec.cs());
		_queue.emplace(_ptr);
	}

	template<typename... _Args>
	inline static _Ty* noplacement_alloc(_Args&&... args)
	{
		_Ty* memory = nullptr;
		do {
			LockSecScope lock(_csec.cs());
			if (_queue.empty())
				break;

			auto memory = _queue.front();
			_queue.pop();
			return memory;
		} while (false);

		if (!memory)
		{
			return new _Ty(std::forward<_Args>(args)...);
		}
		else
		{
			memory->Init(std::forward<_Args>(args)...);
			return memory;
		}
	}

	inline static void noplacement_free(_Ty* _ptr)
	{
		if (!_ptr)
			return;

		_ptr->Destroy();
		free(_ptr);
	}
};

template<typename _Type, typename _Alloc = DefaultAllocator<_Type>>
class ObjectPool
{
public:
	template <typename... _Args>
	static std::shared_ptr<_Type> PoolShared(_Args&&... args) noexcept(
		std::is_nothrow_constructible_v<_Type, _Args...>) //생성자가 noexcept일 경우에만 noexcept
	{
		return std::shared_ptr<_Type>(Pop(std::forward<_Args>(args)...), Push);
	}

	template <typename... _Args>
	static _Type* Pop(_Args&&... args) 
		noexcept(std::is_nothrow_constructible_v<_Type, _Args>)
	{
		_Type* memory = nullptr;

		if constexpr (std::is_base_of_v<objpool::NoPlacemnetNew, stdex::pure_type_t<_Type>>)
		{
			memory = _Alloc::noplacement_alloc(std::forward<_Args>(args)...);
			return memory;
		}
		else 
		{
			memory = _Alloc::alloc();
			new(memory) _Type(std::forward<_Args>(args)...);
			return memory;
		}
	}
	static void Push(_Type* ptr)
		// 소멸자 noexcept or trivial 경우에만 noexcept (소멸자는 어지간하면 noexcept)
		noexcept(std::disjunction_v<std::is_trivially_destructible<_Type>, std::is_nothrow_destructible<_Type>>) 
	{
		if constexpr (std::is_base_of_v<objpool::NoPlacemnetNew, stdex::pure_type_t<_Type>>)
		{
			_Alloc::noplacement_free(ptr);
			return;
		}
		else 
		{
			//소멸자 호출
			ptr->~_Type();
			_Alloc::free(ptr);
		}
	}
};