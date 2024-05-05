#pragma once

namespace stdex
{
	template<typename... _Substitutions>
	struct substitution_helper 
	{
		using type = void;
	};

	template<typename... _Substitutions>
	using substitution_helper_t = substitution_helper<_Substitutions...>;
	
	// =================================
	// bool
	// =================================
	template<typename _Val>
	struct _not
	{
		static constexpr bool value = (!_Val::value);
	};

	template<typename... _Inspectors>
	struct _or
	{
		static constexpr bool value = std::disjunction<_Inspectors...>::value;
	};

	template<typename... _Inspectors>
	struct _and
	{
		static constexpr bool value = std::conjunction<_Inspectors...>::value;
	};


	template<typename _Val>
	static constexpr bool _not_v = _not<_Val>::value;

	template<typename... _Inspectors>
	static constexpr bool _or_v = _or<_Inspectors...>::value;

	template<typename... _Inspectors>
	static constexpr bool _and_v = _and<_Inspectors...>::value;


	template<typename _Ty, typename _Drain = void>
	struct is_stl_container : std::false_type {};

	//1개짜리면 무조건 먼저 빨려옴
	template<typename _Ty>
	struct is_stl_container<typename _Ty, typename substitution_helper<
									typename _Ty::value_type,
									typename _Ty::size_type,
									typename _Ty::allocator_type,
									typename _Ty::iterator,
									typename _Ty::const_iterator,
									decltype(std::declval<_Ty>().size()),	//has size()
									decltype(std::declval<_Ty>().begin()),	//has being()
									decltype(std::declval<_Ty>().end()),	//has end()
									decltype(std::declval<_Ty>().cbegin()),	//has cbeing()
									decltype(std::declval<_Ty>().cend())	//has cend()
								>::type> : std::true_type {};


	template<typename _Ty>
	static constexpr bool is_stl_container_v = is_stl_container<_Ty>::value;


	// =================================
	// types
	// =================================
	//const ty에서
	//const 제거
	template<typename _Ty> struct remove_all_const : std::remove_const<_Ty> {};

	//ptr const *  일경우
	//ptr const, * 분리 후
	//ptr const에서 const제거 후
	//type* 로 * 부착 => ptr*
	template<typename _Ty> struct remove_all_const<_Ty*> {
		typedef typename remove_all_const<_Ty>::type* type;
	};

	//ptr const * const 일 경우
	template<typename _Ty> struct remove_all_const<_Ty* const> {
		typedef typename remove_all_const<_Ty>::type* type;
	};

	template<typename _Ty>
	struct remove_volatile : std::remove_volatile<_Ty>
	{
	};
	template<typename _Ty>
	struct remove_volatile<_Ty*>
	{
		typedef typename std::remove_volatile<_Ty>::type* type;
	};
	template<typename _Ty>
	struct remove_volatile<volatile _Ty*>
	{
		typedef typename remove_volatile<_Ty*>::type type;
	};


	template <typename _Ty>
	struct pure_type
	{
		typedef  typename remove_volatile<
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


	
	template<typename _Ty>
	using remove_all_const_t = typename remove_all_const<_Ty>::type;

	template<typename _Ty>
	using pure_type_t = pure_type<_Ty>::type;


	// =================================
	// values
	// =================================
	template<typename _Ty>
	static constexpr std::size_t static_arr_size = static_arr_size<_Ty>::value;

	// =================================
	// ctype
	// =================================
	template<typename _Ty>
	struct is_char_str
	{
		using pure_t = typename pure_type_t<_Ty>;
		static constexpr bool value =	std::is_same_v<pure_t, char*> ||
										_and_v<std::is_array<pure_t>, std::is_convertible<pure_t, const char*>>;
	};

	template<typename _Ty>
	struct is_wchar_str
	{
		using pure_t = typename pure_type_t<_Ty>;
		static constexpr bool value =	std::is_same_v<pure_t, wchar_t*> ||
										_and_v<std::is_array<pure_t>, std::is_convertible<pure_t, const wchar_t*>>;
	};

	template<typename _Ty>
	struct is_string_stl
	{
		using pure_t = typename pure_type_t<_Ty>;
		static constexpr bool value =	std::is_same_v<pure_t, std::string>	 ||
										std::is_same_v<pure_t, std::wstring>;
	};

	template<typename _Ty>
	static constexpr bool is_char_str_v = is_char_str<_Ty>::value;

