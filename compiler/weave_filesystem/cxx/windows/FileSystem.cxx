#include "weave/platform/Compiler.hxx"
#include "weave/platform/windows/string.hxx"
#include "weave/filesystem/FileSystem.hxx"
#include "weave/filesystem/FileHandle.hxx"
#include "weave/platform/SystemError.hxx"

WEAVE_EXTERNAL_HEADERS_BEGIN

#define NOMINMAX
#include <windows.h>

WEAVE_EXTERNAL_HEADERS_END

namespace weave::filesystem
{
    std::expected<std::string, platform::SystemError> File::Copy(std::string_view existing, std::string_view destination, NameCollisionResolve collision)
    {
        platform::StringBuffer<wchar_t, MAX_PATH> wExisting{};
        platform::StringBuffer<wchar_t, MAX_PATH> wDestination{};

        if (platform::WidenString(wExisting, existing) and platform::WidenString(wDestination, destination))
        {
            bool exists;
            bool fail;

            if (DWORD const dwAttributes = GetFileAttributesW(wDestination.GetBuffer()); dwAttributes == INVALID_FILE_ATTRIBUTES)
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
                        assert(false); // not implemented
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

            if (FAILED(CopyFile2(wExisting.GetBuffer(), wDestination.GetBuffer(), &params)))
            {
                return std::unexpected(platform::SystemError::FileExists);
            }

            return std::string{destination};
        }

        return std::unexpected(platform::SystemError::InvalidArgument);
    }
}
