//
// This file is part of ChuniX and is released under the terms of
// GNU GPLv3 License, see License for details.
//
// Copyright (C) 2014, Chunis Deng (<chunchengfh@gmail.com>)
//
#ifndef _ERRNO_H__
#define _ERRNO_H__

#define	E2BIG		 1	/* Arg list too long */
#define	EACCES		 2	/* Permission denied */
#define	EADDRINUSE	 3	/* Address already in use */
#define	EADDRNOTAVAIL	 4	/* Cannot assign requested address */
#define	EADV		 5	/* Advertise error */
#define	EAFNOSUPPORT	 6	/* Address family not supported by protocol */
#define	EAGAIN		 7	/* Try again */
#define	EALREADY	 8	/* Operation already in progress */
#define	EBADE		 9	/* Invalid exchange */
#define	EBADF		10	/* Bad file number */
#define	EBADFD		11	/* File descriptor in bad state */
#define	EBADMSG		12	/* Not a data message */
#define	EBADR		13	/* Invalid request descriptor */
#define	EBADRQC		14	/* Invalid request code */
#define	EBADSLT		15	/* Invalid slot */
#define	EBFONT		16	/* Bad font file format */
#define	EBUSY		17	/* Device or resource busy */
#define	ECHILD		18	/* No child processes */
#define	ECHRNG		19	/* Channel number out of range */
#define	ECOMM		20	/* Communication error on send */
#define	ECONNABORTED	21	/* Software caused connection abort */
#define	ECONNREFUSED	22	/* Connection refused */
#define	ECONNRESET	23	/* Connection reset by peer */
#define	EDEADLK		24	/* Resource deadlock would occur */
#define	EDEADLOCK	25	/* File locking deadlock error */
#define	EDESTADDRREQ	26	/* Destination address required */
#define	EDOM		27	/* Math argument out of domain of func */
#define	EDOTDOT		28	/* RFS specific error */
#define	EDQUOT		29	/* Quota exceeded */
#define	EEXIST		30	/* File exists */
#define	EFAULT		31	/* Bad address */
#define	EFBIG		32	/* File too large */
#define	EHOSTDOWN	33	/* Host is down */
#define	EHOSTUNREACH	34	/* No route to host */
#define	EIDRM		35	/* Identifier removed */
#define	EILSEQ		36	/* Illegal byte sequence */
#define	EINPROGRESS	37	/* Operation now in progress */
#define	EINTR		38	/* Interrupted system call */
#define	EINVAL		39	/* Invalid argument */
#define	EIO		40	/* I/O error */
#define	EISCONN		41	/* Transport endpoint is already connected */
#define	EISDIR		42	/* Is a directory */
#define	EISNAM		43	/* Is a named type file */
#define	EL2HLT		44	/* Level 2 halted */
#define	EL2NSYNC	45	/* Level 2 not synchronized */
#define	EL3HLT		46	/* Level 3 halted */
#define	EL3RST		47	/* Level 3 reset */
#define	ELIBACC		48	/* Can not access a needed shared library */
#define	ELIBBAD		49	/* Accessing a corrupted shared library */
#define	ELIBEXEC	50	/* Cannot exec a shared library directly */
#define	ELIBMAX		51	/* Attempting to link in too many shared libraries */
#define	ELIBSCN		52	/* .lib section in a.out corrupted */
#define	ELNRNG		53	/* Link number out of range */
#define	ELOOP		54	/* Too many symbolic links encountered */
#define	EMFILE		55	/* Too many open files */
#define	EMLINK		56	/* Too many links */
#define	EMSGSIZE	57	/* Message too long */
#define	EMULTIHOP	58	/* Multihop attempted */
#define	ENAMETOOLONG	59	/* File name too long */
#define	ENAVAIL		60	/* No XENIX semaphores available */
#define	ENETDOWN	61	/* Network is down */
#define	ENETRESET	62	/* Network dropped connection because of reset */
#define	ENETUNREACH	63	/* Network is unreachable */
#define	ENFILE		64	/* File table overflow */
#define	ENOANO		65	/* No anode */
#define	ENOBUFS		66	/* No buffer space available */
#define	ENOCSI		67	/* No CSI structure available */
#define	ENODATA		68	/* No data available */
#define	ENODEV		69	/* No such device */
#define	ENOENT		70	/* No such file or directory */
#define	ENOEXEC		71	/* Exec format error */
#define	ENOLCK		72	/* No record locks available */
#define	ENOLINK		73	/* Link has been severed */
#define	ENOMEM		74	/* Out of memory */
#define	ENOMSG		75	/* No message of desired type */
#define	ENONET		76	/* Machine is not on the network */
#define	ENOPKG		77	/* Package not installed */
#define	ENOPROTOOPT	78	/* Protocol not available */
#define	ENOSPC		79	/* No space left on device */
#define	ENOSR		80	/* Out of streams resources */
#define	ENOSTR		81	/* Device not a stream */
#define	ENOSYS		82	/* Function not implemented */
#define	ENOTBLK		83	/* Block device required */
#define	ENOTCONN	84	/* Transport endpoint is not connected */
#define	ENOTDIR		85	/* Not a directory */
#define	ENOTEMPTY	86	/* Directory not empty */
#define	ENOTNAM		87	/* Not a XENIX named type file */
#define	ENOTSOCK	88	/* Socket operation on non-socket */
#define	ENOTTY		89	/* Not a typewriter */
#define	ENOTUNIQ	90	/* Name not unique on network */
#define	ENXIO		91	/* No such device or address */
#define	EOPNOTSUPP	92	/* Operation not supported on transport endpoint */
#define	EOVERFLOW	93	/* Value too large for defined data type */
#define	EPERM		94	/* Operation not permitted */
#define	EPFNOSUPPORT	95	/* Protocol family not supported */
#define	EPIPE		96	/* Broken pipe */
#define	EPROTO		97	/* Protocol error */
#define	EPROTONOSUPPORT	98	/* Protocol not supported */
#define	EPROTOTYPE	99	/* Protocol wrong type for socket */
#define	ERANGE		100	/* Math result not representable */
#define	EREMCHG		101	/* Remote address changed */
#define	EREMOTE		102	/* Object is remote */
#define	EREMOTEIO	103	/* Remote I/O error */
#define	ERESTART	104	/* Interrupted system call should be restarted */
#define	EROFS		105	/* Read-only file system */
#define	ESHUTDOWN	106	/* Cannot send after transport endpoint shutdown */
#define	ESOCKTNOSUPPORT	107	/* Socket type not supported */
#define	ESPIPE		108	/* Illegal seek */
#define	ESRCH		109	/* No such process */
#define	ESRMNT		110	/* Srmount error */
#define	ESTALE		111	/* Stale NFS file handle */
#define	ESTRPIPE	112	/* Streams pipe error */
#define	ETIME		113	/* Timer expired */
#define	ETIMEDOUT	114	/* Connection timed out */
#define	ETOOMANYREFS	115	/* Too many references: cannot splice */
#define	ETXTBSY		116	/* Text file busy */
#define	EUCLEAN		117	/* Structure needs cleaning */
#define	EUNATCH		118	/* Protocol driver not attached */
#define	EUSERS		119	/* Too many users */
#define	EWOULDBLOCK	EAGAIN	/* Operation would block */
#define	EXDEV		121	/* Cross-device link */
#define	EXFULL		122	/* Exchange full */

/* Should never be seen by user programs */
#define ERESTARTSYS	512
#define ERESTARTNOINTR	513
#define ERESTARTNOHAND	514	/* restart if no handler.. */
#define ENOIOCTLCMD	515	/* No ioctl command */

#endif
