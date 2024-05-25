#pragma once

//TODO : Error 처리
//장점은 _Ty랑 Container의 요소가 _Ty가 안닐 경우에도 static assert가 일어남
//단점은 그럼 Container 추가할 때마다 새로 정의해야하나?
//=> 그렇다고 해결하기 위해 enable_if를 사용하니, 코드 작성 시,
//=> 없어야할 함수가 표출되지 않도록 하고 싶은데, 그런 부분에선 해결이 어려움
//=> 현재 상태로는 그러한 부분을 아주 손쉽게 (통째로 재정의)함으로 해결이 가능
template<class _Ty, class _Container = std::deque<_Ty>>
class LockContainer
{
public:
	LockContainer()
	{
		static_assert(std::is_same_v<std::deque<_Ty>, _Container>);
	}

public:
	template<typename _Uy, typename = stdex::enable_same_t<_Ty, _Uy>>
	void push_front(_Uy&& item)
	{
		WRITE_LOCK;
		_container.push_front(std::forward<_Uy>(item));
	}
	_Ty& pop_front()
	{
		WRITE_LOCK;
		_Ty& result = _container.front();
		_container.pop_front();
		return result;
	}

	template<typename _Uy, typename = stdex::enable_same_t<_Ty, _Uy>>
	void push_back(_Uy&& item)
	{
		WRITE_LOCK;
		_container.push_back(std::forward<_Uy>(item));
	}
	_Ty& pop_back()
	{
		WRITE_LOCK;
		_Ty& result = _container.back();
		_container.pop_back();
		return result;
	}

	
	template<typename... Args>
	void emplace_front(Args&&... args)
	{
		WRITE_LOCK;
		_container.emplace_front(std::forward<Args>(args)...);
	}

	template<typename... Args>
	void emplace_back(Args&&... args)
	{
		WRITE_LOCK;
		_container.emplace_back(std::forward<Args>(args)...);
	}

public:
	void pop_front_all(std::vector<_Ty>& result)
	{
		WRITE_LOCK;
		while (_container.empty() == false)
		{
			result.push_back(std::move(_container.front()));
			_container.pop_front();
		}
			
	}

	void pop_back_all(std::vector<_Ty>& result)
	{
		WRITE_LOCK;
		while (_container.empty() == false)
		{
			result.push_back(std::move(_container.back()));
			_container.pop_back();
		}
	}

	constexpr bool emmpty() const noexcept
	{
		READ_LOCK;
		return _container.empty();
	}

public:
	_Ty& operator[](const int& index) const
	{
		READ_LOCK;
		return _container[index];
	}

	constexpr int32 size() const noexcept { READ_LOCK; return _container.size(); }

private:
	USE_LOCK;
	_Container _container;
};

template<class _Ty>
class LockContainer < _Ty, std::queue<_Ty>> 
{
public:
	_Ty& dequeue()
	{
		WRITE_LOCK;
		_Ty& result = _container.front();
		_container.pop();
		return result;
	}

	template<typename _Uy, typename = stdex::enable_same_t<_Ty, _Uy>>
	void enqueue(_Uy&& item)
	{
		WRITE_LOCK;
		_container.push(std::forward<_Uy>(item));
	}

	void enqueue_all(std::vector<_Ty>& items)
	{
		WRITE_LOCK;
		for (auto& item : items)
			_container.push(std::move(item));
	}

	template<typename... Args>
	void emplace_back(Args&&... args)
	{
		WRITE_LOCK;
		_container.emplace(std::forward<Args>(args)...);
	}

public:
	void dequeue_all(std::vector<_Ty>& result)
	{
		WRITE_LOCK;
		while (_container.empty() == false)
		{
			result.push_back(std::move(_container.front()));
			_container.pop();
		}
	}

	int32 size() { return _container.size(); }

private:
	USE_LOCK;
	std::queue<_Ty> _container;
};

template<class _Ty>
class LockContainer < _Ty, std::stack<_Ty>>
{
public:
	_Ty& pop()
	{
		WRITE_LOCK;
		_Ty& result = _container.top();
		_container.pop();
		return result;
	}

	template<typename _Uy, typename = stdex::enable_same_t<_Ty, _Uy>>
	void push(_Uy&& item)
	{
		WRITE_LOCK;
		_container.push(std::forward<_Uy>(item));
	}

	template<typename... Args>
	void emplace(Args&&... args)
	{
		WRITE_LOCK;
		_container.emplace(std::forward<Args>(args)...);
	}

public:
	void pop_all(std::vector<_Ty>& result)
	{
		WRITE_LOCK;
		while (_container.empty() == false)
		{
			result.push_back(std::move(_container.top()));
			_container.pop();
		}
	}

	int32 size() { return _container.size(); }

private:
	USE_LOCK;
	std::stack<_Ty> _container;
};