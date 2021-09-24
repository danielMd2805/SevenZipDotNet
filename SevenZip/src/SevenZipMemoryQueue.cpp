#include "SevenZipMemoryQueue.h"
#include <mutex>
#include <iostream>

using namespace SevenZip;

struct SevenZipMemoryQueue::MyMutexStruct
{
	std::mutex mutex;
	std::condition_variable cond;
};

SevenZipMemoryQueue::SevenZipMemoryQueue()
{
	Init();
}

SevenZipMemoryQueue::SevenZipMemoryQueue(int maxQueueSize)
{
	Init(maxQueueSize);
}

void SevenZipMemoryQueue::Add(std::vector<unsigned char> &item)
{
	std::unique_lock<std::mutex> mlock(_mutex->mutex);
	while (_currentQueueSize == _maxQueueSize)
	{
		_mutex->cond.wait(mlock);
	}

	_queue.push(std::move(item));
	_currentQueueSize++;
	mlock.unlock();
	_mutex->cond.notify_one();
}

bool SevenZipMemoryQueue::TryTake(std::vector<unsigned char> *result)
{
	std::unique_lock<std::mutex> mlock(_mutex->mutex);
	while (_queue.empty())
	{
		_mutex->cond.wait(mlock);
	}
	*result = std::move(_queue.front());
	_currentQueueSize--;
	_queue.pop();
	mlock.unlock();
	_mutex->cond.notify_one();
	return true;
}

void SevenZipMemoryQueue::CompleteAdding()
{
	_completeAdding = true;
}

bool SevenZipMemoryQueue::Completed() const
{
	return _completeAdding;
}


void SevenZipMemoryQueue::Init()
{
	Init(5);
}

void SevenZipMemoryQueue::Init(int maxQueueSize)
{
	_mutex = new MyMutexStruct();
	_maxQueueSize = maxQueueSize;
	_completeAdding = false;
	_currentQueueSize = 0;
}
