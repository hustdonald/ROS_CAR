#include "sys.h"

#define PID_Param_LEN 15		//PID�������鳤��
#define PID_KP1 0			//��������������е�λ��
#define PID_KI1 1
#define PID_KD1 2

#define PID_KP2 3
#define PID_KI2 4
#define PID_KD2 5

#define PID_KP3 6
#define PID_KI3 7
#define PID_KD3 8

#define PID4_Way_Angle 9

#define PID5_set_speed 12
#define PID5_ZHONGZHI  13
#define PID5_Turn_Target 14
extern u16 PID_Param[PID_Param_LEN];//PID��������
void Param_Read(void); //��flash��ȡ����
void Param_Save(void);//��flashд�����
void Para_ResetToFactorySetup(void); //�ָ�Ĭ�ϲ���
void Param_Init(void); //�ϵ��ʼ����flash��ȡ���� 

