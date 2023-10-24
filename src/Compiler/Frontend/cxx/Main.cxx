#include "Compiler.Core/Platform/Windows.hxx"
#include "Compiler.Core/HandleTable.hxx"
#include "Compiler.Core/Diagnostic.hxx"
#include "Compiler.Core/IO/FileHandle.hxx"
#include "Compiler.Core/IO/FileSystem.hxx"
#include "Compiler.Core/IO/FileReader.hxx"
#include "Compiler.Core/IO/FileWriter.hxx"

#include <fmt/format.h>


int main()
{
    {
#if defined(WIN32)
        for (size_t capacity = 1; capacity <= (64 << 10u); capacity <<= 1)
        {
            size_t count = 64;
            {
                using namespace Weave::IO;

                auto handle = FileHandle::Create("d:/test.txt", FileMode::CreateAlways, FileAccess::Write).value();
                FileWriter writer{handle, capacity};

                auto write = [&writer](auto& v)
                {
                    auto r = writer.Write(&v, sizeof(v));
                    assert(r.has_value());
                    assert(r.value() == sizeof(v));
                };

                for (size_t i = 0; i < count; ++i)
                {
                    uint32_t x = 0x41414141 + 0x01010101 * static_cast<uint32_t>(i);
                    write(x);
                }

                writer.Flush();
            }
            {
                using namespace Weave::IO;
                FileHandle handle = FileHandle::Create("d:/test.txt", FileMode::OpenExisting, FileAccess::Read).value();
                FileReader reader{handle, capacity};

                auto read = [&reader](auto& v)
                {
                    auto r = reader.Read(&v, sizeof(v));
                    assert(r.has_value());
                    assert(r.value() == sizeof(v));
                };

                size_t current = 0;

                for (; current < 8; ++current)
                {
                    uint32_t const expected = 0x41414141 + 0x01010101 * static_cast<uint32_t>(current);
                    uint32_t v{};
                    read(v);
                    assert(v == expected);
                }

                {
                    uint32_t buffer[8]{};


                    auto r = reader.Read(buffer, sizeof(buffer));
                    assert(r.has_value());
                    assert(r.value() == sizeof(buffer));

                    for (auto const& item : buffer)
                    {
                        uint32_t const expected = 0x41414141 + 0x01010101 * static_cast<uint32_t>(current);
                        assert(expected == item);
                        ++current;
                    }
                }

                for (; current < count; ++current)
                {
                    uint32_t const expected = 0x41414141 + 0x01010101 * static_cast<uint32_t>(current);
                    uint32_t v{};
                    read(v);
                    assert(v == expected);
                }

                bool end{};
                auto r = reader.Read(&end, sizeof(end));
                assert(r.has_value() == false);
                assert(r.error() == FileSystemError::EndOfFile);
            }
        }
#endif
    }
    {
        Weave::HandleTable<std::string> table{};

        fmt::println("=========================");

        for (auto h : table)
        {
            if (auto t = table.get(h))
            {
                fmt::println("{}", *t);
            }
            else
            {
                fmt::println("t is null");
            }
        }

        fmt::println("=========================");

        auto const h1 = table.add("Hello");
        table.add();
        auto const h2 = table.add(std::string{"World"});
        auto const h2a = table.add(42, '9');

        Weave::HandleTable<std::string>::Handle h3 = {2137};

        if (auto t1 = table.get(h1))
        {
            fmt::println("{}", *t1);
        }
        else
        {
            fmt::println("t1 is null");
        }

        if (auto t2 = table.get(h2))
        {
            fmt::println("{}", *t2);
        }
        else
        {
            fmt::println("t2 is null");
        }

        if (auto t3 = table.get(h3))
        {
            fmt::println("{}", *t3);
        }
        else
        {
            fmt::println("t3 is null");
        }

        for (auto h : table)
        {
            if (auto t = table.get(h))
            {
                fmt::println("{}", *t);
            }
            else
            {
                fmt::println("t is null");
            }
        }
    }
    {
        Weave::DiagnosticSink sink{};
        auto hr = sink.AddError({}, fmt::format("This is message"));
        sink.AddInfo(hr, {}, fmt::format("this is some info"));
        sink.AddHint(hr, {}, fmt::format("this is some info"));
        sink.AddWarning({}, fmt::format("this is warning as well"));

        fmt::println("=========================");
        sink.Enumerate([&](Weave::DiagnosticSink::Entry const& entry)
            {
                fmt::println("{} {} {}", entry.Source.Start.Offset, entry.Level, entry.Message);
            });
        fmt::println("=========================");
        sink.EnumerateRoots([&](Weave::DiagnosticSink::Entry const& entry, Weave::DiagnosticSink::Handle handle)
            {
                fmt::println("[root: {}] = {} {} {}", handle.Value, entry.Source.Start.Offset, entry.Level, entry.Message);
            });

        fmt::println("=========================");
        sink.EnumerateChildren(hr, [&](Weave::DiagnosticSink::Handle h)
            {
                auto const& ch = sink.TryGet(h);
                fmt::println("children of root: {} {} {}", ch->Source.Start.Offset, ch->Level, ch->Message);
            });
        fmt::println("=========================");
    }
    return 0;
}
