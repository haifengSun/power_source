#include <stdbool.h>
#include <stdint.h>
#include "driverlib/rom.h"
#include "driverlib/rom_map.h"
#include "inc/hw_gpio.h"
#include "inc/hw_ints.h"
#include "inc/hw_memmap.h"
#include "inc/hw_sysctl.h"
#include "inc/hw_types.h"
#include "driverlib/debug.h"
#include "driverlib/gpio.h"
#include "driverlib/interrupt.h"
#include "key.h"
#include "driverlib/sysctl.h"

unsigned char key_pressed;//按键是否按下标志位
unsigned char key_val;//按键值存储变量
unsigned char key_flag;//按键事件标志
/*******************************************
函数名称: key_init()
功    能: 键盘相关端口变量初始化
参    数: 无
返回值  : 无
********************************************/

void key_init()
{
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOD);
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOP);

	///////解锁PD7//////
	HWREG(GPIO_PORTD_BASE + GPIO_O_LOCK) = GPIO_LOCK_KEY;
	HWREG(GPIO_PORTD_BASE + GPIO_O_CR)  = 0x80;
	ROM_GPIOPinTypeGPIOOutput(GPIO_PORTD_BASE, GPIO_PIN_7);
	HWREG(GPIO_PORTD_BASE + GPIO_O_LOCK) = GPIO_LOCK_KEY;
	HWREG(GPIO_PORTD_BASE + GPIO_O_CR)   = 0x00;
	HWREG(GPIO_PORTD_BASE + GPIO_O_LOCK) = 0;
	///////解锁PD7//////

	GPIOPinTypeGPIOOutput(GPIO_PORTD_BASE, GPIO_PIN_1|GPIO_PIN_4|GPIO_PIN_5|GPIO_PIN_7);
	GPIOPinTypeGPIOInput(GPIO_PORTP_BASE, GPIO_PIN_2|GPIO_PIN_3|GPIO_PIN_4|GPIO_PIN_5);    //初始化键盘行、列引脚

	 key_pressed = 0;
	 key_val = 255;
	 key_flag = 0;
}

/*******************************************
函数名称: keyWrite
功    能: 向键盘行列引脚写数据
参    数: ui8Val---待写入键盘引脚的数据
返回值  : 无
********************************************/
void
KeyWrite(uint8_t ui8Val)
{
	GPIOPinWrite(GPIO_PORTD_BASE, GPIO_PIN_1, (ui8Val & 0x01)*2);//将ui8Val最低位写入PD1
	GPIOPinWrite(GPIO_PORTD_BASE, GPIO_PIN_4, (ui8Val & 0x02)*8);//将ui8Val倒数第二位写入PD4
	GPIOPinWrite(GPIO_PORTD_BASE, GPIO_PIN_5, (ui8Val & 0x04)*8);//将ui8Val倒数第三位写入PD5
	GPIOPinWrite(GPIO_PORTD_BASE, GPIO_PIN_7, (ui8Val & 0x08)*16);//将ui8Val倒数第四位写入PD7
	GPIOPinWrite(GPIO_PORTP_BASE, GPIO_PIN_2, (ui8Val & 0x10)/4);//将ui8Val倒数低第五位写入PP2
	GPIOPinWrite(GPIO_PORTP_BASE, GPIO_PIN_3, (ui8Val & 0x20)/4);//将ui8Val倒数低第六位写入PP3
	GPIOPinWrite(GPIO_PORTP_BASE, GPIO_PIN_4, (ui8Val & 0x40)/4);//将ui8Val倒数低第七位写入PP4
	GPIOPinWrite(GPIO_PORTP_BASE, GPIO_PIN_5, (ui8Val & 0x80)/4);//将ui8Val最高位写入PP5

}

/*******************************************
函数名称:  KeyRead
功    能: 读取键盘引脚的电平状态
参    数: 无
返回值  : 键盘引脚的电平状态
********************************************/
uint8_t KeyRead(void)
{
	uint8_t temp;
	int32_t temp1,temp2;
	temp1=GPIOPinRead(GPIO_PORTD_BASE, GPIO_PIN_1|GPIO_PIN_4|GPIO_PIN_5|GPIO_PIN_7);
	temp2=GPIOPinRead(GPIO_PORTP_BASE, GPIO_PIN_2|GPIO_PIN_3|GPIO_PIN_4|GPIO_PIN_5);
	temp=(temp1&0x02)/2+(temp1&0x30)/8+(temp1&0x80)/16+(temp2&0x3C)*4;//temp从最高位到最低位的值分别等于PP5,PP4,PP3,PP2,PD7,PD5,PD4,PD1
	return temp;
}

/*******************************************
函数名称: check_key
功    能: 获得键值
参    数: 无
返回值  : 如果有键按下，返回相应键值，如果没有，返回0xFF；
********************************************/
void check_key(void)
{
	unsigned char row ,col,tempout,tempin,tmp1,tmp2;
	// tmp1用来设置键盘引脚输出值，使列引脚电平低四位中有一个为0
	tmp1 = 0x08;
	for(col=0;col<4;col++)
	{
		KeyWrite(0x0F);//输出引脚电平低四位为全1
		tempout=0x0F-tmp1;
		KeyWrite(tempout);//输出引脚电平低四位有一个为0
		tmp1 /= 2; // tmp1 右移一位
		tempin=KeyRead();
		if ((tempin & 0xF0) < 0xF0)// 是否键盘引脚高四位有一个为0
		{ 
			tmp2 = 0x10; // tmp2用于检测出哪一位为0
			for(row =0;row<4;row++)// 行检测
			{ 
				if((tempin & tmp2)==0x00)// 是否是该行
				{ 
					key_val = row*4 + col;  // 获取键值 退出循环
				}
				tmp2 *= 2; // tmp2左移一位
			}
		}
	}
}
/*******************************************
函数名称: key_event
功    能: 检测是否有键按下
参    数: 无
返回值  : 无
********************************************/
void key_event(void)
{
	unsigned char tmp;
	KeyWrite(0xF0);// 设置键盘引脚输出值 使得如果有键按下 键盘引脚电平将不为全高
	tmp = KeyRead();
	if ((key_pressed == 0x00)&&((tmp & 0xF0) < 0xF0))//是否有键按下
	{ 
		key_pressed = 1; // 如果有按键按下，设置key_pressed标识
		SysCtlDelay(1000000); //消除抖动
		check_key(); // 调用check_key(),获取键值
	}
	else if ((key_pressed ==1)&&((tmp & 0xF0) == 0xF0))//是否按键已经释放
	{ 
		key_pressed = 0; // 清除key_pressed标识
		key_flag = 1;
	}
}


