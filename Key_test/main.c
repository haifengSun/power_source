#include <stdbool.h>
#include <stdint.h>
#include "inc/hw_gpio.h"
#include "inc/hw_ints.h"
#include "inc/hw_memmap.h"
#include "inc/hw_sysctl.h"
#include "inc/hw_types.h"
#include "driverlib/rom.h"
#include "driverlib/rom_map.h"
#include "driverlib/debug.h"
#include "driverlib/fpu.h"
#include "driverlib/pin_map.h"
#include "driverlib/gpio.h"
#include "driverlib/interrupt.h"
#include "driverlib/rom.h"
#include "driverlib/rom_map.h"
#include "key.h"
#include "driverlib/sysctl.h"
#include "inc/tm4c1294ncpdt.h"
#include "driverlib/timer.h"
#include "driverlib/systick.h"
#include <string.h>
#include "12864.h"
#include "driverlib/adc.h"
#include "driverlib/pwm.h"
#include "driverlib/pin_map.h"
#include "driverlib/timer.h"





#define PWM_FREQUENCY 10000//����PWMƵ��
#define NUM_SSI_DATA 256//1602�洢�������
#define USER_BITRATE 16000000//SSI����������


uint32_t Buffer[NUM_SSI_DATA];//��Ŷ�ȡ��������


volatile uint32_t ui32Load;// PWM period
volatile uint32_t ui32PWMClock;//PWM CLOCK FREQUENCY

int Duty_Cycle;             //ռ�ձ�
int k=0;
volatile uint32_t ui32SysClkFreq;//�洢ϵͳʱ��Ƶ��
uint8_t ui8PinData=0;
uint32_t ui32ACCValues[6];//�洢6��ADͨ��������
unsigned char Result[10];//�洢����ת���ɵ��ַ�������LCD��ʾ
unsigned char Result_0[10];//�洢ADC������ѹֵת���ɵ��ַ������飬��LCD��ʾ
unsigned char Result_01[10];
unsigned char Result_02[10];


double power=0;//����
int count=0;//����
double vol_data_in;//�趨�ĵ�ѹֵ
double vol_data_adc;//ADC�����ĵ�ѹֵ
uint32_t cur_data_adc;//ADC�����ĵ���ֵ
/*******************************************
��������: int_to_char
��    ��: ����ת��Ϊ�ַ��ͣ������������Result[10]��
��    ��: data--��ת������
����ֵ : ��
********************************************/
void int_to_char(int data)
{
    unsigned char i=0,j=0;
    unsigned char temp[10];
    if(data==0)
    {
        Result[0]='0';
        Result[1]='\0';
        return;
    }

    //��data�����λ�����λ����ʮ���ƣ�˳�����Result[10]��
    while(data!=0)
    {
        temp[i] = data%10+'0';
        data /= 10;
        i++;
    }

    while(i--)
    {
        Result[j]=temp[i];
        j++;
    }
    Result[j]='\0';


}


void int_to_char1(int data)
{
    unsigned char i=0,j=0;
    unsigned char temp[10];
    if(data==0)
    {
        Result_01[0]='0';
        Result_01[1]='\0';
        return;
    }

    //��data�����λ�����λ����ʮ���ƣ�˳�����Result[10]��
    while(data!=0)
    {
        temp[i] = data%10+'0';
        data /= 10;
        i++;
    }

    while(i--)
    {
        Result_01[j]=temp[i];
        j++;
    }
    Result_01[j]='\0';


}





/*******************************************
��������: double_to_char
��    ��: double��ת��Ϊ�ַ��ͣ������������Result_1[]��
��    ��: data_1--��ת������
����ֵ  : ��
********************************************/
void double_to_char(unsigned char Result_1[10],double data_1)//�洢double������ת���ɵ��ַ�������LCD��ʾ
{
    int t;
    for(t=0;t<4;t++)   //initial
    {
        Result_1[t]=' ';
    }
    int tempdata_1=data_1;
    unsigned char i=0,j=0;
    unsigned char temp_0[10],temp_1[10];
    if(tempdata_1==0)
        {
            Result_1[0]='0';
            Result_1[1]='.';
            j=2;
        }
    else{
    while(tempdata_1!=0)
    {
        temp_0[i] = tempdata_1%10+'0';
        tempdata_1 /= 10;
        i++;
    }
    while(i--)
    {
        Result_1[j]=temp_0[i];
        j++;
    }
    Result_1[j++]='.';
    }
    tempdata_1=data_1;
    int tempdata_2=(data_1-tempdata_1)*10;
    i=0;
    if(tempdata_2==0)
        {
            Result_1[j]='0';
            Result_1[4]='\0';
            return;
        }
    while(tempdata_2!=0)
    {
        temp_1[i] = tempdata_2%10+'0';
        tempdata_2 /= 10;
        i++;
    }
    while(i--)
    {
        Result_1[j]=temp_1[i];
        j++;
    }
    Result_1[4]='\0';

}

