#ifdef __DYN_LIB
.section	.data
#else
.section	.rodata
#endif

.align 4
.global sys_errlist
.type	sys_errlist,@object
sys_errlist:
	.long .LC000
	.long .LC001
	.long .LC002
	.long .LC003
	.long .LC004
	.long .LC005
	.long .LC006
	.long .LC007
	.long .LC008
	.long .LC009
	.long .LC010
	.long .LC011
	.long .LC012
	.long .LC013
	.long .LC014
	.long .LC015
	.long .LC016
	.long .LC017
	.long .LC018
	.long .LC019
	.long .LC020
	.long .LC021
	.long .LC022
	.long .LC023
	.long .LC024
	.long .LC025
	.long .LC026
	.long .LC027
	.long .LC028
	.long .LC029
	.long .LC030
	.long .LC031
	.long .LC032
	.long .LC033
	.long .LC034
	.long .LC035
	.long .LC036
	.long .LC037
	.long .LC038
	.long .LC039
	.long .LC040
	.long .LC041
	.long .LC042
	.long .LC043
	.long .LC044
	.long .LC045
	.long .LC046
	.long .LC047
	.long .LC048
	.long .LC049
	.long .LC050
	.long .LC051
	.long .LC052
	.long .LC053
	.long .LC054
	.long .LC055
	.long .LC056
	.long .LC057
	.long .LC058
	.long .LC059
	.long .LC060
	.long .LC061
	.long .LC062
	.long .LC063
	.long .LC064
	.long .LC065
	.long .LC066
	.long .LC067
	.long .LC068
	.long .LC069
	.long .LC070
	.long .LC071
	.long .LC072
	.long .LC073
	.long .LC074
	.long .LC075
	.long .LC076
	.long .LC077
	.long .LC078
	.long .LC079
	.long .LC080
	.long .LC081
	.long .LC082
	.long .LC083
	.long .LC084
	.long .LC085
	.long .LC086
	.long .LC087
	.long .LC088
	.long .LC089
	.long .LC090
	.long .LC091
	.long .LC092
	.long .LC093
	.long .LC094
	.long .LC095
	.long .LC096
	.long .LC097
	.long .LC098
	.long .LC099
	.long .LC100
	.long .LC100
	.long .LC100
	.long .LC100
	.long .LC100
	.long .LC100
	.long .LC100
	.long .LC100
	.long .LC100
	.long .LC100
	.long .LC100
	.long .LC100
	.long .LC100
	.long .LC100
	.long .LC100
	.long .LC100
	.long .LC100
	.long .LC100
	.long .LC100
	.long .LC100
	.long .LC120
	.long .LC121
	.long .LC122
	.long .LC123
	.long .LC124
	.long .LC125
	.long .LC126
	.long .LC127
	.long .LC128
	.long .LC129
	.long .LC130
	.long .LC131
	.long 0
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
.LC127: .string "Operation Cancelled"
.LC128: .string "Required key not available"
.LC129: .string "Key has expired"
.LC130: .string "Key has been revoked"
.LC131: .string "Key was rejected by service"


