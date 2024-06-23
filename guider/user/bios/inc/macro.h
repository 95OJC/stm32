#ifndef __macro_h
#define	__macro_h

#ifndef BOOL
#define BOOL        u8
#endif

#ifndef	FALSE
#define	FALSE	0;
#endif

#ifndef	TRUE
#define	TRUE	1;
#endif

#ifndef NULL
#define NULL	0
#endif

#ifndef	null
#define null	0
#endif


typedef void *HWD;
typedef void *(*CallBackFunc)(void *p);


#define	ARRAY_SIZE(x)				(sizeof(x)/sizeof((x)[0]))
#define	BIT_CLR(x,bit)				{(x) &= ~ (1 << (bit));}
#define	BIT_SET(x,bit)				{(x) |= (1 << (bit));}
#define	BIT_IS(x,bit)				((x) & (1 << (bit)))	//���з���ֵ 0��1
#define MAX_VAL(x, y)				((x) < (y) ? (y) : (x))
#define MIN_VAL(x, y)				((x) < (y) ? (x) : (y))

#define UINT8_BREAK_HI(Byte)    ((Byte) >> 4)
#define UINT8_BREAK_LO(Byte)    ((Byte) & 0x0f)
#define BUILD_UINT8(loByte,hiByte)    (u8)(UINT8_BREAK_LO(loByte) + (((hiByte)&0x0f)<<4)) 

#define UINT8_BUILD_UINT16(loByte,hiByte)	\
			(((u16)(hiByte)<<8) + (u16)(loByte))

#define	UINT8_BUILD_UINT32(Byte0,Byte1,Byte2,Byte3) \
			(((u32)(Byte3)<<24)   \
			+ ((u32)(Byte2)<<16)  \
			+ ((u32)(Byte1)<<8)   \
			+ (u32)(Byte0))

#define	UINT16_BREAK_HiByte(xWORD)			((u8)((xWORD) >> 8))
#define	UINT16_BREAK_LoByte(xWORD)			((u8)(xWORD))

/* xDWORD : uint32,  ByteNum : (0 - 3) */
#define UINT32_BREAK_UINT8(xDWORD,ByteNum)	\
			(u8)(((xDWORD)>>(ByteNum*8)) & 0xff)

/* xDWORD : uint32,  WORDNum : (0 - 1) */
#define UINT32_BREAK_UINT16(xDWORD,WORDNum)	\
			(u8)(((xDWORD)>>(WORDNum*16)) & 0xffff)

#define BUILD_UINT16_HL(hiByte,loByte)  				UINT8_BUILD_UINT16(loByte, hiByte)
#define BUILD_UINT32_HL(Byte3, Byte2, Byte1, Byte0)  	UINT8_BUILD_UINT32(Byte0, Byte1, Byte2, Byte3)


#define MEM_CPY(Daddr,Saddr,l)		memcpy((Daddr),(Saddr),(l))
#define MEM_SET(addr,val,l)			memset((addr),(val),(l))
//#define	STR_CPY(d,s)		strcpy((d),(s))//��sָ����ַ������Ƶ�d
//#define	STR_LEN(s)			strlen(s)//�����ַ������ȣ�ֱ��\0����,��������������
//#define	STR_CMP(s1,s2)		strcmp(s1,s1)//��ASCII����Ա��ַ�����С��ֱ��������ͬ�ַ���\0����������0�ַ������


#endif//__macro_h

