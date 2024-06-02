#pragma once
#include <cwctype>	//std::iswdigit

namespace stdex
{
	template<typename... _Substitutions>
	struct substitution_helper 
	{
		using type = void;
	};

	template<typename... _Substitutions>
	using substitution_helper_t = substitution_helper<_Substitutions...>;

	template<typename _Ty>
	struct always_failed {};
	
	template<typename _Ty>
	using always_failed_t = always_failed<_Ty>::type;

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

	
	template<typename _Ty>
	using remove_all_const_t = typename remove_all_const<_Ty>::type;

	template<typename _Ty, bool _RemovePtr = false>
	using pure_type_t = pure_type<_Ty, _RemovePtr>::type;


	// =================================
	// values 2
	// =================================
	template<typename _Left, typename _Right>
	struct is_same
	{
		static constexpr bool value = std::is_same_v<stdex::pure_type_t<_Left>, stdex::pure_type_t<_Right>>;
	};

	template<typename _Left, typename _Right>
	static constexpr bool is_same_v = is_same<_Left, _Right>::value;

	template<typename _Ty>
	struct is_array
	{
		static constexpr bool value = std::is_array_v<stdex::pure_type_t<_Ty>>;
	};

	template<typename _Ty>
	static constexpr bool is_array_v = is_array<_Ty>::value;


	// =================================
	// shared traits
	// =================================
	template<typename _Ty>
	struct unwrap_shared
	{
		using type = _Ty;
	};

	template<typename _Ty>
	struct unwrap_shared<std::shared_ptr<_Ty>>
	{
		using type = typename stdex::unwrap_shared<_Ty>::type;
	};

	template<typename _Ty>
	using unwrap_shared_t = typename stdex::unwrap_shared<_Ty>::type;

	// =================================
	// ctype
	// =================================

	template<typename _Ty>
	struct is_convertible_char
	{
		static constexpr bool value = _and_v<std::is_array<stdex::pure_type_t<_Ty>>, std::is_convertible<stdex::pure_type_t<_Ty>, const char*>>;
	};

	template<typename _Ty>
	struct is_convertible_wchar
	{
		static constexpr bool value = _and_v<std::is_array<stdex::pure_type_t<_Ty>>, std::is_convertible<stdex::pure_type_t<_Ty>, const wchar_t*>>;
	};

	template<typename _Ty>
	static constexpr bool is_convertible_ccptr_v = is_convertible_char<_Ty>::value;
	template<typename _Ty>
	static constexpr bool is_convertible_wccptr_v = is_convertible_wchar<_Ty>::value;

	template<typename _Ty>
	struct is_char_str
	{
		using pure_t = typename pure_type_t<_Ty>;
		static constexpr bool value =	std::is_same_v<pure_t, char*> ||
										is_convertible_ccptr_v<pure_t>;
	};

	template<typename _Ty>
	struct is_wchar_str
	{
		using pure_t = typename pure_type_t<_Ty>;
		static constexpr bool value =	std::is_same_v<pure_t, wchar_t*> ||
										is_convertible_wccptr_v<pure_t>;
	};

	template<typename _Ty>
	struct is_stl_string
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
	static constexpr bool is_stl_string_v = is_stl_string<_Ty>::value;


	template<typename _Ty>
	struct is_string
	{
		static constexpr bool value =	is_char_str_v<_Ty>		|| 
										is_wchar_str_v<_Ty>		||
										is_stl_string_v<_Ty>;
	};

	template<typename _Ty>
	static constexpr bool is_string_v = is_string<_Ty>::value;



	template<typename _Ty>
	struct is_convertible_ctype_char
	{
		using pure_t = typename pure_type_t<_Ty>;
		static constexpr bool value =	is_char_str_v<pure_t>		||
										std::is_same_v<pure_t, std::string>;
	};

	template<typename _Ty>
	struct is_convertible_ctype_wchar
	{
		using pure_t = typename pure_type_t<_Ty>;
		static constexpr bool value =	is_wchar_str_v<pure_t>		||
										std::is_same_v<pure_t, std::wstring>;
	};

	template<typename _Ty>
	static constexpr bool is_convertible_ctype_char_v = is_convertible_ctype_char<_Ty>::value;

	template<typename _Ty>
	static constexpr bool is_convertible_ctype_wchar_v = is_convertible_ctype_wchar<_Ty>::value;

	template<typename _Left, typename _Right>
	struct enable_same : std::enable_if<std::is_same_v<stdex::pure_type_t<_Left>, stdex::pure_type_t<_Right>>> {};

	template<typename _Left, typename _Right>
	using enable_same_t = enable_same<_Left, _Right>::type;


	template<typename _CppType>
	struct ctype_traits
	{
		using pure_t = pure_type_t<_CppType>;

