#include "common.h"
#include "boot.h"

#define AppAddr  0x08100000    /* APP��ַ */

void JumpToApp(void)
{
	uint32_t i=0;
	
	typedef void (*AppJump)(void); /* ����һ������ָ�� */
	
	/* �ر�ȫ���ж� */
	DI();
	
	/* ��������ʱ�ӵ�Ĭ��״̬��ʹ��HSIʱ�� */
	HAL_RCC_DeInit();
	
	/* �رյδ�ʱ������λ��Ĭ��ֵ */
	SysTick->CTRL = 0;
  SysTick->LOAD = 0;
  SysTick->VAL = 0;	
	
	/* �ر������жϣ���������жϹ����־ */
	for (i = 0; i < 8; i++)
	{
		NVIC->ICER[i]=0xFFFFFFFF;
		NVIC->ICPR[i]=0xFFFFFFFF;
	}	
	
	/* ��ת��Ӧ�ó����׵�ַ��MSP����ַ+4�Ǹ�λ�жϷ�������ַ��Ȼ��ִ�иõ�ַ��ֵ���Ǹ�λ�жϷ������ */
	//AppJump = (void (*)(void)) (*((uint32_t *) (AppAddr + 4)));
	AppJump AppJumpp = (AppJump) ( *( (uint32_t *)(AppAddr + 4) ) );
	
	/* ��������ջָ�� */
	__set_MSP(*(uint32_t *)AppAddr);	
	
	/* �����ж��������ַ */
	SCB->VTOR = AppAddr;
	/* ��ת��app */
	//AppJump(); 
	AppJumpp();
	
	/* ��ת�ɹ��Ļ�������ִ�е�����û�������������Ӵ��� */
	while (1)
	{

	}	
}