void double_to_char1(unsigned char Result_1[10],double data_1)//�洢double������ת���ɵ��ַ�������LCD��ʾ
{
    int t;
    for(t=0;t<4;t++)   //initial
    {
        Result_1[t]=' ';
    }
    int tempdata_1=data_1;
    unsigned char i=0,j=0;
    unsigned char temp_0[10],temp_1[10];
    if(tempdata_1==0)
        {
            Result_1[0]='0';
            Result_1[1]='.';
            j=2;
        }
    else{
    while(tempdata_1!=0)
    {
        temp_0[i] = tempdata_1%10+'0';
        tempdata_1 /= 10;
        i++;
    }
    while(i--)
    {
        Result_1[j]=temp_0[i];
        j++;
    }
    Result_1[j++]='.';
    }
    tempdata_1=data_1;
    int tempdata_2=(data_1-tempdata_1)*10;
    i=0;
    if(tempdata_2==0)
        {
            Result_1[j]='0';
            Result_1[4]='\0';
            return;
        }
    while(tempdata_2!=0)
    {
        temp_1[i] = tempdata_2%10+'0';
        tempdata_2 /= 10;
        i++;
    }
    while(i--)
    {
        Result_1[j]=temp_1[i];
        j++;
    }
    Result_1[4]='\0';

}



/*��������*/
void key_design()
{
            unsigned char Result_1[]=' ';
                key_flag = 0;
                switch(key_val)
                {
                case 3:Result_1[0]='1';vol_data_in=1.0;break;
                case 2:Result_1[0]='2';vol_data_in=2.0;break;
                case 1:Result_1[0]='3';vol_data_in=3.0;break;
                case 7:Result_1[0]='4';vol_data_in=4.0;break;
                case 6:Result_1[0]='5';vol_data_in=5.0;break;
                case 5:Result_1[0]='6';vol_data_in=6.0;break;
                case 11:Result_1[0]='7';vol_data_in=7.0;break;
                case 10:Result_1[0]='8';vol_data_in=8.0;break;
                case 9:Result_1[0]='9';vol_data_in=9.0;break;
                case 14:Result_1[0]='0';vol_data_in=0.0;break;
                }
                Result_1[1]='\0';

                if(key_val==0)
                {
                    vol_data_in+=0.5;
                    double_to_char(Result_1,vol_data_in);
                }
                if(key_val==4)
                {
                    vol_data_in-=0.5;
                    double_to_char(Result_1,vol_data_in);

                }
                if(key_val == 8 || key_val == 15 || key_val == 13)
                {
                    Lcd_init_basic();
                    LCD_gotoXY(2,0);
                    LCD_sendstr("��鿴����˵��");
                    delay(clock*2);
                    Lcd_init_basic();
                    LCD_gotoXY(2,1);
                    LCD_sendstr("A");
                    LCD_gotoXY(3,1);
                    LCD_sendstr("B");
                    LCD_gotoXY(2,2);
                    LCD_sendstr("Ϊ����");
                    LCD_gotoXY(3,2);
                    LCD_sendstr("Ϊ��С");
                    LCD_gotoXY(4,1);
                    LCD_sendstr("����Ϊ0.5V");
                    delay(clock*2);
                    Lcd_init_basic();
                    LCD_design();

                }
                LCD_gotoXY(2,5);
                LCD_sendstr(Result_1);



}



