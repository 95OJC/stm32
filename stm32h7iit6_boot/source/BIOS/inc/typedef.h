/******************************************************************************
* Copyright(C) 2017-2018 DASCOM Corporation All Rights Reserved
******************************************************************************/

#ifndef _TYPEDEFINE_H
#define _TYPEDEFINE_H

//#ifdef __cplusplus
//extern "C" {
//#endif
#include "stdint.h"

typedef unsigned char   U8;
typedef signed char     S8;
typedef unsigned short  U16;
typedef signed short    S16;
typedef unsigned int    U32;
typedef signed int      S32;
typedef char		    C8;

#define u8              U8
#define s8              S8
#define c8              C8
#define u16             U16
#define s16             S16
#define u32             U32
#define s32             S32

#ifndef FLOAT
#define FLOAT		float
#endif

#ifndef DOUBLE
#define DOUBLE		double
#endif

#ifndef VOID
#define VOID		void
#endif


#ifndef NULL
#define NULL		0
#endif

#ifndef null
#define null		NULL
#endif

#ifndef STATIC
#define STATIC 		static
#endif

#ifndef CONST
#define CONST 		const
#endif

#ifndef EXTERN
#define EXTERN 		extern
#endif

#ifndef BOOL
#define BOOL        u8
#endif

#ifndef FALSE
#define FALSE		0
#endif

#ifndef TRUE
#define TRUE		1
#endif


//#ifdef __cplusplus
//}
//#endif

#endif
