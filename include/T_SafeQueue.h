#pragma once
#ifndef __T_SAFE_QUEUE_H_
#define __T_SAFE_QUEUE_H_
#include <queue>
#include <condition_variable>

using namespace std;

template<typename T> class SafeQueue
{
private:
	mutable mutex m_mut;
	queue<T> m_queue;
	condition_variable m_data_cond;

public:
	SafeQueue() {}
	SafeQueue(const SafeQueue&) = delete;

	void push(T data)
	{
		lock_guard<mutex> lg(m_mut);
		m_queue.push(data);
		m_data_cond.notify_one();
	}

	void WaitPop(T &t)
	{
		unique_lock<mutex> ul(m_mut);
		m_data_cond.wait(ul, [this] {return !m_queue.empty(); });
		t = m_queue.front();
		m_queue.pop();
	}

	shared_ptr<T> WaitPop()
	{
		unique_lock<mutex> ul(m_mut);
		m_data_cond.wait(ul, [this] {return !m_queue.empty(); });

		shared_ptr<T> res(make_shared<T>(m_queue.front()));
		m_queue.pop();
		return res;
	}

	bool TryPop(T &t)
	{
		lock_guard<mutex> lg(m_mut);
		if (m_queue.empty())
		{
			return false;
		}

		t = m_queue.front();
		m_queue.pop();
		return true;
	}

	shared_ptr<T> TryPop()
	{
		lock_guard<mutex> lg(m_mut);
		if (m_queue.empty())
		{
			return shared_ptr<T>();
		}

		shared_ptr<T> res(make_shared<T>(m_queue.front()));
		m_queue.pop();
		return res;
	}

	bool IsEmpty()
	{
		lock_guard<mutex> lg(m_mut);
		return m_queue.empty();
	}
};


#endif