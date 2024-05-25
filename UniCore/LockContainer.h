#pragma once

//TODO : Error ó��
//������ _Ty�� Container�� ��Ұ� _Ty�� �ȴ� ��쿡�� static assert�� �Ͼ
//������ �׷� Container �߰��� ������ ���� �����ؾ��ϳ�?
//=> �׷��ٰ� �ذ��ϱ� ���� enable_if�� ����ϴ�, �ڵ� �ۼ� ��,
//=> ������� �Լ��� ǥ����� �ʵ��� �ϰ� ������, �׷� �κп��� �ذ��� �����
//=> ���� ���·δ� �׷��� �κ��� ���� �ս��� (��°�� ������)������ �ذ��� ����
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