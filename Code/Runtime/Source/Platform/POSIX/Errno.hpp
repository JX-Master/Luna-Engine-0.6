// Copyright 2018-2021 JXMaster. All rights reserved.
/*
* @file Errno.hpp
* @author JXMaster
* @date 2020/9/22
 */
#pragma once
#include <Runtime/PlatformDefines.hpp>
#ifdef LUNA_PLATFORM_POSIX

#include <errno.h>

namespace Luna
{
    namespace OS
    {
        const c8* display_errno(errno_t err)
        {
            switch(err)
            {
                case EPERM:
                    return u8"EPERM Operation not permitted";
                case ENOENT:
                    return u8"ENOENT No such file or directory";
                case ESRCH:
                    return u8"ESRCH No such process";
                case EINTR:
                    return u8"EINTR Interrupted system call";
                case EIO:
                    return u8"EIO Input/output error";
                case ENXIO:
                    return u8"ENXIO Device not configured";
                case E2BIG:
                    return u8"E2BIG Argument list too long";
                case ENOEXEC:
                    return u8"ENOEXEC Exec format error";
                case EBADF:
                    return u8"EBADF Bad file descriptor";
                case ECHILD:
                    return u8"ECHILD No child processes";
                case EDEADLK:
                    return u8"Resource deadlock avoided";
                case ENOMEM:
                    return u8"ENOMEM Cannot allocate memory";
                case EACCES:
                    return u8"EACCES Permission denied";
                case EFAULT:
                    return u8"EFAULT Bad address";
                case ENOTBLK:
                    return u8"ENOTBLK Block device required";
                case EBUSY:
                    return u8"EBUSY Device / Resource busy";
                case EEXIST:
                    return u8"EEXIST File exists";
                case EXDEV:
                    return u8"EXDEV Cross-device link";
                case ENODEV:
                    return u8"ENODEV Operation not supported by device";
                case ENOTDIR:
                    return u8"ENOTDIR Not a directory";
                case EISDIR:
                    return u8"EISDIR Is a directory";
                case EINVAL:
                    return u8"EINVAL Invalid argument";
                case ENFILE:
                    return u8"ENFILE Too many open files in system";
                case EMFILE:
                    return u8"EMFILE Too many open files";
                case ENOTTY:
                    return u8"ENOTTY Inappropriate ioctl for device";
                case ETXTBSY:
                    return u8"ETXTBSY Text file busy";
                case EFBIG:
                    return u8"EFBIG File too large";
                case ENOSPC:
                    return u8"ENOSPC No space left on device";
                case ESPIPE:
                    return u8"ESPIPE Illegal seek";
                case EROFS:
                    return u8"EROFS Read-only file system";
                case EMLINK:
                    return u8"EMLINK Too many links";
                case EPIPE:
                    return u8"EPIPE Broken pipe";
                case EDOM:
                    return u8"EDOM Numerical argument out of domain";
                case ERANGE:
                    return u8"ERANGE Result too large";
                case EAGAIN:
                    return u8"EAGAIN|EWOULDBLOCK Resource temporarily unavailable / Operation would block";
                case EINPROGRESS:
                    return u8"EINPROGRESS Operation now in progress";
                case EALREADY:
                    return u8"EALREADY Operation already in progress";
                default:
                    return u8"Undefined Error Code";
            }
        }
    }
}

#endif