	template<typename _Ty>
	static constexpr bool is_wchar_str_v = is_wchar_str<_Ty>::value;

	template<typename _Ty>
	static constexpr bool is_string_stl_v = is_string_stl<_Ty>::value;


	template<typename _Ty>
	struct is_string
	{
		using pure_t = typename pure_type_t<_Ty>;
		static constexpr bool value =	is_char_str_v<_Ty>		|| 
										is_wchar_str_v<_Ty>		||
										is_string_stl_v<_Ty>;
	};

	template<typename _Ty>
	static constexpr bool is_string_v = is_string<_Ty>::value;

	template<typename _CppType>
	struct ctype_traits
	{
		using pure_t = pure_type_t<_CppType>;

		template<	typename _Ty, 
					typename = std::enable_if_t<std::is_same_v<pure_t, pure_type_t<_Ty>>>,
					typename = std::enable_if_t<is_string_stl_v<pure_t>>
		>
		decltype(auto) ctype(_Ty&& item)
		{
			return item.c_str();		// const char* / const wchar_t*
		}


		template<	typename _Ty, 
					typename = std::enable_if_t<std::is_same_v<pure_t, pure_type_t<_Ty>>>
		>
		std::enable_if_t<is_char_str_v<_Ty>, 
			const char*> ctype(_Ty&& item)
		{
			return item;
		}

		template<	typename _Ty,
			typename = std::enable_if_t<std::is_same_v<pure_t, pure_type_t<_Ty>>>
		>
		std::enable_if_t<is_wchar_str_v<_Ty>,
			const wchar_t*> ctype(_Ty&& item)
		{
			return item;
		}

		template<	typename _Ty, 
					typename = std::enable_if_t<std::is_same_v<pure_t, pure_type_t<_Ty>>>,
					typename = std::enable_if_t<_not<is_string_v<pure_t>>>					//	문자열 아님
			>
		std::enable_if_t<std::is_fundamental_v<pure_t>,		// 기본 타입. (포인터 제외)
			_Ty> ctype(_Ty&& item)
		{
			return item;
		}

		template<	typename _Ty, 
					typename = std::enable_if_t<std::is_same_v<pure_t, pure_type_t<_Ty>>>,
					typename = std::enable_if_t<_not<is_string_v<pure_t>>>					//	문자열 아님
			>
		std::enable_if_t<std::is_pointer_v<pure_t>,			// 포인터?
			_Ty> ctype(_Ty&& item)
		{
			// recursive

			// 껍질 깐다~
			return ctype_traits<*_Ty>::ctype(*item);
		}
	};


	template<typename _Type>
	struct sprintf_traits
	{
		using pure_t = typename pure_type_t<_Type>;
	};
	
	// =================================
	// view
	// =================================

	template<	typename _Ty
				/*typename = substitution_helper_t<
					decltype(std::declval<_Ty>().begin()),
					decltype(std::declval<_Ty>().end()),
					typename _Ty::iterator,
					typename _Ty::const_iterator
		>*/>
	struct view_base
	{
	protected:
		_Ty* _container;
		using iterator = _Ty::iterator;

	public:
		view_base(_Ty& container)
			: _container(&container)
		{}

	public:
		auto begin() { return _container->begin(); }
		auto end() { return _container->end(); }
		auto size() { return _container->size(); }
		auto empty() { return _container->end(); }
	};
	                                             
	template<typename _Ty>
	struct reverse_view : public view_base<_Ty>
	{
	public:
		reverse_view(_Ty& container)
			: view_base<_Ty>::_container(&container)
		{}

		auto begin() { return view_base<_Ty>::_container->rbegin(); }
		auto end() { return view_base<_Ty>::_container->rend(); }
	};

	
	template<template<typename> class _Shared>
	struct shared_ptr_traits
	{
		template<typename _Ty, typename = std::enable_if_t<std::is_same_v<_Shared, std::shared_ptr>>>
		static inline std::weak_ptr<_Ty> get_weak(const _Shared<_Ty>& shared_ptr)
		{
			return std::weak_ptr<_Ty>(shared_ptr);
		}

		template<typename _Ty, typename = substitution_helper_t<decltype(std::declva<_Shared<_Ty>>().make_weak())>>
		static inline decltype(auto) get_weak(const _Shared<_Ty>& shared_ptr)
		{
			return shared_ptr.make_weak();
		}

