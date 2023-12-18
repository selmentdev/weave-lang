#include "weave/filesystem/FileInfo.hxx"
#include "weave/platform/windows/Helpers.hxx"
#include "weave/time/impl/Time.hxx"

#include "Common.hxx"

namespace weave::filesystem
{
    std::optional<FileInfo> FileInfo::FromPath(std::string_view path)
    {
        platform::windows::win32_FilePathW wpath{};
        if (platform::windows::win32_WidenString(wpath, path))
        {
            WIN32_FILE_ATTRIBUTE_DATA wfad;

            if (GetFileAttributesExW(wpath.c_str(), GetFileExInfoStandard, &wfad) != FALSE)
            {
                ULARGE_INTEGER const size{
                    .LowPart = wfad.nFileSizeLow,
                    .HighPart = wfad.nFileSizeHigh,
                };

                return FileInfo{
                    .CreationTime = time::impl::FromNative(wfad.ftCreationTime, time::DateTimeKind::Local),
                    .LastAccessTime = time::impl::FromNative(wfad.ftLastAccessTime, time::DateTimeKind::Local),
                    .LastWriteTime = time::impl::FromNative(wfad.ftLastWriteTime, time::DateTimeKind::Local),
                    .Size = std::bit_cast<int64_t>(size),
                    .Type = impl::ConvertToFileType(wfad.dwFileAttributes),
                    .Readonly = (wfad.dwFileAttributes & FILE_ATTRIBUTE_READONLY) != 0,
                };
            }
        }

        return {};
    }

    bool FileInfo::Exists(std::string_view path)
    {
        platform::windows::win32_FilePathW wpath{};
        if (platform::windows::win32_WidenString(wpath, path))
        {
            return GetFileAttributesW(wpath.c_str()) != INVALID_FILE_ATTRIBUTES;
        }

        return false;
    }
}
