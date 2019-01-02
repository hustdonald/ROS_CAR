#include "control.h"		
u8 key_PS2=0,PS_PSS_RY=0,PS_PSS_RX=0,PS_PSS_LX=0,PS_PSS_LY=0;
u8 Flag_5ms=0,Flag_10ms=0,Flag_stop=1,Angle_warning=0;
int Moto1,Moto2,Moto3,Moto4;   
int turn_PWM = 0;
/**************************************************************************
�������ܣ����еĿ��ƴ��붼��������
         5ms��ʱ�ж���MPU6050��INT���Ŵ���
         �ϸ�֤���������ݴ����ʱ��ͬ��				 
**************************************************************************/
int EXTI15_10_IRQHandler(void) 
{    
	if(INT==0)		
	{   
		EXTI->PR=1<<14;                                                      //����жϱ�־λ	
		Flag_5ms=!Flag_5ms;
		if(Flag_5ms==1)	//5ms��ȡһ�������Ǻͼ��ٶȼƵ�ֵ�����ߵĲ���Ƶ�ʿ��Ը��ƿ������˲��ͻ����˲���Ч��
		{
			if(mpu_dmp_get_data(&pit,&roll,&yaw)==0)
			{
//					MPU_Get_Accelerometer(&aacx,&aacy,&aacz);	//�õ����ٶȴ���������
//					MPU_Get_Gyroscope(&gyrox,&gyroy,&gyroz);	//�õ�����������
			}
			return 0;
		}
		
		//10ms����һ�Σ�Ϊ�˱�֤M�����ٵ�ʱ���׼�����ȶ�ȡ����������		

		if(mpu_dmp_get_data(&pit,&roll,&yaw)==0)//������̬
		{
//			MPU_Get_Accelerometer(&aacx,&aacy,&aacz);	//�õ����ٶȴ���������
//			MPU_Get_Gyroscope(&gyrox,&gyroy,&gyroz);	//�õ�����������
		}
		  turn_PWM=turn(Turn_Target,-yaw);
		  KEY_ps2_handle();//PS2����ɨ�账��
		
		 if(yaw>=175||yaw<=-175)//�Ƕ��쳣
		 {
			Angle_warning=1;//�Ƕ��쳣��־λ
			Set_Pwm(0,0,0,0); //ͣ��
			PS2_Vibration(254,254);//�ֱ��𶯾���
		 }
		 if(Angle_warning==1)//�Ƕ��쳣���� ��λMPU6050 �Ե�ǰ����ΪĿ��Ƕ�
		 {
			Angle_warning=0;
			mpu_dmp_init();//��λ6050
			mpu_dmp_get_data(&pit,&roll,&yaw);
			Turn_Target=0;//У׼��ǰĿ��Ƕ�
			PS2_Vibration(0,0);
		 }
		 
   		  Xianfu_Pwm();                                                       //===PWM�޷�
		
		  if(Flag_stop==1)//ͣ��
		  {
			Set_Pwm(0,0,0,0); 
		  }
		  else
			Set_Pwm(Moto1,Moto2,Moto3,Moto4);                                               //===��ֵ��PWM�Ĵ���  
	}       	
	 return 0;	  
} 

/**************************************************************************
�������ܣ��ٶ�PI���� �޸�ǰ�������ٶȣ����޸�set_speed
��ڲ��������ֱ����������ֱ�����
����  ֵ���ٶȿ���PWM
��    �ߣ�ƽ��С��֮��
**************************************************************************/
int velocity(int encoder_left,int encoder_right)
{  
     static float Velocity;

	  return Velocity;
}

