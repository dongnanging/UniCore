#include "stdafx.h"
#include "MemoryPool.h"

void* MemoryPool::pop(const std::size_t& size)
{
    void* mem = nullptr;
    auto map_size = (size - 1) / 4;

    //std::cout << "size::" << size << std::endl;

    if (map_size < MAX_ARR_SIZE)
    {
        _pool[map_size].pop(mem);
    }

    if (!mem)
    {
        while (mem = ::malloc(map_size * 4 + 4), !mem)
            _clear_all();
    }

    return mem;
}

void MemoryPool::push(void* ptr, const std::size_t & size)
{
    if (!ptr)
        return;

    if (size - 1 > MAX_MERMORY_SIZE)
    {
        ::free(ptr);
        return;
    }

    auto map_size = (size - 1) / 4;
    _pool[map_size].push(ptr);

    //_monitor[map_size].fetch_add(1);
}

void MemoryPool::Monitor()
{
    while (true)
    {
        system("cls");
        for (auto& itr : _monitor)
        {
            std::cout << itr.first << "::" << itr.second << std::endl;
        }

        THREAD_SLEEP_TIME(1000);
    }
}

void MemoryPool::_clear_all()
{
    for (auto& pool : _pool)
    {
        void* ptr = nullptr;

        while (pool.pop(ptr), ptr)
        {
            ::free(ptr);
            ptr = nullptr;
        }
    }
}
