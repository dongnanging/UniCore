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

	//construct  ����
	//std::allocate_traits::construct ���
	//destroy ����
	//std::allocate_traits::destroy ���

	//�⺻ �����ڴ� �ʼ��� ������ �־���Ѵ�.
	BaseAllocator() = default;
	//baseallocator�� �����ؾ��� ��Ҹ� ������ ����
	template<typename _Uy, typename = std::enable_if_t<std::is_same_v<_Ty, _Uy>, _Ty>>
	BaseAllocator(const BaseAllocator<_Uy>& other) noexcept {}

	template<typename _Uy, typename = std::enable_if_t<std::is_same_v<_Ty, _Uy>, _Ty>>
	bool operator==(const BaseAllocator<_Uy>& other) noexcept { return true; } //���� ���Ҹ� ������ ����
	template<typename _Uy, typename = std::enable_if_t<std::is_same_v<_Ty, _Uy>, _Ty>>
	bool operator!=(const BaseAllocator<_Uy>& other) noexcept { return false; } // �豳�� ���Ҹ� ������ ����
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

	//construct  ����
	//std::allocate_traits::construct ���
	//destroy ����
	//std::allocate_traits::destroy ���

	//�⺻ �����ڴ� �ʼ��� ������ �־���Ѵ�.
	PoolAllocator() = default;
	//PoolAllocator�� �����ؾ��� ��Ҹ� ������ ����
	template<typename _Uy>//, typename = std::enable_if_t<std::is_same_v<_Ty, _Uy>, _Ty>>
	PoolAllocator(const PoolAllocator<_Uy>& other) noexcept {}

	template<typename _Uy, typename = std::enable_if_t<std::is_same_v<_Ty, _Uy>, _Ty>>
	bool operator==(const PoolAllocator<_Uy>& other) noexcept { return true; } //���� ���Ҹ� ������ ����
	template<typename _Uy, typename = std::enable_if_t<std::is_same_v<_Ty, _Uy>, _Ty>>
	bool operator!=(const PoolAllocator<_Uy>& other) noexcept { return false; } // �豳�� ���Ҹ� ������ ����
};