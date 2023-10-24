#include "Utils.Windows.hxx"

#if defined(WIN32)

#include <cassert>

WEAVE_EXTERNAL_HEADERS_BEGIN

#define NOMINMAX
#include <windows.h>

WEAVE_EXTERNAL_HEADERS_END

namespace Weave::IO
{
    FileSystemError TranslateErrorCode(uint32_t dwError)
    {
        assert(dwError != ERROR_SUCCESS);

        switch (dwError)
        {
        case ERROR_FILE_NOT_FOUND:
            return FileSystemError::FileNotFound;

        case ERROR_PATH_NOT_FOUND:
            return FileSystemError::PathNotFound;

        case ERROR_ACCESS_DENIED:
            return FileSystemError::AccessDenied;

        case ERROR_FILENAME_EXCED_RANGE:
            return FileSystemError::PathTooLong;

        case ERROR_SHARING_VIOLATION:
            return FileSystemError::SharingViolation;

        case ERROR_FILE_EXISTS:
            return FileSystemError::FileExists;

        case ERROR_OPERATION_ABORTED:
            return FileSystemError::OperationCancelled;

        case ERROR_INVALID_PARAMETER:
            return FileSystemError::InvalidParameter;

        case ERROR_INVALID_HANDLE:
            return FileSystemError::InvalidHandle;

        default:
            break;
        }

        return FileSystemError::InvalidOperation;
    }
}

#endif
