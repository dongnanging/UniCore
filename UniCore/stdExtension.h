#pragma once

namespace stdex
{
	//const ty����
	//const ����
	template<typename _Ty> struct remove_all_const : std::remove_const<_Ty> {};

	//ptr const *  �ϰ��
	//ptr const, * �и� ��
	//ptr const���� const���� ��
	//type* �� * ���� => ptr*
	template<typename _Ty> struct remove_all_const<_Ty*> {
		typedef typename remove_all_const<_Ty>::type* type;
	};

	//ptr const * const �� ���
	template<typename _Ty> struct remove_all_const<_Ty* const> {
		typedef typename remove_all_const<_Ty>::type* type;
	};

	template<typename _Ty>
	struct perfect_remove_volatile : std::remove_volatile<_Ty>
	{
	};
	template<typename _Ty>
	struct perfect_remove_volatile<_Ty*>
	{
		typedef typename std::remove_volatile<_Ty>::type* type;
	};
	template<typename _Ty>
	struct perfect_remove_volatile<volatile _Ty*>
	{
		typedef typename perfect_remove_volatile<_Ty*>::type type;
	};


	template <typename _Ty>
	struct pure_type
	{
		typedef  typename perfect_remove_volatile<
							typename remove_all_const<
									typename std::remove_reference<_Ty>::type
							>::type
				>::type type;
	};


	template<typename _Ty, std::size_t N>
	struct static_arr_size
	{
		static constexpr std::size_t value = N;
	};


	// =================================
	// types
	// =================================
	template<typename _Ty>
	using remove_all_const_t = typename remove_all_const<_Ty>::type;

	template<typename _Ty>
	using pure_type_t = pure_type<_Ty>::type;


	// =================================
	// values
	// =================================
	template<typename _Ty>
	static constexpr std::size_t static_arr_size = static_arr_size<_Ty>::value;


	template<typename... _Args>
	void safe_sprintf(std::string& buffer, const char* format, _Args&&... args)
	{

		auto size = snprintf(nullptr, 0, format, std::forward<_Args>(args)...);
		buffer.resize(size);

		snprintf(&buffer[0], size, format, std::forward<_Args>(args)...);
	}
}