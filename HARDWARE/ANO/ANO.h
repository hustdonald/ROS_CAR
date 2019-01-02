#include "stm32f10x.h"
void ANO_DT_Send_Data(u8 *dataToSend , u8 length);
void Uart1_Put_Buf(u8 *dataToSend , u8 length) ;
void ANO_DT_Send_User1(void);
void ANO_DT_Send_Senser(s16 a_x,s16 a_y,s16 a_z,s16 g_x,s16 g_y,s16 g_z,s16 m_x,s16 m_y,s16 m_z); //�ɻ�����������
void ANO_DT_Send_Status(float angle_rol, float angle_pit, float angle_yaw, s32 alt, u8 fly_model, u8 armed);///�ɿ���̬�Ȼ�����Ϣ  
void ANO_DT_Send_Power(u16 votage, u16 current);//��ѹ����
void ANO_DT_Send_MotoPWM(u16 m_1,u16 m_2,u16 m_3,u16 m_4,u16 m_5,u16 m_6,u16 m_7,u16 m_8);//������PWM
void ANO_DT_Send_PID(u8 group,s16 p1_p,s16 p1_i,s16 p1_d,s16 p2_p,s16 p2_i,s16 p2_d,s16 p3_p,s16 p3_i,s16 p3_d);////PID����֡ ע��group���ǵڼ�֡PID ����group = 1 ����PID1
void ANO_DT_Data_Receive_Prepare(u8 data);
void ANO_DT_Data_Receive_Anl(u8 *data_buf,u8 num);
void ANO_DT_Send_PID_ALL(void);  //һ�������ϴ�����PID�����

