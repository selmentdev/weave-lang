#pragma once
#include "weave/threading/Runnable.hxx"
#include "weave/bugcheck/Assert.hxx"

#include <atomic>

namespace weave::threading
{
    struct TaskHandle
    {
        void* Native;

        [[nodiscard]] constexpr auto operator<=>(TaskHandle const&) const = default;
    };
}

namespace weave::threading::impl
{

}

namespace weave::threading
{
    enum class TaskState
    {
        Initialized,
        Dispatched,
        Executing,
        Completed,
    };

    class Task
    {
        std::atomic_size_t Waiters{0};

        TaskHandle Parent{};
        TaskState State{TaskState::Completed};
        threading::Runnable* Callback{};

        void InitializeBarrier(TaskHandle parent)
        {
            WEAVE_ASSERT(this->Waiters == 0);
            WEAVE_ASSERT(this->State == TaskState::Completed);

            this->Waiters = 1;
            this->Callback = nullptr;
            this->Parent = parent;
            this->State = TaskState::Initialized;

            if (Task* task = static_cast<Task*>(parent.Native))
            {
                ++task->Waiters;
            }
        }

        void InitializeRunnable(threading::Runnable* callback, TaskHandle parent)
        {
            WEAVE_ASSERT(this->Waiters == 0);
            WEAVE_ASSERT(this->State == TaskState::Completed);

            this->Waiters = 1;
            this->Callback = callback;
            this->Parent = parent;
            this->State = TaskState::Initialized;

            if (Task* task = static_cast<Task*>(parent.Native))
            {
                ++task->Waiters;
            }
        }

        void Finish()
        {
            if (--this->Waiters == 0)
            {
                WEAVE_ASSERT(this->State == TaskState::Executing);
                this->State = TaskState::Completed;

                if (Task* task = static_cast<Task*>(this->Parent.Native))
                {
                    task->Finish();
                }
            }
        }

        void Dispatched()
        {
            WEAVE_ASSERT(this->State == TaskState::Initialized);
            this->State = TaskState::Dispatched;
        }

        void Execute()
        {
            WEAVE_ASSERT(this->State == TaskState::Dispatched);
            this->State = TaskState::Executing;

            if (threading::Runnable* const callback = this->Callback)
            {
                callback->Run();
            }

            this->Finish();
        }

        bool IsFinished() const
        {
            return this->Waiters == 0;
        }
    };
}
