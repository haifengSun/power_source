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

unsigned char key_pressed;//�����Ƿ��±�־λ
unsigned char key_val;//����ֵ�洢����
unsigned char key_flag;//�����¼���־
/*******************************************
��������: key_init()
��    ��: ������ض˿ڱ�����ʼ��
��    ��: ��
����ֵ  : ��
********************************************/

void key_init()
{
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOD);
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOP);

	///////����PD7//////
	HWREG(GPIO_PORTD_BASE + GPIO_O_LOCK) = GPIO_LOCK_KEY;
	HWREG(GPIO_PORTD_BASE + GPIO_O_CR)  = 0x80;
	ROM_GPIOPinTypeGPIOOutput(GPIO_PORTD_BASE, GPIO_PIN_7);
	HWREG(GPIO_PORTD_BASE + GPIO_O_LOCK) = GPIO_LOCK_KEY;
	HWREG(GPIO_PORTD_BASE + GPIO_O_CR)   = 0x00;
	HWREG(GPIO_PORTD_BASE + GPIO_O_LOCK) = 0;
	///////����PD7//////

	GPIOPinTypeGPIOOutput(GPIO_PORTD_BASE, GPIO_PIN_1|GPIO_PIN_4|GPIO_PIN_5|GPIO_PIN_7);
	GPIOPinTypeGPIOInput(GPIO_PORTP_BASE, GPIO_PIN_2|GPIO_PIN_3|GPIO_PIN_4|GPIO_PIN_5);    //��ʼ�������С�������

	 key_pressed = 0;
	 key_val = 255;
	 key_flag = 0;
}

/*******************************************
��������: keyWrite
��    ��: �������������д����
��    ��: ui8Val---��д��������ŵ�����
����ֵ  : ��
********************************************/
void
KeyWrite(uint8_t ui8Val)
{
	GPIOPinWrite(GPIO_PORTD_BASE, GPIO_PIN_1, (ui8Val & 0x01)*2);//��ui8Val���λд��PD1
	GPIOPinWrite(GPIO_PORTD_BASE, GPIO_PIN_4, (ui8Val & 0x02)*8);//��ui8Val�����ڶ�λд��PD4
	GPIOPinWrite(GPIO_PORTD_BASE, GPIO_PIN_5, (ui8Val & 0x04)*8);//��ui8Val��������λд��PD5
	GPIOPinWrite(GPIO_PORTD_BASE, GPIO_PIN_7, (ui8Val & 0x08)*16);//��ui8Val��������λд��PD7
	GPIOPinWrite(GPIO_PORTP_BASE, GPIO_PIN_2, (ui8Val & 0x10)/4);//��ui8Val�����͵���λд��PP2
	GPIOPinWrite(GPIO_PORTP_BASE, GPIO_PIN_3, (ui8Val & 0x20)/4);//��ui8Val�����͵���λд��PP3
	GPIOPinWrite(GPIO_PORTP_BASE, GPIO_PIN_4, (ui8Val & 0x40)/4);//��ui8Val�����͵���λд��PP4
	GPIOPinWrite(GPIO_PORTP_BASE, GPIO_PIN_5, (ui8Val & 0x80)/4);//��ui8Val���λд��PP5

}

/*******************************************
��������:  KeyRead
��    ��: ��ȡ�������ŵĵ�ƽ״̬
��    ��: ��
����ֵ  : �������ŵĵ�ƽ״̬
********************************************/
uint8_t KeyRead(void)
{
	uint8_t temp;
	int32_t temp1,temp2;
	temp1=GPIOPinRead(GPIO_PORTD_BASE, GPIO_PIN_1|GPIO_PIN_4|GPIO_PIN_5|GPIO_PIN_7);
	temp2=GPIOPinRead(GPIO_PORTP_BASE, GPIO_PIN_2|GPIO_PIN_3|GPIO_PIN_4|GPIO_PIN_5);
	temp=(temp1&0x02)/2+(temp1&0x30)/8+(temp1&0x80)/16+(temp2&0x3C)*4;//temp�����λ�����λ��ֵ�ֱ����PP5,PP4,PP3,PP2,PD7,PD5,PD4,PD1
	return temp;
}

/*******************************************
��������: check_key
��    ��: ��ü�ֵ
��    ��: ��
����ֵ  : ����м����£�������Ӧ��ֵ�����û�У�����0xFF��
********************************************/
void check_key(void)
{
	unsigned char row ,col,tempout,tempin,tmp1,tmp2;
	// tmp1�������ü����������ֵ��ʹ�����ŵ�ƽ����λ����һ��Ϊ0
	tmp1 = 0x08;
	for(col=0;col<4;col++)
	{
		KeyWrite(0x0F);//������ŵ�ƽ����λΪȫ1
		tempout=0x0F-tmp1;
		KeyWrite(tempout);//������ŵ�ƽ����λ��һ��Ϊ0
		tmp1 /= 2; // tmp1 ����һλ
		tempin=KeyRead();
		if ((tempin & 0xF0) < 0xF0)// �Ƿ�������Ÿ���λ��һ��Ϊ0
		{ 
			tmp2 = 0x10; // tmp2���ڼ�����һλΪ0
			for(row =0;row<4;row++)// �м��
			{ 
				if((tempin & tmp2)==0x00)// �Ƿ��Ǹ���
				{ 
					key_val = row*4 + col;  // ��ȡ��ֵ �˳�ѭ��
				}
				tmp2 *= 2; // tmp2����һλ
			}
		}
	}
}
/*******************************************
��������: key_event
��    ��: ����Ƿ��м�����
��    ��: ��
����ֵ  : ��
********************************************/
void key_event(void)
{
	unsigned char tmp;
	KeyWrite(0xF0);// ���ü����������ֵ ʹ������м����� �������ŵ�ƽ����Ϊȫ��
	tmp = KeyRead();
	if ((key_pressed == 0x00)&&((tmp & 0xF0) < 0xF0))//�Ƿ��м�����
	{ 
		key_pressed = 1; // ����а������£�����key_pressed��ʶ
		SysCtlDelay(1000000); //��������
		check_key(); // ����check_key(),��ȡ��ֵ
	}
	else if ((key_pressed ==1)&&((tmp & 0xF0) == 0xF0))//�Ƿ񰴼��Ѿ��ͷ�
	{ 
		key_pressed = 0; // ���key_pressed��ʶ
		key_flag = 1;
	}
}


