#include "ANO_Drv_Nrf24l01.h"
#include "ANO.h"
#include "delay.h"
//***************************************NRF24L01�Ĵ���ָ��*******************************************************
#define NRF_READ_REG        0x00  	// ���Ĵ���ָ��
#define NRF_WRITE_REG       0x20 	// д�Ĵ���ָ��
#define R_RX_PL_WID   	0x60     //                                            ���߽��ռĴ��������ֽ�������� �Ĵ��� 
#define RD_RX_PLOAD     0x61  	// ��ȡ��������ָ��  ��С�����0x61 255λ��0λ ���߽��ռĴ��������ֽ����������
#define WR_TX_PLOAD     0xA0  	// д��������ָ��
#define FLUSH_TX        0xE1 	// ��ϴ���� FIFOָ��
#define FLUSH_RX        0xE2  	// ��ϴ���� FIFOָ��
#define REUSE_TX_PL     0xE3  	// �����ظ�װ������ָ��
#define NOP             0xFF  	// ����
//*************************************SPI(nRF24L01)�Ĵ�����ַ****************************************************
#define CONFIG          0x00  // �����շ�״̬��CRCУ��ģʽ�Լ��շ�״̬��Ӧ��ʽ
#define EN_AA           0x01  // �Զ�Ӧ��������
#define EN_RXADDR       0x02  // �����ŵ�����
#define SETUP_AW        0x03  // �շ���ַ�������
#define SETUP_RETR      0x04  // �Զ��ط���������
#define RF_CH           0x05  // ����Ƶ������
#define RF_SETUP        0x06  // �������ʡ����Ĺ�������
#define NRFRegSTATUS    0x07  // ״̬�Ĵ���
#define OBSERVE_TX      0x08  // ���ͼ�⹦��
#define CD              0x09  // ��ַ���           
#define RX_ADDR_P0      0x0A  // Ƶ��0�������ݵ�ַ
#define RX_ADDR_P1      0x0B  // Ƶ��1�������ݵ�ַ
#define RX_ADDR_P2      0x0C  // Ƶ��2�������ݵ�ַ
#define RX_ADDR_P3      0x0D  // Ƶ��3�������ݵ�ַ
#define RX_ADDR_P4      0x0E  // Ƶ��4�������ݵ�ַ
#define RX_ADDR_P5      0x0F  // Ƶ��5�������ݵ�ַ
#define TX_ADDR         0x10  // ���͵�ַ�Ĵ���
#define RX_PW_P0        0x11  // ����Ƶ��0�������ݳ���
#define RX_PW_P1        0x12  // ����Ƶ��1�������ݳ���
#define RX_PW_P2        0x13  // ����Ƶ��2�������ݳ���
#define RX_PW_P3        0x14  // ����Ƶ��3�������ݳ���
#define RX_PW_P4        0x15  // ����Ƶ��4�������ݳ���
#define RX_PW_P5        0x16  // ����Ƶ��5�������ݳ���
#define FIFO_STATUS     0x17  // FIFOջ��ջ��״̬�Ĵ�������
//**************************************************************************************
//*********************************************NRF24L01*************************************
#define RX_DR				6		//�жϱ�־
#define TX_DS				5
#define MAX_RT			4

u8	TX_ADDRESS[TX_ADR_WIDTH] = {0xAA,0xBB,0xCC,0x00,0x01};	//���ص�ַ
u8	RX_ADDRESS[RX_ADR_WIDTH] = {0xAA,0xBB,0xCC,0x00,0x01};	//���յ�ַ	
	
uint8_t NRF24L01_2_RXDATA[RX_PLOAD_WIDTH];//nrf24l01���յ�������
uint8_t NRF24L01_2_TXDATA[RX_PLOAD_WIDTH];//nrf24l01��Ҫ���͵�����
uint8_t NRF24L01_2_RXDATA_LEN =0;
/*
*****************************************************************
* д�Ĵ���
*****************************************************************
*/
uint8_t ANO_NRF_Write_Reg(uint8_t reg, uint8_t value)
{
	uint8_t status;
	ANO_SPI_CSN_L();					  /* ѡͨ���� */
	status = ANO_SPI_RW(reg);  /* д�Ĵ�����ַ */
	ANO_SPI_RW(value);		  /* д���� */
	ANO_SPI_CSN_H();					  /* ��ֹ������ */
  return 	status;
}
/*
*****************************************************************
* ���Ĵ���
*****************************************************************
*/
uint8_t ANO_NRF_Read_Reg(uint8_t reg)
{
	uint8_t reg_val;
	ANO_SPI_CSN_L();					  /* ѡͨ���� */
	ANO_SPI_RW(reg);			  /* д�Ĵ�����ַ */
	reg_val = ANO_SPI_RW(0);	  /* ��ȡ�üĴ����������� */
	ANO_SPI_CSN_H();					  /* ��ֹ������ */
   return 	reg_val;
}
	
