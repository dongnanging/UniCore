#pragma once

namespace ads
{
	//alloc
	template<typename _Ty>
	using CustomAllocator = now_allocator<_Ty>;


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
	template<typename _Kty, typename _Vty, typename _Less = std::less<_Kty>, typename _CustAlloc = CustomAllocator<std::pair<_Kty, _Vty>>>
	using map = std::map<_Kty, _Vty, _Less, _CustAlloc>;


	//unordered_map
	template<typename _Kty, typename _Vty, typename _Hasher = std::hash<_Kty>, typename _Equal = std::equal_to<_Kty> ,typename _Less = std::less<_Kty>, 
		typename _CustAlloc = CustomAllocator<std::pair<_Kty, _Vty>>>
	using unordered_map = std::unordered_map<_Kty, _Vty, _Hasher, _Equal, _CustAlloc>;


	//unordered_set
	template<typename _Kty, typename _Hasher = std::hash<_Kty>, typename _Equal = std::equal_to<_Kty>, typename _Less = std::less<_Kty>,
		typename _CustAlloc = CustomAllocator<_Kty>>
	using unordered_set = std::unordered_map<_Kty, _Hasher, _Equal, _CustAlloc>;
}