/*UI���*/
void LCD_design()
{
    clock=SysCtlClockFreqSet((SYSCTL_XTAL_25MHZ |SYSCTL_OSC_MAIN | SYSCTL_USE_PLL | SYSCTL_CFG_VCO_480), 120000000);
    ui32SysClkFreq=SysCtlClockFreqSet((SYSCTL_XTAL_25MHZ | SYSCTL_OSC_MAIN | SYSCTL_USE_PLL | SYSCTL_CFG_VCO_480), 120000000);

    Lcd_init_basic();//��ʼ��

    LCD_gotoXY(1,0);
    LCD_sendstr("ʵʱ��ѹ��");
    LCD_gotoXY(2,0);
    LCD_sendstr("���õ�ѹ��");
    LCD_gotoXY(3,0);
    LCD_sendstr("ʵʱ������");
    LCD_gotoXY(1,7);
    LCD_sendstr("V");
    LCD_gotoXY(2,7);
    LCD_sendstr("V");
    LCD_gotoXY(3,7);
    LCD_sendstr("mA");
    LCD_gotoXY(4,1);
    LCD_sendstr("���ʣ�");
    LCD_gotoXY(4,7);
    LCD_sendstr("W");
}


/****************************************
 �������ƣ�ADC������ƺ���
 ���ܣ���ȡADCģ��������ݣ����洢��ui32ACCValues[6]������
 ****************************************/
void ADC_design()
{

    ADCIntClear(ADC0_BASE, 0);
    ADCProcessorTrigger(ADC0_BASE, 0);
    while(!ADCIntStatus(ADC0_BASE, 0, false))
    {
    }
    ADCSequenceDataGet(ADC0_BASE, 0, ui32ACCValues);

            cur_data_adc=ui32ACCValues[4];
            cur_data_adc=(cur_data_adc*3.3*1000/4096/60/0.02);
            int_to_char(cur_data_adc);
            LCD_gotoXY(3,5);
            LCD_sendstr(Result);

            vol_data_adc=ui32ACCValues[5];
            vol_data_adc=(vol_data_adc*5.91*3.3)/4096;
            double_to_char(Result_0,vol_data_adc);
            LCD_gotoXY(1,5);
            LCD_sendstr(Result_0);





}



void ADC_design_xiuding()
{
    ADCSequenceConfigure(ADC0_BASE, 0, ADC_TRIGGER_PROCESSOR, 0);
    ADCSequenceStepConfigure(ADC0_BASE, 0, 0, ADC_CTL_CH3);
    ADCSequenceStepConfigure(ADC0_BASE, 0, 1, ADC_CTL_CH2);
    ADCSequenceStepConfigure(ADC0_BASE, 0, 2, ADC_CTL_CH1);
    ADCSequenceStepConfigure(ADC0_BASE, 0, 3, ADC_CTL_CH0);
    ADCSequenceStepConfigure(ADC0_BASE, 0, 4, ADC_CTL_CH17);
    ADCSequenceStepConfigure(ADC0_BASE, 0, 5, ADC_CTL_CH16|ADC_CTL_IE|ADC_CTL_END);
    ADCSequenceEnable(ADC0_BASE, 0);

    ADCIntClear(ADC0_BASE, 0);
    ADCProcessorTrigger(ADC0_BASE, 0);
    while(!ADCIntStatus(ADC0_BASE, 0, false))
    {
    }

    ADCSequenceDataGet(ADC0_BASE, 0, ui32ACCValues);
    int n;
    vol_data_adc=0;
    for(n=0;n<20000;n++)
    {
    vol_data_adc+=ui32ACCValues[5]*3.3*5.91/4096/20000;
    key_event();
    if(key_flag)
        break;
    }
    if(key_flag==0)
    {
    if(vol_data_adc-vol_data_in<0.1 && vol_data_in-vol_data_adc<0.1)
    {
    int_to_char(vol_data_adc);
    LCD_gotoXY(1,5);
    LCD_sendstr(Result_0);
    }
    }

    cur_data_adc=0;
    for(n=0;n<20000;n++)
    {
    cur_data_adc+=ui32ACCValues[4]*3.3/4096/60/0.02/20000*1000;
    if(key_flag)
            break;
    key_event();
    if(key_flag)
            break;
    }
    if(key_flag==0)
    {
        if(vol_data_adc-vol_data_in<0.05 && vol_data_in-vol_data_adc<0.05)
            {
    int_to_char1(cur_data_adc);
    LCD_gotoXY(3,5);
    LCD_sendstr(Result_01);
            }
    }

    if(key_flag==0)
    {
    power=vol_data_adc*cur_data_adc;
    double_to_char1(Result_02,power);
    LCD_gotoXY(4,4);
    LCD_sendstr(Result_02);
    }
}






