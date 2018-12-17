#ifndef PORTOCOL_MNG_H
#define PORTOCOL_MNG_H
#include "de_type.h"
#include "Queue.h"
#include "PrinterCtrl.h"
#include "globals.h"

extern u8 cmdhead;
extern u16 cmdtemp,cmdspeed;
extern void (*cmdCbFunc[EXTRUDER_MAX])(u8);		//���̲����ص�����

#define		AGREEMENT_HEAD_H			0xAA
#define		AGREEMENT_HEAD_L			0x55
#define 	SENDMAXLEN	128		//������������ֽ���

#define		SENDQUEUELEN		8//����Ͷ��д洢����

#define ACK 0x00
#define NAK 0x01

#define		SERIALDEBUG					//���Դ��д�ӡ�궨��,�����,printf���������������


#define		AGREEMENT_DATALEN_INDEX		0x02
#define		AGREEMENT_ID_INDEX			0x04
#define		AGREEMENT_DATAHEAD_INDEX	0x05
#define		AGREEMENT_ID_SIZE	0
#define		AGREEMENT_ACK_SIZE	2

#define RX_BUF_SIZE		 (4*1024 + 64)
#define TX_BUF_SIZE		 (1*1024)

#define MAST_WAIT_ASK 4
#define MESSAGE_ID_SIZE 4

#define PROTOCOL_RESEND_COUNT 3
#define PROTOCOL_TIME_OUT 2000	//ͨѶ��ʱ1S ���ط�ʱ����

//ͨ������״̬
typedef enum 
{
	LINK_PORT_COM,		//���⴮������
	LINK_PORT_WIFI,		//wifi����
	LINK_PORT_MAX,
}LINK_PORT;

//���ݽ���״̬
typedef enum 
{
	PORT_STATUS_WAIT,		//�ȴ�����
	PORT_STATUS_RCV,		//������
	PORT_STATUS_ACK_DEFEAT	//ACK����У��ʧ��
}PORT_STATUS;
typedef struct{
	u32 Ack_ID;
	u32 repetime;
	u8 AckRepeBuf[SENDMAXLEN];
	u8 ackBuf_len;
	u8 timeout_repe_count;
	u8 nak_repe_count;
	bool WaitAck;
}AckRepetition;
typedef struct{
	bool wait;
	u8 SendDataLen;
	u8 SendData[TX_BUF_SIZE];
	u32 id;
}Send_Data;
typedef struct {
	uint8_t RxBuf[RX_BUF_SIZE];		//���ջ���
	uint8_t TxBuf[TX_BUF_SIZE];		//���ͻ���
	QUEUE8_t RxQueue;			//���ն���
	QUEUE8_t TxQueue;			//���Ͷ���
	Send_Data SendDataBuf[SENDQUEUELEN];//���Ͷ���
	QUEUE_STRUCT_t	SendDataQueue;
	LINK_PORT linkport;			//��ǰ����
	PORT_STATUS status;			//��ǰ״̬
	u32 RcvTimeOutCnt;			//��������ͨ�ų�ʱ
	u32 HeartBeatTimeOutCnt;	//������ʱʱ��
	AckRepetition AckRecord;//��¼��Ҫ�ط���ACK����
	bool Check_HeartBeat;	//�Ƿ���������ʱ
	bool CanSent;			//���Է�������
}PortCtrl;

extern PortCtrl g_PortCtrl;

extern int heart_count;
extern int send_flag;	
/*******************************************
	����ӿ�
********************************************/
//��ʼ��
extern void ProtocolMng_InitPort(LINK_PORT port);
extern void ProtocolMng_Pro(void);
extern void ProtocolMng_HearBeat_Check(void);
extern void PortEventCbFunc_PrintPause( u8 state );
extern void PortEventCbFunc_Standby( u8 state );

extern void ProtocolMng_DataParse(u8 *buf, u32 len);
extern void Logger_string(char *string);
extern void Logger_long(long num);
extern void Logger_float(float num);
#endif
