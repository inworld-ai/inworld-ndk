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
	class SharedQueue
	{
	public:
		SharedQueue();
		~SharedQueue();

		T& Front();
		void PopFront();
		bool PopFront(T& Item);
	    void PopAll(std::deque<T>& Queue);

	    template <typename U>
		void PushBack(U&& Item);

		auto Size();
		bool IsEmpty();

	private:
		std::deque<T> _Queue;
		std::mutex _Mutex;
	};

	template <typename T>
	SharedQueue<T>::SharedQueue() {}

	template <typename T>
	SharedQueue<T>::~SharedQueue() {}

	template <typename T>
	T& SharedQueue<T>::Front()
	{
		std::unique_lock<std::mutex> Lock(_Mutex);
		return _Queue.front();
	}

	template <typename T>
	void SharedQueue<T>::PopFront()
	{
		std::unique_lock<std::mutex> Lock(_Mutex);
		_Queue.pop_front();
	}

	template <typename T>
	bool SharedQueue<T>::PopFront(T& Item)
	{
		std::unique_lock<std::mutex> Lock(_Mutex);
		if (!_Queue.empty())
		{
			Item = _Queue.front();
			_Queue.pop_front();
			return true;
		}
		return false;
	}

    template <typename T>
    void SharedQueue<T>::PopAll(std::deque<T>& Queue)
    {
        std::unique_lock<std::mutex> Lock(_Mutex);
        Queue = std::move(_Queue);
	    _Queue = {};
    }

	template <typename T>
    template <typename U>
	void SharedQueue<T>::PushBack(U&& Item)
	{
		std::unique_lock<std::mutex> Lock(_Mutex);
		_Queue.push_back(std::forward<U>(Item));
	}

	template <typename T>
	auto SharedQueue<T>::Size()
	{
		std::unique_lock<std::mutex> Lock(_Mutex);
		return _Queue.size();
	}

	template <typename T>
	bool Inworld::SharedQueue<T>::IsEmpty()
	{
		return Size() == 0;
	}
}