/*PWM�������*/
void PWM_design()
{
    while(1)
    {
                    if(vol_data_adc-vol_data_in>0.3)
                    {
                        Duty_Cycle=Duty_Cycle-4;
                        PWMGenConfigure(PWM0_BASE, PWM_GEN_0, PWM_GEN_MODE_DOWN);  //PWM 0, generator 0,count-down mode
                        PWMGenPeriodSet(PWM0_BASE, PWM_GEN_0, ui32Load);
                        PWMPulseWidthSet(PWM0_BASE, PWM_OUT_1, Duty_Cycle);                // a starter for the load value
                        PWMOutputState(PWM0_BASE, PWM_OUT_1_BIT, true);
                        PWMGenEnable(PWM0_BASE, PWM_GEN_0);
                        ADC_design();
                        delay(120000);

                    }
                    else if (vol_data_in-vol_data_adc>0.3)
                    {
                        Duty_Cycle=Duty_Cycle+4;
                        PWMGenConfigure(PWM0_BASE, PWM_GEN_0, PWM_GEN_MODE_DOWN);  //PWM 0, generator 0,count-down mode
                        PWMGenPeriodSet(PWM0_BASE, PWM_GEN_0, ui32Load);
                        PWMPulseWidthSet(PWM0_BASE, PWM_OUT_1, Duty_Cycle);                // a starter for the load value
                        PWMOutputState(PWM0_BASE, PWM_OUT_1_BIT, true);
                        PWMGenEnable(PWM0_BASE, PWM_GEN_0);
                        ADC_design();
                        delay(120000);
                    }
                    else if (vol_data_in-vol_data_adc<=0.3&&vol_data_in-vol_data_adc>0.1)
                    {
                        Duty_Cycle=Duty_Cycle+1;
                        PWMGenConfigure(PWM0_BASE, PWM_GEN_0, PWM_GEN_MODE_DOWN);  //PWM 0, generator 0,count-down mode
                        PWMGenPeriodSet(PWM0_BASE, PWM_GEN_0, ui32Load);
                        PWMPulseWidthSet(PWM0_BASE, PWM_OUT_1, Duty_Cycle);                // a starter for the load value
                        PWMOutputState(PWM0_BASE, PWM_OUT_1_BIT, true);
                        PWMGenEnable(PWM0_BASE, PWM_GEN_0);
                        ADC_design();
                        delay(120000);
                    }
                    else if (vol_data_adc-vol_data_in<=0.3&&vol_data_adc-vol_data_in>0.1)
                    {
                        Duty_Cycle=Duty_Cycle-1;
                        PWMGenConfigure(PWM0_BASE, PWM_GEN_0, PWM_GEN_MODE_DOWN);  //PWM 0, generator 0,count-down mode
                        PWMGenPeriodSet(PWM0_BASE, PWM_GEN_0, ui32Load);
                        PWMPulseWidthSet(PWM0_BASE, PWM_OUT_1, Duty_Cycle);                // a starter for the load value
                        PWMOutputState(PWM0_BASE, PWM_OUT_1_BIT, true);
                        PWMGenEnable(PWM0_BASE, PWM_GEN_0);
                        ADC_design();
                        delay(120000);
                    }
                    else
                    {
                        while(1)
                        {
                            ADC_design_xiuding();
                            delay(12000000);
                            key_event();
                            if(key_flag)
                            {

                                break;
                            }
                            else if(vol_data_adc-vol_data_in>0.1 || vol_data_in-vol_data_adc>0.1)
                                break;
                        }
                    }
                    key_event();
                    if(key_flag == 1 )
                        {
                        key_flag=0;
                        break;
                        }
                        }
}



