#pragma once

namespace weave::filesystem
{
    enum class FileSystemError
    {
        EndOfFile,
        InvalidPath,
        PathNotFound,
        FileNotFound,
        DirectoryNotFound,
        ReadFailure,
        WriteFailure,
        AccessDenied,
        PathTooLong,
        SharingViolation,
        FileExists,
        InvalidParameter,
        InvalidOperation,
        InvalidHandle,
        OperationCancelled,
    };
}
