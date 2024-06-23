#ifndef __regwrApi_h
#define	__regwrApi_h

#define	REG8(x)					(*((volatile u8*)(x)))
#define	REG16(x)				(*((volatile u16*)(x)))
#define	REG32(x)				(*((volatile u32*)(x)))

#define	read_reg8(x)			REG8(x)
#define	read_reg16(x)			REG16(x)
#define	read_reg32(x)			REG32(x)

#define	write_reg8(x,d)			{REG8(x) = (d);}
#define	write_reg16(x,d)		{REG16(x) = (d);}
#define	write_reg32(x,d)		{REG32(x) = (d);}

#define	OrBit_reg8(x,b)			{REG8(x) |= (b);}
#define	OrBit_reg16(x,b)		{REG16(x) |= (b);}
#define	OrBit_reg32(x,b)		{REG32(x) |= (b);}

#define	ClsBit_reg8(x,b)		{REG8(x) &= ~(b);}
#define	ClsBit_reg16(x,b)		{REG16(x) &= ~(b);}
#define	ClsBit_reg32(x,b)		{REG32(x) &= ~(b);}

#define	Toggle_reg8(x,b)		{REG8(x) ^= (b);}
#define	Toggle_reg16(x,b)		{REG16(x) ^= (b);}
#define	Toggle_reg32(x,b)		{REG32(x) ^= (b);}

	

#endif//__regwrApi_h

