#ifndef __keyTTP224_h
#define __keyTTP224_h


enum{
	KEY_NULL		= 0,
	KEY_PUMP		= 1,
	KEY_HEAT		= 2,
	KEY_TEMP_UP		= 3,
	KEY_TEMP_DOWN	= 4,
};

void keyTTP224_init(void);
u8 keyTTP224_scan(void);
u8 shell_keyTTP224_test(void);


#endif//__keyTTP224_h

