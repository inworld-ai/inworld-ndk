/**
 * Copyright 2023-2024 Theai, Inc. dba Inworld AI
 *
 * Use of this source code is governed by the Inworld.ai Software Development Kit License Agreement
 * that can be found in the LICENSE.md file or at https://www.inworld.ai/sdk-license
 */

#pragma once

#include <queue>
#include <mutex>

namespace Inworld
{
    template <typename T>
    class WaitQueue
    {
    public:
        T& Front();
        void PopFront();
        void PopFront(T& Item);
        void PopAll(std::deque<T>& Queue);
    
        template <typename U>
        void PushBack(U&& Item);
    
        auto Size();
        bool IsEmpty();
    
    private:
        std::deque<T> _Queue;
        std::mutex _Mutex;
        std::condition_variable _CondVar;
    };
    
    template <typename T>
    T& WaitQueue<T>::Front()
    {
        std::unique_lock<std::mutex> Lock(_Mutex);
        _CondVar.wait(Lock, [this]{ return !_Queue.empty(); });
        return _Queue.front();
    }
    
    template <typename T>
    void WaitQueue<T>::PopFront()
    {
        std::unique_lock<std::mutex> Lock(_Mutex);
        _Queue.pop_front();
    }
    
    template <typename T>
    void WaitQueue<T>::PopFront(T& Item)
    {
        std::unique_lock<std::mutex> Lock(_Mutex);
        _CondVar.wait(Lock, [this]{ return !_Queue.empty(); });
        Item = _Queue.front();
        _Queue.pop_front();
    }
    
    template <typename T>
    void WaitQueue<T>::PopAll(std::deque<T>& Queue)
    {
        std::unique_lock<std::mutex> Lock(_Mutex);
        _CondVar.wait(Lock, [this]{ return !_Queue.empty(); });
        Queue = std::move(_Queue);
        _Queue.clear();
    }
    
    template <typename T>
    template <typename U>
    void WaitQueue<T>::PushBack(U&& Item)
    {
        {
            std::unique_lock<std::mutex> Lock(_Mutex);
            _Queue.push_back(std::forward<U>(Item));
        }
        _CondVar.notify_one();
    }
    
    template <typename T>
    auto WaitQueue<T>::Size()
    {
        std::unique_lock<std::mutex> Lock(_Mutex);
        return _Queue.size();
    }
    
    template <typename T>
    bool WaitQueue<T>::IsEmpty()
    {
        return Size() == 0;
    }
}