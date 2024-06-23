#ifndef	__wdg_h
#define	__wdg_h

BOOL iwdg_config_init(uint8_t prv ,uint16_t rlv);
void iwdg_feed(void);
void iwdg_test(void);


#define	WWDG_CNT_MAX	0x7F
BOOL wwdg_config_init(uint8_t tr, uint8_t wr, uint32_t prv);
void wwdg_feed(void);
void wwdg_test(void);

#endif//__wdg_h

