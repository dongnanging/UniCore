#pragma once
namespace objpool
{
	class NoPlacemnetNew
	{
	};
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

template
<class _Type>
class ObjectPool
{
public:
	template <typename... _Args>
	static std::shared_ptr<_Type> PoolShared(_Args&&... args) noexcept(
		std::is_nothrow_constructible_v<_Type, _Args...>) //생성자가 noexcept일 경우에만 noexcept
	{
		return std::shared_ptr<_Type>(_Pop(std::forward<_Args>(args)...), _Push);
	}

private:
	//template<typename = std::enable_if_t<std::is_base_of_v<objpool::NoPlacemnetNew, std::pure_type_t<_Type>>, _Type>>
	//inline static MemoryPool::raw_lock_queue<_Type*> _queue{};

	template <typename... _Args>
	static _Type* _Pop(_Args&&... args) 
		noexcept(std::is_nothrow_constructible_v<_Type, _Args>)
	{
		_Type* memory = nullptr;

		/*if constexpr (std::is_base_of_v<objpool::NoPlacemnetNew, std::pure_type_t<_Type>>)
		{
			_queue.pop(memory);
			if (memory)
				return memory;

			memory = static_cast<_Type*>(::malloc(sizeof(_Type)));
			new(memory) _Type(std::forward<_Args>(args)...);
			return memory;
		}*/


		
		memory = static_cast<_Type*>(ObjectAllocator<_Type>::alloc());

		new(memory) _Type(std::forward<_Args>(args)...);

		return memory;
	}
	static void _Push(_Type* ptr)
		// 소멸자 noexcept or trivial 경우에만 noexcept (소멸자는 어지간하면 noexcept)
		noexcept(std::disjunction_v<std::is_trivially_destructible<_Type>, std::is_nothrow_destructible<_Type>>) 
	{
		/*if constexpr (std::is_base_of_v<objpool::NoPlacemnetNew, std::pure_type_t<_Type>>)
		{
			_queue.push(ptr);
			return;
		}*/


		//소멸자 호출
		ptr->~_Type();
			
		ObjectAllocator<_Type>::free(ptr);
	}
};