/*
*****************************************************************
*
* д������
*
*****************************************************************
*/
uint8_t ANO_NRF_Write_Buf(uint8_t reg, uint8_t *pBuf, uint8_t uchars)
{
	uint8_t i;
	uint8_t status;
	ANO_SPI_CSN_L();				        /* ѡͨ���� */
	status = ANO_SPI_RW(reg);	/* д�Ĵ�����ַ */
	for(i=0; i<uchars; i++)
	{
		ANO_SPI_RW(pBuf[i]);		/* д���� */
	}
	ANO_SPI_CSN_H();						/* ��ֹ������ */
    return 	status;	
}
/*
*****************************************************************
* ��������
*****************************************************************
*/
uint8_t ANO_NRF_Read_Buf(uint8_t reg, uint8_t *pBuf, uint8_t uchars)
{
	uint8_t i;
	uint8_t status;
	ANO_SPI_CSN_L();						/* ѡͨ���� */
	status = ANO_SPI_RW(reg);	/* д�Ĵ�����ַ */
	for(i=0; i<uchars; i++)
	{
		pBuf[i] = ANO_SPI_RW(0); /* ��ȡ�������� */ 	
	}
	ANO_SPI_CSN_H();						/* ��ֹ������ */
    return 	status;
}

//NRF�������� ����<=32�ֽ�
void ANO_NRF_TxPacket(uint8_t * tx_buf, uint8_t len)
{	
	ANO_SPI_CE_L();		 //StandBy Iģʽ	
	ANO_NRF_Write_Buf(NRF_WRITE_REG + RX_ADDR_P0, TX_ADDRESS, TX_ADR_WIDTH); // װ�ؽ��ն˵�ַ
	ANO_NRF_Write_Buf(WR_TX_PLOAD, tx_buf, len); 			 // װ������	
	ANO_SPI_CE_H();		 //�ø�CE���������ݷ���
}

void ANO_NRF_TxPacket_AP(uint8_t * tx_buf, uint8_t len)
{	
	ANO_SPI_CE_L();		 //StandBy Iģʽ	
	ANO_NRF_Write_Buf(0xa8, tx_buf, len); 			 // װ������
	ANO_SPI_CE_H();		 //�ø�CE
}


//NRF��ʼ�� ��ڲ��� ģʽ��ͨ��
void ANO_NRF_Init(u8 model, u8 ch)
{

	ANO_SPI_CE_L();
	ANO_NRF_Write_Buf(NRF_WRITE_REG+RX_ADDR_P0,RX_ADDRESS,RX_ADR_WIDTH);	//дRX�ڵ��ַ 
	ANO_NRF_Write_Buf(NRF_WRITE_REG+TX_ADDR,TX_ADDRESS,TX_ADR_WIDTH); 		//дTX�ڵ��ַ  
	ANO_NRF_Write_Reg(NRF_WRITE_REG+EN_AA,0x01); 													//ʹ��ͨ��0���Զ�Ӧ�� 
	ANO_NRF_Write_Reg(NRF_WRITE_REG+EN_RXADDR,0x01);											//ʹ��ͨ��0�Ľ��յ�ַ 
	ANO_NRF_Write_Reg(NRF_WRITE_REG+SETUP_RETR,0x1a);											//�����Զ��ط����ʱ��:500us;����Զ��ط�����:10�� 2M��������
	ANO_NRF_Write_Reg(NRF_WRITE_REG+RF_CH,ch);														//����RFͨ��ΪCHANAL
	ANO_NRF_Write_Reg(NRF_WRITE_REG+RF_SETUP,0x0f); 												//����TX�������,0db����,2Mbps,���������濪��
	//Write_Reg(NRF_WRITE_REG+RF_SETUP,0x07); 												//����TX�������,0db����,1Mbps,���������濪��
	//Write_Reg(NRF_WRITE_REG+RF_SETUP,0x27); 												//����TX�������,0db����,250Kbps,���������濪��
/////////////////////////////////////////////////////////
	if(model==1)				//RX
	{
		ANO_NRF_Write_Reg(NRF_WRITE_REG+RX_PW_P0,RX_PLOAD_WIDTH);								//ѡ��ͨ��0����Ч���ݿ�� 
		ANO_NRF_Write_Reg(NRF_WRITE_REG + CONFIG, 0x0f);   		 // IRQ�շ�����жϿ���,16λCRC,������
	}
	else if(model==2)		//TX
	{
		ANO_NRF_Write_Reg(NRF_WRITE_REG+RX_PW_P0,RX_PLOAD_WIDTH);								//ѡ��ͨ��0����Ч���ݿ�� 
		ANO_NRF_Write_Reg(NRF_WRITE_REG + CONFIG, 0x0e);   		 // IRQ�շ�����жϿ���,16λCRC,������
	}
	else if(model==3)		//RX2
	{
		ANO_NRF_Write_Reg(FLUSH_TX,0xff);
		ANO_NRF_Write_Reg(FLUSH_RX,0xff);
		ANO_NRF_Write_Reg(NRF_WRITE_REG + CONFIG, 0x0f);   		 // IRQ�շ�����жϿ���,16λCRC,������
		
		ANO_SPI_RW(0x50);
		ANO_SPI_RW(0x73);
		ANO_NRF_Write_Reg(NRF_WRITE_REG+0x1c,0x01);
		ANO_NRF_Write_Reg(NRF_WRITE_REG+0x1d,0x06);
	}
	else								//TX2
	{
		ANO_NRF_Write_Reg(NRF_WRITE_REG + CONFIG, 0x0e);   		 // IRQ�շ�����жϿ���,16λCRC,������
		ANO_NRF_Write_Reg(FLUSH_TX,0xff);
		ANO_NRF_Write_Reg(FLUSH_RX,0xff);
		
		ANO_SPI_RW(0x50);
		ANO_SPI_RW(0x73);
		ANO_NRF_Write_Reg(NRF_WRITE_REG+0x1c,0x01);
		ANO_NRF_Write_Reg(NRF_WRITE_REG+0x1d,0x06);
	}
	ANO_SPI_CE_H();
}

