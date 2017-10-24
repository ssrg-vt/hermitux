#define S11   7
#define S12  12
#define S13  17
#define S14  22
#define S21   5
#define S22   9
#define S23  14
#define S24  20
#define S31   4
#define S32  11
#define S33  16
#define S34  23
#define S41   6
#define S42  10
#define S43  15
#define S44  21


// #define F(x, y, z)   (x & y) + (~x & z) = x&y + (z - z&x) = z + y&x - z&x
//#define F(x, y, z)    movl x,%ebx; movl x,%eax; notl %ebx; andl y,%eax; andl z,%ebx; addl %ebx,%eax
#define F(x, y, z)	movl y,%eax; movl z,%ebx; andl x,%eax; andl x,%ebx; addl z,%eax; subl %ebx,%eax

// #define G(x, y, z)   (x & z) + (y & ~z) = x&z + (y - y&z) = y + x&z - y&z
#define G(x, y, z)      movl z,%ebx; movl z,%eax; notl %ebx; andl x,%eax; andl y,%ebx; addl %ebx,%eax
//#define G(x, y, z)	movl x,%eax; movl y,%ebx; andl z,%eax; andl z,%ebx; addl y,%eax; subl %ebx,%eax
//#define G(x, y, z)	movl z,%eax; movl z,%ebx; andl x,%eax; andl y,%ebx; addl y,%eax; subl %ebx,%eax

// #define H(x, y, z)   x ^ y ^ z
#define H(x, y, z)	movl z,%eax; xorl y,%eax; xorl x,%eax

// #define I(x, y, z)   y ^ (x | ~z)
#define I(x, y, z)	movl z,%eax; notl %eax; orl x,%eax; xorl y,%eax


// #define ROTATE_LEFT(x, n)  (((x) << (n)) | ((x) >> (32-(n))))
#define ROTATE_LEFT(x, n)     roll $n,x

// #define FF(a, b, c, d, x, s, ac) { (a) += F (b, c, d) + (x) + (word)(ac); (a) = ROTATE_LEFT (a, s); (a) += (b); }
// #define GG(a, b, c, d, x, s, ac) { (a) += G (b, c, d) + (x) + (word)(ac); (a) = ROTATE_LEFT (a, s); (a) += (b); }
// #define HH(a, b, c, d, x, s, ac) { (a) += H (b, c, d) + (x) + (word)(ac); (a) = ROTATE_LEFT (a, s); (a) += (b); }
// #define II(a, b, c, d, x, s, ac) { (a) += I (b, c, d) + (x) + (word)(ac); (a) = ROTATE_LEFT (a, s); (a) += (b); }

#define FF(a, b, c, d, x, s, ac)   addl x, a; addl $ac,a; F (b, c, d); addl %eax,a; roll $s,a; addl b,a
#define GG(a, b, c, d, x, s, ac)   addl x, a; addl $ac,a; G (b, c, d); addl %eax,a; roll $s,a; addl b,a
#define HH(a, b, c, d, x, s, ac)   addl x, a; addl $ac,a; H (b, c, d); addl %eax,a; roll $s,a; addl b,a
#define II(a, b, c, d, x, s, ac)   addl x, a; addl $ac,a; I (b, c, d); addl %eax,a; roll $s,a; addl b,a


//			    // x is ecx
//    word  a = state [0];  // a is edx
//    word  b = state [1];  // b is esi
//    word  c = state [2];  // c is edi
//    word  d = state [3];  // d is ebp

#define X(n)	4*(n)(%ecx)
#define X0	(%ecx)
#define a       %edx
#define b       %esi    
#define c       %edi
#define d       %ebp



/* MD5 basic transformation: Transforms state based on data block */

// void  __MD5Transform ( word state[4], const word* x, size_t repeat )

.text
.type __MD5Transform, @function
.align 32
.global __MD5Transform

	.align	32
