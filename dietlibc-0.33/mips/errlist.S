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
	.long .LC132
	.long .LC133
	.long .LC134
	.long .LC135
	.long .LC136
	.long .LC137
	.long .LC138
	.long .LC139
	.long .LC140
	.long .LC141
	.long .LC142
	.long .LC143
	.long .LC144
	.long .LC145
	.long .LC146
	.long .LC147
	.long .LC148
	.long .LC149
	.long .LC150
	.long .LC151
	.long .LC152
	.long .LC153
	.long .LC154
	.long .LC155
	.long .LC156
	.long .LC157
	.long .LC158
	.long .LC159
	.long .LC160
	.long .LC161
	.long .LC162
	.long .LC163
	.long .LC164
	.long 0
.size sys_errlist,.-sys_errlist

.align 4
.global sys_nerr
.type	sys_nerr,@object
sys_nerr:
	.long 161
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
.LC035: .string "No message of desired type"
.LC036: .string "Identifier removed"
.LC037: .string "Channel number out of range"
.LC038: .string "Level 2 not synchronized"
.LC039: .string "Level 3 halted"
.LC040: .string "Level 3 reset"
.LC041: .string "Link number out of range"
.LC042: .string "Protocol driver not attached"
.LC043: .string "No CSI structure available"
.LC044: .string "Level 2 halted"
.LC045: .string "Resource deadlock would occur"
.LC046: .string "No record locks available"
.LC047: .string "Error 47"
.LC048: .string "Error 48"
.LC049: .string "Error 49"
.LC050: .string "Invalid exchange"
.LC051: .string "Invalid request descriptor"
.LC052: .string "Exchange full"
.LC053: .string "No anode"
.LC054: .string "Invalid request code"
.LC055: .string "Invalid slot"
.LC056: .string "File locking deadlock error"
.LC057: .string "Error 57"
.LC058: .string "Error 58"
.LC059: .string "Bad font file format"
.LC060: .string "Device not a stream"
.LC061: .string "No data available"
.LC062: .string "Timer expired"
.LC063: .string "Out of streams resources"
.LC064: .string "Machine is not on the network"
.LC065: .string "Package not installed"
.LC066: .string "Object is remote"
.LC067: .string "Link has been severed"
.LC068: .string "Advertise error"
.LC069: .string "Srmount error"
.LC070: .string "Communication error on send"
.LC071: .string "Protocol error"
.LC072: .string "Error 72"
.LC073: .string "RFS specific error"
.LC074: .string "Multihop attempted"
.LC075: .string "Error 75"
.LC076: .string "Error 76"
.LC077: .string "Not a data message"
.LC078: .string "File name too long"
.LC079: .string "Value too large for defined data type"
.LC080: .string "Name not unique on network"
.LC081: .string "File descriptor in bad state"
.LC082: .string "Remote address changed"
.LC083: .string "Can not access a needed shared library"
.LC084: .string "Accessing a corrupted shared library"
.LC085: .string ".lib section in a.out corrupted"
.LC086: .string "Attempting to link in too many shared libraries"
.LC087: .string "Cannot exec a shared library directly"
.LC088: .string "Illegal byte sequence"
.LC089: .string "Function not implemented"
.LC090: .string "Too many symbolic links encountered"
.LC091: .string "Interrupted system call should be restarted"
.LC092: .string "Streams pipe error"
.LC093: .string "Directory not empty"
.LC094: .string "Too many users"
.LC095: .string "Socket operation on non-socket"
.LC096: .string "Destination address required"
.LC097: .string "Message too long"
.LC098: .string "Protocol wrong type for socket"
.LC099: .string "Protocol not available"
.LC100: .string "Error 100-119"
.LC120: .string "Protocol not supported"
.LC121: .string "Socket type not supported"
.LC122: .string "Operation not supported on transport endpoint"
.LC123: .string "Protocol family not supported"
.LC124: .string "Address family not supported by protocol"
.LC125: .string "Address already in use"
.LC126: .string "Cannot assign requested address"
.LC127: .string "Network is down"
.LC128: .string "Network is unreachable"
.LC129: .string "Network dropped connection because of reset"
.LC130: .string "Software caused connection abort"
.LC131: .string "Connection reset by peer"
.LC132: .string "No buffer space available"
.LC133: .string "Transport endpoint is already connected"
.LC134: .string "Transport endpoint is not connected"
.LC135: .string "Structure needs cleaning"
.LC136: .string "Error 136"
.LC137: .string "Not a XENIX named type file"
.LC138: .string "No XENIX semaphores available"
.LC139: .string "Is a named type file"
.LC140: .string "Remote I/O error"
.LC141: .string "Reserved"
.LC142: .string "Error 142"
.LC143: .string "Cannot send after transport endpoint shutdown"
.LC144: .string "Too many references: cannot splice"
.LC145: .string "Connection timed out"
.LC146: .string "Connection refused"
.LC147: .string "Host is down"
.LC148: .string "No route to host"
.LC149: .string "Operation already in progress"
.LC150: .string "Operation now in progress"
.LC151: .string "Stale NFS file handle"
.LC152: .string "Error 152"
.LC153: .string "Error 153"
.LC154: .string "Error 154"
.LC155: .string "Error 155"
.LC156: .string "Error 156"
.LC157: .string "Error 157"
.LC158: .string "AIO operation canceled"
.LC159: .string "No medium found"
.LC160: .string "Wrong medium type"
.LC161: .string "Required key not available"
.LC162: .string "Key has expired"
.LC163: .string "Key has been revoked"
.LC164: .string "Key was rejected by service"
/* handled in strerror.c
.LC1133: .string "Quota exceeded"
*/
