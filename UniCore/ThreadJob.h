#pragma once
using thread_job_func = std::function<void()>;

struct ThreadJob
{
public:
	//��� ��ü���� ���ӵ��� �ʴ� ���� ���ٷ� �̷���� �۾�
	//=> ������ �� ������ Ȯ���ϰ� �ƹ��� ��ü���� ���������� �ʰų�,
	//�ܼ��� ���� ĸ�ĸ����� �������� ������ �� ���� ��쿡�� ���

	template<typename _Job_Func,
		typename = std::enable_if_t<std::is_same_v<stdex::pure_type_t<_Job_Func>, thread_job_func> || std::is_convertible_v<stdex::pure_type_t<_Job_Func>, thread_job_func>>>
	ThreadJob(_Job_Func&& lambda)
		: _callback(std::move(lambda))
	{}

	//���� ĸ�ķ� �������� Ȯ���� ���� ������ ��� ��ü�� ���ӵǾ�����
	//� ��ü�� �������� ������ ��� �ش� ��ü�� ������� ��쿡�� ������ �� �ֵ��� ����
	template<typename _Job_Func, typename _Validator>
	ThreadJob(_Job_Func&& lambda, std::shared_ptr<_Validator>&& execValidator)
	{
		std::weak_ptr<_Validator> valid(std::move(execValidator));
		_callback = [validtor = std::move(valid), exec = std::forward<_Job_Func>(lambda)]() mutable
			{
				//������ Ȯ�� => �� ���� �׾����� ���� �ȵ�
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
				//������ Ȯ�� => �� ���� �׾����� ���� �ȵ�
				auto keepAlive = validtor.lock();
				if (keepAlive)
					exec();
			};
	}

	//��� ��ü�� � �Լ��� �����ϰ��� �Ҷ��� �ݵ�� ������ �Բ� ����;� �Ѵ�.
	//��� �̷��� �������� �����ҰŸ� ù��° ������ �� ���Ѱ� ����.
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