#pragma once

#include <unordered_set>

namespace stdex
{
	//alloc
	template<typename _Ty>
	using CustomAllocator = now_allocator<_Ty>;

	//string
	template<typename _Ty = char, typename _Alloc = CustomAllocator<_Ty>>
	using string = std::basic_string<_Ty, std::char_traits<_Ty>, _Alloc>;

	//wstring 
	template<typename _Ty = wchar_t, typename _Alloc = CustomAllocator<_Ty>>
	using wstring = std::basic_string<_Ty, std::char_traits<_Ty>, _Alloc>;


	//vector
	template<typename _Ty, typename _CustAlloc = CustomAllocator<_Ty>>
	using vector = std::vector<_Ty, _CustAlloc>;


	//deque
	template<typename _Ty, typename _CustAlloc = CustomAllocator<_Ty>>
	using deque = std::deque<_Ty, _CustAlloc>;


	//stack
	template<typename _Ty, typename _Container = deque<_Ty>>
	using stack = std::stack<_Ty, _Container>;


	//queue
	template<typename _Ty, typename _Container = deque<_Ty>, typename _CustAlloc = CustomAllocator<_Ty>>
	using queue = std::queue<_Ty, _Container>;


	//set
	template<typename _Kty, typename _Less = std::less<_Kty>, typename _CustAlloc = CustomAllocator<_Kty>>
	using set = std::set<_Kty, _Less, _CustAlloc>;


	//map
	template<typename _Kty, typename _Vty, typename _Less = std::less<_Kty>, typename _CustAlloc = CustomAllocator<std::pair<const _Kty, _Vty>>>
	using map = std::map<_Kty, _Vty, _Less, _CustAlloc>;


	//unordered_map
	template<typename _Kty, typename _Vty, typename _Hasher = std::hash<_Kty>, typename _Equal = std::equal_to<_Kty> ,typename _Less = std::less<_Kty>, 
		typename _CustAlloc = CustomAllocator<std::pair<const _Kty, _Vty>>>
	using unordered_map = std::unordered_map<_Kty, _Vty, _Hasher, _Equal, _CustAlloc>;


	//unordered_set
	template<typename _Kty, typename _Hasher = std::hash<_Kty>, typename _Equal = std::equal_to<_Kty>, typename _Less = std::less<_Kty>,
		typename _CustAlloc = CustomAllocator<_Kty>>
	using unordered_set = std::unordered_set<_Kty, _Hasher, _Equal, _CustAlloc>;


	template<typename _Ty, typename = void>
	struct is_findable_container : std::false_type {};

	template<typename _Ty>
	struct is_findable_container<_Ty, decltype(_Ty::find)> : std::true_type {};

	template<typename _Ty>
	static constexpr bool is_findable_container_v = is_findable_container<_Ty>::value_type;

	template<template<typename...> class _Container, typename... _Args>
	class ContainerWrapper
	{
	public:
		using _ContainerType = _Container<_Args...>;
		using iterator = typename _ContainerType::iterator;
		using const_iterator = typename _ContainerType::const_iterator;
		using size_type = typename _ContainerType::size_type;
		
	private:
		_ContainerType _ct;

	public:
		// 기본 생성자로 만들 수 있는지
		template<typename = std::enable_if_t<std::is_default_constructible_v<_ContainerType>>>
		ContainerWrapper() {}

		// 인자를 제공받아 만들 수 있다면 해당 생성자 제공.
		template<typename... _ConstructArgs, typename  = std::enable_if_t<std::is_constructible_v<_ContainerType, _ConstructArgs...>>>
		ContainerWrapper(_ConstructArgs&&... args)
			: _ct(std::forward< _ConstructArgs>(args)...)
		{}

	public:
		constexpr iterator begin() { return _ct.begin(); }
		constexpr iterator end() { return _ct.end(); }

		template<typename = decltype(std::declval<_ContainerType>().rbegin())>
		constexpr iterator rbegin() { return _ct.rbegin(); }
		template<typename = decltype(std::declval<_ContainerType>().rend())>
		constexpr iterator rend() { return _ct.rend(); }

		template<typename _Ty, typename = decltype(std::declval<_ContainerType>().find(std::declval<_Ty>()))>
		constexpr iterator find(const _Ty& item) { return _ct.find(item); }
		template<typename _Ty>
		constexpr iterator find(const _Ty& item) { return std::find(_ct.begin(), _ct.end(), item); }

		template<typename _Ty, typename = decltype(std::declval<_ContainerType>().push_front(std::declval<_Ty>()))>
		constexpr iterator push_front(_Ty&& item) { return _ct.push_front(std::forward<_Ty>(item)); }
		template<typename _Ty, typename = decltype(std::declval<_ContainerType>().push_back(std::declval<_Ty>()))>
		constexpr iterator push_back(_Ty&& item) { return _ct.push_back(std::forward<_Ty>(item)); }
		template<typename _Ty, typename = decltype(std::declval<_ContainerType>().push(std::declval<_Ty>()))>
		constexpr iterator push(_Ty&& item) { return _ct.push(std::forward<_Ty>(item)); }

		template<typename _Ty, typename = decltype(std::declval<_ContainerType>().erase(std::declval<_Ty>()))>
		constexpr decltype(auto) erase(_Ty&& item) { return _ct.erase(std::forward<_Ty>(item)); }
		template<typename = decltype(std::declval<_ContainerType>().erase(std::declval<iterator>()))>
		constexpr decltype(auto) erase(iterator&& item) { return _ct.erase(std::forward<iterator>(item)); }

		template<typename = decltype(std::declval<_ContainerType>().pop())>
		decltype(auto) pop() { return _ct.pop(); }
		template<typename = decltype(std::declval<_ContainerType>().pop_front())>
		decltype(auto) pop_front() { return _ct.pop_front(); }
		template<typename = decltype(std::declval<_ContainerType>().pop_back())>
		decltype(auto) pop_back() { return _ct.pop_back(); }

		template<typename _Ty, typename = decltype(std::declval<_ContainerType>().insert(std::declval<_Ty>()))>
		decltype(auto) insert(_Ty&& item) { return _ct.insert(std::forward<_Ty>(item)); }

		template<typename... _EmplaceArgs, typename = decltype(std::declval<_ContainerType>().emplace(std::declval<_EmplaceArgs>()...))>
		void emplace(_EmplaceArgs&&... args) { _ct.emplace(std::forward<_EmplaceArgs>(args)...); }

	public:
		constexpr const_iterator begin() const { return _ct.begin(); }
		constexpr const_iterator end() const { return _ct.end(); }

		template<typename = decltype(std::declval<_ContainerType>().rbegin())>
		constexpr const_iterator rbegin() const { return _ct.rbegin(); }
		template<typename = decltype(std::declval<_ContainerType>().rend())>
		constexpr const_iterator rend() const { return _ct.rend(); }

		constexpr size_type size() const { _ct.size(); }
		constexpr bool empty() const { _ct.empty(); }

		template<typename _Ty, typename = decltype(std::declval<_ContainerType>().find(std::declval<_Ty>()))>
		constexpr const_iterator find(const _Ty& item) const { return _ct.find(item); }

		template<typename _Ty>
		constexpr const_iterator find(const _Ty& item) const { return std::find(_ct.begin(), _ct.end(), item); }

		template<typename _Ty>
		constexpr bool contains(const _Ty& item) const { return find(item) != end(); }

		template<typename = decltype(std::declval<_ContainerType>().top())>
		decltype(auto) top() const { return _ct.top(); }
	};
}