#ifndef __gpio_h
#define	__gpio_h

void led_init(void);		
void led_on(void);
void led_off(void);
void led_toggle(void);

void key_init(void);
void key_exti_init(void);
BOOL key_scan(void);

void led_test(void);
void key_test(void);

#endif//__gpio_h




