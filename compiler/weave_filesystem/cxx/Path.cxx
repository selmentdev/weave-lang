#include "weave/filesystem/Path.hxx"

namespace weave::filesystem::impl
{
#if defined(WIN32)
    inline constexpr char DirectorySeparatorChar = '/';
    inline constexpr char AlternativeDirectorySeparatorChar = '\\';
    inline constexpr char VolumeSeparatorChar = ':';
    inline constexpr char PathSeparatorChar = ';';
    inline constexpr char ExtensionSeparatorChar = '.';
    inline constexpr std::string_view AllDirectorySeparatorsString = "/\\";
#elif defined(__linux__)
    inline constexpr char DirectorySeparatorChar = '/';
    inline constexpr char AlternativeDirectorySeparatorChar = '/';
    inline constexpr char VolumeSeparatorChar = '/';
    inline constexpr char PathSeparatorChar = ':';
    inline constexpr char ExtensionSeparatorChar = '.';
    inline constexpr std::string_view AllDirectorySeparatorsString = "/";
#else
#error "Unsupported platform"
#endif

    constexpr bool IsDirectorySeparator(char c)
    {
#if defined(WIN32)
        return c == DirectorySeparatorChar or c == AlternativeDirectorySeparatorChar;
#else
        return c == DirectorySeparatorChar;
#endif
    }

    constexpr std::string_view::size_type FindLastDirectorySeparator(std::string_view path)
    {
#if defined(WIN32)
        return path.find_last_of(AllDirectorySeparatorsString);
#elif defined(__linux__)
        return path.find_last_of(DirectorySeparatorChar);
#else
#error "Unsupported platform"
#endif
    }
}

namespace weave::filesystem::path
{
    void AddDirectorySeparator(std::string& path)
    {
        if (path.empty() or not impl::IsDirectorySeparator(path.back()))
        {
            path.push_back(impl::DirectorySeparatorChar);
        }
    }

    void RemoveDirectorySeparator(std::string& path)
    {
        if (not path.empty() and impl::IsDirectorySeparator(path.back()))
        {
            path.pop_back();
        }
    }

    void NormalizeDirectorySeparators(std::string& path)
    {
        auto it = path.begin();
        auto const end = path.end();

        auto out = it;

        while (it != end)
        {
            if (impl::IsDirectorySeparator(*it))
            {
                // Skip all consecutive directory separators
                *out++ = impl::DirectorySeparatorChar;

                while ((it != end) and impl::IsDirectorySeparator(*it))
                {
                    ++it;
                }
            }
            else
            {
                // Copy character
                *out++ = *it++;
            }
        }

        // Trim path
        path.resize(out - path.begin());
    }

    void Push(std::string& path, std::string_view part)
    {
        if (path.empty())
        {
            path = part;
        }
        else
        {
            AddDirectorySeparator(path);
            path.append(part);
        }
    }

    bool Pop(std::string& path)
    {
        if (not path.empty())
        {
            // Find directory separator and remove everything after it
            auto const separator = impl::FindLastDirectorySeparator(path);

            if (separator != std::string_view::npos)
            {
                path.resize(separator);
            }
            else
            {
                path.clear();
            }

            return true;
        }

        return false;
    }

    bool Pop(std::string& path, std::string& tail)
    {
        if (not path.empty())
        {
            // Find directory separator and remove everything after it
            auto const separator = impl::FindLastDirectorySeparator(path);

            if (separator != std::string_view::npos)
            {
                // Tail is everything after the separator
                tail = path.substr(separator + 1);
                path.resize(separator);
            }
            else
            {
                // Path don't contain any directory separators, so the whole path is the tail
                tail = std::move(path);
                path.clear();
            }

            return true;
        }

        tail.clear();
        return false;
    }

    void SetExtension(std::string& path, std::string_view extension)
    {
        // Strip any leading dots
        while (extension.starts_with('.'))
        {
            extension.remove_prefix(1);
        }

        std::string_view::size_type const separator = path.find_last_of(impl::ExtensionSeparatorChar);

        if (extension.empty())
        {
            // Remove extension
            if (separator != std::string_view::npos)
            {
                path.resize(separator);
            }
        }
        else
        {
            // Append extension
            if (separator == std::string_view::npos)
            {
                // Extension separator not found, so append it
                path.push_back(impl::ExtensionSeparatorChar);
            }
            else
            {
                // Extension separator found, so remove everything after it
                path.resize(separator + 1);
            }

            path.append(extension);
        }
    }

    void SetFilename(std::string& path, std::string_view filename)
    {
        Pop(path);
        Push(path, filename);
    }

    std::string_view GetExtension(std::string_view path)
    {
        std::string_view::size_type const separator = path.find_last_of(impl::ExtensionSeparatorChar);

        if (separator != std::string_view::npos)
        {
            return path.substr(separator);
        }
        else
        {
            return {};
        }
    }

    std::string_view GetFilename(std::string_view path)
    {
        std::string_view::size_type const separator = impl::FindLastDirectorySeparator(path);

        if (separator != std::string_view::npos)
        {
            return path.substr(separator + 1);
        }

        return {};
    }

    std::string_view GetFilenameWithoutExtension(std::string_view path)
    {
        std::string_view filename = GetFilename(path);

        std::string_view::size_type const separator = filename.find_last_of(impl::ExtensionSeparatorChar);

        if (separator != std::string_view::npos)
        {
            filename = filename.substr(0, separator);
        }

        // ReSharper disable once CppDFALocalValueEscapesFunction
        return filename;
    }
}