		template<	typename _Ty, 
					typename = enable_same_t<_Ty, pure_t>,
					typename = std::enable_if_t<is_stl_string_v<pure_t>>
		>
		static constexpr decltype(auto) ctype(_Ty&& item)
		{
			return item.c_str();		// const char* / const wchar_t*
		}


		template<	typename _Ty, 
					typename = enable_same_t<_Ty, pure_t>
		>
		static constexpr std::enable_if_t<is_char_str_v<_Ty>,
			const char*> ctype(_Ty&& item)
		{
			return item;
		}

		template<	typename _Ty,
					typename = enable_same_t<_Ty, pure_t>
		>
		static constexpr std::enable_if_t<is_wchar_str_v<_Ty>,
			const wchar_t*> ctype(_Ty&& item)
		{
			return item;
		}

		template<	typename _Ty, 
					typename = enable_same_t<_Ty, pure_t>,
					typename = std::enable_if_t<_not_v<is_string<pure_t>>>,					//	문자열 아님
					typename = std::enable_if_t<std::is_fundamental_v<pure_t>>
			>
	    static constexpr _Ty ctype(_Ty&& item)
		{
			return item;
		}

		template<	typename _Ty, 
					typename = enable_same_t<_Ty, pure_t>,
					typename = std::enable_if_t<_not_v<is_string<pure_t>>>,					//	문자열 아님
					typename = std::enable_if_t<std::is_pointer_v<pure_t>>
			>
		static constexpr decltype(auto) ctype(_Ty&& item)
		{
			// recursive

			// 껍질 깐다~
			return ctype_traits<_Ty>::ctype(*item);
		}
	};


	template<typename _Type>
	struct sprintf_traits
	{
		using pure_t = typename pure_type_t<_Type>;

		//포인터 제외 ctype으로 변환
		template<	typename _Ty,
					typename = enable_same_t<_Ty, pure_t>,
					typename = std::enable_if_t<!std::is_pointer_v<pure_t>>
		>
		static constexpr decltype(auto) safe_type(_Ty&& item)
		{
			return ctype_traits<_Type>::ctype(item);
		}


		//포인터는 %p로 쓰지 않았을 가능성을 생각해서 uintptr_t로 변경
		template<	typename _Ty,
					typename = enable_same_t<_Ty, pure_t>,
					typename = std::enable_if_t<std::is_pointer_v<pure_t>>
		>
		static constexpr std::uintptr_t safe_type(_Ty&& item)
		{
			return reinterpret_cast<std::uintptr_t>(item);
		}
	};


	// =================================
	// iterator traits
	// =================================
	template<typename _Ty>
	struct pointer_iterator
	{
	private:
		_Ty* _ptr;

	public:
		pointer_iterator(_Ty* ptr)
			: _ptr(ptr)
		{}

		pointer_iterator& operator++() { _ptr++; return *this; }
		_Ty& operator*() const { return *_ptr; }
		_Ty* operator->() const { return *_ptr; }
		bool operator!=(const pointer_iterator& other) const noexcept { return _ptr != other._ptr; }
		bool operator==(const pointer_iterator& other) const noexcept { return _ptr == other._ptr; }
	};

	template<typename _Ty>
	struct pointer_const_iterator
	{
	private:
		_Ty* _ptr;

	public:
		pointer_const_iterator(_Ty* ptr)
			: _ptr(ptr)
		{}

		const pointer_const_iterator& operator++() const { _ptr++; return *this; }
		const _Ty& operator*() const { return *_ptr; }
		const _Ty* operator->() const { return *_ptr; }
		bool operator!=(const pointer_const_iterator& other) const { return _ptr != other._ptr; }
		bool operator==(const pointer_const_iterator& other) const { return _ptr == other._ptr; }
	};


	//이러면 view에 넣을 수 잇다.
	template<typename _Ty>
	struct range_ptr
	{
	private:
		using pure_t = stdex::pure_type_t<_Ty>;
		_Ty* _ptr;
		
	public:
		using iterator = pointer_iterator<_Ty>;
		using const_iterator = pointer_const_iterator<_Ty>;
		using iterator_category = std::forward_iterator_tag;	//일단 단방향

		range_ptr(_Ty* ptr)
			: _ptr(ptr)
		{}

		virtual iterator begin() const noexcept { return iterator(_ptr); }
		virtual iterator end() const noexcept { return iterator(nullptr); }
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
	};


	template<typename _Ty>
	struct view<_Ty*> : public std::conditional_t<has_reverse_iterator_v<_Ty>, reverse_view_types<_Ty>, forward_view_types<_Ty>>
	{
	protected:
		using pure_t = stdex::pure_type_t<_Ty>;
		mutable range_ptr<_Ty> _view_ptr;

