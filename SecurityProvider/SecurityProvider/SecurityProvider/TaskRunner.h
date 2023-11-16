#pragma once
#include <functional>
#include <sstream>
#include <mutex>
#include <vector>
#include <thread>
#include <queue>


namespace taskrunner {
	using TaskFunction = std::function <void ()>;
	class TaskQueue {
	private:
		std::mutex mtx;
		std::queue<TaskFunction> tasks;
		std::condition_variable cv;
		bool AcceptingTasks;

	public:
		TaskQueue();
		void AddTask(const TaskFunction& task);
		TaskFunction GetTask();
		void Shutdown();
		bool isEmpty();
	};

	class ThreadPool {
	private:
		TaskQueue taskQueue;
		std::vector<std::thread> workers;
		std::atomic<bool> shutdown;
	public:
		ThreadPool(size_t threadNum);
		ThreadPool() = default;
		~ThreadPool();
		void Shutdown();
		void AddTask(const TaskFunction& task);

	};
}