.Lrepeat:	

        /* Round 1 */
        FF (a, b, c, d, X0   , S11, 0xd76aa478); /*  1 */
        FF (d, a, b, c, X( 1), S12, 0xe8c7b756); /*  2 */
        FF (c, d, a, b, X( 2), S13, 0x242070db); /*  3 */
        FF (b, c, d, a, X( 3), S14, 0xc1bdceee); /*  4 */
        FF (a, b, c, d, X( 4), S11, 0xf57c0faf); /*  5 */
        FF (d, a, b, c, X( 5), S12, 0x4787c62a); /*  6 */
        FF (c, d, a, b, X( 6), S13, 0xa8304613); /*  7 */
        FF (b, c, d, a, X( 7), S14, 0xfd469501); /*  8 */
        FF (a, b, c, d, X( 8), S11, 0x698098d8); /*  9 */
        FF (d, a, b, c, X( 9), S12, 0x8b44f7af); /* 10 */
        FF (c, d, a, b, X(10), S13, 0xffff5bb1); /* 11 */
        FF (b, c, d, a, X(11), S14, 0x895cd7be); /* 12 */
        FF (a, b, c, d, X(12), S11, 0x6b901122); /* 13 */
        FF (d, a, b, c, X(13), S12, 0xfd987193); /* 14 */
        FF (c, d, a, b, X(14), S13, 0xa679438e); /* 15 */
        FF (b, c, d, a, X(15), S14, 0x49b40821); /* 16 */
        
        /* Round 2 */
        GG (a, b, c, d, X( 1), S21, 0xf61e2562); /* 17 */
        GG (d, a, b, c, X( 6), S22, 0xc040b340); /* 18 */
        GG (c, d, a, b, X(11), S23, 0x265e5a51); /* 19 */
        GG (b, c, d, a, X0   , S24, 0xe9b6c7aa); /* 20 */
        GG (a, b, c, d, X( 5), S21, 0xd62f105d); /* 21 */
        GG (d, a, b, c, X(10), S22, 0x02441453); /* 22 */
        GG (c, d, a, b, X(15), S23, 0xd8a1e681); /* 23 */
        GG (b, c, d, a, X( 4), S24, 0xe7d3fbc8); /* 24 */
        GG (a, b, c, d, X( 9), S21, 0x21e1cde6); /* 25 */
        GG (d, a, b, c, X(14), S22, 0xc33707d6); /* 26 */
        GG (c, d, a, b, X( 3), S23, 0xf4d50d87); /* 27 */
        GG (b, c, d, a, X( 8), S24, 0x455a14ed); /* 28 */
        GG (a, b, c, d, X(13), S21, 0xa9e3e905); /* 29 */
        GG (d, a, b, c, X( 2), S22, 0xfcefa3f8); /* 30 */
        GG (c, d, a, b, X( 7), S23, 0x676f02d9); /* 31 */
        GG (b, c, d, a, X(12), S24, 0x8d2a4c8a); /* 32 */
        
        /* Round 3 */
        HH (a, b, c, d, X( 5), S31, 0xfffa3942); /* 33 */
        HH (d, a, b, c, X( 8), S32, 0x8771f681); /* 34 */
        HH (c, d, a, b, X(11), S33, 0x6d9d6122); /* 35 */
        HH (b, c, d, a, X(14), S34, 0xfde5380c); /* 36 */
        HH (a, b, c, d, X( 1), S31, 0xa4beea44); /* 37 */
        HH (d, a, b, c, X( 4), S32, 0x4bdecfa9); /* 38 */
        HH (c, d, a, b, X( 7), S33, 0xf6bb4b60); /* 39 */
        HH (b, c, d, a, X(10), S34, 0xbebfbc70); /* 40 */
        HH (a, b, c, d, X(13), S31, 0x289b7ec6); /* 41 */
        HH (d, a, b, c, X0   , S32, 0xeaa127fa); /* 42 */
        HH (c, d, a, b, X( 3), S33, 0xd4ef3085); /* 43 */
        HH (b, c, d, a, X( 6), S34, 0x04881d05); /* 44 */
        HH (a, b, c, d, X( 9), S31, 0xd9d4d039); /* 45 */
        HH (d, a, b, c, X(12), S32, 0xe6db99e5); /* 46 */
        HH (c, d, a, b, X(15), S33, 0x1fa27cf8); /* 47 */
        HH (b, c, d, a, X( 2), S34, 0xc4ac5665); /* 48 */
        
        /* Round 4 */
        II (a, b, c, d, X0   , S41, 0xf4292244); /* 49 */
        II (d, a, b, c, X( 7), S42, 0x432aff97); /* 50 */
        II (c, d, a, b, X(14), S43, 0xab9423a7); /* 51 */
        II (b, c, d, a, X( 5), S44, 0xfc93a039); /* 52 */
        II (a, b, c, d, X(12), S41, 0x655b59c3); /* 53 */
        II (d, a, b, c, X( 3), S42, 0x8f0ccc92); /* 54 */
        II (c, d, a, b, X(10), S43, 0xffeff47d); /* 55 */
        II (b, c, d, a, X( 1), S44, 0x85845dd1); /* 56 */
        II (a, b, c, d, X( 8), S41, 0x6fa87e4f); /* 57 */
        II (d, a, b, c, X(15), S42, 0xfe2ce6e0); /* 58 */
        II (c, d, a, b, X( 6), S43, 0xa3014314); /* 59 */
        II (b, c, d, a, X(13), S44, 0x4e0811a1); /* 60 */
        II (a, b, c, d, X( 4), S41, 0xf7537e82); /* 61 */
        II (d, a, b, c, X(11), S42, 0xbd3af235); /* 62 */
        II (c, d, a, b, X( 2), S43, 0x2ad7d2bb); /* 63 */
        II (b, c, d, a, X( 9), S44, 0xeb86d391); /* 64 */
    
