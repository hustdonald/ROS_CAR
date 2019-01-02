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
#include "Init.h"
u8 Tx_Rx_mode = Tx_mode;  //24l01ģʽ Tx_mode���� Rx_mode����
float pit,roll,yaw; 		//ŷ����
short aacx,aacy,aacz;		//���ٶȴ�����ԭʼ����
short gyrox,gyroy,gyroz;	//������ԭʼ����
short temp;					//�¶�	
 int main(void)
 {	 
	u16 i=0,j=0;	
	 
	Init_sys();  //��ʼ��
 	
 	while(1)
	{	

				///////////////////////////*��ʱ�ϴ�*///////////////////////////////////////////
				i++;
				if(i==1)
					ANO_DT_Send_Status(-roll, pit, -yaw, 0, 0, 0);///�ɿ���̬�Ȼ�����Ϣ
//				if(i==2)
//					ANO_DT_Send_Senser(aacx,aacy,aacz,gyrox,gyroy,gyroz,0,0,0); //�ɻ�����������//���ͼ��ٶȺ�������ԭʼ����
//				if(i==3)
					ANO_DT_Send_MotoPWM(set_speed,(int)Turn_Target,0,0,0,0,0,0);				
//				if(i==4)
//					ANO_DT_Send_User1( );	
				if(i>=1) i=0;
				
				j++;
				if(j==2500)//�¶ȳ�ʱ����ϴ�һ��
				{	
					j=0;
					temp=MPU_Get_Temperature();
					ANO_DT_Send_Power(111, temp);  //�������ʵ��ΪMPU6050���¶ȴ�����
				}
				delay_ms(1);
	}			
 
}

	 

 