//���NRF�Ƿ�����
uint8_t ANO_NRF_Check(void)
{ 
	u8 buf1[5]; 
	u8 i; 
	/*д��5���ֽڵĵ�ַ. */ 
	ANO_NRF_Write_Buf(NRF_WRITE_REG+TX_ADDR,TX_ADDRESS,5); 
	/*����д��ĵ�ַ */ 
	ANO_NRF_Read_Buf(TX_ADDR,buf1,5); 
	/*�Ƚ�*/ 
	for(i=0;i<5;i++) 
	{ 
		if(buf1[i]!=TX_ADDRESS[i]) 
			break; 
	} 
	if(i==5)
		return 1; //MCU��NRF�ɹ����� 
	else
		return 0; //MCU��NRF���������� 
}

uint8_t NRF_SSI,NRF_SSI_CNT;//NRF�ź�ǿ��

void ANO_NRF_Check_Event(void)//NRF�շ��¼����
{
	u8 sta = ANO_NRF_Read_Reg(NRF_READ_REG + NRFRegSTATUS);
	////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////
	if(sta & (1<<RX_DR))//���յ�����
	{
		u8 rx_len = ANO_NRF_Read_Reg(R_RX_PL_WID);//���ݳ���
		NRF24L01_2_RXDATA_LEN = rx_len;
		if(rx_len<33)
		{

			ANO_NRF_Read_Buf(RD_RX_PLOAD,NRF24L01_2_RXDATA,rx_len);// read receive payload from RX_FIFO buffer
			if(Tx_Rx_mode==0)	//��λ��
			{
				Uart1_Put_Buf(NRF24L01_2_RXDATA,rx_len);////24l01�յ������ݷ��������ڣ���λ����ʹ�� 
			}
			else  //mode=1 ��λ��
			{
				delay_ms(1);  //�б�Ҫ��ʱ������PID����ʧ�� ԭ��δ֪
				ANO_DT_Data_Receive_Anl(NRF24L01_2_RXDATA,rx_len);
			}	

			NRF_SSI_CNT++;
			
			/////////
			ANO_NRF_Write_Reg(FLUSH_RX,0xff);//С����ӣ�����һ��������� ��ջ����������⴫��һ��ʱ���ֹͣ����
			/////////
		}
		else 
		{
			ANO_NRF_Write_Reg(FLUSH_RX,0xff);//��ջ�����
			
		}
	}

	////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////
	if(sta & (1<<TX_DS))
	{
		
	}
	////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////
	if(sta & (1<<MAX_RT))
	{
		if(sta & 0x01)	//TX FIFO FULL
		{
			ANO_NRF_Write_Reg(FLUSH_TX,0xff);
		}
	}
	////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////
	ANO_NRF_Write_Reg(NRF_WRITE_REG + NRFRegSTATUS, sta);
}