/**************************************************************************
�������ܣ�ת�����  
��ڲ��������ֱ����������ֱ�������Z��������
����  ֵ��ת�����PWM
pwm=Kp*e(k)+Ki*��e(k)+Kd[e��k��-e(k-1)]
e(k)������ƫ�� 
e(k-1)������һ�ε�ƫ��  
��e(k)����e(k)�Լ�֮ǰ��ƫ����ۻ���;����kΪ1,2,,k;
pwm�������
*************
**************************************************************************/
int turn(float Target,float Yaw)//ת����� ���� TargetΪĿ�꺽��� yaw��ʵ�ʺ����
{
	 static float Bias,Pwm,Integral_bias,Last_Bias;
	
		if(Flag_stop==1) //ͣ�����PID���� �����ۻ�
	 {
		 Bias=0;
		 Pwm=0;
		 Integral_bias=0;
		 Last_Bias=0;
	 }
	 
	 Bias=-yaw-Target;                                  //����ƫ��
	 Integral_bias+=Bias;	                                 //���ƫ��Ļ���
	 if(Integral_bias>7200) Integral_bias=7200;        //�����޷�
	 if(Integral_bias<-7200) Integral_bias=-7200;
	 Pwm=Turn_Kp*Bias+Turn_Ki*Integral_bias+Turn_Kd*(Bias-Last_Bias);       //λ��ʽPID������
	 Last_Bias=Bias;                                       //������һ��ƫ�� 
	 return Pwm;                                           //���PWM
}

/**************************************************************************
�������ܣ���ֵ��PWM�Ĵ���
��ڲ���������PWM������PWM
����  ֵ����
ע�ͣ�����ת�������ж� ��Գ��� PWMΪ��ʱ˳ʱ����ת PWMΪ��ʱ��ʱ����ת
**************************************************************************/
void Set_Pwm(int moto1,int moto2,int moto3,int moto4)
{
    	if(moto1>0)			AIN2=0,			AIN1=1;
			else 	          AIN2=1,			AIN1=0;
			PWMA=myabs(moto1);
		  if(moto2>0)	BIN1=0,			BIN2=1;
			else        BIN1=1,			BIN2=0;
			PWMB=myabs(moto2);
    	if(moto3>0)			CIN2=0,			CIN1=1;
			else 	          CIN2=1,			CIN1=0;
			PWMC=myabs(moto3);
    	if(moto4>0)			DIN2=1,			DIN1=0;
			else 	          DIN2=0,			DIN1=1;
			PWMD=myabs(moto4);	
}

