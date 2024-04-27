#pragma once


template<typename _Type, typename... Args>
_Type* mutable_new(Args&&... args)
{
	//메모리만 할당(생성자 호출 x)
	_Type* memory = static_cast<_Type*>(custom_malloc(sizeof(_Type)));

	//정식 명칭은 placement new 라고 한다.
	new(memory)_Type(std::forward<Args>(args)...);

	return memory;
}

template<typename _Type>
void mutable_delete(_Type* memory)
{
	memory->~_Type();

	custom_free(memory);
}

template<typename _Type, typename... _Args>
inline static std::shared_ptr<_Type> J_MakeShared(_Args&&... args)
{
	//return std::shared_ptr<_Type>({ mutable_new<_Type>(std::forward<_Args>(args)...), mutable_delete<_Type>});
	//return std::make_shared<_Type>(std::forward<_Args>(args)...);
	return ObjectPool<_Type>::PoolShared(std::forward< _Args>(args)...);
}

template<typename _Type, typename... _Args>
inline static std::unique_ptr<_Type> J_MakeUnique(_Args&&... args)
{
	//return std::shared_ptr<_Type>({ mutable_new<_Type>(std::forward<_Args>(args)...), mutable_delete<_Type>});
	return std::make_unique<_Type>(std::forward<_Args>(args)...);
}

template<typename _Cast, typename _Raw>
inline auto unwrap_cast(const std::shared_ptr<_Raw>& raw)
{
	//#if defined(_DEBUG)
	//	return std::static_pointer_cast<_cast>(raw);
	//#else
	return reinterpret_cast<_Cast*>(raw.get());
	//#endif
}