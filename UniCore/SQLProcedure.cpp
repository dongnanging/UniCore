#include "stdafx.h"
#include "SQLProcedure.h"


//template<typename Item>
//static inline void print(Item item)
//{
//	std::cout << item << std::endl;
//}
//
//template<std::size_t... _Seq, typename... Args>
//static inline void CompileTest_impl2(std::index_sequence<_Seq...>, std::tuple<Args&...>& tup)
//{
//	print(std::get<_Seq>(tup)...);
//}
//
//template<typename... Args>
//static inline void CompileTest_impl(std::tuple<Args&...>& tup)
//{
//	CompileTest_impl2(std::make_index_sequence<sizeof...(Args)>(), tup);
//}
//
//
//template<typename... Args1, typename... Args2>
//static inline void CompileTest(std::tuple<Args1&...> tup1, std::tuple<Args2&...> tup2)
//{
//	CompileTest_impl(tup1);
//	CompileTest_impl(tup2);
//}