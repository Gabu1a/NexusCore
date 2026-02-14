#ifndef _THREADPOOL
#define _THREADPOOL
#include "fmt/base.h"
#include <climits>
#include <functional>
#include <limits>
#include <queue>
#include <thread>
#include <vector>

struct _ThreadInfo {
    std::thread thread;
    std::queue<std::function<void()>> queue;
    bool shouldStop;
    unsigned id;

    // std::thread is not copy-able :)
    _ThreadInfo(std::thread t, unsigned i)
            : thread(std::move(t)), id(i), shouldStop(false) {}
};

class ThreadPool {
public:
    ThreadPool(const size_t &thread_number) {
        if (thread_number <= 0) {
            fmt::print("Trebuie cel putin 1 thread");
            nrThreads = 1;
        } else {
            nrThreads = thread_number;
        }
        GenerateThreads();
    }

    ~ThreadPool() {
        PoolCleanup();
    }

    void Add(std::function<void()> &func) {
        int best_thread = -1;
        int count = std::numeric_limits<int>().max();

        for (auto &thread : threads) {
            if (thread.queue.empty()) {
                best_thread = thread.id;
                break;
            }
            if (thread.queue.size() < count) {
                best_thread = thread.id;
                count = thread.queue.size();
            }
        }

        if (best_thread < threads.size()) {
            threads[best_thread].queue.emplace(std::move(func));
        } else {
            fmt::print("Failed to get best thread, adding to the first one :(");
            threads[0].queue.emplace(std::move(func));
        }
    }

private:
    void PoolCleanup() {
    }

    void GenerateThreads() {
        threads.reserve(nrThreads);
        for (size_t i = 0; i < nrThreads; i++) {
            std::thread t([this, i]() {
                while (!threads[i].shouldStop) {
                    if (!threads[i].queue.empty()) {
                        auto func = threads[i].queue.front();
                        func();
                        threads[i].queue.pop();
                    }
                }
            });
            threads.emplace_back(_ThreadInfo(std::move(t), i));
        }
    }

    std::vector<_ThreadInfo> threads;
    size_t nrThreads;
};

#endif
