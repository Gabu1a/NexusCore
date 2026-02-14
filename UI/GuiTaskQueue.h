#pragma once
#include <deque>
#include <functional>
#include <mutex>

class GuiTaskQueue {
    public:
        using Task = std::function<void()>;

        void push(Task t) {
            std::lock_guard<std::mutex> lk(m_);
            q_.emplace_back(std::move(t));
        }

        bool pop(Task &t) {
            std::lock_guard<std::mutex> lk(m_);
            if (q_.empty())
                return false;
            t = std::move(q_.front());
            q_.pop_front();
            return true;
        }

    private:
      std::mutex m_;
      std::deque<Task> q_;
    };

inline GuiTaskQueue g_guiTasks;
