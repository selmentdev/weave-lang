#include "Weave.Core/StringBuffer.hxx"
#include "Weave.Core/IO/FileSystem.hxx"
#include "Weave.Core/Platform/Compiler.hxx"

#if defined(WIN32)

#include "Weave.Core/IO/FileHandle.hxx"
#include "Weave.Core/Platform/Windows.hxx"

#include "Utils.Windows.hxx"

WEAVE_EXTERNAL_HEADERS_BEGIN

#define NOMINMAX
#include <windows.h>

WEAVE_EXTERNAL_HEADERS_END

namespace Weave::IO
{
    std::expected<std::string, FileSystemError> File::Copy(std::string_view existing, std::string_view destination, NameCollisionResolve collision)
    {
        StringBuffer<wchar_t, MAX_PATH> wExisting{};
        StringBuffer<wchar_t, MAX_PATH> wDestination{};

        if (WidenString(wExisting, existing) and WidenString(wDestination, destination))
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
                    return std::unexpected(TranslateErrorCode(dwError));
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
                        return std::unexpected(FileSystemError::FileExists);
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
                        return std::unexpected(FileSystemError::InvalidParameter);
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
                return std::unexpected(FileSystemError::FileExists);
            }

            return std::string{destination};
        }

        return std::unexpected(FileSystemError::InvalidParameter);
    }
}

#endif
