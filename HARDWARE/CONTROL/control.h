#ifndef __CONTROL_H
#define __CONTROL_H
#include "sys.h"
  /**************************************************************************
���ߣ�ƽ��С��֮��
�ҵ��Ա�С�꣺http://shop114407458.taobao.com/
**************************************************************************/
#define PI 3.14159265
#define DIFFERENCE 100
int EXTI15_10_IRQHandler(void);
int velocity(int encoder_left,int encoder_right);
int turn(float Target,float gyro);//ת�����
void Set_Pwm(int moto1,int moto2,int moto3,int moto4);
void Xianfu_Pwm(void);
int myabs(int a);
void KEY_ps2_handle(void);
#endif
