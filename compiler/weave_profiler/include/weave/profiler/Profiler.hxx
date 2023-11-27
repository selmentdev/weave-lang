#pragma once
#include "weave/time/Instant.hxx"
#include "weave/memory/TypedLinearAllocator.hxx"
#include "weave/filesystem/FileWriter.hxx"

#include <fmt/format.h>

// Implements https://docs.google.com/document/d/1CvAClvFfyA5R-PhYUmn5OOQtYMH4h6I0nSsKchNAySU/preview
// Usage https://www.chromium.org/developers/how-tos/trace-event-profiling-tool

namespace weave::profiler
{
    struct InstantEvent final
    {
        const char* Category{};
        const char* Name{};
        time::Duration Timestamp{};
        uintptr_t ThreadId{};
    };

    struct CompleteEvent final
    {
        const char* Category{};
        const char* Name{};
        time::Duration Started{};
        time::Duration Finished{};
        uintptr_t ThreadId{};
    };

    class Profiler
    {
    private:
        memory::TypedLinearAllocator<InstantEvent> _events{};
        memory::TypedLinearAllocator<CompleteEvent> _complete_events{};
        time::Instant _started{};

    public:
        Profiler();

    public:
        CompleteEvent* Start(const char* category, const char* name);
        void Stop(CompleteEvent* e);
        void Event(const char* category, const char* name);

    public:
        void Serialize(filesystem::FileWriter& writer);
    };

    struct EventScope final
    {
    private:
        CompleteEvent* _handle{};
        Profiler* _profiler{};

    public:
        EventScope(Profiler& profiler, const char* category, const char* name)
            : _handle{profiler.Start(category, name)}
            , _profiler{&profiler}
        {
        }

        ~EventScope()
        {
            this->_profiler->Stop(this->_handle);
        }

        EventScope(EventScope const&) = delete;
        EventScope(EventScope&&) = delete;
        EventScope& operator=(EventScope const&) = delete;
        EventScope& operator=(EventScope&&) = delete;
    };
}
