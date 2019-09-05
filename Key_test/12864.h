/*
 * 12864.h
 *
 *  Created on: 2016年5月15日
 *      Author: Administrator
 */

#ifndef __12864LCD_H_
#define __12864LCD_H_

//*****************************************************************************
//DATA-SIGNALS
#define LCD_DB0_DB1    GPIOPinTypeGPIOOutput(GPIO_PORTA_BASE,GPIO_PIN_4|GPIO_PIN_5) //PA4,PA5
#define LCD_DB2    	   GPIOPinTypeGPIOOutput(GPIO_PORTH_BASE,GPIO_PIN_1)//PH1
#define LCD_DB3    	   GPIOPinTypeGPIOOutput(GPIO_PORTM_BASE,GPIO_PIN_4)//PM4
#define LCD_DB4_DB5    GPIOPinTypeGPIOOutput(GPIO_PORTE_BASE,GPIO_PIN_4|GPIO_PIN_5)//PE4,PE5
#define LCD_DB6_DB7    GPIOPinTypeGPIOOutput(GPIO_PORTN_BASE,GPIO_PIN_4|GPIO_PIN_5)//PN4,PN5
//CONTROL-SIGNALS
#define LCD_RS_RW    GPIOPinTypeGPIOOutput(GPIO_PORTB_BASE,GPIO_PIN_4|GPIO_PIN_5)//PB4,PB5
#define LCD_EN_PSB   GPIOPinTypeGPIOOutput(GPIO_PORTK_BASE,GPIO_PIN_2|GPIO_PIN_3)//PK2,PK3
#define LCD_RST   	 GPIOPinTypeGPIOOutput(GPIO_PORTM_BASE,GPIO_PIN_6)//PM6

//CONTROL SIGNALS WRITING(5个控制引脚的操作)
#define LCD_RS_H	GPIOPinWrite(GPIO_PORTB_BASE,GPIO_PIN_4,0x10)//RS置高
#define LCD_RS_L	GPIOPinWrite(GPIO_PORTB_BASE,GPIO_PIN_4,0x00)//RS置低

#define LCD_RW_H	GPIOPinWrite(GPIO_PORTB_BASE,GPIO_PIN_5,0x20)//RW置高
#define LCD_RW_L	GPIOPinWrite(GPIO_PORTB_BASE,GPIO_PIN_5,0x00)//RW置低

#define LCD_EN_H	GPIOPinWrite(GPIO_PORTK_BASE,GPIO_PIN_2,0x4)//EN置高
#define LCD_EN_L	GPIOPinWrite(GPIO_PORTK_BASE,GPIO_PIN_2,0x00)//EN置低

#define LCD_PSB_EN	GPIOPinWrite(GPIO_PORTK_BASE,GPIO_PIN_3,0x8)//PSB置高
#define LCD_PSB_No	GPIOPinWrite(GPIO_PORTK_BASE,GPIO_PIN_3,0x00)//PSB置低

#define LCD_RST_EN	GPIOPinWrite(GPIO_PORTM_BASE,GPIO_PIN_6,0x40)//RST置高
#define LCD_RST_NO	GPIOPinWrite(GPIO_PORTM_BASE,GPIO_PIN_6,0x00)//RST置低

#define LCD_L1		0x80	//第一行的地址
#define LCD_L2		0x90	//第二行的地址
#define LCD_L3		0x88	//第三行的地址
#define LCD_L4		0x98	//第四行的地址

#define DAT 1       //数据标志
#define CMD 0       //指令标志

volatile unsigned long long int clock,period;
extern void Lcd_init_basic();
extern void Lcd_data();
extern void LCD_Senddata(uint32_t DatCmad,uint32_t dByte);
extern void delay(uint32_t num);
extern void LCD_sendstr(uint8_t *ptString);
extern void LCD_gotoXY(uint32_t Row, uint32_t Col);


#endif /* 12864_H_ */
