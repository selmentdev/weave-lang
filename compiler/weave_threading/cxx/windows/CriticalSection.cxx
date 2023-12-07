#include "weave/threading/CriticalSection.hxx"

#include "Platform.hxx"

namespace weave::threading
{
    static_assert(sizeof(impl::NativeCriticalSection) >= sizeof(impl::PlatformCriticalSection));
    static_assert(alignof(impl::NativeCriticalSection) >= alignof(impl::PlatformCriticalSection));

    inline impl::PlatformCriticalSection& CriticalSection::AsPlatform()
    {
        return *reinterpret_cast<impl::PlatformCriticalSection*>(&this->_native);
    }

    CriticalSection::CriticalSection()
    {
        InitializeSRWLock(&this->AsPlatform().Native);
    }

    CriticalSection::~CriticalSection() = default;

    void CriticalSection::Enter()
    {
        AcquireSRWLockExclusive(&this->AsPlatform().Native);
    }

    bool CriticalSection::TryEnter()
    {
        return TryAcquireSRWLockExclusive(&this->AsPlatform().Native) != FALSE;
    }

    void CriticalSection::Leave()
    {
        ReleaseSRWLockExclusive(&this->AsPlatform().Native);
    }
}
