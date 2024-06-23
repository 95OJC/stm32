#ifndef __OLED096_H
#define	__OLED096_H

#ifdef USE_OLED_LIB

//OLED¿ØÖÆÓÃº¯Êý
void OLED_WR_Byte(unsigned dat,unsigned cmd);  
void OLED_Display_On(void);
void OLED_Display_Off(void);	   							   		    
void OLED_Init(void);
void OLED_Clear(void);
//void OLED_DrawPoint(uint8_t x,uint8_t y,uint8_t t);
//void OLED_Fill(uint8_t x1,uint8_t y1,uint8_t x2,uint8_t y2,uint8_t dot);
void OLED_ShowChar(uint8_t x,uint8_t y,uint8_t chr,uint8_t Char_Size);
void OLED_ShowNum(uint8_t x,uint8_t y,uint32_t num,uint8_t len,uint8_t size);
void OLED_ShowString(uint8_t x,uint8_t y, uint8_t *p,uint8_t Char_Size);	 
void OLED_Set_Pos(unsigned char x, unsigned char y);
void OLED_ShowCHinese(uint8_t x,uint8_t y,uint8_t no);
void OLED_DrawBMP(unsigned char x0, unsigned char y0,unsigned char x1, unsigned char y1,unsigned char BMP[]);
//void Delay_50ms(unsigned int Del_50ms);
//void Delay_1ms(unsigned int Del_1ms);
void fill_picture(unsigned char fill_Data);

void Write_IIC_Command(unsigned char IIC_Command);
void Write_IIC_Data(unsigned char IIC_Data);
void Write_IIC_Byte(unsigned char IIC_Byte);

#else
void OLED096_init(void);
void OLED096_full_test(void);
void OLED096_clear_test(void);

#if 1
void OLED096_SoftIIC_init(void);
void OLED096_SoftIIC_full_test(void);
void OLED096_SoftIIC_clear_test(void);
#endif//soft iic

#endif//USE_OLED_LIB

#endif//__OLED096_H

