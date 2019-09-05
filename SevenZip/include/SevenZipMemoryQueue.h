#pragma once

#ifndef SEVENZIPMEMORYQUEUE_H
#define SEVENZIPMEMORYQUEUE_H

#include "SevenZipCpp.h"
#include <queue>
#include <memory>


namespace SevenZip
{
	class SEVENZIPCPP_API SevenZipMemoryQueue 
	{
	public:
		SevenZipMemoryQueue();
		SevenZipMemoryQueue(int maxQueueSize);
		

		void Add(std::vector<unsigned char> &item);

		bool TryTake(std::vector<unsigned char> *result);
		
		void CompleteAdding();

		bool Completed() const;

	private:
		void Init();
		void Init(int maxQueueSize);

	private:
		int _maxQueueSize;
		bool _completeAdding;
		int _currentQueueSize;
	
		std::queue<std::vector<unsigned char>> _queue;

		struct MyMutexStruct;
		MyMutexStruct *_mutex;

	};
}


#endif