#ifndef __MACRO_H
#define __MACRO_H

/* 把CPU优先级提升为0(可编程优先级中的最高优先级)，仅能产生NMI和hardfault等负数优先级的异常 */
#define	DI()	__disable_irq()//即__asm volatile ("cpsid i")
#define	EI()	__enable_irq()//即__asm volatile ("cpsie i")

#define	DIP()	__set_PRIMASK(1)//等效于__disable_irq()
#define	EIP()	__set_PRIMASK(0)//等效于__enable_irq()

/* 把CPU优先级提升为-1，仅能产生NMI异常，hardfault异常都不会产生，因为hardfault优先级也为-1 */
#define	DIF()	__set_FAULTMASK(1)
#define	EIF()	__set_FAULTMASK(0)


/* 代码存储空间分配 */
#define	SRAMCODE	__attribute__(section(".sramcode"))



#define MEM_CPY(d,s,n)		memcpy((d),(s),(n))
#define MEM_SET(addr,d,n)	memset((addr),(d),(n))
#define MEM_CMP(addr,d,n)	memcmp((addr),(d),(n))
#define STR_CPY(d,s)        strcpy((d),(s))
#define STR_LEN(s)          strlen(s)
#define STR_CMP(s1,s2)      strcmp(s1,s2)
#define STR_STR(s1,s2)      strstr(s1,s2)

#endif//__MACRO_H