//    state [0] += a;
//    state [1] += b;
//    state [2] += c;
//    state [3] += d;

	addl	$64,%ecx
	
	movl	4+32(%esp),%eax		// state
	addl	  (%eax),a
	addl	 4(%eax),b
	addl	 8(%eax),c
	addl	12(%eax),d
	movl	a,  (%eax)
	movl	b, 4(%eax)
	movl	c, 8(%eax)
	movl	d,12(%eax)

.Lstart:
	decl	12+32(%esp)
	jns	.Lrepeat
    
	popal
	ret

__MD5Transform:
__MD5TransformLocal:
	pushal
	movl	8+32(%esp),%ecx		// x
	movl	4+32(%esp),%eax		// state
	movl	  (%eax),a
	movl	 4(%eax),b
	movl	 8(%eax),c
	movl	12(%eax),d
	jmp	.Lstart
	
.Lende:
.size	__MD5Transform, .Lende-__MD5Transform

.type	 MD5Init,@function
.global	 MD5Init

MD5Init:
	movl 	4(%esp), %ecx
	xorl	%eax,%eax
	movl 	$0x67452301,   (%ecx)
	movl 	$0xefcdab89,  4(%ecx)
	movl 	$0x98badcfe,  8(%ecx)
	movl 	$0x10325476, 12(%ecx)
	movl 	%eax, 16(%ecx)
	movl 	%eax, 20(%ecx)
	ret


.global MD5Update
.type	MD5Update,@function


// void  MD5Update ( MD5_CTX* context, const byte* input, size_t inputBytes )

#define CONTEXT		 4+32(%esp)
#define INPUT		 8+32(%esp)
#define INPUTBYTES	12+32(%esp)

#define	COUNT		16(%ebx)
#define BUFFER		24(%ebx)


MD5Update:
	pushal
	cld
	movl 	CONTEXT, %ebx

//  ByteIndex = (context->count[0] >> 3) & 0x3F;

	movl 	COUNT, %ebp
	shrl 	$3, %ebp
	andl 	$63,%ebp		// ebp = ByteIndex	

//  if ( (context->count[0] += inputBytes << 3 ) < (inputBytes << 3) )
//      context->count[1]++;
//  context->count[1] += inputBytes >> (32-3);

	movl	$8, %eax
	mull	INPUTBYTES
	addl	%eax, 0+COUNT
	adcl	%edx, 4+COUNT

// partLen = 64 - ByteIndex;

	movl	$64, %eax
	subl	%ebp, %eax		// eax = partLen

// i = 0;
// if ( partLen <= inputBytes ) {

	xorl	%ecx,%ecx		// ecx = i
	cmpl	INPUTBYTES, %eax	
	ja 	.Lende2

//      memcpy ( context->buffer + ByteIndex, input, partLen );

	leal	24(%ebx,%ebp,1), %edi
	movl	INPUT, %esi
	movl	%eax, %ecx
	rep	movsb
	
//      MD5Transform ( context->state, context->buffer, 1 );

	pushl	$1
	leal	BUFFER, %ecx
	pushl	%ecx
	push	%ebx
	call	__MD5TransformLocal

//      len = (inputBytes - partLen) / 64;

	movl	12+INPUTBYTES, %ecx
	subl	%eax, %ecx
	shrl	$6, %ecx

//      MD5Transform ( context->state, input+partLen, len );
	
	pushl	%ecx
	pushl	%esi
	pushl	%ebx
	call	__MD5TransformLocal
	addl	$24,%esp
	
//	i = partLen + 64 * len;
//	ByteIndex = 0;
	
	shll	$6, %ecx
	addl	%eax, %ecx
	xorl	%ebp, %ebp
	
.Lende2:	

//  }
//  memcpy ( context->buffer + ByteIndex, input + i, inputBytes - i );

	movl	INPUT, %esi
	addl	%ecx, %esi

	negl	%ecx
	addl	INPUTBYTES, %ecx
	
	leal	24(%ebx,%ebp,1), %edi
	rep	movsb

	popal
	ret


