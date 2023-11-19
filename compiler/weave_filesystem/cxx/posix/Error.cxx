#include "Error.hxx"

WEAVE_EXTERNAL_HEADERS_BEGIN

#include <errno.h>

WEAVE_EXTERNAL_HEADERS_END


namespace weave::filesystem::impl
{
    FileSystemError TranslateErrno(uint32_t error)
    {
        switch (error)
        {
        case EACCES:
        case EBADF:
        case EPERM:
            return FileSystemError::AccessDenied;

        case ENOENT:
            return FileSystemError::FileNotFound;

        case ENOTDIR:
            return FileSystemError::DirectoryNotFound;

        case ENAMETOOLONG:
            return FileSystemError::PathTooLong;

        case EWOULDBLOCK:
            return FileSystemError::SharingViolation;

        case ECANCELED:
            return FileSystemError::OperationCancelled;

        case EEXIST:
            return FileSystemError::FileExists;

        default:
            break;
        }

        return FileSystemError::InvalidOperation;
    }
}
