#pragma once
#include <unordered_map>
#include <set>
#include <mutex>

//�⺻���� ������ ���� �׷�����, �� ���� �׷����� ������ ���� ��, ����Ŭ�� ������� �Ǻ�
//1���� ������ Ŭ������ �̸��� ������ ������ �Ǻ��� ���°ſ��µ�
//��������, Ŭ���� �̸��� � �ν��Ͻ�, �׸��� �� �ν��Ͻ��� ����ȭ ��ü(lock)�� ����Ű�� �ʴ´�.
//================================
//�̹� Ʈ������ ���� ������ �׷� �ν��Ͻ���, ����ȭ ��ü�� �ϳ��� ���� ġ���ϰ�,
//�׷� ��尣�� ���� �ӿ��� ��ȯ ������ Ž���Ѵ�.

//������ �����忡�� ��Ÿ���� ��ȯ ������ ������ ���ϸ� deadlock�� �ƴϴ�.
//������ �ݳ������� �����غ� ���, �Ǻ� ��ҿ� ��������� ���� ����� ����������
//������ ����������, �Ǻ��� ��������� �ӵ��� �ſ� ���������� �����ǰ�
//�������� ������ ������������ ��ȯ ������ ����� ���(lock)������ ������ ���ո����̶�� ������ ���.

//������ ���� �������� ���ܵΰڴ�.
class DeadLockProfiler
{
	struct PairHash
	{
	public:
		std::size_t operator() (const std::pair<int64, int32> item) const
		{
			return std::hash<long long>()(item.first ^ (item.second * (item.second + 1) * (item.second + 2)));
		}
	};

public:
	void PushLock(const int64& instance_id, const int32& lockIdx);
	void PopLock(const int64& instance_id, const int32& lockIdx);

private:
	void DFS(const std::pair<int64, int32>& now);
	void PushStack(const int64& instance_id, const int32& lockIdx);
	void CheckCycle();

private:
	// ***unordered_map< instance_id(prev), ***unordered_map< lockIdx(prev), ***std::unordered_map< instance_id(now), ***std::set< lockIdx(now) > > > >
	std::unordered_map<std::pair<int64, int32>, std::set<std::pair<int64, int32>>, PairHash> _lockHistory;
	std::mutex _lock;

private:
	int32 _discovered_count = 1;
	std::unordered_map<std::pair<int64, int32>, int32, PairHash>	_discovered;
	std::unordered_map<std::pair<int64, int32>, bool, PairHash>	_ended;
	std::unordered_map<std::pair<int64, int32>, std::pair<int64, int32>, PairHash> _traceback;
};