#include "weave/profiler/Profiler.hxx"

namespace weave::profiler::impl
{
    void Serialize(fmt::memory_buffer& buffer, InstantEvent const& e)
    {
        fmt::format_to(
            std::back_inserter(buffer),
            R"__({{ "cat": "{}", "name": "{}", "ph": "i", "ts": {}, "pid": 1, "tid": {} }},)__",
            e.Category,
            e.Name,
            e.Timestamp.SinceEpoch().AsMicroseconds(),
            e.ThreadId);
    }

    void Serialize(fmt::memory_buffer& buffer, CompleteEvent const& e)
    {
        fmt::format_to(
            std::back_inserter(buffer),
            R"__({{ "cat": "{}", "name": "{}", "ph": "X", "ts": {}, "dur": {}, "pid": 1, "tid": {} }},)__",
            e.Category,
            e.Name,
            e.Timestamp.SinceEpoch().AsMicroseconds(),
            e.Duration.AsMicroseconds(),
            e.ThreadId);
    }
}

namespace weave::profiler
{
    Profiler::Profiler() = default;

    CompleteEvent* Profiler::Start(const char* category, const char* name)
    {
        return this->_complete_events.Emplace(
            category,
            name,
            time::Instant::Now(),
            2137);
    }

    void Profiler::Stop(CompleteEvent* e)
    {
        e->Duration = e->Timestamp.QueryElapsed();
    }

    void Profiler::Event(const char* category, const char* name)
    {
        (void)this->_events.Emplace(
            category,
            name,
            time::Instant::Now(),
            2137);
    }

    void Profiler::Serialize(filesystem::FileWriter& writer)
    {
        (void)filesystem::Write(writer, R"__({ "traceEvents": [)__");

        fmt::memory_buffer buffer{};

        this->_complete_events.Enumerate([&](CompleteEvent const* e)
            {
                buffer.clear();
                impl::Serialize(buffer, *e);
                (void)writer.Write(buffer.data(), buffer.size());
                return false;
            });

        this->_events.Enumerate([&](InstantEvent const* e)
            {
                buffer.clear();
                impl::Serialize(buffer, *e);
                (void)writer.Write(buffer.data(), buffer.size());
                return false;
            });

        (void)filesystem::Write(writer, R"__(] })__");
    }
}
