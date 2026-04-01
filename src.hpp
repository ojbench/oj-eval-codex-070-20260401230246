// Implementation for Problem 070 - CPU Task Scheduling
// You should only implement the TODO parts in derived classes.

#ifndef _SJTU_CPP_FINAL_SRC_HPP_
#define _SJTU_CPP_FINAL_SRC_HPP_

#include <utility>
#include <vector>
#include "task.hpp"

using namespace std;

typedef unsigned int uint;

namespace sjtu {
    class CPU {
    protected:
        CPUState state;
        vector<Task> tasks;

    public:
        CPU() : tasks() { state = idle; }

        int addTask(const Task &t) {
            tasks.push_back(t);
            return 1;
        }

        int changePriority(uint task_id, uint priority) {
            for (auto &task: tasks)
                if (task.task_id == task_id) {
                    task.priority = priority;
                    return 1;
                }
            return 0;
        }

        virtual pair<CPUState, uint> run() = 0;

        virtual ~CPU() = default;
    };

    // FCFS method based CPU.
    class CPU_FCFS : public CPU {
    public:
        pair<CPUState, uint> run() override {
            // Remove finished tasks (time == 0) if any linger.
            // In our design we always decrement and remove finished immediately.
            if (tasks.empty()) {
                state = idle;
                return make_pair(state, 0u);
            }
            // FCFS: choose earliest added non-finished task (front of vector).
            // Find first task with time > 0
            size_t idx = 0;
            while (idx < tasks.size() && tasks[idx].time == 0) ++idx;
            if (idx == tasks.size()) {
                // All finished
                tasks.clear();
                state = idle;
                return make_pair(state, 0u);
            }
            // Execute this task for one unit
            state = busy;
            uint id = tasks[idx].task_id;
            if (tasks[idx].time > 0) {
                --tasks[idx].time;
                if (tasks[idx].time == 0) {
                    // Remove the finished task
                    tasks.erase(tasks.begin() + static_cast<long>(idx));
                }
            }
            return make_pair(state, id);
        }
    };

    // SRTF method based CPU.
    class CPU_SRTF : public CPU {
    public:
        pair<CPUState, uint> run() override {
            if (tasks.empty()) {
                state = idle;
                return make_pair(state, 0u);
            }
            // Choose the task with smallest remaining time; tie-breaker: earliest arrival (lower index)
            size_t best = tasks.size();
            for (size_t i = 0; i < tasks.size(); ++i) {
                if (tasks[i].time == 0) continue;
                if (best == tasks.size() || tasks[i].time < tasks[best].time) {
                    best = i;
                }
            }
            if (best == tasks.size()) {
                // No executable tasks
                tasks.clear();
                state = idle;
                return make_pair(state, 0u);
            }
            state = busy;
            uint id = tasks[best].task_id;
            --tasks[best].time;
            if (tasks[best].time == 0) {
                tasks.erase(tasks.begin() + static_cast<long>(best));
            }
            return make_pair(state, id);
        }
    };

    // Priority method based CPU (lower priority value means higher priority).
    class CPU_PRIORITY : public CPU {
    public:
        pair<CPUState, uint> run() override {
            if (tasks.empty()) {
                state = idle;
                return make_pair(state, 0u);
            }
            // Choose the task with highest priority (smallest priority value)
            size_t best = tasks.size();
            for (size_t i = 0; i < tasks.size(); ++i) {
                if (tasks[i].time == 0) continue;
                if (best == tasks.size() || tasks[i].priority < tasks[best].priority) {
                    best = i;
                }
            }
            if (best == tasks.size()) {
                tasks.clear();
                state = idle;
                return make_pair(state, 0u);
            }
            state = busy;
            uint id = tasks[best].task_id;
            --tasks[best].time;
            if (tasks[best].time == 0) {
                tasks.erase(tasks.begin() + static_cast<long>(best));
            }
            return make_pair(state, id);
        }
    };
}

#endif // _SJTU_CPP_FINAL_SRC_HPP_

