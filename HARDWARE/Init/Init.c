#include "Init.h"
#include "led.h"
#include "delay.h"
#include "key.h"
#include "sys.h"
#include "usart.h"
#include "mpu6050.h"  
#include "inv_mpu.h"
#include "inv_mpu_dmp_motion_driver.h" 
#include "ANO.h"
#include "ANO_Drv_Nrf24l01.h"
#include "stmflash.h"
#include "Parameter.h"
#include "exti.h"

void Init_sys(void)
{
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);	 //����NVIC�жϷ���2:2λ��ռ���ȼ���2λ��Ӧ���ȼ�
//	uart_init(115200);	 	//���ڳ�ʼ��Ϊ500000
	uart2_init(921600);
	delay_init();	//��ʱ��ʼ�� 
	Param_Init();
//	LED_Init();		  			//��ʼ����LED���ӵ�Ӳ���ӿ�
//	KEY_Init();					//��ʼ������
	MPU_Init();					//��ʼ��MPU6050
//	Param_Init();				//�ϵ��ʼ����flash��ȡ���� 
	PS2_Init();			 //�����˿ڳ�ʼ��
	PS2_SetInit();		 //�����ó�ʼ��,���á����̵�ģʽ������ѡ���Ƿ�����޸�
	                     //������ģʽ 
	ANO_SPI_Init();	
	MiniBalance_PWM_Init(7199,0);
	
	if(mpu_dmp_init())   //MPU_DMP��ʼ��
 	{
		printf("MPU6050 Error\r\n");
 		delay_ms(100);
	}
	else	
		printf("MPU6050 OK\r\n");
	
	MPU6050_EXTI_Init();

}



