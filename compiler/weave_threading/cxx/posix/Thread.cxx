#include "weave/threading/Thread.hxx"
#include "weave/threading/Runnable.hxx"

#include "Platform.hxx"

namespace weave::threading::impl
{
    constexpr int ConvertThreadPriority(ThreadPriority priority) noexcept
    {
        switch (priority)
        {
        case ThreadPriority::TimeCritical:
        case ThreadPriority::Highest:
            return 30;
        case ThreadPriority::AboveNormal:
            return 25;
        case ThreadPriority::Normal:
            return 15;
        case ThreadPriority::BelowNormal:
            return 5;
        case ThreadPriority::Lower:
            return 14;
        case ThreadPriority::Lowest:
            return 1;
        }

        return 5;
    }

    static void* ThreadEntryPoint(void* context)
    {
        if (context == nullptr)
        {
            WEAVE_BUGCHECK("Thread started without context.");
        }

        Runnable* const runnable = static_cast<Runnable*>(context);
        runnable->Run();

        pthread_exit(nullptr);
        return nullptr;
    }
}

namespace weave::threading
{
    static_assert(sizeof(impl::NativeThread) >= sizeof(impl::PlatformThread));
    static_assert(alignof(impl::NativeThread) >= alignof(impl::PlatformThread));

    ThreadId GetThisThreadId()
    {
        return ThreadId{std::bit_cast<void*>(pthread_self())};
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
        this->AsPlatform().Native = 0;
    }

    Thread::Thread(ThreadStart const& start)
    {
        if (start.Callback == nullptr)
        {
            WEAVE_BUGCHECK("Thread started without callback.");
        }

        pthread_attr_t attr{};

        if (int const rc = pthread_attr_init(&attr); rc != 0)
        {
            WEAVE_BUGCHECK("pthread_attr_init (rc: {}, `{}`)", rc, strerror(rc));
        }

        if (start.StackSize)
        {
            if (int const rc = pthread_attr_setstacksize(&attr, *start.StackSize); rc != 0)
            {
                WEAVE_BUGCHECK("pthread_attr_init (rc: {}, `{}`)", rc, strerror(rc));
            }
        }


        if (int const rc = pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE); rc != 0)
        {
            WEAVE_BUGCHECK("pthread_attr_setdetachstate (rc: {}, `{}`)", rc, strerror(rc));
        }

        if (int const rc = pthread_create(&this->AsPlatform().Native, &attr, impl::ThreadEntryPoint, start.Callback); rc != 0)
        {
            WEAVE_BUGCHECK("pthread_create (rc: {}, `{}`)", rc, strerror(rc));
        }

        if (int const rc = pthread_attr_destroy(&attr); rc != 0)
        {
            WEAVE_BUGCHECK("pthread_attr_destroy (rc: {}, `{}`)", rc, strerror(rc));
        }

        if (this->AsPlatform().Native == 0)
        {
            WEAVE_BUGCHECK("Failed to start thread");
        }

        if (start.Name)
        {
            std::string const name{*start.Name};

            if (int const rc = pthread_setname_np(this->AsPlatform().Native, name.c_str()); rc != 0)
            {
                WEAVE_BUGCHECK("pthread_setname_np (rc: {}, `{}`)", rc, strerror(rc));
            }
        }

        if (start.Priority)
        {
            sched_param sched{};
            int32_t policy = SCHED_RR;

            if (int const rc = pthread_getschedparam(this->AsPlatform().Native, &policy, &sched); rc != 0)
            {
                WEAVE_BUGCHECK("pthread_getschedparam (rc: {}, `{}`)", rc, strerror(rc));
            }

            sched.sched_priority = impl::ConvertThreadPriority(*start.Priority);

            if (int const rc = pthread_setschedparam(this->AsPlatform().Native, policy, &sched); rc != 0)
            {
                WEAVE_BUGCHECK("pthread_setschedparam (rc: {}, `{}`)", rc, strerror(rc));
            }
        }
    }

    Thread::Thread(Thread&& other)
    {
        this->AsPlatform().Native = std::exchange(other.AsPlatform().Native, 0);
    }

    Thread& Thread::operator=(Thread&& other)
    {
        if (this != std::addressof(other))
        {
            std::swap(this->AsPlatform().Native, other.AsPlatform().Native);
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
        if (this->AsPlatform().Native == 0)
        {
            WEAVE_BUGCHECK("Cannot join non-started thread.");
        }

        if (this->AsPlatform().Native == pthread_self())
        {
            WEAVE_BUGCHECK("Joining thread from itself");
        }

        if (int const rc = pthread_join(this->AsPlatform().Native, nullptr); rc != 0)
        {
            WEAVE_BUGCHECK("pthread_setschedparam (rc: {}, `{}`)", rc, strerror(rc));
        }

        this->AsPlatform().Native = 0;
    }

    void Thread::Detach()
    {
        if (this->AsPlatform().Native == 0)
        {
            WEAVE_BUGCHECK("Failed to detach from thread");
        }

        if (int const rc = pthread_detach(this->AsPlatform().Native); rc != 0)
        {
            WEAVE_BUGCHECK("pthread_detach (rc: {}, `{}`)", rc, strerror(rc));
        }

        this->AsPlatform().Native = 0;
    }

    bool Thread::IsJoinable() const
    {
        return this->AsPlatform().Native != 0;
    }

    ThreadId Thread::GetId() const
    {
        return ThreadId{std::bit_cast<void*>(this->AsPlatform().Native)};
    }
}