/**************************************************************************
�������ܣ�����PWM��ֵ 
��ڲ�������
����  ֵ����
**************************************************************************/
void Xianfu_Pwm(void)
{	
	  int maxSpeed=7100;    //===PWM������7200 ������7100
      if(Moto1<-maxSpeed)
	  {		  
		  Moto1=-maxSpeed;	
	  }
	  else if(Moto1>maxSpeed)  
	 {

		  Moto1=maxSpeed;	
	 }
      if(Moto2<-maxSpeed)
	  {		  
		  Moto2=-maxSpeed;	
	  }
	  else if(Moto2>maxSpeed)  
	 {

		  Moto2=maxSpeed;	
	 }
	 if(Moto3<-maxSpeed)
	  {		  
		  Moto3=-maxSpeed;	
	  }
	  else if(Moto1>maxSpeed)  
	 {

		  Moto3=maxSpeed;	
	 }
	 if(Moto4<-maxSpeed)
	  {		  
		  Moto4=-maxSpeed;	
	  }
	  else if(Moto4>maxSpeed)  
	 {

		  Moto4=maxSpeed;	
	 }	 
}



	
/**************************************************************************
�������ܣ���ȡ�Ƕ� �����㷨�������ǵĵ�У�����ǳ����� 
��ڲ�������ȡ�Ƕȵ��㷨 1��DMP  2�������� 3�������˲�
����  ֵ����
**************************************************************************/
void Get_Angle(u8 way)
{ 

}
/**************************************************************************
�������ܣ�����ֵ����
��ڲ�����int
����  ֵ��unsigned int
**************************************************************************/
int myabs(int a)
{ 		   
	  int temp;
		if(a<0)  temp=-a;  
	  else temp=a;
	  return temp;
}
/**************************************************************************
�������ܣ�ps2�ֱ�����������
��ڲ�����int
����  ֵ��unsigned int
**************************************************************************/
void KEY_ps2_handle(void)
{
	key_PS2=PS2_DataKey();									//��ȡPS�ֱ�������Ϣ
	PS_PSS_RX=PS2_AnologData(PSS_RX);						//��ȡҡ��ģ����0-254
	PS_PSS_RY=PS2_AnologData(PSS_RY);
	PS_PSS_LX=PS2_AnologData(PSS_LX);
	PS_PSS_LY=PS2_AnologData(PSS_LY);
	switch(key_PS2)//��������
	{
		case 0://none
			Moto1=0;
			Moto2=0;
			Moto3=0;
			Moto4=0;
			break;
		case 1: //select
			Turn_Target=-yaw;
			break;
		case 2://Lҡ���м�
			Moto1=0;
			Moto2=0;
			Moto3=0;
			Moto4=0;
			break;
		case 3:	//Rҡ���м�							
			Moto1=0;
			Moto2=0;
			Moto3=0;
			Moto4=0;
			break;
		case 4://start
			mpu_dmp_init();//��λ6050
			Turn_Target=0;//У׼��ǰĿ��Ƕ�
			break;
		case 5:   //��
			Moto1=-set_speed+turn_PWM;
			Moto2=set_speed+turn_PWM;
			Moto3=set_speed+turn_PWM;
			Moto4=-set_speed+turn_PWM;
			break;
		case 6://��
			Moto1=-set_speed+turn_PWM;
			Moto2=-set_speed+turn_PWM;
			Moto3=set_speed+turn_PWM;
			Moto4=set_speed+turn_PWM;
			break;
		case 7://��
			Moto1=set_speed+turn_PWM;
			Moto2=-set_speed+turn_PWM;
			Moto3=-set_speed+turn_PWM;
			Moto4=set_speed+turn_PWM;
			break;
		case 8://��
			Moto1=set_speed+turn_PWM;
			Moto2=set_speed+turn_PWM;
			Moto3=-set_speed+turn_PWM;
			Moto4=-set_speed+turn_PWM;
			break;
		case 9://L2
			Moto1=0;
			Moto2=0;
			Moto3=0;
			Moto4=0;
			break;
		case 10://R2
			Moto1=0;
			Moto2=0;
			Moto3=0;
			Moto4=0;
			break;	
		case 11://L1
			Moto1=4000;
			Moto2=4000;
			Moto3=4000;
			Moto4=4000;
			Turn_Target=-yaw;
			break;
		case 12://R1
			Moto1=-4000;
			Moto2=-4000;
			Moto3=-4000;
			Moto4=-4000;
			Turn_Target=-yaw;
			break;
		case 13://������
			set_speed=5000;		
			break;
		case 14://Բ��
			set_speed=6000;		
			break;
		case 15://X
			set_speed=7000;		
			break;
		case 16://������
			set_speed=4500;		
			break;
		default : 
			break; 		
	}
       /********ҡ�˴���*********/
	if(PS_PSS_LY>131)
	{
		Flag_stop = 0;		
			Moto1=set_speed+turn_PWM;
			Moto2=-set_speed+turn_PWM;
			Moto3=-set_speed+turn_PWM;
			Moto4=set_speed+turn_PWM;


		}
	else if(PS_PSS_LY<125)
	{
		Flag_stop = 0;			
			Moto1=-set_speed+turn_PWM;
			Moto2=set_speed+turn_PWM;
			Moto3=set_speed+turn_PWM;
			Moto4=-set_speed+turn_PWM;

	}
	
	if(PS_PSS_RX>131)
	{
			Flag_stop = 0;
		//Turn_Target=(float)(PS_PSS_RX-128)/128*50;
			Moto1=-set_speed+turn_PWM;
			Moto2=-set_speed+turn_PWM;
			Moto3=set_speed+turn_PWM;
			Moto4=set_speed+turn_PWM;		
	}
	else if(PS_PSS_RX<125)
	{
			Flag_stop = 0;
		//Turn_Target = -(float)(128-PS_PSS_RX)/128*50;
			Moto1=set_speed+turn_PWM;
			Moto2=set_speed+turn_PWM;
			Moto3=-set_speed+turn_PWM;
			Moto4=-set_speed+turn_PWM;		
	}
	else if(PS_PSS_LY>125&&PS_PSS_LY<131&&PS_PSS_RX>125&&PS_PSS_RX<131&&key_PS2==0) 
	{
			Flag_stop = 1;
	}
	if(key_PS2==11||key_PS2==12)
	{
			Flag_stop = 0;
	}
		
	
	
	
	


//		  


}




