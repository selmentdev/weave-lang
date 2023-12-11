#include "weave/platform/SystemError.hxx"

#include <cerrno>

namespace weave::platform::impl
{
    SystemError SystemErrorFromErrno(int error)
    {
        switch (error)
        {
        case EAFNOSUPPORT:
            return SystemError::AddressFamilyNotSupported;

        case EADDRINUSE:
            return SystemError::AddressInUse;

        case EADDRNOTAVAIL:
            return SystemError::AddressNotAvailable;

        case EISCONN:
            return SystemError::AlreadyConnected;

        case E2BIG:
            return SystemError::ArgumentListTooLong;

        case EDOM:
            return SystemError::ArgumentOutOfDomain;

        case EFAULT:
            return SystemError::BadAddress;

        case EBADF:
            return SystemError::BadFileDescriptor;

        case EBADMSG:
            return SystemError::BadMessage;

        case EPIPE:
            return SystemError::BrokenPipe;

        case ECONNABORTED:
            return SystemError::ConnectionAborted;

        case EALREADY:
            return SystemError::ConnectionAlreadyInProgress;

        case ECONNREFUSED:
            return SystemError::ConnectionRefused;

        case ECONNRESET:
            return SystemError::ConnectionReset;

        case EXDEV:
            return SystemError::CrossDeviceLink;

        case EDESTADDRREQ:
            return SystemError::DestinationAddressRequired;

        case EBUSY:
            return SystemError::DeviceOrResourceBusy;

        case ENOTEMPTY:
            return SystemError::DirectoryNotEmpty;

        case ENOEXEC:
            return SystemError::ExecutableFormatError;

        case EEXIST:
            return SystemError::FileExists;

        case EFBIG:
            return SystemError::FileTooLarge;

        case ENAMETOOLONG:
            return SystemError::FileNameTooLong;

        case ENOSYS:
            return SystemError::FunctionNotSupported;

        case EHOSTUNREACH:
            return SystemError::HostUnreachable;

        case EIDRM:
            return SystemError::IdentifierRemoved;

        case EILSEQ:
            return SystemError::IllegalByteSequence;

        case ENOTTY:
            return SystemError::InappropriateIoControlOperation;

        case EINTR:
            return SystemError::Interrupted;

        case EINVAL:
            return SystemError::InvalidArgument;

        case ESPIPE:
            return SystemError::InvalidSeek;

        case EIO:
            return SystemError::IoError;

        case EISDIR:
            return SystemError::IsADirectory;

        case EMSGSIZE:
            return SystemError::MessageSize;

        case ENETDOWN:
            return SystemError::NetworkDown;

        case ENETRESET:
            return SystemError::NetworkReset;

        case ENETUNREACH:
            return SystemError::NetworkUnreachable;

        case ENOBUFS:
            return SystemError::NoBufferSpace;

        case ECHILD:
            return SystemError::NoChildProcess;

        case ENOLINK:
            return SystemError::NoLink;

        case ENOLCK:
            return SystemError::NoLockAvailable;

#ifdef ENODATA
        case ENODATA:
            return SystemError::NoMessageAvailable;
#endif

        case ENOMSG:
            return SystemError::NoMessage;

        case ENOPROTOOPT:
            return SystemError::NoProtocolOption;

        case ENOSPC:
            return SystemError::NoSpaceOnDevice;

#ifdef ENOSR
        case ENOSR:
            return SystemError::NoStreamResources;
#endif

        case ENODEV:
            return SystemError::NoSuchDevice;

        case ENXIO:
            return SystemError::NoSuchDeviceOrAddress;

        case ENOENT:
            return SystemError::NoSuchFileOrDirectory;

        case ESRCH:
            return SystemError::NoSuchProcess;

        case ENOTDIR:
            return SystemError::NotADirectory;

        case ENOTSOCK:
            return SystemError::NotASocket;
#ifdef ENOSTR
        case ENOSTR:
            return SystemError::NotAStream;
#endif

        case ENOTCONN:
            return SystemError::NotConnected;

        case ENOMEM:
            return SystemError::NotEnoughMemory;

        case ENOTSUP:
            return SystemError::NotSupported;

        case ECANCELED:
            return SystemError::OperationCanceled;

        case EINPROGRESS:
            return SystemError::OperationInProgress;

        case EPERM:
            return SystemError::OperationNotPermitted;

#if defined (EOPNOTSUPP) && (EOPNOTSUPP != ENOTSUP)
        case EOPNOTSUPP:
            return SystemError::OperationNotSupported;
#endif

#if defined(WOULDBLOCK) && (WOULDBLOCK != EAGAIN)
        case EWOULDBLOCK:
            return SystemError::OperationWouldBlock;
#endif

        case EOWNERDEAD:
            return SystemError::OwnerDead;

        case EACCES:
            return SystemError::PermissionDenied;

        case EPROTO:
            return SystemError::ProtocolError;

        case EPROTONOSUPPORT:
            return SystemError::ProtocolNotSupported;

        case EROFS:
            return SystemError::ReadOnlyFileSystem;

        case EDEADLK:
            return SystemError::ResourceDeadlockWouldOccur;

        case EAGAIN:
            return SystemError::ResourceUnavailableTryAgain;

        case ERANGE:
            return SystemError::ResultOutOfRange;

        case ENOTRECOVERABLE:
            return SystemError::StateNotRecoverable;

#ifdef ETIME
        case ETIME:
            return SystemError::StreamTimeout;
#endif

        case ETXTBSY:
            return SystemError::TextFileBusy;

        case ETIMEDOUT:
            return SystemError::TimedOut;

        case EMFILE:
            return SystemError::TooManyFilesOpen;

        case ENFILE:
            return SystemError::TooManyFilesOpenInSystem;

        case EMLINK:
            return SystemError::TooManyLinks;

        case ELOOP:
            return SystemError::TooManySymbolicLinkLevels;

        case EOVERFLOW:
            return SystemError::ValueTooLarge;

        case EPROTOTYPE:
            return SystemError::WrongProtocolType;

        default:
            break;
        }

        return SystemError::Unknown;
    }
}
