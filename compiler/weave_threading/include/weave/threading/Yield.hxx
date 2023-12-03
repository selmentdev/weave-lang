#pragma once
#include "weave/time/Duration.hxx"

#include <cstddef>

namespace weave::threading::impl
{
    inline constexpr size_t YieldThreadTreshold = 0xFF;
}

namespace weave::threading
{
    enum class YieldTarget
    {
        None,
        AnyThreadOnAnyProcessor,
        AnyThreadOnSameProcessor,
        SameOrHigherPriorityOnAnyProcessor,
    };

    void YieldThread();

    void YieldThread(YieldTarget target);

    void Sleep(time::Duration const& timeout);

    void Pause();
}

namespace weave::threading
{

    template <typename PredicateT = bool()>
    void WaitForCompletion(PredicateT&& predicate) noexcept
    {
        size_t counter{};

        while (!predicate())
        {
            ++counter;

            if ((counter & impl::YieldThreadTreshold) != 0)
            {
                Pause();
            }
            else
            {
                YieldThread();
            }
        }
    }

    template <typename PredicateT = bool(), typename ActionT = bool()>
    void WaitForCompletion(PredicateT&& predicate, ActionT&& action) noexcept
    {
        size_t counter{};

        while (!predicate())
        {
            if (action())
            {
                // Restart spinning here.
                counter = 0;
            }
            else
            {
                ++counter;

                if ((counter & impl::YieldThreadTreshold) != 0)
                {
                    Pause();
                }
                else
                {
                    YieldThread();
                }
            }
        }
    }

    template <typename PredicateT = bool()>
    bool TryWaitForCompletion(PredicateT&& predicate, size_t spins) noexcept
    {
        for (size_t i = 0; i < spins; ++i)
        {
            if (predicate())
            {
                return true;
            }
            else
            {
                Pause();
            }
        }

        return false;
    }

    template <typename PredicateT = bool()>
    bool TryWaitForCompletion(
        PredicateT&& predicate,
        size_t spins,
        size_t yields,
        YieldTarget target) noexcept
    {
        for (size_t i = 0; i < yields; ++i)
        {
            for (size_t j = 0; j < spins; ++j)
            {
                if (predicate())
                {
                    // Wait was successful.
                    return true;
                }
                else
                {
                    // Pause CPU for some time.
                    Pause();
                }
            }

            // Otherwise, yield.
            YieldThread(target);
        }

        return false;
    }
}
