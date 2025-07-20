#pragma once

template<typename _Ty>
struct BaseAllocator
{
	using value_type = _Ty;

	value_type* allocate(std::size_t n)
	{
		return static_cast<value_type*>(::malloc(n));
	}

	void deallocate(value_type* ptr, std::size_t n) noexcept
	{
		if (ptr)
			::free(ptr);
	}

	//construct  생략
	//std::allocate_traits::construct 사용
	//destroy 생략
	//std::allocate_traits::destroy 사용

	//기본 생성자는 필수로 가지고 있어야한다.
	BaseAllocator() = default;
	//baseallocator는 복사해야할 요소를 가지지 않음
	template<typename _Uy, typename = std::enable_if_t<std::is_same_v<_Ty, _Uy>, _Ty>>
	BaseAllocator(const BaseAllocator<_Uy>& other) noexcept {}

	template<typename _Uy, typename = std::enable_if_t<std::is_same_v<_Ty, _Uy>, _Ty>>
	bool operator==(const BaseAllocator<_Uy>& other) noexcept { return true; } //비교할 원소를 가지지 않음
	template<typename _Uy, typename = std::enable_if_t<std::is_same_v<_Ty, _Uy>, _Ty>>
	bool operator!=(const BaseAllocator<_Uy>& other) noexcept { return false; } // 배교할 원소를 가지지 않음
};

template<typename _Ty, typename = std::enable_if_t<!std::is_pointer_v<_Ty>>>
struct PoolAllocator
{
	using value_type = _Ty;

	value_type* allocate(std::size_t n)
	{
		if constexpr (std::is_fundamental_v<value_type>)
			return static_cast<value_type*>(::malloc(n));
		else
			return static_cast<value_type*>(MemoryPool::pop(n * sizeof(value_type)));
	}

	void deallocate(value_type* ptr, std::size_t n) noexcept
	{
		if constexpr (std::is_fundamental_v<value_type>)
		{
			if (ptr)
				::free(ptr);
		}
		else 
		{
			if (ptr)
				MemoryPool::push(ptr, n * sizeof(value_type));
		}
	}

	//construct  생략
	//std::allocate_traits::construct 사용
	//destroy 생략
	//std::allocate_traits::destroy 사용

	//기본 생성자는 필수로 가지고 있어야한다.
	PoolAllocator() = default;
	//PoolAllocator는 복사해야할 요소를 가지지 않음
	template<typename _Uy>//, typename = std::enable_if_t<std::is_same_v<_Ty, _Uy>, _Ty>>
	PoolAllocator(const PoolAllocator<_Uy>& other) noexcept {}

	template<typename _Uy, typename = std::enable_if_t<std::is_same_v<_Ty, _Uy>, _Ty>>
	bool operator==(const PoolAllocator<_Uy>& other) noexcept { return true; } //비교할 원소를 가지지 않음
	template<typename _Uy, typename = std::enable_if_t<std::is_same_v<_Ty, _Uy>, _Ty>>
	bool operator!=(const PoolAllocator<_Uy>& other) noexcept { return false; } // 배교할 원소를 가지지 않음
};