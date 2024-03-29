#include "weave/Version.hxx"
#include "weave/commandline/CommandLineParser.hxx"
#include "weave/filesystem/DirectoryEnumerator.hxx"
#include "weave/bugcheck/Assert.hxx"
#include "weave/system/Process.hxx"
#include "weave/memory/PageAllocator.hxx"

#include <utility>

int main(int argc, char* argv[])
{
#if defined(_WIN32)
    {
        size_t buffer_size = 16u << 20u;
        void* buffer1 = VirtualAlloc(reinterpret_cast<void*>(0x1000'0000u), buffer_size, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
        void* buffer2 = VirtualAlloc(reinterpret_cast<void*>(0x3000'0000u), buffer_size, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
        void* buffer3 = VirtualAlloc(reinterpret_cast<void*>(0x2000'0000u), buffer_size, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);

        weave::memory::v2::PageFrameDatabase db{};
        weave::memory::v2::PageFrameDatabase_Initialize(db);
        weave::memory::v2::PageFrameDatabase_CreatePageFrame(db, buffer2, buffer_size, nullptr, 0);
        weave::memory::v2::PageFrameDatabase_CreatePageFrame(db, buffer1, buffer_size, nullptr, 0);
        weave::memory::v2::PageFrameDatabase_CreatePageFrame(db, buffer3, buffer_size, nullptr, 0);

        void* p1 = weave::memory::v2::PageFrameDatabase_AllocatePages(db, weave::memory::v2::PageFrameGranularity, weave::memory::v2::PageFrameGranularity, nullptr);
        void* p2 = weave::memory::v2::PageFrameDatabase_AllocatePages(db, weave::memory::v2::PageFrameGranularity, weave::memory::v2::PageFrameGranularity, nullptr);
        void* p3 = weave::memory::v2::PageFrameDatabase_AllocatePagesExplicit(db, reinterpret_cast<void*>(0x2004'0000u), weave::memory::v2::PageFrameGranularity, nullptr);
        weave::memory::v2::PageFrameDatabase_DeallocatePages(db, p1);
        weave::memory::v2::PageFrameDatabase_DeallocatePages(db, p2);
        weave::memory::v2::PageFrameDatabase_DeallocatePages(db, p3);
    }
#endif


    std::string_view const appname = weave::filesystem::path::GetFilenameWithoutExtension(argv[0]);

    fmt::println("working-directory: {}", weave::system::GetStartupDirectory());

    weave::commandline::ArgumentEnumerator arguments{argc, argv};

    weave::commandline::ArgumentParser parser{};
    parser.AddOption("-help", "Display this help message");
    parser.AddOption("-version", "Display version information");
    parser.AddOption("-verbose", "Enable verbose output");
    parser.AddOption("-o:output", "The output directory", "path");
    parser.AddOption("-o:immediate", "The directory for immediate files", "path");
    parser.AddOption("-x:print-syntax-tree", "Prints syntax tree");
    parser.AddOption("-x:print-semantic-tree", "Prints semantic tree");
    parser.AddOption("-w:disable", "Disables a specific warning", "warning");
    parser.AddOption("-w:enable", "Enables a specific warning", "warning");
    parser.AddOption("-w:default", "Restores default for a specific warning", "warning");

    parser.AddCommand("add", "Adds new");
    parser.AddCommand("remove", "Removes");

    if (auto result = parser.Parse(arguments))
    {
        auto command = result->GetCommand();

        if (command == "add")
        {
            weave::commandline::ArgumentParser commandAdd{};
            commandAdd.AddOption("-help", "ho ho ho this is other help!");
            commandAdd.AddOption("-version", "this is silly version option");

            if (auto resultAdd = commandAdd.Parse(arguments))
            {
                if (resultAdd->Contains("-help"))
                {
                    commandAdd.PrintUsage(appname);
                    return 0;
                }

                if (resultAdd->Contains("-version"))
                {
                    fmt::println("weave-docgen add");
                    fmt::println("branch: {}", WEAVE_PROJECT_BRANCH);
                    fmt::println("hash: {}", WEAVE_PROJECT_HASH);
                    return 0;
                }

                for (auto const& value : resultAdd->GetPositional())
                {
                    fmt::println("positional: {}", value);
                }

                if (auto value = resultAdd->GetValue("-o:output"))
                {
                    fmt::println("output: {}", *value);
                }

                if (auto value = resultAdd->GetValue("-o:immediate"))
                {
                    fmt::println("immediate: {}", *value);
                }

                for (auto const& value : resultAdd->GetValues("-w:disable"))
                {
                    fmt::println("disable: {}", value);
                }

                for (auto const& value : resultAdd->GetValues("-w:enable"))
                {
                    fmt::println("enable: {}", value);
                }

                for (auto const& value : resultAdd->GetValues("-w:default"))
                {
                    fmt::println("default: {}", value);
                }

                return 0;
            }
            else
            {
                fmt::print("Error: {}, Option: {}\n", std::to_underlying(resultAdd.error().Reason), resultAdd.error().Argument);
                return -1;
            }
        }


        if (result->Contains("-version"))
        {
            fmt::println("weave-docgen");
            fmt::println("branch: {}", WEAVE_PROJECT_BRANCH);
            fmt::println("hash: {}", WEAVE_PROJECT_HASH);
            return 0;
        }

        if (result->Contains("-help"))
        {
            parser.PrintUsage(appname);
            return 0;
        }
    }
    else
    {
        fmt::print("Error: {}, Option: {}\n", std::to_underlying(result.error().Reason), result.error().Argument);
        return -1;
    }

    return 0;
}
