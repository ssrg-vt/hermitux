#ifdef __DYN_LIB
.section	.data
#else
.section	.rodata
#endif

.align 8
.global sys_errlist
.type	sys_errlist,@object
sys_errlist:
	.quad .LC000
	.quad .LC001
	.quad .LC002
	.quad .LC003
	.quad .LC004
	.quad .LC005
	.quad .LC006
	.quad .LC007
	.quad .LC008
	.quad .LC009
	.quad .LC010
	.quad .LC011
	.quad .LC012
	.quad .LC013
	.quad .LC014
	.quad .LC015
	.quad .LC016
	.quad .LC017
	.quad .LC018
	.quad .LC019
	.quad .LC020
	.quad .LC021
	.quad .LC022
	.quad .LC023
	.quad .LC024
	.quad .LC025
	.quad .LC026
	.quad .LC027
	.quad .LC028
	.quad .LC029
	.quad .LC030
	.quad .LC031
	.quad .LC032
	.quad .LC033
	.quad .LC034
	.quad .LC035
	.quad .LC036
	.quad .LC037
	.quad .LC038
	.quad .LC039
	.quad .LC040
	.quad .LC041
	.quad .LC042
	.quad .LC043
	.quad .LC044
	.quad .LC045
	.quad .LC046
	.quad .LC047
	.quad .LC048
	.quad .LC049
	.quad .LC050
	.quad .LC051
	.quad .LC052
	.quad .LC053
	.quad .LC054
	.quad .LC055
	.quad .LC056
	.quad .LC057
	.quad .LC058
	.quad .LC059
	.quad .LC060
	.quad .LC061
	.quad .LC062
	.quad .LC063
	.quad .LC064
	.quad .LC065
	.quad .LC066
	.quad .LC067
	.quad .LC068
	.quad .LC069
	.quad .LC070
	.quad .LC071
	.quad .LC072
	.quad .LC073
	.quad .LC074
	.quad .LC075
	.quad .LC076
	.quad .LC077
	.quad .LC078
	.quad .LC079
	.quad .LC080
	.quad .LC081
	.quad .LC082
	.quad .LC083
	.quad .LC084
	.quad .LC085
	.quad .LC086
	.quad .LC087
	.quad .LC088
	.quad .LC089
	.quad .LC090
	.quad .LC091
	.quad .LC092
	.quad .LC093
	.quad .LC094
	.quad .LC095
	.quad .LC096
	.quad .LC097
	.quad .LC098
	.quad .LC099
	.quad .LC100
	.quad .LC100
	.quad .LC100
	.quad .LC100
	.quad .LC100
	.quad .LC100
	.quad .LC100
	.quad .LC100
	.quad .LC100
	.quad .LC100
	.quad .LC100
	.quad .LC100
	.quad .LC100
	.quad .LC100
	.quad .LC100
	.quad .LC100
	.quad .LC100
	.quad .LC100
	.quad .LC100
	.quad .LC100
	.quad .LC120
	.quad .LC121
	.quad .LC122
	.quad .LC123
	.quad .LC124
	.quad .LC125
	.quad .LC126
	.quad 0
.size sys_errlist,.-sys_errlist

.align 4
.global sys_nerr
.type	sys_nerr,@object
sys_nerr:
	.long 127
.size	 sys_nerr,4

#ifdef __DYN_LIB
.section	.rodata
#endif

