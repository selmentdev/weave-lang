#pragma once
#include "weave/time/Instant.hxx"
#include "weave/memory/TypedLinearAllocator.hxx"
#include "weave/filesystem/FileWriter.hxx"

#include <fmt/format.h>

// Implements https://docs.google.com/document/d/1CvAClvFfyA5R-PhYUmn5OOQtYMH4h6I0nSsKchNAySU/preview
// Usage https://www.chromium.org/developers/how-tos/trace-event-profiling-tool

namespace weave::profiler
{
    struct Event
    {
    public:
        const char* Category{};
        const char* Name{};
        time::Instant Timestamp{};
        uintptr_t ThreadId{};

    public:
        Event(const char* category, const char* name, time::Instant const& timestamp, uintptr_t thread_id)
            : Category{category}
            , Name{name}
            , Timestamp{timestamp}
            , ThreadId{thread_id}
        {
        }
    };

    struct InstantEvent final : public Event
    {
        InstantEvent(const char* category, const char* name, time::Instant const& timestamp, uintptr_t thread_id)
            : Event{category, name, timestamp, thread_id}
        {
        }
    };

    struct CompleteEvent final : public Event
    {
        time::Duration Duration{};

        CompleteEvent(const char* category, const char* name, time::Instant const& timestamp, uintptr_t thread_id)
            : Event{category, name, timestamp, thread_id}
        {
        }
    };

    class Profiler
    {
    private:
        memory::TypedLinearAllocator<InstantEvent> _events{};
        memory::TypedLinearAllocator<CompleteEvent> _complete_events{};

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
