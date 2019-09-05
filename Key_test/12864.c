/*
 * 12864.c
 *
 *  Created on: 2016��5��16��
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
��������: delay
��    ��: ��ʱ����
��    ��: ��
����ֵ  : ��
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
��������: Lcd_init_basic()
��    ��: 12864�˿ڳ�ʼ��
��    ��: ��
����ֵ  : ��
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

//Lcd_instruction(0x30) :�����趨;
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

//Lcd_instruction(0x06)��������趨;
LCD_RS_L;
LCD_RW_L;
LCD_Senddata(CMD, 0x06);
LCD_EN_H;
delay(1200);
LCD_EN_L;
delay(8640);
}





/******************************************
��������: LCD_Senddata
��    ��: ��12864Һ��д��һ���ֽ����ݻ���ָ��
��    ��: DatCmd--ΪDATʱ�����ݣ�ΪCMDʱ��ָ��
	  	  dByte--Ϊд��12864�����ݻ���ָ��
����ֵ  : ��
********************************************/


void LCD_Senddata(uint32_t DatCmad,uint32_t dByte)
{
	if(DatCmad==CMD)
		LCD_RS_L;//ָ�����
	else
		LCD_RS_H;//���ݲ���

	LCD_RW_L;
/////// д���� /////////
	uint32_t temp=0;
	temp=(dByte&0x01)*16;
	GPIOPinWrite(GPIO_PORTA_BASE,GPIO_PIN_4,temp);//��DB0дdByte�����λ
	temp=(dByte&0x02)*16;
	GPIOPinWrite(GPIO_PORTA_BASE,GPIO_PIN_5,temp);//��DB1дdByte�ĵ����ڶ�λ
	temp=(dByte&0x04)/2;
	GPIOPinWrite(GPIO_PORTH_BASE,GPIO_PIN_1,temp);//��DB2дdByte�ĵ�������λ
	temp=(dByte&0x08)*2;
	GPIOPinWrite(GPIO_PORTM_BASE,GPIO_PIN_4,temp);//��DB3дdByte�ĵ�������λ
	temp=dByte&0x10;
	GPIOPinWrite(GPIO_PORTE_BASE,GPIO_PIN_4,temp);//��DB4дdByte�ĵ�������λ
	temp=dByte&0x20;
	GPIOPinWrite(GPIO_PORTE_BASE,GPIO_PIN_5,temp);//��DB5дdByte�ĵ�������λ
    temp=(dByte&0x40)/4;
	GPIOPinWrite(GPIO_PORTN_BASE,GPIO_PIN_4,temp);//��DB6дdByte�ĵ�������λ
	temp=(dByte&0x80)/4;
	GPIOPinWrite(GPIO_PORTN_BASE,GPIO_PIN_5,temp);//��DB7дdByte�ĵ����ڰ�λ
	LCD_EN_H;
	delay(120);
	LCD_EN_L;
	delay(12000);
	delay(12000);

}

/*******************************************
��������: LCD12864_sendstr
��    ��: ��12864Һ��д��һ���ַ���
��    ��: ptString--�ַ���ָ��
����ֵ  : ��
********************************************/
void LCD_sendstr(uint8_t *ptString)
{
	while((*ptString)!='\0')		 //�ַ���δ����һֱд
	{
		LCD_Senddata(DAT,*ptString++);
	}
}

/*******************************************
��������: LCD12864_gotoXY
��    ��: �ƶ���ָ��λ��
��    ��: Row--ָ������
	  	  Col--ָ������
����ֵ  : ��
********************************************/
void LCD_gotoXY(uint32_t Row, uint32_t Col)
{
	switch (Row)		  //ѡ����
	{
		case 2:
			LCD_Senddata(CMD, LCD_L2 + Col); break;	//д���2�е�ָ����
		case 3:
			LCD_Senddata(CMD, LCD_L3 + Col); break;	//д���3�е�ָ����
		case 4:
			LCD_Senddata(CMD, LCD_L4 + Col); break;	//д���4�е�ָ����
		default:
			LCD_Senddata(CMD, LCD_L1 + Col); break;	//д���1�е�ָ����
	}
}

