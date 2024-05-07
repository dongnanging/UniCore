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
	// bool - has
	// =================================
	template<typename _Ty, typename _Has>
	struct has_type {
	private:
		// T가 V를 가진다면 이쪽 -> auto = true_type;
		template<typename _X>
		static auto test(typename _X::_Has*) -> std::true_type;

		// 아니면 이쪽 전부(...) -> auto = false_type
		template<typename>
		static auto test(...) -> std::false_type;

	public:
		static constexpr bool value = decltype(test<_Ty>(nullptr))::value;
	};

	template<typename _Ty, typename _Has>
	static constexpr bool has_type_t = has_type<_Ty, _Has>::value;


	template<typename _Ty, typename _Drain = void>
	struct has_reverse_iterator : std::false_type {};

	template<typename _Ty>
	struct has_reverse_iterator<_Ty, typename substitution_helper<typename _Ty::reverse_iterator>::type>
		: std::true_type {};
	
	template<typename _Ty>
	static constexpr bool has_reverse_iterator_v = has_reverse_iterator<_Ty>::value;

	// =================================
	// bool - is
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

	template<typename _Ty>
	struct remove_all_pointer : std::remove_pointer<_Ty> {};

	template<typename _Ty>
	struct remove_all_pointer<_Ty*> : remove_all_pointer<_Ty> {};


	template <typename _Ty, bool _RemovePtr = false>
	struct pure_type
	{
		typedef  typename remove_volatile<
							typename remove_all_const<
									typename std::remove_reference<_Ty>::type
							>::type
				>::type type;
	};

	template <typename _Ty>
	struct pure_type<_Ty, true> : remove_all_pointer<pure_type<_Ty>> {};


	template<typename _Ty, std::size_t N>
	struct static_arr_size
	{
		static constexpr std::size_t value = N;
	};


	
	template<typename _Ty>
	using remove_all_const_t = typename remove_all_const<_Ty>::type;

	template<typename _Ty, bool _RemovePtr = false>
	using pure_type_t = pure_type<_Ty, _RemovePtr>::type;


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


	struct view_base {};

	template<typename _Ty>
	struct forward_view_types : public view_base
	{

	};
	template<typename _Ty>
	struct reverse_view_types : public view_base
	{
	public:
		using reverse_iterator = typename _Ty::reverse_iterator;
	};

	template<	typename _Ty>
	struct view : public std::conditional_t<has_reverse_iterator_v<_Ty>, reverse_view_types<_Ty>, forward_view_types<_Ty>>
	{
	protected:
		mutable _Ty* _view_ptr;

	public:
		//이거 정의해 둬야 view에 view 넣기 가능
		using iterator = typename _Ty::iterator;
		using const_iterator = typename _Ty::const_iterator;
		using iterator_category = typename _Ty::iterator::iterator_category;

		using iterator_type = iterator;

	public:
		template<typename = substitution_helper_t<
					decltype(std::declval<_Ty>().begin()),
					decltype(std::declval<_Ty>().end()),
					typename _Ty::iterator,
					typename _Ty::const_iterator
			>>
		view(_Ty& container)
			: _view_ptr(&container)
		{}

	public:
		virtual iterator begin() const noexcept { return _view_ptr->begin(); }
		virtual iterator end() const noexcept { return _view_ptr->end(); }
		virtual std::size_t size() const noexcept { return _view_ptr->size(); }
		virtual bool empty() const noexcept { return _view_ptr->empty(); }
	};
	
	template<typename _Ty>
	view(_Ty) -> view<_Ty>;

	                                             
	template<	typename _Ty>
	struct reverse_view : public view_base
	{
	protected:
		mutable _Ty* _view_ptr;

	public:
		using iterator = typename _Ty::iterator;
		using const_iterator = typename _Ty::const_iterator;
		using reverse_iterator = typename _Ty::reverse_iterator;
		using iterator_category = typename _Ty::iterator::iterator_category;

		using iterator_type = reverse_iterator;

	public:
		template<typename = substitution_helper_t<
					decltype(std::declval<_Ty>().begin()),
					decltype(std::declval<_Ty>().end()),
					typename _Ty::iterator,
					typename _Ty::const_iterator,
					typename _Ty::reverse_iterator
			>>
		reverse_view(_Ty& container)
			: _view_ptr(&container)
		{}

	public:
		virtual reverse_iterator begin() const noexcept { return reverse_iterator(_view_ptr->end()); }
		virtual reverse_iterator end() const noexcept { return reverse_iterator(_view_ptr->begin()); }
		virtual std::size_t size() const noexcept { return _view_ptr->size(); }
		virtual bool empty() const noexcept { return _view_ptr->empty(); }
	};

	template<typename _Ty>
	reverse_view(_Ty) -> reverse_view<_Ty>;

	
	template<typename _Shared>
	struct shared_ptr_traits
	{
		template<typename _Ty>
		static inline std::weak_ptr<_Ty> get_weak(const std::shared_ptr<_Ty>& shared_ptr)
		{
			return std::weak_ptr<_Ty>(shared_ptr);
		}

		template<typename _Ty, typename = substitution_helper_t<decltype(std::declval<_Shared>().make_weak())>>
		static inline decltype(auto) get_weak(const _Shared& shared_ptr)
		{
			return shared_ptr.make_weak();
		}

		template<typename _Weak, typename = substitution_helper_t<decltype(std::declval<_Weak>().lock())>>
		static inline decltype(auto) lock(_Weak&& ptr)
		{
			return ptr.lock();
		}
	};

	template<typename _Ty, typename _Shared, template<typename> class _ViewType>
	struct owned_view : _ViewType<_Ty>
	{
	protected:
		mutable _Shared _shared;

		using _View = _ViewType<_Ty>;

	public:
		using iterator = typename _View::iterator;
		using const_iterator = typename _View::const_iterator;
		using iterator_category = typename _View::iterator::iterator_category;

		using iterator_type = _View::iterator_type;

	public:
		owned_view(_Ty& container, _Shared&& shared)
			: _View(container)
		{
			_shared = std::move(shared);
		}

		bool alive() { return _shared != nullptr; }

		virtual typename _View::iterator_type begin() const noexcept override { if (!_shared) return _View::iterator_type(); return _View::begin(); }
		virtual typename _View::iterator_type end() const noexcept override { if (!_shared) return _View::iterator_type(); return _View::end(); }
		virtual std::size_t size() const noexcept override { if (!_shared) return 0; return _View::size(); }
		virtual bool empty() const noexcept override { if (!_shared) return true; return _View::empty(); }
	};

	template<typename _Ty, typename _Shared>
	struct shared_view
	{
	protected:
		mutable bool _released;
		mutable _Ty* _view_ptr;
		mutable decltype(shared_ptr_traits<_Shared>::get_weak(std::declval<_Shared>())) _weak_ptr;
	public:
		shared_view(_Ty& container, const _Shared& origin)
			: _view_ptr(&container), _released(false)
		{
			_weak_ptr = shared_ptr_traits<_Shared>::get_weak(origin);
		}

		template<template<typename> class _View = stdex::view>
		owned_view<_Ty, _Shared, _View> view() const noexcept
		{
			if(_released)
				return owned_view<_Ty, _Shared, _View>(*_view_ptr, nullptr);

			auto lock = shared_ptr_traits<_Shared>::lock(_weak_ptr);
			if (!lock)
			{
				_weak_ptr.reset(); //release control block;
				_released = true;
				return owned_view<_Ty, _Shared, _View>(*_view_ptr, nullptr);
			}

			return owned_view<_Ty, _Shared, _View>(*_view_ptr, std::move(lock));
		}
	};

	//이거 없어도 어차피 자동으로 deduction 됨
	template<typename _Ty, typename _Shared>
	shared_view(_Ty, _Shared) -> shared_view<_Ty, _Shared>;


	template<class _LockRAII>
	struct lock_share
	{
	private:
		std::function<_LockRAII()> _get_lock;
		std::function<_LockRAII*()> _new_lock;

	public:
		template<	typename... _Args,
					typename = std::enable_if_t<std::is_constructible_v<_LockRAII, _Args...>>>
		lock_share(_Args&&... args)
		{
			if constexpr (sizeof...(_Args) > 0)
			{
				std::tuple<_Args...> pack(std::forward<_Args>(args)...);
				_get_lock = [pack]() {
					return std::apply([](auto&&... pargs) {
						return _LockRAII(std::forward<_Args>(pargs)...);
						}, pack);

					};
				_new_lock = [pack]() {
					return std::apply([](auto&&... pargs) {
						return new _LockRAII(std::forward<_Args>(pargs)...);
						}, pack);

					};
			}
			else
			{
				_get_lock = []() {
						return _LockRAII();
					};

				_new_lock = []() {
					return new _LockRAII();
					};
			}
			
		}

		_LockRAII get_lock() const noexcept
		{
			// 무조건 우측값으로 줘야됨
			return _get_lock();
		}

		_LockRAII* new_lock() const noexcept
		{
			// 무조건 우측값으로 줘야됨
			return _new_lock();
		}
	};

	template<class _LockRAII>
	lock_share(_LockRAII) -> lock_share<std::lock_guard<_LockRAII>>;


	template<typename _Ty, typename _Shared, typename _LockRAII, template<typename> class _ViewType>
	struct lock_owned_view : public owned_view<_Ty, _Shared, _ViewType>
	{
	protected:
		_LockRAII* _lock;

	public:
		using iterator = typename owned_view<_Ty, _Shared, _ViewType>::iterator;
		using const_iterator = typename owned_view<_Ty, _Shared, _ViewType>::const_iterator;
		using iterator_category = typename owned_view<_Ty, _Shared, _ViewType>::iterator::iterator_category;

		using iterator_type = owned_view<_Ty, _Shared, _ViewType>::iterator_type;

	public:
		lock_owned_view(_Ty& container, _Shared&& shared, const lock_share<_LockRAII>& share)
			: owned_view<_Ty, _Shared, _ViewType>(container, std::forward<_Shared>(shared))
		{
			_lock = share.new_lock();
		}

		lock_owned_view(_Ty& container, _Shared&& shared)
			: owned_view<_Ty, _Shared, _ViewType>(container, std::forward<_Shared>(shared)), _lock(nullptr)
		{}

		~lock_owned_view()
		{
			if (_lock)
				delete _lock;
		}


		lock_owned_view(const lock_owned_view& other) = delete;
		lock_owned_view(lock_owned_view&& other) = delete;
		lock_owned_view& operator=(const lock_owned_view& other) = delete;
		lock_owned_view& operator=(lock_owned_view&& other) = delete;
	};

	template<typename _Ty, typename _Shared, typename _LockRAII>
	struct lock_view
	{
	protected:
		mutable lock_share<_LockRAII> _share;
		mutable bool _released;
		mutable _Ty* _view_ptr;
		mutable decltype(shared_ptr_traits<_Shared>::get_weak(std::declval<_Shared>())) _weak_ptr;

	public:
		lock_view(_Ty& container, const _Shared& origin, lock_share<_LockRAII> share)
			: _view_ptr(&container), _weak_ptr(origin), _share(share), _released(false)
		{}

		template<template<typename> class _ViewType = stdex::view>
		auto scope_lock_view() const noexcept
		{
			if (_released)
				return lock_owned_view<_Ty, _Shared, _LockRAII, _ViewType>(*_view_ptr, nullptr);

			auto lock = shared_ptr_traits<_Shared>::lock(_weak_ptr);
			if (!lock)
			{
				_weak_ptr.reset(); //release control block;
				_released = true;
				return lock_owned_view<_Ty, _Shared, _LockRAII, _ViewType>(*_view_ptr, nullptr);
			}

			return lock_owned_view<_Ty, _Shared, _LockRAII, _ViewType>(*_view_ptr, std::move(lock), _share);
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