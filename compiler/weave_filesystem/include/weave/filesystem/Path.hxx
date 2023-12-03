#pragma once
#include <string>
#include <string_view>

namespace weave::filesystem::path
{
    inline void AddDirectorySeparator(std::string& path)
    {
        if (path.empty() or ((path.back() != '/') and (path.back() != '\\')))
        {
            path.push_back('/');
        }
    }

    inline void RemoveDirectorySeparator(std::string& path)
    {
        if (not path.empty() and ((path.back() == '/') or (path.back() == '\\')))
        {
            path.pop_back();
        }
    }

    inline void Append(std::string& path, std::string_view part)
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

    inline std::string_view GetFileName(std::string_view path)
    {
        if (path.empty())
        {
            return path;
        }

        size_t const separator = path.find_last_of("\\/");

        if (separator == std::string_view::npos)
        {
            return path;
        }

        return path.substr(separator + 1);
    }

    inline std::string_view GetFileNameWithoutExtension(std::string_view path)
    {
        if (path.empty())
        {
            return path;
        }

        size_t const separator = path.find_last_of("\\/");

        if (separator == std::string_view::npos)
        {
            path = path.substr(0, path.find_last_of('.'));
            return path;
        }

        path = path.substr(separator + 1);
        path = path.substr(0, path.find_last_of('.'));
        return path;
    }

    inline std::string_view GetExtension(std::string_view path)
    {
        size_t const separator = path.find_last_of('.');

        if (separator == std::string_view::npos)
        {
            return {};
        }

        return path.substr(separator);
    }
}
