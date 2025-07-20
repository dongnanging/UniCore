#pragma once
/*===========================
	Head Definition Macro
============================*/
#define __LOCK_MUTEX

#define __THREAD_ECHO
#define __USE_LOCKFREE
#define __SQL_ADO



#if defined(__USE_LOCKFREE)
#include <boost/lockfree/queue.hpp>
#endif


/*==========================
*	Lock Policy
============================*/
//TODO �⺻ mutex�� csec�̶�� �ϴ��� ����Ŭ������ ����ϰ� �����ϴ°� ���ƺ��δ�.
#if defined(__LOCK_MUTEX)	//�⺻ mutex
#include <mutex>
#define LOCK_TYPE std::recursive_mutex
#define LOCK_LAII std::lock_guard<LOCK_TYPE>
#define USE_MULT_LOCK(count) mutable LOCK_TYPE _locks[count]
#define READ_LOCK_IDX(idx) LOCK_LAII read_lock_guard_##idx(_locks[idx])
#define WRITE_LOCK_IDX(idx) LOCK_LAII write_lock_guard_##idx(_locks[idx])
#define GET_LOCK_IDX(idx) _locks[idx]


#elif defined(__LOCK_CSEC)	//critical section

#elif defined(__LOCK_CUSTOM)//custom lock
#include "RWLock.h"
#define USE_MULT_LOCK(count) RWLock _locks[count]
#define READ_LOCK_IDX(idx) ReadLock read_lock_guard_##idx(_locks[idx])
#define WRITE_LOCK_IDX(idx) WriteLock write_lock_guard_##idx(_locks[idx])

#endif
//���� �κ��� �׳� ������ �� ������ �Ʒ��� ������
#define USE_LOCK USE_MULT_LOCK(1)
#define READ_LOCK READ_LOCK_IDX(0)
#define WRITE_LOCK WRITE_LOCK_IDX(0)
#define GET_LOCK GET_LOCK_IDX(0)

#define LOCK_WITH(lock_item, name) LOCK_LAII lock_instance_##name(lock_item);



/*==========================
*	Thread
============================*/
#define THREAD_SLEEP_TIME(time_ms) std::this_thread::sleep_for(std::chrono::milliseconds(static_cast<long long>(time_ms)))

#if defined(__THREAD_ECHO)
#define THREAD_SLEEP THREAD_SLEEP_TIME(1)
//#elif defined(__THREAD_PERFORMANCE)
#else
#define THREAD_SLEEP std::this_thread::yield()
#endif



/*==========================
*	LOG
============================*/
#ifdef _DEBUG
#define file_log_dest stdout
#else
#define file_log_dest
#endif
template <typename _Ty> concept is_character = stdex::is_convertible_ctype_char_v<_Ty>;
template <typename _Ty> concept is_wide_character = stdex::is_convertible_ctype_wchar_v<_Ty>;

struct variadic_format_string
{
	//��� �׸��� wchar_t �� �ƴϰ�, format�� const char* ���� ��ȯ ����
	template<typename _Format, typename... _Args, typename = std::enable_if_t<!stdex::_or_v<stdex::is_convertible_ctype_wchar<_Args>...>>>
	requires is_character<_Format>
	static inline void format_print(_Format&& form_str, _Args&&... args)
	{
		try
		{
			std::fprintf(file_log_dest, stdex::sprintf_buffer(stdex::ctype_traits<_Format>::ctype(std::forward<_Format>(form_str)), stdex::sprintf_traits<_Args>::safe_type(std::forward<_Args>(args))...).c_str());
			std::fprintf(file_log_dest, "\n");
		}
		catch (...)
		{
			std::fprintf(file_log_dest, "failed write log.");
		}
	}

	// �ϳ��� wchar_t �� ����
	template<typename _Format, typename... _Args, typename = std::enable_if_t<stdex::_or_v<stdex::is_convertible_ctype_wchar<_Args>...>>>
	static inline void format_print(_Format&& form_str, _Args&&... args)
	{
		try
		{
			auto format_string = stdex::ctype_traits<_Format>::ctype(std::forward<_Format>(form_str));

			//const wchar_t* �� �ƴ϶� const char* �̸�
			if constexpr (stdex::is_convertible_ctype_char_v<decltype(format_string)>)
			{
				auto wide_format_string = stdex::ctype_traits<std::wstring>::ctype(stdex::str_elem<std::wstring>::ascii(format_string));
				std::fwprintf(file_log_dest, stdex::sprintf_buffer(wide_format_string, stdex::sprintf_traits<_Args>::safe_type(std::forward<_Args>(args))...).c_str());
			}	
			else
				std::fwprintf(file_log_dest, stdex::sprintf_buffer(format_string, stdex::sprintf_traits<_Args>::safe_type(std::forward<_Args>(args))...).c_str());

			
			std::fwprintf(file_log_dest, L"\n");
		}
		catch (...)
		{
			std::fprintf(file_log_dest, "failed write log.");
		}
	}
};


#define OUT			//reference�� ���޵Ǿ� ����� ����� ������ �Ķ���Ϳ� ���
#define PARAM_INPUT
#define PARAM_OUTPUT

// ��� ���� ��'\' �ڿ� ���� ���ڸ� �ڵ����� escape sequence�� �ν��Ϸ��� ���� ����
#pragma warning( disable : 4129 ) 

#ifdef _DEBUG
#define DEBUG_CODE(cpp_std_cout) cpp_std_cout
#define FILE_LOG(source_string, ...) variadic_format_string::format_print(source_string, __VA_ARGS__);
//�������� Ȯ��� �߸� __FUNCTION__ �� �׳� �� string���� ���̴µ� ��Ÿ�ӿ� ����ϸ� �� ����
#define ASSERT_LOG(msg, ...) FILE_LOG(msg, __VA_ARGS__);\
FILE_LOG("FILE::%s\mLINE::%s, %d", __FILE__, __FUNCTION__, __LINE__)
#else
#define DEBUG_CODE(print_line)
#define FILE_LOG(source_string, ...)
#define ASSERT_LOG(msg)
#endif



/*==========================
*	Network
============================*/
#define PORT_BOUNDARY 40000 //no use under this


/*==========================
*	Error Handle
============================*/
#define CRASH(cause, ...) \
{ \
	ASSERT_LOG(cause, __VA_ARGS__); \
	std::exit(1);\
}
//uint32* crash = nullptr; __analysis_assume(crash != nullptr); *crash = 0xDEADBEEF; \

#if defined(_DEBUG)		//����� �߿� ������ CRASH!!
#define DEBUG_ASSERT(expr, msg, ...) 	\
{									\
	if(!(expr))						\
		CRASH(msg, __VA_ARGS__);\
}
#else	//File Log���⵵�� �������� => traceback
#define DEBUG_ASSERT(expr, msg)	FILE_LOG();\				
#endif

#define DYNAMIC_ASSERT(expr, msg, ...)	\
{									\
	DEBUG_ASSERT(expr, msg, __VA_ARGS__);\
}