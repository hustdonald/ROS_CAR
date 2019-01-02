#include "usart2.h"
u8 Usart2_Receive;

/**************************************************************************
�������ܣ�����2��ʼ��
��ڲ����� bound:������
����  ֵ����
**************************************************************************/
void uart2_init(u32 bound)
{  	 
	  //GPIO�˿�����
  GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	 
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);	//ʹ��UGPIOBʱ��
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);	//ʹ��USART3ʱ��
	//USART2_TX  
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2; //PB.10
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	//�����������
  GPIO_Init(GPIOA, &GPIO_InitStructure);
   
  //USART3_RX	  
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;//PB11
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//��������
  GPIO_Init(GPIOA, &GPIO_InitStructure);

  //Usart3 NVIC ����
  NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=0 ;//��ռ���ȼ�
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;		//�����ȼ�
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQͨ��ʹ��
	NVIC_Init(&NVIC_InitStructure);	//����ָ���Ĳ�����ʼ��VIC�Ĵ���
   //USART ��ʼ������
	USART_InitStructure.USART_BaudRate = bound;//���ڲ�����
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//�ֳ�Ϊ8λ���ݸ�ʽ
	USART_InitStructure.USART_StopBits = USART_StopBits_1;//һ��ֹͣλ
	USART_InitStructure.USART_Parity = USART_Parity_No;//����żУ��λ
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//��Ӳ������������
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//�շ�ģʽ
  USART_Init(USART2, &USART_InitStructure);     //��ʼ������3
  USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);//�������ڽ����ж�
  USART_Cmd(USART2, ENABLE);                    //ʹ�ܴ���3 
}

/**************************************************************************
�������ܣ�����3�����ж�
��ڲ�������
����  ֵ����
**************************************************************************/
u8 TxBuffer_U2[256];
u8 TxCounter_U2=0;
u8 count_U2=0;
static u8 RxBuffer_U2[256];  //���ڽ��ջ���
static u8 RxState_U2 = 0;

void USART2_IRQHandler(void)
{	
	//u8 i = 0;
	if (USART_GetFlagStatus(USART2, USART_FLAG_ORE) != RESET)//
    {
        USART_ReceiveData(USART2);
    }
		
	//�����ж�
	if((USART2->SR & (1<<7))&&(USART2->CR1 & USART_CR1_TXEIE))//if(USART_GetITStatus(USART1,USART_IT_TXE)!=RESET)
	{
		USART2->DR = TxBuffer_U2[TxCounter_U2++]; //дDR����жϱ�־          
		if(TxCounter_U2 == count_U2)
		{
			USART2->CR1 &= ~USART_CR1_TXEIE;		//�ر�TXE�ж�
			//USART_ITConfig(USART1,USART_IT_TXE,DISABLE);
		}
	}
	//�����ж� (���ռĴ����ǿ�) 
	if(USART2->SR & (1<<5))//if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)    
	{
		u8 com_data = USART2->DR;
		static u8 _data_len = 0,_data_cnt = 0;
		
		

		
		///////////////////////////*��λ������*/////////////////////////////
		if(RxState_U2==0&&com_data==0xAA)      //�ж�֡ͷAAAF
		{
			RxState_U2=1;
			RxBuffer_U2[0]=com_data;
		}
		else if(RxState_U2==1&&com_data==0xAF)
		{
			RxState_U2=2;
			RxBuffer_U2[1]=com_data;
		}
		else if(RxState_U2==2&&com_data<=0XF1) //������
		{
			RxState_U2=3;
			RxBuffer_U2[2]=com_data;
		}
		else if(RxState_U2==3&&com_data<255) // ����LEN
		{
			RxState_U2 = 4;
			RxBuffer_U2[3]=com_data;
			_data_len = com_data;
			_data_cnt = 0;
		}
		else if(RxState_U2==4&&_data_len>0) //����
		{
			_data_len--;
			RxBuffer_U2[4+_data_cnt++]=com_data;
			if(_data_len==0)
				RxState_U2 = 5;
		}
		else if(RxState_U2==5)
		{
			RxState_U2 = 0;
			RxBuffer_U2[4+_data_cnt]=com_data;
//			ANO_NRF_TxPacket(RxBuffer,5+_data_cnt);   //���ݵ�NRF����
			ANO_DT_Data_Receive_Anl(RxBuffer_U2,5+_data_cnt);   //���ݵ����ݷ���
//				for(i=0;i<5+_data_cnt;i++)  //���ڴ�ӡ�յ������ݣ�����ʱ�ã�������
//				{
//					while(USART_GetFlagStatus(USART1,USART_FLAG_TC)==RESET); //ѭ������,ֱ���������    //���Ƿ���Ϊ16���Ƶ�����ʾ
//					USART_SendData(USART1,RxBuffer[i]); 
//				}
			
		}
		else
			RxState_U2 = 0;
		
		//////////////////*ң����*/////////////////
//		if(com_data>10)  //Ĭ��ʹ��
//		{			
//			if(com_data==0x5A)	Flag_Qian=0,Flag_Hou=0,Flag_Left=0,Flag_Right=0;//////////////ɲ��
//			else if(com_data==0x41)	Flag_Qian=1,Flag_Hou=0,Flag_Left=0,Flag_Right=0,Movement=50,Turn_Target=0;//////////////ǰ
//			else if(com_data==0x45)	Flag_Qian=0,Flag_Hou=1,Flag_Left=0,Flag_Right=0,Movement=-50,Turn_Target=0;//////////////��
//			else if(com_data==0x42||com_data==0x43||com_data==0x44)	
//														Flag_Qian=0,Flag_Hou=0,Flag_Left=0,Flag_Right=1,Turn_Target=-50,Movement=0;  //��
//			else if(com_data==0x46||com_data==0x47||com_data==0x48)	    //��
//														Flag_Qian=0,Flag_Hou=0,Flag_Left=1,Flag_Right=0,Turn_Target=50,Movement=0;
//			else Flag_Qian=0,Flag_Hou=0,Flag_Left=0,Flag_Right=0,Turn_Target=0,Movement=0;//////////////ɲ��
//		}
		
	}
} 



