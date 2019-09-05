/*
 * 12864.c
 *
 *  Created on: 2016年5月16日
 *      Author: Administrator
 */


#include <stdint.h>
#include "inc/tm4c1294ncpdt.h"
#include <stdbool.h>
#include "driverlib/sysctl.h"
#include "driverlib/gpio.h"
#include"inc/hw_types.h"
#include"inc/hw_memmap.h"
#include"driverlib/timer.h"
#include"driverlib/interrupt.h"
#include"driverlib/systick.h"
#include "driverlib/rom.h"
#include "driverlib/rom_map.h"
#include <string.h>
#include "12864.h"


void Lcd_init_basic();
void LCD_Senddata(uint32_t DatCmad,uint32_t dByte);
void delay(uint32_t num);
void LCD_sendstr(uint8_t *ptString);
void LCD_gotoXY(uint32_t Row, uint32_t Col);

/******************************************
函数名称: delay
功    能: 延时函数
参    数: 无
返回值  : 无
********************************************/
void delay(uint32_t num){

 TimerIntClear(TIMER0_BASE,TIMER_TIMA_TIMEOUT);
  TimerConfigure(TIMER0_BASE,TIMER_CFG_PERIODIC);
    TimerLoadSet(TIMER0_BASE,TIMER_A,num-1);
TimerIntEnable(TIMER0_BASE, TIMER_TIMA_TIMEOUT);
 TimerEnable(TIMER0_BASE, TIMER_A);
 while(!TimerIntStatus(TIMER0_BASE,false)){

	}
}

/******************************************
函数名称: Lcd_init_basic()
功    能: 12864端口初始化
参    数: 无
返回值  : 无
********************************************/
void Lcd_init_basic(){

SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOB);
SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOK);
SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);
SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOH);
SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOM);
SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOE);
SysCtlPeripheralEnable(SYSCTL_PERIPH_GPION);
SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER0);

LCD_RS_RW;
LCD_EN_PSB;
LCD_RST;

LCD_DB0_DB1;
LCD_DB2;
LCD_DB3;
LCD_DB4_DB5;
LCD_DB6_DB7;
LCD_PSB_EN;
delay(4800000);
LCD_RST_NO;
delay(1200);
LCD_RST_EN;

//Lcd_instruction(0x30) :功能设定;
LCD_RS_L;
LCD_RW_L;
LCD_Senddata(CMD, 0x30);
LCD_EN_H;
delay(1200);
LCD_EN_L;
delay(17280);

//Lcd_instruction(0x0C) :display on;
LCD_RS_L;
LCD_RW_L;
LCD_Senddata(CMD, 0x0C);
LCD_EN_H;
delay(1200);
LCD_EN_L;
delay(12000);


//Lcd_instruction(0x01) :clear;
LCD_RS_L;
LCD_RW_L;
LCD_Senddata(CMD, 0x01);
LCD_EN_H;
delay(1200);
LCD_EN_L;
delay(1200000);

//Lcd_instruction(0x06)：进入点设定;
LCD_RS_L;
LCD_RW_L;
LCD_Senddata(CMD, 0x06);
LCD_EN_H;
delay(1200);
LCD_EN_L;
delay(8640);
}





/******************************************
函数名称: LCD_Senddata
功    能: 向12864液晶写入一个字节数据或者指令
参    数: DatCmd--为DAT时是数据，为CMD时是指令
	  	  dByte--为写入12864的数据或者指令
返回值  : 无
********************************************/


void LCD_Senddata(uint32_t DatCmad,uint32_t dByte)
{
	if(DatCmad==CMD)
		LCD_RS_L;//指令操作
	else
		LCD_RS_H;//数据操作

	LCD_RW_L;
/////// 写操作 /////////
	uint32_t temp=0;
	temp=(dByte&0x01)*16;
	GPIOPinWrite(GPIO_PORTA_BASE,GPIO_PIN_4,temp);//向DB0写dByte的最低位
	temp=(dByte&0x02)*16;
	GPIOPinWrite(GPIO_PORTA_BASE,GPIO_PIN_5,temp);//向DB1写dByte的倒数第二位
	temp=(dByte&0x04)/2;
	GPIOPinWrite(GPIO_PORTH_BASE,GPIO_PIN_1,temp);//向DB2写dByte的倒数第三位
	temp=(dByte&0x08)*2;
	GPIOPinWrite(GPIO_PORTM_BASE,GPIO_PIN_4,temp);//向DB3写dByte的倒数第四位
	temp=dByte&0x10;
	GPIOPinWrite(GPIO_PORTE_BASE,GPIO_PIN_4,temp);//向DB4写dByte的倒数第五位
	temp=dByte&0x20;
	GPIOPinWrite(GPIO_PORTE_BASE,GPIO_PIN_5,temp);//向DB5写dByte的倒数第六位
    temp=(dByte&0x40)/4;
	GPIOPinWrite(GPIO_PORTN_BASE,GPIO_PIN_4,temp);//向DB6写dByte的倒数第七位
	temp=(dByte&0x80)/4;
	GPIOPinWrite(GPIO_PORTN_BASE,GPIO_PIN_5,temp);//向DB7写dByte的倒数第八位
	LCD_EN_H;
	delay(120);
	LCD_EN_L;
	delay(12000);
	delay(12000);

}

/*******************************************
函数名称: LCD12864_sendstr
功    能: 向12864液晶写入一个字符串
参    数: ptString--字符串指针
返回值  : 无
********************************************/
void LCD_sendstr(uint8_t *ptString)
{
	while((*ptString)!='\0')		 //字符串未结束一直写
	{
		LCD_Senddata(DAT,*ptString++);
	}
}

/*******************************************
函数名称: LCD12864_gotoXY
功    能: 移动到指定位置
参    数: Row--指定的行
	  	  Col--指定的列
返回值  : 无
********************************************/
void LCD_gotoXY(uint32_t Row, uint32_t Col)
{
	switch (Row)		  //选择行
	{
		case 2:
			LCD_Senddata(CMD, LCD_L2 + Col); break;	//写入第2行的指定列
		case 3:
			LCD_Senddata(CMD, LCD_L3 + Col); break;	//写入第3行的指定列
		case 4:
			LCD_Senddata(CMD, LCD_L4 + Col); break;	//写入第4行的指定列
		default:
			LCD_Senddata(CMD, LCD_L1 + Col); break;	//写入第1行的指定列
	}
}

