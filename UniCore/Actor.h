#pragma once

template<typename _MsgType>
class Actor
{
	using _container = std::queue<_MsgType>;
	USE_LOCK;
	_container _queue;

protected:
	virtual void _Handle(_MsgType& msg) = 0;
	bool _PopAll(OUT _container& out) {
		WRITE_LOCK;
		if (_queue.empty())
			return false;

		_queue.swap(out);
		return true;
	}

public:
	void SendMsg(_MsgType&& mail) {
		WRITE_LOCK;
		_queue.emplace(std::move(mail));
	}

	// 스레드 할당.
	bool HandleMsg()
	{
		_container out;
		_PopAll(out);
		if (out.empty())
			return false;

		while (!out.empty())
		{
			_Handle(out.front());
			out.pop();
		}
		return true;
	}
};

