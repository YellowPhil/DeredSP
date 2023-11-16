#include <thread>
#include <condition_variable>
#include <queue>
#include <mutex>
#include <iostream>

#include "TaskRunner.h"

namespace taskrunner {
	TaskQueue::TaskQueue() : AcceptingTasks(true) {}
	void TaskQueue::AddTask(const TaskFunction& task) {
		std::unique_lock<std::mutex> lock(mtx);
		if (!AcceptingTasks) { return; }
		tasks.push(task);
		cv.notify_one();
	}

	TaskFunction TaskQueue::GetTask() {
		std::unique_lock<std::mutex> lock(mtx);
		cv.wait(lock, [this] { return !AcceptingTasks || !tasks.empty(); });

		if (tasks.empty()) {
			return nullptr;
		}

		auto task = tasks.front();
		tasks.pop();
		return task;
	}

	void TaskQueue::Shutdown() {
		{
			std::unique_lock<std::mutex> lock(mtx);
			AcceptingTasks = false;
		}
		cv.notify_all();
	}

	bool TaskQueue::isEmpty() {
		std::unique_lock<std::mutex> lock(mtx);
		return tasks.empty();
	}

	ThreadPool::ThreadPool(size_t threadNum) {
		for (auto i = 0; i < threadNum; i++) {
			workers.emplace_back([this] {
				while (true) {
					auto task = taskQueue.GetTask();
					if (!task) { break; }
					task();
				}
				}
			);
		}
	}

	void ThreadPool::AddTask(const TaskFunction& task) {
		if (!shutdown.load()) {
			taskQueue.AddTask(task);
		}
	}

	void ThreadPool::Shutdown() {
		if (!shutdown.exchange(true)) {
			taskQueue.Shutdown();
			for (auto& worker : workers) {
				if (worker.joinable()) {
					worker.join();
				}
			}
			workers.clear();
		}
	}

	ThreadPool::~ThreadPool() {
		if (!shutdown.load()) {
			Shutdown();
		}
	}
}
