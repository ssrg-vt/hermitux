/*
  Copyright (C) 2001, 2002 Thomas M. Ogrisegg

  This is free software. You can redistribute and
  modify it under the terms of the GNU General Public
  Public License.

  This file is part of the hppa-Port of the dietlibc

  __longjmp.S
    longjmp(3) implementation
*/

.level 1.1

.text

.export __longjmp
.globl  __longjmp
.type   __longjmp,@function

__longjmp:
	copy %r25, %r28

	/* Restore general registers */
	ldw 0x00(%sr0, %r26), %r3
	ldw 0x08(%sr0, %r26), %r4
	ldw 0x0c(%sr0, %r26), %r5
	ldw 0x10(%sr0, %r26), %r6
	ldw 0x14(%sr0, %r26), %r7
	ldw 0x18(%sr0, %r26), %r8
	ldw 0x1c(%sr0, %r26), %r9
	ldw 0x20(%sr0, %r26), %r10
	ldw 0x24(%sr0, %r26), %r11
	ldw 0x28(%sr0, %r26), %r12
	ldw 0x2c(%sr0, %r26), %r13
	ldw 0x30(%sr0, %r26), %r14
	ldw 0x34(%sr0, %r26), %r15
	ldw 0x38(%sr0, %r26), %r16
	ldw 0x3c(%sr0, %r26), %r17
	ldw 0x40(%sr0, %r26), %r18
	ldw 0x44(%sr0, %r26), %r19
	ldw 0x48(%sr0, %r26), %dp
	ldw 0x4c(%sr0, %r26), %sp
	ldw 0x50(%sr0, %r26), %rp

	ldo 0x58(%r26), %r20

	/* Restore the Floating point registers */
	fldd,ma 0x8(%sr0, %r20), %fr12
	fldd,ma 0x8(%sr0, %r20), %fr13
	fldd,ma 0x8(%sr0, %r20), %fr14
	fldd,ma 0x8(%sr0, %r20), %fr15
	fldd,ma 0x8(%sr0, %r20), %fr16
	fldd,ma 0x8(%sr0, %r20), %fr16
	fldd,ma 0x8(%sr0, %r20), %fr17
	fldd,ma 0x8(%sr0, %r20), %fr18
	fldd,ma 0x8(%sr0, %r20), %fr19
	fldd,ma 0x8(%sr0, %r20), %fr20
	fldd 0(%sr0, %r20), %fr21

	bv,n %r0(%rp)