int main(void)
{
    vol_data_adc=9;
    Duty_Cycle=11999*0.6;
    vol_data_in=9;
	SysCtlClockFreqSet((SYSCTL_XTAL_25MHZ | SYSCTL_OSC_MAIN | SYSCTL_USE_PLL | SYSCTL_CFG_VCO_480), 120000000);//����ʱ��Ƶ��
	/*��ʼ��*/
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPION);
	GPIOPinTypeGPIOOutput(GPIO_PORTN_BASE, GPIO_PIN_0);
	GPIOPinWrite(GPIO_PORTN_BASE, GPIO_PIN_0, 0x00);
	key_init();
	clock=SysCtlClockFreqSet((SYSCTL_XTAL_25MHZ |SYSCTL_OSC_MAIN | SYSCTL_USE_PLL | SYSCTL_CFG_VCO_480), 120000000);
	Lcd_init_basic();
    /*LCD��ʼ��*/
    LCD_gotoXY(2,2);
    LCD_sendstr("��ӭʹ��");
    LCD_gotoXY(3,1);
    LCD_sendstr("����ĵ�Դ!");
    delay(clock*3);

    //�����ʼ��//
    SysCtlPeripheralEnable(SYSCTL_PERIPH_ADC0);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOE);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOK);
    GPIOPinTypeADC(GPIO_PORTE_BASE, GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2| GPIO_PIN_3 );
    GPIOPinTypeADC(GPIO_PORTK_BASE, GPIO_PIN_0 | GPIO_PIN_1 );

    //ADת����ʼ��//
    ADCSequenceConfigure(ADC0_BASE, 0, ADC_TRIGGER_PROCESSOR, 0);//ADC0,sample sequencer 1,processor to trigger the sequence,highest priority
    ADCSequenceStepConfigure(ADC0_BASE, 0, 0, ADC_CTL_CH3);
    ADCSequenceStepConfigure(ADC0_BASE, 0, 1, ADC_CTL_CH2);
    ADCSequenceStepConfigure(ADC0_BASE, 0, 2, ADC_CTL_CH1);
    ADCSequenceStepConfigure(ADC0_BASE, 0, 3, ADC_CTL_CH0);
    ADCSequenceStepConfigure(ADC0_BASE, 0, 4, ADC_CTL_CH17);
    ADCSequenceStepConfigure(ADC0_BASE, 0, 5, ADC_CTL_CH16|ADC_CTL_IE|ADC_CTL_END);//tells the sequencer that this is the final sample in the sequence
    ADCSequenceEnable(ADC0_BASE, 0);

        volatile uint32_t ui32Load;// PWM period
        volatile uint32_t ui32PWMClock;//PWM CLOCK FREQUENCY


        ui32SysClkFreq = SysCtlClockFreqSet((SYSCTL_XTAL_25MHZ | SYSCTL_OSC_MAIN | SYSCTL_USE_PLL | SYSCTL_CFG_VCO_480), 120000000);

        SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);
        SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOG);
        SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOK);
        SysCtlPeripheralEnable(SYSCTL_PERIPH_PWM0);


        PWMClockSet(PWM0_BASE,PWM_SYSCLK_DIV_1);
        GPIOPinConfigure(GPIO_PF1_M0PWM1);//configure the PF1 pin to M0PWM1
        GPIOPinTypePWM(GPIO_PORTF_BASE, GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3);
        GPIOPinTypePWM(GPIO_PORTK_BASE, GPIO_PIN_4|GPIO_PIN_5);
        GPIOPinTypePWM(GPIO_PORTG_BASE, GPIO_PIN_1);


        ui32PWMClock = ui32SysClkFreq / 1;
        ui32Load = (ui32PWMClock / PWM_FREQUENCY) - 1;

        PWMGenConfigure(PWM0_BASE, PWM_GEN_0, PWM_GEN_MODE_DOWN);  //PWM 0, generator 0,count-down mode
        PWMGenPeriodSet(PWM0_BASE, PWM_GEN_0, ui32Load);
        PWMPulseWidthSet(PWM0_BASE, PWM_OUT_1, Duty_Cycle);              // a starter for the load value
        PWMOutputState(PWM0_BASE, PWM_OUT_1_BIT, true);
        PWMGenEnable(PWM0_BASE, PWM_GEN_0);



        LCD_design();


   while(1)
   {
       key_event();
       if(key_val == 12)
           break;
   }

        while(1)
        {   PWM_design();
            key_design();//switch on Voltage setting


            //count++;
        }








}
