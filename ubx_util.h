#ifndef __UBX_UTIL_H
#define __UBX_UTIL_H
/*
	Type conversions
*/

#define READ_UCHAR(p)	(*(p))
#define READ_USHORT(p)	((*(p+1)<<8)|(*(p)))
#define READ_ULONG(p)	((*(p+3)<<24)|(*(p+2)<<16)|(*(p+1)<<8)|(*(p)))
#define READ_SCHAR(p)	(signed char)READ_UCHAR(p)
#define READ_SSHORT(p)	(signed char)READ_USHORT(p)
#define READ_SLONG(p)	(signed char)READ_ULONG(p)
#define READ_FLOAT(p) (float)(((u4)(*(p+3))<<24)|((u4)(*(p+2))<<16)|((u4)(*(p+1))<<8)|((u4)(*(p+0)))
#define READ_DOUBLE(p) (double)(((u8)(*(p+7))<<56)|((u8)(*(p+6))<<48)|((u8)(*(p+5))<<40)|((u8)(*(p+4))<<32)|((u8)(*(p+3))<<24)|((u8)(*(p+2))<<16)|((u8)(*(p+1))<<8)|((u8)(*(p+0)))

#endif