.LC000:	.string	"Success"
.LC001: .string "Operation not permitted"
.LC002: .string "No such file or directory"
.LC003: .string "No such process"
.LC004: .string "Interrupted system call"
.LC005: .string "I/O error"
.LC006: .string "No such device or address"
.LC007: .string "Arg list too long"
.LC008: .string "Exec format error"
.LC009: .string "Bad file number"
.LC010: .string "No child processes"
.LC011: .string "Try again"
.LC012: .string "Out of memory"
.LC013: .string "Permission denied"
.LC014: .string "Bad address"
.LC015: .string "Block device required"
.LC016: .string "Device or resource busy"
.LC017: .string "File exists"
.LC018: .string "Cross-device link"
.LC019: .string "No such device"
.LC020: .string "Not a directory"
.LC021: .string "Is a directory"
.LC022: .string "Invalid argument"
.LC023: .string "File table overflow"
.LC024: .string "Too many open files"
.LC025: .string "Not a typewriter"
.LC026: .string "Text file busy"
.LC027: .string "File too large"
.LC028: .string "No space left on device"
.LC029: .string "Illegal seek"
.LC030: .string "Read-only file system"
.LC031: .string "Too many links"
.LC032: .string "Broken pipe"
.LC033: .string "Math argument out of domain of func"
.LC034: .string "Math result not representable"
.LC035: .string "Operation would block"
.LC036: .string "Operation now in progress"
.LC037: .string "Operation already in progress"
.LC038: .string "Socket operation on non-socket"
.LC039: .string "Destination address required"
.LC040: .string "Message too long"
.LC041: .string "Protocol wrong type for socket"
.LC042: .string "Protocol not available"
.LC043: .string "Protocol not supported"
.LC044: .string "Socket type not supported"
.LC045: .string "Op not supported on transport endpoint"
.LC046: .string "Protocol family not supported"
.LC047: .string "Address family not supported by protocol"
.LC048: .string "Address already in use"
.LC049: .string "Cannot assign requested address"
.LC050: .string "Network is down"
.LC051: .string "Network is unreachable"
.LC052: .string "Net dropped connection because of reset"
.LC053: .string "Software caused connection abort"
.LC054: .string "Connection reset by peer"
.LC055: .string "No buffer space available"
.LC056: .string "Transport endpoint is already connected"
.LC057: .string "Transport endpoint is not connected"
.LC058: .string "No send after transport endpoint shutdown"
.LC059: .string "Too many references: cannot splice"
.LC060: .string "Connection timed out"
.LC061: .string "Connection refused"
.LC062: .string "Too many symbolic links encountered"
.LC063: .string "File name too long"
.LC064: .string "Host is down"
.LC065: .string "No route to host"
.LC066: .string "Directory not empty"
.LC067: .string "SUNOS: Too many processes"
.LC068: .string "Too many users"
.LC069: .string "Quota exceeded"
.LC070: .string "Stale NFS file handle"
.LC071: .string "Object is remote"
.LC072: .string "Device not a stream"
.LC073: .string "Timer expired"
.LC074: .string "Out of streams resources"
.LC075: .string "No message of desired type"
.LC076: .string "Not a data message"
.LC077: .string "Identifier removed"
.LC078: .string "Resource deadlock would occur"
.LC079: .string "No record locks available"
.LC080: .string "Machine is not on the network"
.LC081: .string "SunOS: Too many lvls of remote in path"
.LC082: .string "Link has been severed"
.LC083: .string "Advertise error"
.LC084: .string "Srmount error"
.LC085: .string "Communication error on send"
.LC086: .string "Protocol error"
.LC087: .string "Multihop attempted"
.LC088: .string "RFS specific error"
.LC089: .string "Remote address changed"
.LC090: .string "Function not implemented"
.LC091: .string "Streams pipe error"
.LC092: .string "Value too large for defined data type"
.LC093: .string "File descriptor in bad state"
.LC094: .string "Channel number out of range"
.LC095: .string "Level 2 not synchronized"
.LC096: .string "Level 3 halted"
.LC097: .string "Level 3 reset"
.LC098: .string "Link number out of range"
.LC099: .string "Protocol driver not attached"
.LC100: .string "No CSI structure available"
.LC101: .string "Level 2 halted"
.LC102: .string "Invalid exchange"
.LC103: .string "Invalid request descriptor"
.LC104: .string "Exchange full"
.LC105: .string "No anode"
.LC106: .string "Invalid request code"
.LC107: .string "Invalid slot"
.LC108: .string "File locking deadlock error"
.LC109: .string "Bad font file format"
.LC110: .string "Cannot exec a shared library directly"
.LC111: .string "No data available"
.LC112: .string "Accessing a corrupted shared library"
.LC113: .string "Package not installed"
.LC114: .string "Can not access a needed shared library"
.LC115: .string "Name not unique on network"
.LC116: .string "Interrupted syscall should be restarted"
.LC117: .string "Structure needs cleaning"
.LC118: .string "Not a XENIX named type file"
.LC119: .string "No XENIX semaphores available"
.LC120: .string "Is a named type file"
.LC121: .string "Remote I/O error"
.LC122: .string "Illegal byte sequence"
.LC123: .string "Atmpt to link in too many shared libs"
.LC124: .string ".lib section in a.out corrupted"
.LC125: .string "No medium found"
.LC126: .string "Wrong medium type"


