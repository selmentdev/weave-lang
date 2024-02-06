#include "weave/filesystem/DirectoryEnumerator.hxx"
#include "weave/core/String.hxx"

namespace weave::filesystem
{
    auto EnumerateDirectoryRecursive(std::string_view root, std::string_view pattern) -> std::expected<std::vector<std::string>, platform::SystemError>
    {
        std::vector<DirectoryEnumerator> stack{};

        stack.emplace_back(root);
        std::vector<std::string> result{};

        while (not stack.empty())
        {
            DirectoryEnumerator& enumerator = stack.back();
            if (std::optional<std::expected<DirectoryEntry, platform::SystemError>> const& next = enumerator.Next(); next)
            {
                std::expected<DirectoryEntry, platform::SystemError> const& item = *next;

                if (item)
                {
                    DirectoryEntry const& entry = *item;

                    if (entry.Type == FileType::Directory)
                    {
                        if (not entry.Path.ends_with("/..") and not entry.Path.ends_with("/."))
                        {
                            stack.emplace_back(entry.Path);
                        }
                    }
                    else if (entry.Type == FileType::File)
                    {
                        if (core::MatchWildcard<char>(pattern, entry.Path))
                        {
                            result.push_back(entry.Path);
                        }
                    }
                }
                else
                {
                    stack.pop_back();
                    // Happily ignore errors here
                }
            }
            else
            {
                stack.pop_back();
            }
        }

        return result;
    }
}