		template<typename _Ty, typename _Weak, typename = substitution_helper_t<decltype(std::declval<_Weak>().lock())>>
		static inline decltype(auto) lock(_Weak&& ptr)
		{
			return ptr.lock();
		}
	};

	template<typename _Ty, typename _Shared>
	struct shared_view : public view_base<_Ty>
	{
	public:
		using intrude_func = std::function<void(const view_base<_Ty>&)>;

		std::function<void()> _intrude;
		std::function<void()> _callable;

	public:
		template<typename _Ty>
		shared_view(_Ty& container, const _Shared& origin)
			: view_base<_Ty>(container)
		{
			auto weak = shared_ptr_traits<_Shared>::get_weak(std::forward<_Shared>(origin));
			_intrude = [this, weak_validator = std::move(weak)]() {
				auto alive = shared_ptr_traits<decltype(weak_validator)>::lock(weak_validator);

				if (!alive)
					return;

				_callable();
				};
		}

		template<typename _Callable, typename = substitution_helper_t<decltype(std::declval<_Callable>().operator ())>>
		void view_when_valid(_Callable&& intrude)
		{
			_callable = [_call = std::forward<_Callable>(intrude)]() {
				_call();
				};

			_intrude();
		}


	protected:
		auto begin() = delete;
		auto end() = delete;
		auto size() = delete;
		auto empty() = delete;
	};

	template<class _LockRAII>
	struct lock_traits
	{
	private:
		std::function<_LockRAII()> _get_lcok;

	public:
		template<	typename... _Args,
					typename = std::enable_if_t<std::is_constructible_v<_LockRAII, _Args...>>>
		lock_traits(_Args&&... args)
		{
			if constexpr (sizeof...(_Args) > 1)
			{
				_get_lcok = [pack = std::make_tuple(std::forward<_Args>(args)...)]() {
					return std::apply([](auto&&... pargs) {
						return _LockRAII(std::forward<_Args>(pargs)...);
						}, pack);

					};
			}
			else if constexpr(sizeof...(_Args) == 1)
			{
				_get_lcok = [pack = std::tuple<_Args...>(std::forward<_Args>(args)...)]() {
					return std::apply([](auto&&... pargs) {
						return _LockRAII(std::forward<_Args>(pargs)...);
						}, pack);

					};
			}
			else
			{
				_get_lcok = []() {
						return _LockRAII();
					};
			}
			
		}

		_LockRAII Lock()
		{
			// 무조건 우측값으로 줘야됨
			return _get_lcok();
		}
	};

	// =================================
	// func
	// =================================
	template<typename... _Args>
	static inline void snprintf(std::string& buffer, const char* format, _Args&&... args)
	{
		auto size = snprintf(nullptr, 0, format, std::forward<_Args>(args)...);

		// 쓸때 무조건 '\0' 까지 씀. 만약 자리가 부족하면 마지막 문자열 제거하고 \0이 들어가버림
		size++;
		buffer.resize(size);
		snprintf(&buffer[0], size, format, ctype_traits<_Args>::ctype(std::forward<_Args>(args))...);

		buffer.pop_back();	// \0제거
	}

	template<typename... _Args>
	static inline std::string snprintf_buffer(const char* format, _Args&&... args)
	{
		std::string buffer;
		stdex::snprintf(buffer, format, std::forward<_Args>(args)...);
		return buffer;
	}

	template<typename... _Args>
	static inline void snprintf(std::wstring& buffer, const wchar_t* format, _Args&&... args)
	{
		auto size = swprintf(nullptr, 0, format, std::forward<_Args>(args)...);

		// 쓸때 무조건 '\0' 까지 씀. 만약 자리가 부족하면 마지막 문자열 제거하고 \0이 들어가버림
		size++;
		buffer.resize(size);
		swprintf(&buffer[0], size, format, std::forward<_Args>(args)...);

		buffer.pop_back(); // \0 제거
	}

	template<typename... _Args>
	static inline std::wstring snprintf_buffer(const wchar_t* format, _Args&&... args)
	{
		std::wstring buffer;
		stdex::snprintf(buffer, format, std::forward<_Args>(args)...);
		return buffer;
	}



	// =================================
	// converter
	// =================================
	static inline std::string wstring_to_string(const std::wstring& source)
	{
		return std::string().assign(source.begin(), source.end());
	}

	static inline std::wstring string_to_wstring(const std::string& source)
	{
		return std::wstring().assign(source.begin(), source.end());
	}
}