	public:
		//이거 정의해 둬야 view에 view 넣기 가능
		using iterator = typename range_ptr<_Ty>::iterator;
		using const_iterator = typename range_ptr<_Ty>::const_iterator;
		using iterator_category = typename range_ptr<_Ty>::iterator_category;

		using iterator_type = iterator;

	public:
		view(_Ty* pointer)
			: _view_ptr(pointer)
		{}

	public:
		virtual iterator begin() const noexcept { return _view_ptr.begin(); }
		virtual iterator end() const noexcept { return _view_ptr.end(); }
	};

	                                             
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


	struct view_maker
	{
		template<typename _Container, typename... _Args>
		decltype(auto) get_view(_Container& container, _Args&&... args)
		{
			if constexpr (std::is_constructible_v<stdex::view, _Args...>)
				return stdex::view(std::forward<_Args>(args)...);
			else if constexpr (std::is_constructible_v<stdex::shared_view, _Args...>)
				return stdex::shared_view(std::forward<_Args>(args)...);
			else if constexpr (std::is_constructible_v<stdex::lock_view, _Args...>)
				return stdex::lock_view(std::forward<_Args>(args)...);
			else
			{
				static_assert(std::is_constructible_v<stdex::view, _Args...> ||
					std::is_constructible_v<stdex::shared_view, _Args...> ||
					std::is_constructible_v<stdex::lock_view, _Args...>
					, "connot make view");
				return nullptr;
			}
		}
	};

	// =================================
	// character traits
	// =================================
	template<typename _StrType>
	struct str_elem
	{
		template<typename = std::enable_if_t<stdex::is_convertible_ctype_char_v<_StrType>>>
		static constexpr const char elem(const char ch)
		{
			return ch;
		}

		template<typename = std::enable_if_t<stdex::is_convertible_ctype_wchar_v<_StrType>>>
		static constexpr const wchar_t elem(const char ch)
		{
			return static_cast<wchar_t>(ch);
		}

		//요점은 iterator가 있는가?
		template<typename _StrOther>
		static constexpr _StrType ascii(_StrOther&& other)
		{
			static_assert(stdex::is_string_v<_StrType> &&
				stdex::is_string_v<_StrOther>,
				"invalid string type!");

			auto ctype = stdex::ctype_traits<_StrOther>::ctype(std::forward<_StrOther>(other));
			stdex::view<decltype(ctype)> v(ctype);
				
			_StrType result;
			for (auto itr : v)
			{
				if (itr == str_elem<decltype(ctype)>::elem('\0'))
					break;

				result += str_elem<_StrType>::elem(itr);
			}


			return result;
		}
	};

	// =================================
	// func traits
	// =================================
	
	// Primary template
	template<typename T>
	struct function_traits;

	// Function pointer specialization
	template<typename _Ret, typename... _Args>
	struct function_traits<_Ret(*)(_Args...)>
	{
		using ret_type = _Ret;
		using params_type = std::tuple<_Args...>;
	};

	// Function reference specialization
	template<typename _Ret, typename... _Args>
	struct function_traits<_Ret(&)(_Args...)>
	{
		using ret_type = _Ret;
		using params_type = std::tuple<_Args...>;
	};

	// Member function pointer specialization => mutable lambda operator()
	template<typename _Class, typename _Ret, typename... _Args>
	struct function_traits<_Ret(_Class::*)(_Args...)>
	{
		using ret_type = _Ret;
		using params_type = std::tuple<_Args...>;
	};

	// Const member function pointer specialization => lambda operator()
	template<typename _Class, typename _Ret, typename... _Args>
	struct function_traits<_Ret(_Class::*)(_Args...) const>
	{
		using ret_type = _Ret;
		using params_type = std::tuple<_Args...>;
	};

	// type Deduction guide for lambda expressions
	template<typename _Lambda>
	function_traits(_Lambda) -> function_traits<decltype(&_Lambda::operator())>;


	// =================================
	// func
	// =================================
	template<typename _Format, typename... _Args, typename = std::enable_if_t<stdex::is_convertible_ctype_char_v<_Format>>>
	static inline void sprintf(std::string& buffer, _Format&& format, _Args&&... args)
	{
		//static_assert(stdex::is_convertible_ctype_char_v<_Format>, "invalid format string! valid format: std::string, const char*, char(&)[]");

		auto size = snprintf(nullptr, 0, ctype_traits<_Format>::ctype(std::forward<_Format>(format)), ctype_traits<_Args>::ctype(std::forward<_Args>(args))...);

		// 쓸때 무조건 '\0' 까지 씀. 만약 자리가 부족하면 마지막 문자열 제거하고 \0이 들어가버림
		size++;
		buffer.resize(size);
		snprintf(&buffer[0], size, ctype_traits<_Format>::ctype(std::forward<_Format>(format)), ctype_traits<_Args>::ctype(std::forward<_Args>(args))...);

		buffer.pop_back();	// \0제거
	}

