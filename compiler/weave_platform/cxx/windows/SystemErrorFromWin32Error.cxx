// ReSharper disable CppClangTidyBugproneBranchClone
#include "weave/platform/Compiler.hxx"
#include "weave/platform/SystemError.hxx"

#include "weave/platform/windows/PlatformHeaders.hxx"

WEAVE_EXTERNAL_HEADERS_BEGIN

#include <winsock2.h>

WEAVE_EXTERNAL_HEADERS_END

namespace weave::platform::impl
{
    SystemError SystemErrorFromWin32Error(uint32_t error)
    {
        switch (error)
        {
        case ERROR_INVALID_FUNCTION:
            return SystemError::FunctionNotSupported;
        case ERROR_FILE_NOT_FOUND:
            return SystemError::NoSuchFileOrDirectory;
        case ERROR_PATH_NOT_FOUND:
            return SystemError::NoSuchFileOrDirectory;
        case ERROR_TOO_MANY_OPEN_FILES:
            return SystemError::TooManyFilesOpen;
        case ERROR_ACCESS_DENIED:
            return SystemError::PermissionDenied;
        case ERROR_INVALID_HANDLE:
            return SystemError::InvalidArgument;
        case ERROR_NOT_ENOUGH_MEMORY:
            return SystemError::NotEnoughMemory;
        case ERROR_INVALID_ACCESS:
            return SystemError::PermissionDenied;
        case ERROR_OUTOFMEMORY:
            return SystemError::NotEnoughMemory;
        case ERROR_INVALID_DRIVE:
            return SystemError::NoSuchDevice;
        case ERROR_CURRENT_DIRECTORY:
            return SystemError::PermissionDenied;
        case ERROR_NOT_SAME_DEVICE:
            return SystemError::CrossDeviceLink;
        case ERROR_WRITE_PROTECT:
            return SystemError::PermissionDenied;
        case ERROR_BAD_UNIT:
            return SystemError::NoSuchDevice;
        case ERROR_NOT_READY:
            return SystemError::ResourceUnavailableTryAgain;
        case ERROR_SEEK:
            return SystemError::IoError;
        case ERROR_WRITE_FAULT:
            return SystemError::IoError;
        case ERROR_READ_FAULT:
            return SystemError::IoError;
        case ERROR_SHARING_VIOLATION:
            return SystemError::PermissionDenied;
        case ERROR_LOCK_VIOLATION:
            return SystemError::NoLockAvailable;
        case ERROR_HANDLE_DISK_FULL:
            return SystemError::NoSpaceOnDevice;
        case ERROR_NOT_SUPPORTED:
            return SystemError::NotSupported;
        case ERROR_BAD_NETPATH:
            return SystemError::NoSuchFileOrDirectory;
        case ERROR_DEV_NOT_EXIST:
            return SystemError::NoSuchDevice;
        case ERROR_BAD_NET_NAME:
            return SystemError::NoSuchFileOrDirectory;
        case ERROR_FILE_EXISTS:
            return SystemError::FileExists;
        case ERROR_CANNOT_MAKE:
            return SystemError::PermissionDenied;
        case ERROR_INVALID_PARAMETER:
            return SystemError::InvalidArgument;
        case ERROR_BROKEN_PIPE:
            return SystemError::BrokenPipe;
        case ERROR_OPEN_FAILED:
            return SystemError::IoError;
        case ERROR_BUFFER_OVERFLOW:
            return SystemError::FileNameTooLong;
        case ERROR_DISK_FULL:
            return SystemError::NoSpaceOnDevice;
        case ERROR_SEM_TIMEOUT:
            return SystemError::TimedOut;
        case ERROR_INVALID_NAME:
            return SystemError::NoSuchFileOrDirectory;
        case ERROR_NEGATIVE_SEEK:
            return SystemError::InvalidArgument;
        case ERROR_BUSY_DRIVE:
            return SystemError::DeviceOrResourceBusy;
        case ERROR_DIR_NOT_EMPTY:
            return SystemError::DirectoryNotEmpty;
        case ERROR_BUSY:
            return SystemError::DeviceOrResourceBusy;
        case ERROR_ALREADY_EXISTS:
            return SystemError::FileExists;
        case ERROR_FILENAME_EXCED_RANGE:
            return SystemError::FileNameTooLong;
        case ERROR_LOCKED:
            return SystemError::NoLockAvailable;
        case WAIT_TIMEOUT:
            return SystemError::TimedOut;
        case ERROR_DIRECTORY:
            return SystemError::InvalidArgument;
        case ERROR_OPERATION_ABORTED:
            return SystemError::OperationCanceled;
        case ERROR_NOACCESS:
            return SystemError::PermissionDenied;
        case ERROR_CANTOPEN:
            return SystemError::IoError;
        case ERROR_CANTREAD:
            return SystemError::IoError;
        case ERROR_CANTWRITE:
            return SystemError::IoError;
        case ERROR_RETRY:
            return SystemError::ResourceUnavailableTryAgain;
        case ERROR_TIMEOUT:
            return SystemError::TimedOut;
        case ERROR_OPEN_FILES:
            return SystemError::DeviceOrResourceBusy;
        case ERROR_DEVICE_IN_USE:
            return SystemError::DeviceOrResourceBusy;
        case ERROR_REPARSE_TAG_INVALID:
            return SystemError::InvalidArgument;
        case WSAEINTR:
            return SystemError::Interrupted;
        case WSAEBADF:
            return SystemError::BadFileDescriptor;
        case WSAEACCES:
            return SystemError::PermissionDenied;
        case WSAEFAULT:
            return SystemError::BadAddress;
        case WSAEINVAL:
            return SystemError::InvalidArgument;
        case WSAEMFILE:
            return SystemError::TooManyFilesOpen;
        case WSAEWOULDBLOCK:
            return SystemError::OperationWouldBlock;
        case WSAEINPROGRESS:
            return SystemError::OperationInProgress;
        case WSAEALREADY:
            return SystemError::ConnectionAlreadyInProgress;
        case WSAENOTSOCK:
            return SystemError::NotASocket;
        case WSAEDESTADDRREQ:
            return SystemError::DestinationAddressRequired;
        case WSAEMSGSIZE:
            return SystemError::MessageSize;
        case WSAEPROTOTYPE:
            return SystemError::WrongProtocolType;
        case WSAENOPROTOOPT:
            return SystemError::NoProtocolOption;
        case WSAEPROTONOSUPPORT:
            return SystemError::ProtocolNotSupported;
        case WSAEOPNOTSUPP:
            return SystemError::OperationNotSupported;
        case WSAEAFNOSUPPORT:
            return SystemError::AddressFamilyNotSupported;
        case WSAEADDRINUSE:
            return SystemError::AddressInUse;
        case WSAEADDRNOTAVAIL:
            return SystemError::AddressNotAvailable;
        case WSAENETDOWN:
            return SystemError::NetworkDown;
        case WSAENETUNREACH:
            return SystemError::NetworkUnreachable;
        case WSAENETRESET:
            return SystemError::NetworkReset;
        case WSAECONNABORTED:
            return SystemError::ConnectionAborted;
        case WSAECONNRESET:
            return SystemError::ConnectionReset;
        case WSAENOBUFS:
            return SystemError::NoBufferSpace;
        case WSAEISCONN:
            return SystemError::AlreadyConnected;
        case WSAENOTCONN:
            return SystemError::NotConnected;
        case WSAETIMEDOUT:
            return SystemError::TimedOut;
        case WSAECONNREFUSED:
            return SystemError::ConnectionRefused;
        case WSAENAMETOOLONG:
            return SystemError::FileNameTooLong;
        case WSAEHOSTUNREACH:
            return SystemError::HostUnreachable;

        default:
            break;
        }

        return SystemError::Unknown;
    }
}
