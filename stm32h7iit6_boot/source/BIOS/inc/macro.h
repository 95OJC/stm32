#ifndef __MACRO_H
#define __MACRO_H

/* ��CPU���ȼ�����Ϊ0(�ɱ�����ȼ��е�������ȼ�)�����ܲ���NMI��hardfault�ȸ������ȼ����쳣 */
#define	DI()	__disable_irq()//��__asm volatile ("cpsid i")
#define	EI()	__enable_irq()//��__asm volatile ("cpsie i")

#define	DIP()	__set_PRIMASK(1)//��Ч��__disable_irq()
#define	EIP()	__set_PRIMASK(0)//��Ч��__enable_irq()

/* ��CPU���ȼ�����Ϊ-1�����ܲ���NMI�쳣��hardfault�쳣�������������Ϊhardfault���ȼ�ҲΪ-1 */
#define	DIF()	__set_FAULTMASK(1)
#define	EIF()	__set_FAULTMASK(0)


/* ����洢�ռ���� */
#define	SRAMCODE	__attribute__(section(".sramcode"))



#define MEM_CPY(d,s,n)		memcpy((d),(s),(n))
#define MEM_SET(addr,d,n)	memset((addr),(d),(n))
#define MEM_CMP(addr,d,n)	memcmp((addr),(d),(n))
#define STR_CPY(d,s)        strcpy((d),(s))
#define STR_LEN(s)          strlen(s)
#define STR_CMP(s1,s2)      strcmp(s1,s2)
#define STR_STR(s1,s2)      strstr(s1,s2)

#endif//__MACRO_H


