#ifndef __adda_h
#define __adda_h

//nvic
#define	ADC_NVIC_IRQN               ADC1_2_IRQn
#define	ADC_NVIC_PRIO				0x01

//gpio
#define	ADC_GPIO_CLOCK_FUN(r,s)		RCC_APB2PeriphClockCmd(r,s)
#define	ADC_GPIO_CLOCK				RCC_APB2Periph_GPIOC
#define	ADC_PORT                    GPIOC
#define	ADC_PIN                     GPIO_Pin_1	

//adc
#define	ADC_CLOCK_FUN(r,s)			RCC_APB2PeriphClockCmd(r,s)		
#define ADC_CLOCK					RCC_APB2Periph_ADC2
#define ADCx						ADC2			
#define	ADC_CHANNEL                 ADC_Channel_11


void ad_init(void);

//-------------------------------------------------------------------
//gpio
#define	DAC_GPIO_CLOCK_FUN(r,s)		RCC_APB2PeriphClockCmd(r,s)
#define	DAC_GPIO_CLOCK				RCC_APB2Periph_GPIOA
#define	DAC_PORT                    GPIOA
#define	DAC_PIN                     (GPIO_Pin_4 | GPIO_Pin_5)

//dac
#define	DAC_CLOCK_FUN(r,s)			RCC_APB1PeriphClockCmd(r,s)		
#define DAC_CLOCK					RCC_APB1Periph_DAC

void da_init(void);

#endif//__adda_h


