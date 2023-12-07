#include "weave/threading/RecursiveCriticalSection.hxx"

#include "Platform.hxx"

namespace weave::threading
{
    static_assert(sizeof(impl::NativeRecursiveCriticalSection) >= sizeof(impl::PlatformRecursiveCriticalSection));
    static_assert(alignof(impl::NativeRecursiveCriticalSection) >= alignof(impl::PlatformRecursiveCriticalSection));

    inline impl::PlatformRecursiveCriticalSection& RecursiveCriticalSection::AsPlatform()
    {
        return *reinterpret_cast<impl::PlatformRecursiveCriticalSection*>(&this->_native);
    }

    RecursiveCriticalSection::RecursiveCriticalSection()
    {
        InitializeCriticalSectionEx(&this->AsPlatform().Native, 4000, 0);
    }

    RecursiveCriticalSection::~RecursiveCriticalSection() = default;

    void RecursiveCriticalSection::Enter()
    {
        EnterCriticalSection(&this->AsPlatform().Native);
    }

    bool RecursiveCriticalSection::TryEnter()
    {
        return TryEnterCriticalSection(&this->AsPlatform().Native) != FALSE;
    }

    void RecursiveCriticalSection::Leave()
    {
        LeaveCriticalSection(&this->AsPlatform().Native);
    }
}
