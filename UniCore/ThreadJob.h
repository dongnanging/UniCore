#pragma once
using thread_job_func = std::function<void()>;

struct ThreadJob
{
public:
	//어떠한 객체에도 종속되지 않는 단일 람다로 이루어진 작업
	//=> 위험할 수 있으니 확실하게 아무런 객체에도 종속적이지 않거나,
	//단순히 람다 캡쳐만으로 생존권을 보장할 수 있을 경우에만 사용

	template<typename _Job_Func,
		typename = std::enable_if_t<std::is_same_v<stdex::pure_type_t<_Job_Func>, thread_job_func> || std::is_convertible_v<stdex::pure_type_t<_Job_Func>, thread_job_func>>>
	ThreadJob(_Job_Func&& lambda)
		: _callback(std::move(lambda))
	{}

	//람다 캡쳐로 생존권을 확보할 수는 없지만 어떠한 객체에 종속되어있음
	//어떤 객체에 종속적인 람다의 경우 해당 객체가 살아있을 경우에만 실행할 수 있도록 유도
	template<typename _Job_Func, typename _Validator>
	ThreadJob(_Job_Func&& lambda, std::shared_ptr<_Validator>&& execValidator)
	{
		std::weak_ptr<_Validator> valid(std::move(execValidator));
		_callback = [validtor = std::move(valid), exec = std::forward<_Job_Func>(lambda)]() mutable
			{
				//생존권 확보 => 그 전에 죽었으면 실행 안됨
				auto keepAlive = validtor.lock();
				if (keepAlive)
					exec();
			};
	}

	template<typename _Job_Func, typename _Validator>
	ThreadJob(_Job_Func&& lambda, std::shared_ptr<_Validator>& execValidator)
	{
		std::weak_ptr<_Validator> valid(execValidator);
		_callback = [validtor = std::move(valid), exec = std::forward<_Job_Func>(lambda)]() mutable
			{
				//생존권 확보 => 그 전에 죽었으면 실행 안됨
				auto keepAlive = validtor.lock();
				if (keepAlive)
					exec();
			};
	}

	//어느 객체의 어떤 함수를 실행하고자 할때는 반드시 원본과 함께 따라와야 한다.
	//사실 이렇게 생존권을 보장할거면 첫번째 버전이 더 편한것 같다.
	template<typename _Owner, typename _RetVal, typename... Args>
	ThreadJob(std::shared_ptr<_Owner> owner, _RetVal(_Owner::* memFunc)(Args...), Args... args)
	{
		_callback = [owner, memFunc, args...]() mutable
			{
				(owner.get()->*memFunc)(args...);
			};
	}

public:
	//functor
	//void operator()()
	//{
	//	_callback();
	//}

	void Execute()
	{
		if (_callback)
			_callback();
	}
private:
	std::function<void()> _callback;
};