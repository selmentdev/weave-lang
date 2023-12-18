#include "weave/platform/Compiler.hxx"

WEAVE_EXTERNAL_HEADERS_BEGIN

#include <catch_amalgamated.hpp>

WEAVE_EXTERNAL_HEADERS_END

#include "weave/filesystem/Path.hxx"

TEST_CASE("Path - Add Directory Separator")
{
    using namespace weave::filesystem;
    SECTION("Empty path")
    {
        std::string result{};

        path::AddDirectorySeparator(result);

        REQUIRE(result == "/");
    }

    SECTION("Path without separator")
    {
        std::string result = "/path/to/dir";

        path::AddDirectorySeparator(result);

        REQUIRE(result == "/path/to/dir/");
    }

    SECTION("Path with separator")
    {
        std::string result = "/path/to/dir/";

        path::AddDirectorySeparator(result);

        REQUIRE(result == "/path/to/dir/");
    }
}

TEST_CASE("Path - Remove Directory Separator")
{
    using namespace weave::filesystem;
    SECTION("Empty path")
    {
        std::string result{};

        path::RemoveDirectorySeparator(result);

        REQUIRE(result.empty());
    }

    SECTION("Path without separator")
    {
        std::string result = "/path/to/dir";

        path::RemoveDirectorySeparator(result);

        REQUIRE(result == "/path/to/dir");
    }

    SECTION("Path with separator")
    {
        std::string result = "/path/to/dir/";

        path::RemoveDirectorySeparator(result);

        REQUIRE(result == "/path/to/dir");
    }
}

TEST_CASE("Path - NormalizeDirectorySeparators")
{
    using namespace weave::filesystem;
    SECTION("Normal case")
    {
        std::string path = "path/to/file.txt";

        path::NormalizeDirectorySeparators(path);

        REQUIRE(path == "path/to/file.txt");
    }

#if defined(WIN32)
    SECTION("Windows-style path separators")
    {
        std::string path = "path\\to\\file.txt";

        path::NormalizeDirectorySeparators(path);

        REQUIRE(path == "path/to/file.txt");
    }

    SECTION("Mixed separators")
    {
        std::string path = "path\\to/file.txt";

        path::NormalizeDirectorySeparators(path);

        REQUIRE(path == "path/to/file.txt");
    }
#endif

    SECTION("Collapsing separators")
    {
        std::string path = "path//to/file.txt";

        path::NormalizeDirectorySeparators(path);

        REQUIRE(path == "path/to/file.txt");
    }
}

TEST_CASE("Path - Push / Pop")
{
    using namespace weave::filesystem;
    std::string path{};

    path::Push(path, "path");
    REQUIRE(path == "path");

    path::Push(path, "to");
    REQUIRE(path == "path/to");

    path::Push(path, "file.txt");
    REQUIRE(path == "path/to/file.txt");

    SECTION("Pop")
    {
        REQUIRE(path::Pop(path));
        REQUIRE(path == "path/to");

        REQUIRE(path::Pop(path));
        REQUIRE(path == "path");

        REQUIRE(path::Pop(path));
        REQUIRE(path.empty());

        REQUIRE_FALSE(path::Pop(path));
        REQUIRE(path.empty());
    }

    SECTION("Pop with tail")
    {
        std::string tail{};

        REQUIRE(path::Pop(path, tail));
        REQUIRE(path == "path/to");
        REQUIRE(tail == "file.txt");

        REQUIRE(path::Pop(path, tail));
        REQUIRE(path == "path");
        REQUIRE(tail == "to");

        REQUIRE(path::Pop(path, tail));
        REQUIRE(path.empty());
        REQUIRE(tail == "path");

        REQUIRE_FALSE(path::Pop(path, tail));
        REQUIRE(path.empty());
        REQUIRE(tail.empty());
    }
}

