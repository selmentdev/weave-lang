#pragma once
#include <string>
#include <string_view>

namespace weave::filesystem::path
{
    void AddDirectorySeparator(std::string& path);
    void RemoveDirectorySeparator(std::string& path);

    void NormalizeDirectorySeparators(std::string& path);

    void Push(std::string& path, std::string_view part);
    bool Pop(std::string& path);
    bool Pop(std::string& path, std::string& tail);

    void SetExtension(std::string& path, std::string_view extension);
    void SetFilename(std::string& path, std::string_view filename);

    std::string_view GetExtension(std::string_view path);
    std::string_view GetFilename(std::string_view path);
    std::string_view GetFilenameWithoutExtension(std::string_view path);
}