	template<typename _Format, typename... _Args, typename = std::enable_if_t<stdex::is_convertible_ctype_char_v<_Format>>>
	static inline std::string sprintf_buffer(_Format&& format, _Args&&... args)
	{
		//static_assert(stdex::is_convertible_ctype_char_v<_Format>, "invalid format string! valid format: std::string, const char*, char(&)[]");

		std::string buffer;
		stdex::sprintf(buffer, std::forward<_Format>(format), std::forward<_Args>(args)...);
		return buffer;
	}

	template<typename _Format, typename... _Args, typename = std::enable_if_t<stdex::is_convertible_ctype_wchar_v<_Format>>>
	static inline void sprintf(std::wstring& buffer, _Format&& format, _Args&&... args)
	{
		//static_assert(stdex::is_convertible_ctype_wchar_v<_Format>, "invalid format string! valid format: std::wstring, const wchar_t*, wchar_t(&)[]");

		auto size = swprintf(nullptr, 0, ctype_traits<_Format>::ctype(std::forward<_Format>(format)), ctype_traits<_Args>::ctype(std::forward<_Args>(args))...);

		// 쓸때 무조건 '\0' 까지 씀. 만약 자리가 부족하면 마지막 문자열 제거하고 \0이 들어가버림
		size++;
		buffer.resize(size);
		swprintf(&buffer[0], size, ctype_traits<_Format>::ctype(std::forward<_Format>(format)), ctype_traits<_Args>::ctype(std::forward<_Args>(args))...);

		buffer.pop_back(); // \0 제거
	}

	template<typename _Format, typename... _Args, typename = std::enable_if_t<stdex::is_convertible_ctype_wchar_v<_Format>>>
	static inline std::wstring sprintf_buffer(_Format&& format, _Args&&... args)
	{
		//static_assert(stdex::is_convertible_ctype_wchar_v<_Format>, "invalid format string! valid format: std::wstring, const wchar_t*, wchar_t(&)[]");

		std::wstring buffer;
		stdex::sprintf(buffer, std::forward<_Format>(format), std::forward<_Args>(args)...);
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


	// =================================
	// file stream
	// =================================
	namespace cvt
	{
		enum class Encode : uint8
		{
			None = 0,
			ASCII,
			UniCode,
		};

		class Unicodecvt : public std::codecvt<wchar_t, char, mbstate_t>
		{
		protected:
			virtual bool do_always_noconv() const noexcept
			{
				return true;
			}
		};
	}

	template<typename _Stream>
	static constexpr void imbue(_Stream& stream, cvt::Encode encode)
	{
		stream.imbue(std::locale(std::locale(stream.getloc()), new cvt::Unicodecvt));
	}



	// =================================
	// utils
	// =================================
	// trim from left 
	static constexpr void ltrim(std::wstring& s, const wchar_t* t = L" \t\n\r\f\v")
	{
		s.erase(0, s.find_first_not_of(t));
	}
	// trim from right 
	static constexpr void rtrim(std::wstring& s, const wchar_t* t = L" \t\n\r\f\v")
	{
		s.erase(s.find_last_not_of(t) + 1);
	}
	// trim from left & right 
	static constexpr void trim(std::wstring& s, const wchar_t* t = L" \t\n\r\f\v")
	{
		rtrim(s, t);
		ltrim(s, t);
	}

	static constexpr void ltrim(std::string& s, const char* t = " \t\n\r\f\v")
	{
		s.erase(0, s.find_first_not_of(t));
	}
	// trim from right 
	static constexpr void rtrim(std::string& s, const char* t = " \t\n\r\f\v")
	{
		s.erase(s.find_last_not_of(t) + 1);
	}
	// trim from left & right 
	static constexpr void trim(std::string& s, const char* t = " \t\n\r\f\v")
	{
		rtrim(s, t);
		ltrim(s, t);
	}


	static constexpr void reverse(std::string& str)
	{
		std::reverse(str.begin(), str.end());
	}
	static constexpr void reverse(std::wstring& str)
	{
		std::reverse(str.begin(), str.end());
	}

	template<typename _CharType>
	static constexpr bool isdigit(_CharType&& character)
	{
		if constexpr (stdex::is_same_v<_CharType, char>)
			return std::isdigit(std::forward<_CharType>(character));
		else if constexpr (stdex::is_same_v<_CharType, wchar_t>)
			return std::iswdigit(std::forward<_CharType>(character));
		else
		{
			static_assert(stdex::is_same_v<_CharType, char> || stdex::is_same_v<_CharType, wchar_t>, "invalid character type");
			return false;
		}
	}

	std::vector<std::wstring> split(std::wstring str, wchar_t delimiter);
	bool make_directory(const wchar_t* directory_path);
}