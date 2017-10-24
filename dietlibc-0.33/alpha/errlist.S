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
	.quad .LC101
	.quad .LC102
	.quad .LC103
	.quad .LC104
	.quad .LC105
	.quad .LC106
	.quad .LC107
	.quad .LC108
	.quad .LC109
	.quad .LC110
	.quad .LC111
	.quad .LC112
	.quad .LC113
	.quad .LC114
	.quad .LC115
	.quad .LC116
	.quad .LC117
	.quad .LC118
	.quad .LC119
	.quad .LC120
	.quad .LC121
	.quad .LC122
	.quad .LC123
	.quad .LC124
	.quad .LC125
	.quad .LC126
	.quad .LC127
	.quad .LC128
	.quad .LC129
	.quad .LC130
	.quad .LC131
	.quad .LC132
	.quad .LC133
	.quad .LC134
	.quad .LC135
	.quad 0
.size sys_errlist,.-sys_errlist

.align 4
.global sys_nerr
.type	sys_nerr,@object
sys_nerr:
	.long 130
.size	sys_nerr,4

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
.LC011: .string "Resource deadlock would occur"
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
.LC035: .string "Try again"
.LC036: .string "Operation now in progress"
.LC037: .string "Operation already in progress"
.LC038: .string "Socket operation on non-socket"
.LC039: .string "Destination address required"
.LC040: .string "Message too long"
.LC041: .string "Protocol wrong type for socket"
.LC042: .string "Protocol not available"
.LC043: .string "Protocol not supported"
.LC044: .string "Socket type not supported"
.LC045: .string "Operation not supported on transport endpoint"
.LC046: .string "Protocol family not supported"
.LC047: .string "Address family not supported by protocol"
.LC048: .string "Address already in use"
.LC049: .string "Cannot assign requested address"
.LC050: .string "Network is down"
.LC051: .string "Network is unreachable"
.LC052: .string "Network dropped connection because of reset"
.LC053: .string "Software caused connection abort"
.LC054: .string "Connection reset by peer"
.LC055: .string "No buffer space available"
.LC056: .string "Transport endpoint is already connected"
.LC057: .string "Transport endpoint is not connected"
.LC058: .string "Cannot send after transport endpoint shutdown"
.LC059: .string "Too many references: cannot splice"
.LC060: .string "Connection timed out"
.LC061: .string "Connection refused"
.LC062: .string "Too many symbolic links encountered"
.LC063: .string "File name too long"
.LC064: .string "Host is down"
.LC065: .string "No route to host"
.LC066: .string "Directory not empty"
.LC067: .string "Error 67"
.LC068: .string "Too many users"
.LC069: .string "Quota exceeded"
.LC070: .string "Stale NFS file handle"
.LC071: .string "Object is remote"
.LC072: .string "Error 72"
.LC073: .string "Error 73"
.LC074: .string "Error 74"
.LC075: .string "Error 75"
.LC076: .string "Error 76"
.LC077: .string "No record locks available"
.LC078: .string "Function not implemented"
.LC079: .string "Error 79"
.LC080: .string "No message of desired type"
.LC081: .string "Identifier removed"
.LC082: .string "Out of streams resources"
.LC083: .string "Timer expired"
.LC084: .string "Not a data message"
.LC085: .string "Protocol error"
.LC086: .string "No data available"
.LC087: .string "Device not a stream"
.LC088: .string "Channel number out of range"
.LC089: .string "Level 2 not synchronized"
.LC090: .string "Level 3 halted"
.LC091: .string "Level 3 reset"
.LC092: .string "Package not installed"
.LC093: .string "Link number out of range"
.LC094: .string "Protocol driver not attached"
.LC095: .string "No CSI structure available"
.LC096: .string "Level 2 halted"
.LC097: .string "Invalid exchange"
.LC098: .string "Invalid request descriptor"
.LC099: .string "Exchange full"
.LC100: .string "No anode"
.LC101: .string "Invalid request code"
.LC102: .string "Invalid slot"
.LC103: .string "Error 103"
.LC104: .string "Bad font file format"
.LC105: .string "Machine is not on the network"
.LC106: .string "Link has been severed"
.LC107: .string "Advertise error"
.LC108: .string "Srmount error"
.LC109: .string "Communication error on send"
.LC110: .string "Multihop attempted"
.LC111: .string "RFS specific error"
.LC112: .string "Value too large for defined data type"
.LC113: .string "Name not unique on network"
.LC114: .string "File descriptor in bad state"
.LC115: .string "Remote address changed"
.LC116: .string "Illegal byte sequence"
.LC117: .string "Structure needs cleaning"
.LC118: .string "Not a XENIX named type file"
.LC119: .string "No XENIX semaphores available"
.LC120: .string "Is a named type file"
.LC121: .string "Remote I/O error"
.LC122: .string "Can not access a needed shared library"
.LC123: .string "Accessing a corrupted shared library"
.LC124: .string ".lib section in a.out corrupted"
.LC125: .string "Attempting to link in too many shared libraries"
.LC126: .string "Cannot exec a shared library directly"
.LC127: .string "Interrupted system call should be restarted"
.LC128: .string "Streams pipe error"
.LC129: .string "No medium found"
.LC130: .string "Wrong medium type"
.LC131: .string "Operation canceled"
.LC132: .string "Required key not available"
.LC133: .string "Key has expired"
.LC134: .string "Key has been revoked"
.LC135: .string "Key was rejected by service"

