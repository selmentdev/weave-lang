#include "weave/threading/Thread.hxx"
#include "weave/threading/Runnable.hxx"
#include "weave/bugcheck/BugCheck.hxx"
#include "weave/platform/windows/String.hxx"

#include "Platform.hxx"

namespace weave::threading::impl
{
    constexpr int ConvertThreadPriority(ThreadPriority value) noexcept
    {
        switch (value)
        {
        case ThreadPriority::TimeCritical:
            return THREAD_PRIORITY_TIME_CRITICAL;
        case ThreadPriority::Highest:
            return THREAD_PRIORITY_HIGHEST;
        case ThreadPriority::AboveNormal:
            return THREAD_PRIORITY_ABOVE_NORMAL;
        case ThreadPriority::Normal:
            return THREAD_PRIORITY_NORMAL;
        case ThreadPriority::BelowNormal:
            return THREAD_PRIORITY_BELOW_NORMAL;
        case ThreadPriority::Lower:
            return THREAD_PRIORITY_NORMAL - 1;
        case ThreadPriority::Lowest:
            return THREAD_PRIORITY_LOWEST;
        }

        return THREAD_PRIORITY_NORMAL;
    }

    static DWORD WINAPI ThreadEntryPoint(LPVOID lpThreadParameter)
    {
        if (lpThreadParameter == nullptr)
        {
            WEAVE_BUGCHECK("Thread started without context.");
        }

        if (Runnable* const runnable = static_cast<Runnable*>(lpThreadParameter); runnable != nullptr)
        {
            runnable->Run();
        }

        return 0;
    }
}

namespace weave::threading
{
    static_assert(sizeof(impl::NativeThread) >= sizeof(impl::PlatformThread));
    static_assert(alignof(impl::NativeThread) >= alignof(impl::PlatformThread));

    ThreadId GetThisThreadId()
    {
        return ThreadId{std::bit_cast<void*>(static_cast<uintptr_t>(GetCurrentThreadId()))};
    }
}


namespace weave::threading
{
    inline impl::PlatformThread& Thread::AsPlatform()
    {
        return *reinterpret_cast<impl::PlatformThread*>(&this->_native);
    }

    inline impl::PlatformThread const& Thread::AsPlatform() const
    {
        return *reinterpret_cast<impl::PlatformThread const*>(&this->_native);
    }

    Thread::Thread()
    {
        this->AsPlatform() = {};
    }

    Thread::Thread(ThreadStart const& start)
    {
        if (start.Callback == nullptr)
        {
            WEAVE_BUGCHECK("Cannot create thread without runnable object");
        }

        size_t const stackSize = start.StackSize.value_or(0);

        DWORD dwCreationFlags = CREATE_SUSPENDED;

        if (start.StackSize)
        {
            dwCreationFlags |= STACK_SIZE_PARAM_IS_A_RESERVATION;
        }

        this->AsPlatform().Handle = CreateThread(
            nullptr,
            stackSize,
            impl::ThreadEntryPoint,
            start.Callback,
            dwCreationFlags,
            &this->AsPlatform().Id);

        if (this->AsPlatform().Handle != nullptr)
        {
            if (start.Name)
            {
                platform::windows::win32_string_buffer<wchar_t, 128> wname{};
                platform::windows::win32_WidenString(wname, *start.Name);

                SetThreadDescription(this->AsPlatform().Handle, wname.data());
            }

            if (start.Priority)
            {
                SetThreadPriority(this->AsPlatform().Handle, impl::ConvertThreadPriority(*start.Priority));
            }

            ResumeThread(this->AsPlatform().Handle);
        }
        else
        {
            WEAVE_BUGCHECK("Cannot create thread");
        }
    }

    Thread::Thread(Thread&& other)
    {
        this->AsPlatform() = std::exchange(other.AsPlatform(), {});
    }

    Thread& Thread::operator=(Thread&& other)
    {
        if (this != std::addressof(other))
        {
            std::swap(this->AsPlatform(), other.AsPlatform());
        }

        return *this;
    }

    Thread::~Thread()
    {
        if (this->IsJoinable())
        {
            if (this->GetId() == threading::GetThisThreadId())
            {
                WEAVE_BUGCHECK("Trying do destroy thread from itself");
            }

            this->Join();
        }
    }

    void Thread::Join()
    {
        if (this->AsPlatform().Handle == nullptr)
        {
            WEAVE_BUGCHECK("Failed to join thread");
        }

        if (this->AsPlatform().Id == GetCurrentThreadId())
        {
            WEAVE_BUGCHECK("Joining thread from itself");
        }

        WaitForSingleObject(this->AsPlatform().Handle, INFINITE);

        CloseHandle(this->AsPlatform().Handle);

        this->AsPlatform() = {};
    }

    void Thread::Detach()
    {
        if (this->AsPlatform().Handle == nullptr)
        {
            WEAVE_BUGCHECK("Failed to detach from thread");
        }

        CloseHandle(this->AsPlatform().Handle);

        this->AsPlatform() = {};
    }

    bool Thread::IsJoinable() const
    {
        return this->AsPlatform().Handle != nullptr;
    }

    ThreadId Thread::GetId() const
    {
        return ThreadId{std::bit_cast<void*>(static_cast<uintptr_t>(this->AsPlatform().Id))};
    }
}
