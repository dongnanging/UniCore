#pragma once

namespace stdex
{
	template<typename... Substitutions>
	struct substitution_helper 
	{
		using type = void;
	};
	
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

	//1��¥���� ������ ���� ������
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
					typename = std::enable_if_t<_not<is_string_v<pure_t>>>					//	���ڿ� �ƴ�
			>
		std::enable_if_t<std::is_fundamental_v<pure_t>,		// �⺻ Ÿ��. (������ ����)
			_Ty> ctype(_Ty&& item)
		{
			return item;
		}

		template<	typename _Ty, 
					typename = std::enable_if_t<std::is_same_v<pure_t, pure_type_t<_Ty>>>,
					typename = std::enable_if_t<_not<is_string_v<pure_t>>>					//	���ڿ� �ƴ�
			>
		std::enable_if_t<std::is_pointer_v<pure_t>,			// ������?
			_Ty> ctype(_Ty&& item)
		{
			// recursive

			// ���� ���~
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

	template<	typename _Ty,
				typename = std::enable_if_t<is_stl_container_v<_Ty>>>
	struct stl_view
	{
	protected:
		_Ty* _container;

	public:
		stl_view(_Ty& container)
			: _container(&container)
		{}

		auto begin() { return _container->begin(); }
		auto end() { return _container->end(); }
		auto size() { return _container->size(); }
		auto empty() { return _container->end(); }
	};

	template<	typename _Ty,
				typename = std::enable_if_t<is_stl_container_v<_Ty>>>
	struct reverse_view
	{
	protected:
		_Ty* _container;

	public:
		reverse_view(_Ty& container)
			: _container(&container)
		{}

		auto begin() { return _container->rbegin(); }
		auto end() { return _container->rend(); }
		auto size() { return _container->size(); }
		auto empty() { return _container->end(); }
	};

	template<typename _Ty, typename _View = stl_view<_Ty>>
	struct safe_view
	{
	public:
		using view_type = typename _View;
		using insert_func = std::function<void(view_type&)>;

	protected:
		std::function<bool(const insert_func&)> _valid_view;
		_View _view;

	public:
		template<typename _Validator>
		safe_view(_Ty& container, std::shared_ptr<_Validator> valid)
			: _view(container)
		{
			_valid_view = [this, weak_ = std::move(std::weak_ptr<_Validator>(valid))](const insert_func& insert)
				{
					auto alive = weak_.lock();

					if (!alive)
						return false;

					insert(_view);
					return true;
				};
		}

		virtual bool insertion(const insert_func& insert)
		{
			return _valid_view(insert);
		}
	};

	template<typename _Lockable>
	using lockable = typename substitution_helper<decltype(std::declval<_Lockable>().rlock()), decltype(std::declval<_Lockable>().wlock())>::type;

	template<typename _BaseLock>
	static constexpr bool is_base_lock_v = std::is_same_v<_BaseLock, std::mutex> || std::is_same_v<_BaseLock, std::recursive_mutex>;

	template<typename _LockType>
	struct lock_traits
	{
		using lock_type = typename pure_type_t<_LockType>;

		template<	typename _Ty,
					typename = std::enable_if_t<std::is_same_v<pure_type_t<_Ty>, lock_type>>,
					typename = std::enable_if_t<is_base_lock_v<lock_type>>
		>
		static inline decltype(auto) rlock(_Ty& lock)
		{
			return std::lock_guard<_Ty>(lock);
		}

		template<	typename _Ty,
					typename = std::enable_if_t<std::is_same_v<pure_type_t<_Ty>, lock_type>>,
					typename = std::enable_if_t<is_base_lock_v<lock_type>>
		>
		static inline decltype(auto) wlock(_Ty& lock)
		{
			return std::lock_guard<_Ty>(lock);
		}


		template<	typename _Ty,
					typename = std::enable_if_t<std::is_same_v<pure_type_t<_Ty>, lock_type>>,
					typename = std::enable_if_t<!is_base_lock_v<lock_type>>,
					typename = lockable<lock_type>
		>
		static inline decltype(auto) rlock(_Ty& lock)
		{
			return lock.rlock();
		}

		template<	typename _Ty,
					typename = std::enable_if_t<std::is_same_v<pure_type_t<_Ty>, lock_type>>,
					typename = std::enable_if_t<!is_base_lock_v<lock_type>>,
					typename = lockable<lock_type>
		>
		static inline decltype(auto) wlock(_Ty& lock)
		{
			return lock.wlock();
		}
	};


	template<typename _Ty, typename _LockType = std::mutex, typename _View = stl_view<_Ty>>
	struct lock_view : public safe_view<_Ty, _View>
	{
	protected:
		_LockType& _lock;

	public:
		template<typename _Validator>
		lock_view(_Ty& container, std::shared_ptr<_Validator> valid, _LockType& lock)
			: safe_view<_Ty, _View>(container, valid), _lock(lock)
		{}

		virtual bool insertion(const typename safe_view<_Ty, _View>::insert_func& insert)
		{
			typename safe_view<_Ty, _View>::insert_func lock_insert = [insert, this](safe_view<_Ty, _View>::view_type& view) {
					lock_traits<decltype(_lock)>::rlock(_lock);
					insert(view);
				};

			return safe_view<_Ty, _View>::_valid_view(lock_insert);
		}
	};

	// =================================
	// func
	// =================================
	template<typename... _Args>
	static inline void snprintf(std::string& buffer, const char* format, _Args&&... args)
	{
		auto size = snprintf(nullptr, 0, format, std::forward<_Args>(args)...);

		// ���� ������ '\0' ���� ��. ���� �ڸ��� �����ϸ� ������ ���ڿ� �����ϰ� \0�� ������
		size++;
		buffer.resize(size);
		snprintf(&buffer[0], size, format, ctype_traits<_Args>::ctype(std::forward<_Args>(args))...);

		buffer.pop_back();	// \0����
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

		// ���� ������ '\0' ���� ��. ���� �ڸ��� �����ϸ� ������ ���ڿ� �����ϰ� \0�� ������
		size++;
		buffer.resize(size);
		swprintf(&buffer[0], size, format, std::forward<_Args>(args)...);

		buffer.pop_back(); // \0 ����
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