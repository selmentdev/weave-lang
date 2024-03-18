#include "weave/platform/Compiler.hxx"
#include "weave/platform/windows/string.hxx"
#include "weave/filesystem/FileSystem.hxx"
#include "weave/filesystem/FileHandle.hxx"
#include "weave/platform/SystemError.hxx"
#include "weave/bugcheck/Assert.hxx"
#include "weave/platform/windows/PlatformHeaders.hxx"

namespace weave::filesystem
{
    std::expected<std::string, platform::SystemError> File::Copy(std::string_view existing, std::string_view destination, NameCollisionResolve collision)
    {
        platform::windows::win32_FilePathW wExisting{};
        platform::windows::win32_FilePathW wDestination{};

        if (platform::windows::win32_WidenString(wExisting, existing) and platform::windows::win32_WidenString(wDestination, destination))
        {
            bool exists;
            bool fail;

            if (DWORD const dwAttributes = GetFileAttributesW(wDestination.data()); dwAttributes == INVALID_FILE_ATTRIBUTES)
            {
                DWORD const dwError = GetLastError();

                if ((dwError == ERROR_PATH_NOT_FOUND) or (dwError == ERROR_FILE_NOT_FOUND))
                {
                    exists = false;
                }
                else
                {
                    return std::unexpected(platform::impl::SystemErrorFromWin32Error(dwError));
                }
            }
            else
            {
                exists = true;
            }

            if (exists)
            {
                switch (collision)
                {
                case NameCollisionResolve::Fail:
                    {
                        return std::unexpected(platform::SystemError::FileExists);
                    }

                case NameCollisionResolve::GenerateUnique:
                    {
                        fail = false;
                        WEAVE_ASSERT(false, "Not implemented");
                        break;
                    }

                case NameCollisionResolve::Overwrite:
                    {
                        fail = false;
                        break;
                    }

                default:
                    {
                        return std::unexpected(platform::SystemError::InvalidArgument);
                    }
                }
            }
            else
            {
                fail = true;
            }

            COPYFILE2_EXTENDED_PARAMETERS params{
                .dwSize = sizeof(params),
                .dwCopyFlags = static_cast<DWORD>(fail ? COPY_FILE_FAIL_IF_EXISTS : 0),
            };

            if (FAILED(CopyFile2(wExisting.data(), wDestination.data(), &params)))
            {
                return std::unexpected(platform::SystemError::FileExists);
            }

            return std::string{destination};
        }

        return std::unexpected(platform::SystemError::InvalidArgument);
    }

    std::expected<void, platform::SystemError> File::Remove(
        std::string_view path)
    {
        platform::windows::win32_FilePathW wPath{};

        if (platform::windows::win32_WidenString(wPath, path))
        {
            if (DeleteFileW(wPath.data()) == FALSE)
            {
                return std::unexpected(platform::impl::SystemErrorFromWin32Error(GetLastError()));
            }

            return {};
        }

        return std::unexpected(platform::SystemError::InvalidArgument);
    }
}

namespace weave::filesystem
{
    std::expected<void, platform::SystemError> Directory::Create(
        std::string_view path)
    {
        platform::windows::win32_FilePathW wPath{};

        if (platform::windows::win32_WidenString(wPath, path))
        {
            if (CreateDirectoryW(wPath.data(), nullptr) == FALSE)
            {
                return std::unexpected(platform::impl::SystemErrorFromWin32Error(GetLastError()));
            }

            return {};
        }

        return std::unexpected(platform::SystemError::InvalidArgument);
    }

    std::expected<void, platform::SystemError> Directory::Remove(
        std::string_view path)
    {
        platform::windows::win32_FilePathW wPath{};

        if (platform::windows::win32_WidenString(wPath, path))
        {
            if (RemoveDirectoryW(wPath.data()) == FALSE)
            {
                return std::unexpected(platform::impl::SystemErrorFromWin32Error(GetLastError()));
            }

            return {};
        }

        return std::unexpected(platform::SystemError::InvalidArgument);
    }
}