TEST_CASE("Path - SetExtension")
{
    using namespace weave::filesystem;

    SECTION("Empty path")
    {
        std::string path{};

        path::SetExtension(path, ".txt");

        REQUIRE(path == ".txt");
    }

    SECTION("Path without extension")
    {
        std::string path = "path/to/file";

        path::SetExtension(path, ".txt");

        REQUIRE(path == "path/to/file.txt");
    }

    SECTION("Path with extension")
    {
        std::string path = "path/to/file.txt";

        path::SetExtension(path, ".cfg");

        REQUIRE(path == "path/to/file.cfg");
    }

    SECTION("Extension with multiple dots")
    {
        std::string path = "path/to/file.txt";

        path::SetExtension(path, "....cfg");

        REQUIRE(path == "path/to/file.cfg");
    }

    SECTION("Extension without dots")
    {
        std::string path = "path/to/file.txt";

        path::SetExtension(path, "cfg");

        REQUIRE(path == "path/to/file.cfg");
    }
}

TEST_CASE("Path - Set Filename")
{
    using namespace weave::filesystem;

    SECTION("Empty path")
    {
        std::string path{};

        path::SetFilename(path, "changed.txt");

        REQUIRE(path == "changed.txt");
    }

    SECTION("Path without filename")
    {
        std::string path = "path/to/";

        path::SetFilename(path, "changed.txt");

        REQUIRE(path == "path/to/changed.txt");
    }

    SECTION("Path with filename")
    {
        std::string path = "path/to/existing.cfg";

        path::SetFilename(path, "changed.txt");

        REQUIRE(path == "path/to/changed.txt");
    }
}

TEST_CASE("Path - GetExtension")
{
    using namespace weave::filesystem;

    SECTION("Empty path")
    {
        std::string const path{};

        REQUIRE(path::GetExtension(path).empty());
    }

    SECTION("Path without trailing separator")
    {
        std::string const path = "path/to/dir";

        REQUIRE(path::GetExtension(path).empty());
    }

    SECTION("Path with trailing separator")
    {
        std::string const path = "path/to/dir/";

        REQUIRE(path::GetExtension(path).empty());
    }

    SECTION("Path with trailing separator and extension")
    {
        std::string const path = "path/to/dir/.txt";

        REQUIRE(path::GetExtension(path) == ".txt");
    }

    SECTION("Path to filename with extension")
    {
        std::string const path = "path/to/dir/filename.txt";

        REQUIRE(path::GetExtension(path) == ".txt");
    }
}

TEST_CASE("Path - GetFilename")
{
    using namespace weave::filesystem;

    SECTION("Empty path")
    {
        std::string const path{};

        REQUIRE(path::GetFilename(path).empty());
    }

    SECTION("Path without trailing separator")
    {
        std::string const path = "path/to/dir";

        REQUIRE(path::GetFilename(path) == "dir");
    }

    SECTION("Path with trailing separator")
    {
        std::string const path = "path/to/dir/";

        REQUIRE(path::GetFilename(path).empty());
    }

    SECTION("Path with trailing separator and extension")
    {
        std::string const path = "path/to/dir/.txt";

        REQUIRE(path::GetFilename(path) == ".txt");
    }

    SECTION("Path to filename with extension")
    {
        std::string const path = "path/to/dir/filename.txt";

        REQUIRE(path::GetFilename(path) == "filename.txt");
    }
}

TEST_CASE("Path - GetFilenameWithoutExtension")
{
    using namespace weave::filesystem;

    SECTION("Empty path")
    {
        std::string const path{};

        REQUIRE(path::GetFilenameWithoutExtension(path).empty());
    }

    SECTION("Path without trailing separator")
    {
        std::string const path = "path/to/dir";

        REQUIRE(path::GetFilenameWithoutExtension(path) == "dir");
    }

    SECTION("Path with trailing separator")
    {
        std::string const path = "path/to/dir/";

        REQUIRE(path::GetFilenameWithoutExtension(path).empty());
    }

    SECTION("Path with trailing separator and extension")
    {
        std::string const path = "path/to/dir/.txt";

        REQUIRE(path::GetFilenameWithoutExtension(path).empty());
    }

    SECTION("Path to filename with extension")
    {
        std::string const path = "path/to/dir/filename.txt";

        REQUIRE(path::GetFilenameWithoutExtension(path) == "filename");
    }
}
