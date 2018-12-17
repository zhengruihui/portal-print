#include "ProtocolMng.h"
#include "de_type.h"
#include "ErrorMng.h"
#include "TempCtrl.h"
#include "FDM_config.h"
#include "usbd_cdc_if.h"
#include "PrintDataMng.h"
#include "EventMng.h"
#include "PrinterCtrl.h"
#include "Calculate.h"
#include "ProtocolData.h"
#include "RS485.h"
#include "network.h"
#include "dhcp.h"
#include "w5500.h"
#include "socket.h"

u32 text_time;
u32 text_time_hear[10000];
u32 i = 0;
u32 WriteFarmeTemp;				//���յ���֡��
u32 EmpSize;
u8 cmdhead;
u16 cmdtemp,cmdspeed;
u8 Port_SendBuf[SENDMAXLEN] = {0xAA, 0x55};
/***********************************************************************
    ���غ�
************************************************************************/
#define HEARTBEAT_TIMEOUT	3000	//������ʱʱ��
#define	PROTOCOL_RECVBUF_SIZE		(1024*5)	//���Ż��峤��5KB
/***********************************************************************
    ���ر���
************************************************************************/
/* ����״̬���� */
enum _Recv_Stage
{
	RECV_GETSTART,
	RECV_FINDSTART1,
	RECV_FINDSTART2,
	RECV_FINDLEN,
	RECV_RECVING,
	RECV_END
};
/* ����״̬���� */
enum _Send_Stage
{
	SEND_READY,
	SEND_WAIT_RESPONSE,
	SEND_END,
};
typedef enum 
{
	CMD_CTRL = 0x00,			//����ָ��
	CMD_GET_PRINTSTATE,			//��ȡ��ӡ״̬
	CMD_GET_VERSION,			//��ȡ����汾��
	CMD_RECEIVE_PRINTDATA,		//���ܴ�ӡ����
	CMD_RESIVE,
	CMD_GET_PROGRAMSTATE,		//��ȡ��������״̬
	CMD_UPGRADE,				//��ʼ����
	CMD_AXISCONTROL,			//�������
	CMD_BUZZER,					//������
	CMD_ACK = 0xff,
	CMD_MAX
}_CMD;

typedef enum 
{
	CMD_CTRL_WAIT = 0x00,		//����ָ�� �ص�����
	CMD_CTRL_PRINT,				//����ָ�� ��ʼ��ӡ
	CMD_CTRL_SDWRITE,			//����
	CMD_CTRL_LEVELING,			//����
	CMD_CTRL_HEAD1_OPERATE,		//����ָ�� ���ϲ���
	CMD_CTRL_HEAD2_OPERATE,		//����ָ�� ���ϲ���
	CMD_CTRL_PAUSE_PRINT,		//����ָ�� ��ͣ��ӡ
	CMD_CTRL_CONTINUE_PRINT,	//����ָ�� ������ӡ
	CMD_CRTL_SELECT_FILE,  //ѡ���ӡ�ļ�
	CMD_CTRL_WARM,				//����ָ�� ������ӡ
	CMD_CTRL_PORTALFRAME = 'F',		//�л����ż�
}SERIAL_CMD_CTRL;
typedef enum 
{
	HEAD_STAR_HEAT = 0x00,
	HEAD_STAR_FEEDSTOCK,
	HEAD_PAUSE,
	HEAD_CONTINUE
}CMD_CTRL_HEAD;

/* ���Ż��嶨�� */
typedef struct {
	u8 Buf[PROTOCOL_RECVBUF_SIZE];		//�����ַ
	u16 Len;							//���峤��
}UsartRecvBuffer;

/***********************************************************************
	�ⲿ�ο�����
***********************************************************************/
PortCtrl g_PortCtrl;
u8 Port_TimeOut_Flg;
/***********************************************************************
	�ڲ���������
***********************************************************************/
static void ProtocolMng_CMD_17H(u32 len);
static void ProtocolMng_CMD_16H( void );
static void ProtocolMng_CMD_13H(u16 len);
static void ProtocolMng_CMD_10H(u8 *cmd_data);
static void ProtocolMng_CMD_Config(u8* buf);
static void ProtocolMng_Cmd_SetHeadTemp(u8 warm_type ,u8 number ,u16 temp);
static void ProtocolMng_CMD_Ctrl_HeadOperate(u8 *cmd_data);
static void ProtocolMng_CMD_11H(u8 *data ,u32 len);
static void ProtocolMng_CMD_ACK(u8 *cak ,u8 len);

static void ProtocolMng_17H( u8 state );
static void ProtocolMng_15H( u8 state );
static void ProtocolMng_13H( u8 state );
static void ProtocolMng_12H( void );
static void ProtocolMng_11H(u8 cmd ,u16 len);
static void ProtocolMng_10H(u8 cmd ,u8 sub_cmd ,u8 state);
static void ProtocolMng_OK_OR_NG(u8 cmd ,u8 state);
static void ProtocolMng_ACK(void);
static void ProtocolMng_NAK(void);
static void ProtocolMng_PrintDataQueueFull(void);
static void ProtocolMng_LoadId(u8 cmd);
void ProtocolMng_LoadDataToQueue(u8 *buf, u32 len ,bool wait);
static void ProtocolMng_LoadAckData(u8 *buf,u32 id, u32 len);

static void ProtocolMng_PortSend(void);
static bool ProtocolMng_DataReceiveOneFrame(void);
void ProtocolMng_DataParse(u8 *buf, u32 len);
static u32 ProtocolMng_GetID(u8 *message_id);
uint32_t ProtocolMng_GetTxSize(void);
uint32_t ProtocolMng_TxWrite(uint8_t* Buf, uint16_t Len);
static void PortEventCbFunc_Control( u8 state );
static void PortEventCbFunc_Upgrade( u8 state );
static void PortEventCbFunc_Material( u8 state );
//static void PortEventCbFunc_TransferFile_RxEnd( u8 state );
//static void PortEventCbFunc_TransferFile( u8 state );
static void PortEventCbFunc_PrintContinue( u8 state );
static void PortEventCbFunc_Print( u8 state );
static void ProtocolMng_RcvTimeOutCheck(void);
bool ProtocolMng_DataCheck(u8 *buf, u16 Len);
uint32_t ProtocolMng_RxRead(uint8_t *buffter, uint32_t buffterSize);
uint32_t ProtocolMng_GetRxSize(void);

/***********************************************************************
		�ⲿ����
***********************************************************************/
/***********************************************************************
    ����ȫ�ֱ���
***********************************************************************/
static enum _Recv_Stage m_RecvStage = RECV_GETSTART;
//static enum _Send_Stage m_Sendtage = SEND_READY;
static u16 m_SizeNeedtoRead;
static UsartRecvBuffer m_UartDataBuf;
static u32 m_CmdId[CMD_MAX] = {0};
static Send_Data m_RetryBuf[SENDQUEUELEN];
static Send_Data m_CurrentWaitAckBuf;
static u32 m_CurrentMessage_ID = 0;	//��¼���µ�id
static u32 m_WaitAck_ID = 0;	//��ǰ�ȴ���ack��id
static bool m_WaitReceive = False;


int heart_count = 0;
int send_flag = 0;	
//bool ProtocolMng_WaitIn(Send_Data databuf)
//{
//	u8 i;
//	bool ret = False;
//	for(i = 0 ;i < SENDQUEUELEN ;i++)
//	{
//		if(m_RetryBuf[i].wait !=True)
//		{
//			m_RetryBuf[i].wait =True;
//			m_RetryBuf[i].id = databuf.id;
//			m_RetryBuf[i].SendDataLen = databuf.SendDataLen;
//			memcpy(m_RetryBuf[i].SendData ,databuf.SendData ,TX_BUF_SIZE);		
//			ret = True;
//		}
//	}
//	return ret;
//}
//bool ProtocolMng_FinedWaitOut(u32 id)
//{
//	u8 i;
//	bool ret = False;
//	for(i = 0 ;i < SENDQUEUELEN ;i++)
//	{
//		if(m_RetryBuf[i].wait == True)
//		{
//			if(m_RetryBuf[i].id == id)
//			{
//				memset(m_RetryBuf[i].SendData ,0 ,TX_BUF_SIZE);
//				m_RetryBuf[i].wait =False;
//				m_RetryBuf[i].id = 0;
//				m_RetryBuf[i].SendDataLen = 0;				
//				ret = True;
//			}
//		}
//	}
//	return ret;
//}
//bool ProtocolMng_GetCurrentWaitBuf(u32 id ,Send_Data data)
//{
//	u8 i;
//	bool ret = False;
//	for(i = 0 ;i < SENDQUEUELEN ;i++)
//	{
//		if(m_RetryBuf[i].wait == True)
//		{
//			if(m_RetryBuf[i].wait == id)
//			{
//				memcpy(data.SendData ,m_RetryBuf[i].SendData ,TX_BUF_SIZE);	
//				data.SendDataLen = m_RetryBuf[i].SendDataLen;
//				ret = True;
//			}
//		}
//	}
//	return ret;
//}
/*******************************************************************
���ܣ��洢�����ط�������
�����ߣ�pang-wei
�������ڣ�2017/5/27
������
buf:����
len:����
����ֵ����
********************************************************************/
void ProtocolMng_LoadAckData(u8 *buf,u32 id, u32 len)
{	
	memcpy(g_PortCtrl.AckRecord.AckRepeBuf ,buf ,len);
	g_PortCtrl.AckRecord.Ack_ID = id;
	g_PortCtrl.AckRecord.ackBuf_len = len;
	g_PortCtrl.AckRecord.nak_repe_count = 0;
	g_PortCtrl.AckRecord.timeout_repe_count = 0;
}
/*******************************************************************
���ܣ�����ת�������
�����ߣ�pang-wei
�������ڣ�2017/5/27
������
buf:����
len:����
����ֵ����
********************************************************************/
void ProtocolMng_LoadDataToQueue(u8 *buf, u32 len ,bool wait)
{	
	Send_Data data;
	memcpy(data.SendData ,buf ,len);
	data.SendDataLen = len;
	data.wait = wait;	
	data.id = ProtocolMng_GetID(&buf[4]);

	QUEUE_StructIn(&g_PortCtrl.SendDataQueue, &data, 1);
}
/*******************************************************************
���ܣ�����ID
�����ߣ�pang-wei
�������ڣ�2017/5/27
������
buf:����
len:����
����ֵ����
********************************************************************/
void ProtocolMng_LoadId(u8 cmd)
{	
	Port_SendBuf[4] = m_CmdId[cmd]>>24;
	Port_SendBuf[5] = m_CmdId[cmd]>>16;
	Port_SendBuf[6] = m_CmdId[cmd]>>8;
	Port_SendBuf[7] = m_CmdId[cmd];
}

/*******************************************************************
���ܣ���ӡ���ݻ��治��
�����ߣ�pang-wei
�������ڣ�2017/5/27
������
��
����ֵ����
********************************************************************/
void ProtocolMng_PrintDataQueueFull()//���治��
{
	u16 crc_temp;
	Port_SendBuf[2] = 0x00;
	Port_SendBuf[3] = 0x08;
	ProtocolMng_LoadId(CMD_RECEIVE_PRINTDATA);
	Port_SendBuf[8] = 0x13;
	Port_SendBuf[9] = 0x02;
	crc_temp =  GetCRC16(Port_SendBuf+2,8);
	Port_SendBuf[10] = (crc_temp>>8)&0xFF;
	Port_SendBuf[11] = crc_temp&0xFF;
	ProtocolMng_LoadDataToQueue(Port_SendBuf, 12 ,True);
	ProtocolMng_LoadAckData(Port_SendBuf, m_CmdId[CMD_RECEIVE_PRINTDATA] ,12);
}
/*******************************************************************
���ܣ�NACK
�����ߣ�pang-wei
�������ڣ�2017/5/27
������
����ֵ����
********************************************************************/
void ProtocolMng_NAK()
{
	u16 crc_temp;
	Port_SendBuf[2] = 0x00;
	Port_SendBuf[3] = 0x08;
	Port_SendBuf[4] = m_CurrentMessage_ID>>24;
	Port_SendBuf[5] = m_CurrentMessage_ID>>16;
	Port_SendBuf[6] = m_CurrentMessage_ID>>8;
	Port_SendBuf[7] = m_CurrentMessage_ID;		
	Port_SendBuf[8] = 0xff;
	Port_SendBuf[9] = NAK;
	crc_temp =  GetCRC16(Port_SendBuf+2,8);
	Port_SendBuf[10] = (crc_temp>>8)&0xFF;
	Port_SendBuf[11] = crc_temp&0xFF;
	ProtocolMng_LoadDataToQueue(Port_SendBuf,12 ,False);
}
/*******************************************************************
���ܣ�ACK
�����ߣ�pang-wei
�������ڣ�2017/5/27
������
����ֵ����
********************************************************************/
void ProtocolMng_ACK()
{
	u16 crc_temp;
	Port_SendBuf[2] = 0x00;
	Port_SendBuf[3] = 0x08;
	Port_SendBuf[4] = m_CurrentMessage_ID>>24;
	Port_SendBuf[5] = m_CurrentMessage_ID>>16;
	Port_SendBuf[6] = m_CurrentMessage_ID>>8;
	Port_SendBuf[7] = m_CurrentMessage_ID;			
	Port_SendBuf[8] = 0xff;
	Port_SendBuf[9] = ACK;
	crc_temp =  GetCRC16(Port_SendBuf+2,8);
	Port_SendBuf[10] = (crc_temp>>8)&0xFF;
	Port_SendBuf[11] = crc_temp&0xFF;
	ProtocolMng_LoadDataToQueue(Port_SendBuf, 12 ,False);
}
/*******************************************************************
���ܣ�OK
�����ߣ�pang-wei
�������ڣ�2017/5/27
������
buf:����
len:����
����ֵ����
********************************************************************/
void ProtocolMng_OK_OR_NG(u8 cmd ,u8 state)
{
	u16 crc_temp;
	Port_SendBuf[2] = 0x00;
	Port_SendBuf[3] = 0x08;
	ProtocolMng_LoadId(cmd&0x0f);	
	Port_SendBuf[8] = cmd;
	Port_SendBuf[9] = state;
	crc_temp =  GetCRC16(Port_SendBuf+2,8);
	Port_SendBuf[10] = (crc_temp>>8)&0xFF;
	Port_SendBuf[11] = crc_temp&0xFF;
	ProtocolMng_LoadDataToQueue(Port_SendBuf, 12 ,True);
	ProtocolMng_LoadAckData(Port_SendBuf ,m_CmdId[cmd] ,12);
}
/*******************************************************************
���ܣ�������Ʒ���10H
�����ߣ�liu-cy
�������ڣ�2015/11/16
������
cmk:��ָ�sub_cmd:��ָ�state:����
����ֵ����
********************************************************************/
void ProtocolMng_10H(u8 cmd ,u8 sub_cmd ,u8 state)
{
	u16 crc_temp;
	Port_SendBuf[2] = 0x00;
	Port_SendBuf[3] = 0x05;			
	Port_SendBuf[4] = cmd;	
	Port_SendBuf[5] = state;	
	crc_temp =  GetCRC16(Port_SendBuf+2,4);
	Port_SendBuf[6] = (crc_temp>>8)&0xFF;
	Port_SendBuf[7] = crc_temp&0xFF;
	ProtocolMng_LoadDataToQueue(Port_SendBuf, 8 ,False);
	ProtocolMng_LoadAckData(Port_SendBuf ,m_CmdId[cmd] ,8);
}
/*******************************************************************
���ܣ�11H
�����ߣ�pang-wei
�������ڣ�2017/5/27
������
cmd_data
����ֵ����
********************************************************************/
void ProtocolMng_11H(u8 cmd ,u16 len)
{
	u16 crc_temp;

	float line_percent = (g_printer.portalFrame[PORTALFRAME_2].current_gcode_line / g_printer.portalFrame[PORTALFRAME_2].total_gcode_line) * 100;
	uint8_t *print_percent = (uint8_t *)&line_percent;
	
	float tmpe1 = TempCtrl_getCurrentTemp(1);
	float tmpe2 = TempCtrl_getCurrentTemp(2);
	float tmpe3 = TempCtrl_getCurrentTemp(3);
	float tmpe4 = TempCtrl_getCurrentTemp(4);
	
	
	uint8_t *tmperature1 = (uint8_t*)&tmpe1;
	uint8_t *tmperature2 = (uint8_t*)&tmpe2;
	uint8_t *tmperature3 = (uint8_t*)&tmpe3;
	uint8_t *tmperature4 = (uint8_t*)&tmpe4;

	
	uint8_t *position_x = (uint8_t*)&g_printer.portalFrame[PORTALFRAME_2].positionMng.mainAxesPos[0];
	uint8_t *position_y = (uint8_t*)&g_printer.portalFrame[PORTALFRAME_2].positionMng.mainAxesPos[1];
	uint8_t *position_z = (uint8_t*)&g_printer.portalFrame[PORTALFRAME_2].positionMng.mainAxesPos[2];
	
	Port_SendBuf[0] = AGREEMENT_HEAD_H;
	Port_SendBuf[1] = AGREEMENT_HEAD_L;
	Port_SendBuf[2] = (len)>> 8;
	Port_SendBuf[3] = (len);	
	Port_SendBuf[4] = cmd;

	Port_SendBuf[5] = g_printer.portalFrame[PORTALFRAME_2].stage;	 //��״̬
	Port_SendBuf[6] = g_printer.portalFrame[PORTALFRAME_2].printInfo.stage;	 //��״̬
	Port_SendBuf[7] = 0xff;	 //�쳣
	

	Port_SendBuf[8] = tmperature1[0];	 //�¶�1
	Port_SendBuf[9] = tmperature1[1];
	Port_SendBuf[10] = tmperature1[2];
	Port_SendBuf[11] = tmperature1[3];
	
	Port_SendBuf[12] = tmperature2[0];	 //�¶�2
	Port_SendBuf[13] = tmperature2[1];
	Port_SendBuf[14] = tmperature2[2];
	Port_SendBuf[15] = tmperature2[3];
	
	Port_SendBuf[16] = tmperature3[0];	 //�¶�3
	Port_SendBuf[17] = tmperature3[1];
	Port_SendBuf[18] = tmperature3[2];
	Port_SendBuf[19] = tmperature3[3];
	
	Port_SendBuf[20] = tmperature4[0];	 //�¶�4
	Port_SendBuf[21] = tmperature4[1];
	Port_SendBuf[22] = tmperature4[2];
	Port_SendBuf[23] = tmperature4[3];
 
 
	
	Port_SendBuf[24] = print_percent[0];	 //����
	Port_SendBuf[25] = print_percent[1];	 //x
	Port_SendBuf[26] = print_percent[2];	 
	Port_SendBuf[27] = print_percent[3];
	
	
	
	Port_SendBuf[28] = position_x[0];	 //x
	Port_SendBuf[29] = position_x[1];	 
	Port_SendBuf[30] = position_x[2];	
	Port_SendBuf[31] = position_x[3];	 
	
	Port_SendBuf[32] = position_y[0];	 //y
	Port_SendBuf[33] = position_y[1];	 	
	Port_SendBuf[34] = position_y[2];	 
	Port_SendBuf[35] = position_y[3];	 
	
	Port_SendBuf[36] = position_z[0];	 //z
	Port_SendBuf[37] = position_z[1];	 	
	Port_SendBuf[38] = position_z[2];	 
	Port_SendBuf[39] = position_z[3];	
	

	

	crc_temp = GetCRC16(Port_SendBuf+2, 38);
	Port_SendBuf[40] = (crc_temp>>8)&0xFF;
	Port_SendBuf[41] = crc_temp&0xFF;	
	//ProtocolMng_LoadDataToQueue(Port_SendBuf, 32 ,False);
	//ProtocolMng_LoadAckData(Port_SendBuf, m_CmdId[CMD_GET_PRINTSTATE],32);
	
	sendto(0, Port_SendBuf, 42, net_work.desip, DHCP_SERVER_PORT);
}
/*******************************************************************
���ܣ�12H
�����ߣ�pang-wei
�������ڣ�2017/5/27
������
cmd_data
����ֵ����
********************************************************************/
void ProtocolMng_12H( void )
{
	u16 crc_temp;
	Port_SendBuf[2] = 0x00;
	Port_SendBuf[3] = 0x0c;
	ProtocolMng_LoadId(CMD_GET_VERSION);			
	Port_SendBuf[8] = 0x12;
	Port_SendBuf[9] = MODEL_H;
	Port_SendBuf[10] = MODEl_L;
	Port_SendBuf[11] = VERSION_H;
	Port_SendBuf[12] = VERSION_M;
	Port_SendBuf[13] = VERSION_L;
	crc_temp = GetCRC16(Port_SendBuf+2,12);
	Port_SendBuf[14] = (crc_temp>>8)&0xFF;
	Port_SendBuf[15] = crc_temp&0xFF;
	ProtocolMng_LoadDataToQueue(Port_SendBuf, 16 ,True);
	ProtocolMng_LoadAckData(Port_SendBuf, m_CmdId[CMD_GET_VERSION],16);
}

/*******************************************************************
���ܣ�13H
�����ߣ�pang-wei
�������ڣ�2017/5/27
������
cmd_data
����ֵ����
********************************************************************/
void ProtocolMng_13H( u8 state )
{
	u16 crc_temp;
	Port_SendBuf[2] = 0x00;
	Port_SendBuf[3] = 0x08;
	ProtocolMng_LoadId(CMD_RECEIVE_PRINTDATA);			
	Port_SendBuf[8] = 0x13;
	Port_SendBuf[9] = state;
	crc_temp = GetCRC16(Port_SendBuf+2,8);
	Port_SendBuf[10] = (crc_temp>>8)&0xFF;
	Port_SendBuf[11] = crc_temp&0xFF;
	ProtocolMng_LoadDataToQueue(Port_SendBuf, 12 ,True);
	ProtocolMng_LoadAckData(Port_SendBuf, m_CmdId[CMD_RECEIVE_PRINTDATA],12);
}

/*******************************************************************
���ܣ�14H
�����ߣ�pang-wei
�������ڣ�2017/5/27
������
cmd_data
����ֵ����
********************************************************************/
//void ProtocolMng_14H(void)
//{
//	u16 crc_temp;
//	u32 size;
////	FATFS_MountSDdisk();	//����SD��
////	size = FATFS_GetFree("0:");
////	FATFS_UmountSDdisk();	//ж��SD��
//	Port_SendBuf[2] = 0x00;
//	Port_SendBuf[3] = 0x07;
//	Port_SendBuf[4] = 0x14;
//	Port_SendBuf[5] = (size >> 24) & 0xff;
//	Port_SendBuf[6] = (size >> 16) & 0xff;
//	Port_SendBuf[7] = (size >> 8) & 0xff;
//	Port_SendBuf[8] = size & 0xff;
//	crc_temp = GetCRC16(Port_SendBuf+2,7);
//	Port_SendBuf[9] = (crc_temp>>8)&0xFF;
//	Port_SendBuf[10] = crc_temp&0xFF;
//	QUEUE_PacketIn(&g_PortCtrl.TxQueue, Port_SendBuf, 11);
//	ProtocolMng_LoadAckData(Port_SendBuf, 11);
//}

/*******************************************************************
���ܣ�15H
�����ߣ�pang-wei
�������ڣ�2017/5/27
������
cmd_data
����ֵ����
********************************************************************/
void ProtocolMng_15H( u8 state )
{
	u16 crc_temp;
	Port_SendBuf[2] = 0x00;
	Port_SendBuf[3] = 0x08;
	ProtocolMng_LoadId(CMD_GET_PROGRAMSTATE);		
	Port_SendBuf[8] = 0x15;
	Port_SendBuf[9] = state;
	crc_temp = GetCRC16(Port_SendBuf+2,8);
	Port_SendBuf[10] = (crc_temp>>8)&0xFF;
	Port_SendBuf[11] = crc_temp&0xFF;
	ProtocolMng_LoadDataToQueue(Port_SendBuf, 12 ,True);
	ProtocolMng_LoadAckData(Port_SendBuf, m_CmdId[CMD_GET_PROGRAMSTATE], 12);
}
/*******************************************************************
���ܣ�15H
�����ߣ�pang-wei
�������ڣ�2017/5/27
������
cmd_data
����ֵ����
********************************************************************/
void ProtocolMng_16H( u8 state )
{
	u16 crc_temp;
	Port_SendBuf[2] = 0x00;
	Port_SendBuf[3] = 0x04;	
	Port_SendBuf[4] = 0x16;
	Port_SendBuf[5] = state;
	crc_temp = GetCRC16(Port_SendBuf+2,4);
	Port_SendBuf[6] = (crc_temp>>8)&0xFF;
	Port_SendBuf[7] = crc_temp&0xFF;
//	ProtocolMng_LoadDataToQueue(Port_SendBuf, 8 ,True);
//	ProtocolMng_LoadAckData(Port_SendBuf, m_CmdId[CMD_GET_PROGRAMSTATE], 8);
//	
	
	QUEUE_PacketIn(&g_PortCtrl.TxQueue, Port_SendBuf, 8);
	ProtocolMng_PortSend();
}
/*******************************************************************
���ܣ��������״̬����10H
�����ߣ�liu-cy
�������ڣ�2015/11/16
������
    state:0x00,��0x01
����ֵ����
********************************************************************/
void ProtocolMng_17H( u8 state )
{
	u16 crc_temp;
	Port_SendBuf[2] = 0x00;
	Port_SendBuf[3] = 0x04;
	ProtocolMng_LoadId(CMD_AXISCONTROL);	
	Port_SendBuf[8] = 0x17;
	Port_SendBuf[9] = state;
	crc_temp = GetCRC16(Port_SendBuf+2,8);
	Port_SendBuf[10] = (crc_temp>>8)&0xFF;
	Port_SendBuf[11] = crc_temp&0xFF;
	ProtocolMng_LoadDataToQueue(Port_SendBuf, 12 ,True);
	ProtocolMng_LoadAckData(Port_SendBuf,m_CmdId[CMD_AXISCONTROL], 12);
}
/******************************************
���ܣ���ȡ���ջ����������ֽ���
�����ߣ�liu-cy
�������ڣ�2016/11/17
������
����ֵ��
*******************************************/
uint32_t ProtocolMng_GetRxSize(void)
{
	return QUEUE_PacketLengthGet(&g_PortCtrl.RxQueue);
}
/******************************************
���ܣ��ӽ��ջ����ж�����
�����ߣ�liu-cy
�������ڣ�2016/11/17
������
����ֵ��
*******************************************/
uint32_t ProtocolMng_RxRead(uint8_t *buffter, uint32_t buffterSize)
{
	uint32_t size;
//	__disable_irq();   //�ر����жϷ�ֹ���ݱ����
	size = QUEUE_PacketOut(&g_PortCtrl.RxQueue, buffter, buffterSize);
//	__enable_irq();    //�������ж�
	return size;
}
/*******************************************************************
���ܣ�У������
�����ߣ�liu-cy
�������ڣ�2016/5/12
������
	buf:���ݵ�ַ
	Len:���ݳ���
����ֵ����
********************************************************************/
bool ProtocolMng_DataCheck(u8 *buf, u16 Len)
{
	u16 crc_temp;
	u16 len_temp;

	len_temp = (buf[AGREEMENT_DATALEN_INDEX] << 8) + buf[AGREEMENT_DATALEN_INDEX+1];	//�������ݳ���
	crc_temp = GetCRC16(&buf[AGREEMENT_DATALEN_INDEX],len_temp);	//�����У��
	
	if( ((buf[0] == 0xAA) && (buf[1] == 0x55)) 			//У��֡ͷ
		&& ((len_temp == (Len - 4)) && (Len >= 0x06))		//У�����ݳ���
		&& (((buf[Len-2]) << 8 ) + buf[Len-1] ==  crc_temp) )		//CRCУ��
	{
		return True;	//У��ȫ��ͨ������true
	}
	else	
		return False;
}

/******************************************
���ܣ�����ͨ�Ž��ճ�ʱ���
�����ߣ�liu-cy
�������ڣ�2016/11/17
��������
����ֵ����
*******************************************/
void ProtocolMng_RcvTimeOutCheck(void)
{
	// Send_Data resend_data;
	if((m_WaitReceive == True)&&(HAL_GetTick() - g_PortCtrl.RcvTimeOutCnt) >= PROTOCOL_TIME_OUT)	//
	{
		g_PortCtrl.AckRecord.timeout_repe_count++;
		g_PortCtrl.RcvTimeOutCnt = HAL_GetTick();
//		ProtocolMng_GetCurrentWaitBuf(m_WaitAck_ID ,resend_data);							//�õ���ǰ�ȴ���ID������
//		QUEUE_PacketIn(&g_PortCtrl.TxQueue, m_CurrentWaitAckBuf.SendData, m_CurrentWaitAckBuf.SendDataLen);
		if(g_PortCtrl.AckRecord.timeout_repe_count >= PROTOCOL_RESEND_COUNT)
		{
//			ProtocolMng_FinedWaitOut(m_WaitAck_ID);
		//	QUEUE_PacketCreate(&g_PortCtrl.RxQueue, g_PortCtrl.RxBuf, sizeof(g_PortCtrl.RxBuf));		//��ʼ�����ն���
			g_PortCtrl.status = PORT_STATUS_WAIT;	//�ص��ȴ�����״̬
			m_WaitReceive = False;
			g_PortCtrl.AckRecord.timeout_repe_count = 0;
//			ErrorMng_SetError( ERROR_COM );//ͨѶ�쳣
		}
	}
}

/******************************************
���ܣ���λ���¼��ص�����-����
�����ߣ�liu-cy
�������ڣ�2017/03/23
������state: 1:�ɹ�  0:ʧ��
����ֵ����
*******************************************/
void PortEventCbFunc_Standby( u8 state )
{
	switch(state)
	{
		case _NG:
			
			break;
		case _OK:
			break;
		default:
			break;
	}
	ProtocolMng_10H(0x10 ,0x00 ,state);
}

/******************************************
���ܣ���λ���¼��ص�����-��ӡ
�����ߣ�liu-cy
�������ڣ�2017/03/23
������state: 1:�ɹ�  0:ʧ��
����ֵ����
*******************************************/
void PortEventCbFunc_Print( u8 state )
{
	switch(state)
	{
		case _NG:
			
			break;
		case _OK:
			break;
		default:
			break;
	}
	ProtocolMng_10H(0x10 ,0x01 ,state);
}

/******************************************
���ܣ���λ���¼��ص�����-��ӡ��ͣ
�����ߣ�liu-cy
�������ڣ�2017/03/23
������state: 1:�ɹ�  0:ʧ��
����ֵ����
*******************************************/
void PortEventCbFunc_PrintPause( u8 state )
{
	switch(state)
	{
		case _NG:
			
			break;
		case _OK:
			break;
		default:
			break;
	}
	ProtocolMng_10H(0x10 ,0x06 ,state);
}

/******************************************
���ܣ���λ���¼��ص�����-������ӡ
�����ߣ�liu-cy
�������ڣ�2017/03/23
������state: 1:�ɹ�  0:ʧ��
����ֵ����
*******************************************/
void PortEventCbFunc_PrintContinue( u8 state )
{
	switch(state)
	{
		case _NG:
			
			break;
		case _OK:
			break;
		default:
			break;
	}
	ProtocolMng_10H(0x10 ,0x07 ,state);
}

///******************************************
//���ܣ���λ���¼��ص�����-���ļ�
//�����ߣ�liu-cy
//�������ڣ�2017/03/23
//������state: 1:�ɹ�  0:ʧ��
//����ֵ����
//*******************************************/
//void PortEventCbFunc_TransferFile( u8 state )
//{
//	switch(state)
//	{
//		case _NG:
//			
//			break;
//		case _OK:
//			break;
//		default:
//			break;
//	}
//	ProtocolMng_OK(0x13,state);
//}

///******************************************
//���ܣ���λ���¼��ص�����-���ļ�,�������
//�����ߣ�liu-cy
//�������ڣ�2017/03/23
//������state: 1:�ɹ�  0:ʧ��
//����ֵ����
//*******************************************/
//void PortEventCbFunc_TransferFile_RxEnd( u8 state )
//{
//	switch(state)
//	{
//		case _NG:
//			
//			break;
//		case _OK:
//			
//			break;
//		default:
//			break;
//	}
//	ProtocolMng_OK(0x13,state);
//}

/******************************************
���ܣ���λ���¼��ص�����-��ƽ
�����ߣ�liu-cy
�������ڣ�2017/03/23
������state: 1:�ɹ�  0:ʧ��
����ֵ����
*******************************************/
//void PortEventCbFunc_Leveling( u8 state )
//{
//	switch(state)
//	{
//		case _NG:
//			
//			break;
//		case _OK:
//			
//			break;
//		default:
//			break;
//	}
//	ProtocolMng_10H(0x10 ,0x03 ,state);
//}

/******************************************
���ܣ���λ���¼��ص�����-����
�����ߣ�liu-cy
�������ڣ�2017/03/23
������state: 1:�ɹ�  0:ʧ��
����ֵ����
*******************************************/
void PortEventCbFunc_Material( u8 state )
{

}

/******************************************
���ܣ���λ���¼��ص�����-����
�����ߣ�liu-cy
�������ڣ�2017/03/23
������state: 1:�ɹ�  0:ʧ��
����ֵ����
*******************************************/
void PortEventCbFunc_Upgrade( u8 state )
{

}

/******************************************
���ܣ���λ���¼��ص�����-����,ִ��Gcode
�����ߣ�liu-cy
�������ڣ�2017/03/23
������state: 1:�ɹ�  0:ʧ��
����ֵ����
*******************************************/
void PortEventCbFunc_Control( u8 state )
{
	switch(state)
	{
		case _NG:
			
			break;
		case _OK:
			
			break;
		default:
			break;
	}
	ProtocolMng_OK_OR_NG(0x17,state);
}

/******************************************
���ܣ���������
�����ߣ�liu-cy
�������ڣ�2016/11/17
������
����ֵ��
*******************************************/
uint32_t ProtocolMng_TxWrite(uint8_t* Buf, uint16_t Len)
{
  uint32_t bufsize   = 0;
  bufsize = QUEUE_PacketIn(&g_PortCtrl.TxQueue, Buf, Len);
  return bufsize;
}

/******************************************
���ܣ���ȡ���ͻ����������ֽ���
�����ߣ�liu-cy
�������ڣ�2016/11/17
������
����ֵ��
*******************************************/
uint32_t ProtocolMng_GetTxSize(void)
{
	return QUEUE_PacketLengthGet(&g_PortCtrl.TxQueue);
}
/******************************************
���ܣ�������ʱ���
�����ߣ�
�������ڣ�
������
����ֵ��
*******************************************/
void ProtocolMng_HearBeat_Check()
{
	if((HAL_GetTick() - g_PortCtrl.HeartBeatTimeOutCnt > HEARTBEAT_TIMEOUT) && g_PortCtrl.Check_HeartBeat == True)
	{
		ErrorMng_SetError(ERROR_COMTIMOUT);
	}
}
/******************************************
���ܣ�ͨѶЭ��11H����
�����ߣ�pangwei
�������ڣ�2017/5/27
������data  01H����ָ��  len  01Hָ��������ܳ���
����ֵ����
*******************************************/
void ProtocolMng_CMD_11H(u8 *data ,u32 len)
{
	int i;
	float temp;
	
	for(i = 0; i < TEMPCTRL_ITEMMAX; i++)
	{
		Logger_long(i);
		Logger_string(":");
		Logger_float(TempCtrl_getCurrentTemp(i));
		Logger_string("  ");
	}
	Logger_string("\n");
}
/******************************************
���ܣ���ͷ����
�����ߣ�
�������ڣ�2017/5/27
������cmd_data  02H����ָ��
����ֵ����
*******************************************/
void ProtocolMng_CMD_Ctrl_HeadOperate(u8 *cmd_data)
{

}
/******************************************
���ܣ���ƽ�������õ���ƽ����
�����ߣ�
�������ڣ�2017/5/27
������cmd_data  ��ָ��
����ֵ����
*******************************************/
//void ProtocolMng_CMD_Ctrl_Leveling_GetPosition(u8 *cmd_data)
//{
//	g_printer.level.level_position_x = ((cmd_data[1] << 24) + \
//										(cmd_data[2] << 16) + \
//										(cmd_data[3] << 8) + \
//										cmd_data[4])/1000.0f;
//	g_printer.level.level_position_y = ((cmd_data[5] << 24) + \
//										(cmd_data[6] << 16) + \
//										(cmd_data[7] << 8) + \
//										cmd_data[8])/1000.0f;
//	g_printer.level.level_speed 	= ((cmd_data[9] << 24) + \
//										(cmd_data[10] << 16) + \
//										(cmd_data[11] << 8) + \
//										cmd_data[12])/1000.0f;	
//}
void ProtocolMng_Cmd_SetHeadTemp(u8 warm_type ,u8 number ,u16 temp)
{

}
/******************************************
���ܣ�ͨѶЭ��10H����
�����ߣ�pangwei
�������ڣ�2017/5/27
������cmd_data  00H����ָ��
����ֵ����
*******************************************/
void ProtocolMng_CMD_10H(u8 *cmd_data)
{
	u16 tem;
	switch(cmd_data[0])
	{
		case CMD_CTRL_WAIT://�������
			EventMng_SetEvent_Level(PRINTER_EVENT_STANDBY, NULL ,EVENT_LEVEL_0);	//�ص�����
			ProtocolMng_10H(0x10 ,0x00 ,0x00);
			break;	
		case CMD_CTRL_PRINT://��ʼ��ӡ	
			g_GcodeFileSelect = cmd_data[1];
			EventMng_SetEvent_Level(PRINTER_EVENT_PRINT, NULL ,EVENT_LEVEL_0);		
			ProtocolMng_10H(0x10 ,0x01 ,0x00);
			break;
		case CMD_CTRL_SDWRITE://����
			break;
		case CMD_CTRL_LEVELING://����
//			ProtocolMng_CMD_Ctrl_Leveling_GetPosition(cmd_data);
//			EventMng_SetEvent_Level(PRINTER_EVENT_LEVELING, PortEventCbFunc_Leveling ,EVENT_LEVEL_0);
			break;
		case CMD_CTRL_HEAD1_OPERATE:	//��ͷ����
		case CMD_CTRL_HEAD2_OPERATE:	
			ProtocolMng_CMD_Ctrl_HeadOperate(cmd_data);
			break;

		case CMD_CTRL_WARM:	//����
			tem = (cmd_data[3]<<8) | cmd_data[4];				
			//EventMng_SetEvent_Level(PRINTER_EVENT_PRINTCONTINUE, PortEventCbFunc_PrintContinue ,EVENT_LEVEL_0);
			ProtocolMng_Cmd_SetHeadTemp(cmd_data[1] ,cmd_data[2] ,tem);
			break;	
		case CMD_CTRL_PAUSE_PRINT:	//ѡ�е����ż���ͣ��ӡ
			EventMng_SetEvent_Level(PRINTER_EVENT_PAUSE, NULL ,EVENT_LEVEL_0);
			ProtocolMng_10H(0x10 ,0x06 ,0x00);
			break;
		case CMD_CTRL_CONTINUE_PRINT:	//ѡ�е����żܼ�����ӡ
			EventMng_SetEvent_Level(PRINTER_EVENT_CONTINUE, NULL ,EVENT_LEVEL_0);
			ProtocolMng_10H(0x10 ,0x07 ,0x00);
			break;
		case CMD_CRTL_SELECT_FILE:
			g_GcodeFileSelect = cmd_data[1];
		  ProtocolMng_10H(0x10 ,0x08 ,0x00);
		break;

		case CMD_CTRL_PORTALFRAME:	//�л����ż� 09
			switch(cmd_data[1])
			{
//				case 'S':
//					g_PortalFrameSelect = atoi(&cmd_data[2]);
//					EventMng_SetEvent_Level(PRINTER_EVENT_SW_FRAME, NULL ,EVENT_LEVEL_0);		//�л����ż�
					break;
				case 'R':
					EventMng_SetEvent_Level(PRINTER_EVENT_READCONFIG, NULL ,EVENT_LEVEL_0);		//��ȡ�����ļ�
					break;
				case 'H':
					g_GcodeFileSelect = atoi(&cmd_data[2]);
					EventMng_SetEvent_Level(PRINTER_EVENT_PRINT, NULL ,EVENT_LEVEL_0);		//ѡ�е����żܿ�ʼ��ӡ
				  ProtocolMng_10H(0x10 ,0x00 ,0x00);
					break;
				case 'O':
					EventMng_SetEvent_Level(PRINTER_EVENT_STANDBY, NULL ,EVENT_LEVEL_0);		//ѡ�е����ż�ֹͣ��ӡ
				  ProtocolMng_10H(0x10 ,0x00 ,0x00);
					break;
				case 'o':
					g_PortalFrame_Y_Operation = atoi(&cmd_data[2]);
					EventMng_SetEvent_Level(PRINTER_EVENT_STOP_FRAME3_Y, NULL ,EVENT_LEVEL_0);	//ֹͣ�������żܵ�һ��Y��
					break;
				case 'P':	//ѡ�е����ż���ͣ��ӡ
					EventMng_SetEvent_Level(PRINTER_EVENT_PAUSE, NULL ,EVENT_LEVEL_0);
					break;
				case 'C':	//ѡ�е����żܼ�����ӡ
					EventMng_SetEvent_Level(PRINTER_EVENT_CONTINUE, NULL ,EVENT_LEVEL_0);
					break;
				case 'G':	//ѡ�еĴ�ӡ�ļ�
					g_GcodeFileSelect = atoi(&cmd_data[2]);
					break;
			}
			break;		
		default:
			break;
	}
}
/******************************************
���ܣ����մ�ӡ����
�����ߣ�
�������ڣ�2017/5/27
������cmd_data  ��ָ��
����ֵ����
*******************************************/
void ProtocolMng_CMD_13H(u16 len)
{

}
/******************************************
���ܣ�16H
�����ߣ�
�������ڣ�2017/5/27
������cmd_data  ��ָ��
����ֵ����
*******************************************/
void ProtocolMng_CMD_16H()
{	
}
/******************************************
���ܣ�17H
�����ߣ�
�������ڣ�2017/5/27
������cmd_data  ��ָ��
����ֵ����
*******************************************/
void ProtocolMng_CMD_17H(u32 len)
{
	memset(g_ControlBuf,0,ONELINE_GCODE_MAXSIZE+1);		//�������
	memcpy(g_ControlBuf,m_UartDataBuf.Buf+AGREEMENT_DATAHEAD_INDEX + AGREEMENT_ID_SIZE,len-1);
	EventMng_SetEvent_Level(PRINTER_EVENT_CONTROL, PortEventCbFunc_Control ,EVENT_LEVEL_0);
}
/******************************************
���ܣ���ý������ݵ�ID
�����ߣ�
�������ڣ�2017/5/27
������cmd_data  ��ָ��
����ֵ����
*******************************************/
u32 ProtocolMng_GetID(u8 *message_id)
{
	u32 retid = message_id[0]<<24 | message_id[1]<<16 | message_id[2]<<8 | message_id[3];
	return retid;
}
/******************************************
���ܣ�ͨѶACK����
�����ߣ�liu-cy
�������ڣ�2016/11/17
������cak��λ�����ص�ACK
����ֵ����
*******************************************/
void ProtocolMng_CMD_ACK(u8 *cak ,u8 len)
{
	// Send_Data resend_data;
	g_PortCtrl.AckRecord.timeout_repe_count = 0;
	if(IsErrorFlag(ERROR_COM) == True)
	{
		ErrorMng_ResetError(ERROR_COM);
	}
	if(cak[0] == ACK)
	{
//		if(ProtocolMng_FinedWaitOut(m_WaitAck_ID) != True)
//		{
//			//�쳣
//		}
		m_WaitReceive = False;
		memset(&g_PortCtrl.AckRecord.AckRepeBuf ,0 ,SENDMAXLEN);
		g_PortCtrl.AckRecord.nak_repe_count = 0;
	}
	else if(cak[0] == NAK)
	{
		g_PortCtrl.AckRecord.nak_repe_count++;
		if((HAL_GetTick() - g_PortCtrl.AckRecord.repetime)>1000)
		{
//			ProtocolMng_GetCurrentWaitBuf(m_WaitAck_ID ,resend_data);
//			QUEUE_PacketIn(&g_PortCtrl.TxQueue, m_CurrentWaitAckBuf.SendData, m_CurrentWaitAckBuf.SendDataLen);
			g_PortCtrl.AckRecord.repetime = HAL_GetTick();
		}		
		if(g_PortCtrl.AckRecord.nak_repe_count >= PROTOCOL_RESEND_COUNT)
		{		
//			ProtocolMng_FinedWaitOut(m_WaitAck_ID);
			g_PortCtrl.AckRecord.nak_repe_count	= 0;
			m_WaitReceive = False;
			ErrorMng_SetError( ERROR_COM );//ͨѶ�쳣
//			ProtocolMng_InitPort(LINK_PORT_COM);
		}
	}
}
/******************************************
���ܣ�wifiͨ��״̬,�������ݴ���
�����ߣ�liu-cy
�������ڣ�2016/11/17
��������
����ֵ����
*******************************************/
void ProtocolMng_DataParse(u8 *buf, u32 len)
{
	u8 *data = &buf[AGREEMENT_ID_SIZE];	
	// m_CurrentMessage_ID = ProtocolMng_GetID(buf);	
//	if(IsErrorFlag(ERROR_BREAK) == True)
//	{
//		ErrorMng_ResetError(ERROR_BREAK);
//	}
	if(IsErrorFlag(ERROR_COM) == True)
	{
		ErrorMng_ResetError(ERROR_COM);
	}	
	if(data[0]!=0xff)
	{
		//ProtocolMng_ACK();	
	}
	switch(data[0])//�ܿ���ָ��
	{
		case CMD_CTRL://�������
			m_CmdId[CMD_CTRL] = m_CurrentMessage_ID;
			ProtocolMng_CMD_10H(&data[1]);
			break;	
		case CMD_GET_PRINTSTATE://��ȡ��ӡ״̬
			m_CmdId[CMD_GET_PRINTSTATE] = m_CurrentMessage_ID;
			ProtocolMng_CMD_11H(&data[0] ,len);
			break;
		case CMD_GET_VERSION://�汾�Ż�ȡ
			m_CmdId[CMD_GET_VERSION] = m_CurrentMessage_ID;
			ProtocolMng_12H();
			break;
		case CMD_RECEIVE_PRINTDATA://��ӡ���ݴ���
			m_CmdId[CMD_RECEIVE_PRINTDATA] = m_CurrentMessage_ID;
			ProtocolMng_CMD_13H(len);	
			break;
		case CMD_GET_PROGRAMSTATE://��ȡ����״̬
			m_CmdId[CMD_GET_PROGRAMSTATE] = m_CurrentMessage_ID;
			ProtocolMng_15H(0);//��ʱ��һ������
			break;
//		case CMD_UPGRADE://��ʼ����
//			m_CmdId[CMD_UPGRADE] = m_CurrentMessage_ID;
//			ProtocolMng_CMD_16H();//��OK�����������������
//			break;
		case CMD_AXISCONTROL://�������
			m_CmdId[CMD_AXISCONTROL] = m_CurrentMessage_ID;
			ProtocolMng_CMD_17H(len);	
			break;
		case CMD_BUZZER://������
			// m_CmdId[CMD_BUZZER] = m_CurrentMessage_ID;
			// ProtocolMng_CMD_Config(&data[1]);
			break;		
		case CMD_ACK://ACK����
			ProtocolMng_CMD_ACK(&data[1] ,len);	
			break;		
		default:break;		
	}
}

/******************************************
���ܣ���ʼ�����յĶ˿�
�����ߣ�liu-cy
�������ڣ�2016/11/17
��������
����ֵ����
*******************************************/
//485
void ProtocolMng_PortSend(void)
{	
	u8 buf[TX_BUF_SIZE];
	u32 queue_len;
		queue_len = QUEUE_PacketLengthGet(&g_PortCtrl.TxQueue);
		if(queue_len > 0)
		{
			QUEUE_PacketOut(&g_PortCtrl.TxQueue, buf, queue_len);
			sendto(0, buf, queue_len, net_work.desip, DHCP_SERVER_PORT);
			RS485_TX(buf, queue_len);		//����
		}
}	
//USB-UART
//void ProtocolMng_PortSend(void)
//{	
//	u8 buf[TX_BUF_SIZE];
//	u32 queue_len;
//	if(USBD_OK == USB_GetTxState())	//����״̬
//	{
//		queue_len = QUEUE_PacketLengthGet(&g_PortCtrl.TxQueue);
//		if(queue_len > 0)
//		{
//			QUEUE_PacketOut(&g_PortCtrl.TxQueue, buf, queue_len);
//			CDC_Transmit_HS(buf, queue_len);		//����

//		}
//	}
//}	


void ProtocolMng_PortSendMng( void )
{
	bool inidok = False;
	int len = 0;
	Send_Data data;
	if((m_WaitReceive == False) && QUEUE_StructCountGet(&g_PortCtrl.SendDataQueue) != 0)//��������
	{ 
		len = QUEUE_StructOut(&g_PortCtrl.SendDataQueue, &data, 1);	
		if(0 == len)
		{
			return;
		}	
		if(data.wait == True)//�˴η�����������Ҫ�ȴ�����
		{
			m_WaitReceive = True;
			m_CurrentWaitAckBuf = data;
			m_WaitAck_ID = data.id;
			g_PortCtrl.RcvTimeOutCnt = HAL_GetTick();
		}		
		QUEUE_PacketIn(&g_PortCtrl.TxQueue, data.SendData, data.SendDataLen);
	}
	ProtocolMng_PortSend();
}
/******************************************
���ܣ�����ͨ�Ž��մ���
�����ߣ�liu-cy
�������ڣ�2016/11/17
��������
����ֵ����
*******************************************/
void ProtocolMng_Pro(void)
{	
	//485
	int read_size = RS485_RX(m_UartDataBuf.Buf, RX_BUF_SIZE);
  if(read_size > 0)
	{
		ProtocolMng_DataParse(m_UartDataBuf.Buf+AGREEMENT_ID_INDEX, read_size);		//����ͨ��ʱ���ݴ���	
	}
	ProtocolMng_PortSendMng();
	


	//W5500
	uint16_t get_sn = getSn_RX_RSR(0);		
	if(get_sn)
	{
			int16_t len = recvfrom(0, net_work.read_buffer, READ_BUFFER_SIZE, net_work.vsip, net_work.dport);
			if(len)
			{
					ProtocolMng_DataParse(net_work.read_buffer+AGREEMENT_ID_INDEX, len);	
			}
	}
	
	if(send_flag == 1)
	{
		ProtocolMng_11H(11, 25);
		send_flag = 0;
	}

	
}

/******************************************
���ܣ���ʼ�����յĶ˿�
�����ߣ�liu-cy
�������ڣ�2016/11/17
��������
����ֵ����
*******************************************/
void ProtocolMng_InitPort(LINK_PORT port)
{
	QUEUE_PacketCreate(&g_PortCtrl.RxQueue, g_PortCtrl.RxBuf, sizeof(g_PortCtrl.RxBuf));		//��ʼ�����ն���
	QUEUE_PacketCreate(&g_PortCtrl.TxQueue, g_PortCtrl.TxBuf, sizeof(g_PortCtrl.TxBuf));		//��ʼ�����Ͷ���
	QUEUE_StructCreate(&g_PortCtrl.SendDataQueue, g_PortCtrl.SendDataBuf, sizeof(g_PortCtrl.SendDataBuf), sizeof(Send_Data));
	
	g_PortCtrl.linkport = port;		//���ӵĶ˿�
	g_PortCtrl.status = PORT_STATUS_WAIT;	//��ǰ�˿ڵ�״̬
	g_PortCtrl.HeartBeatTimeOutCnt = 0;		
	g_PortCtrl.RcvTimeOutCnt = 0;			//���ճ�ʱ
	g_PortCtrl.AckRecord.repetime = 0;
	m_CurrentMessage_ID = 0;
	memset(m_RetryBuf ,0 ,sizeof(Send_Data)*SENDQUEUELEN);
	memset(&m_CurrentWaitAckBuf ,0 ,sizeof(Send_Data));
}


bool ProtocolMng_DataReceiveOneFrame(void)
{
	u16 readSize = 0;
	bool dataCheck = False;
	bool ret = False;
	switch (m_RecvStage)
	{
		case RECV_GETSTART:
			m_RecvStage = RECV_FINDSTART1;
		case RECV_FINDSTART1:
			readSize = ProtocolMng_RxRead(&m_UartDataBuf.Buf[0], 1);
			if((readSize == 1) && (m_UartDataBuf.Buf[0] == AGREEMENT_HEAD_H))
			{
				m_RecvStage = RECV_FINDSTART2;
				m_UartDataBuf.Buf[0] = AGREEMENT_HEAD_H;
				m_UartDataBuf.Len = 1;			
			}
			else
			{
				break;
			}
		case RECV_FINDSTART2:
			readSize = ProtocolMng_RxRead((m_UartDataBuf.Buf + 1), 1);
			if(readSize == 1)
			{
				if(AGREEMENT_HEAD_L == m_UartDataBuf.Buf[1])
				{
					m_UartDataBuf.Len += readSize;				
					m_RecvStage = RECV_FINDLEN;
					m_SizeNeedtoRead = 2;
				}
				else if(AGREEMENT_HEAD_H == m_UartDataBuf.Buf[1])
				{
					break;
				}
				else
				{
					m_RecvStage = RECV_GETSTART;
					break;
				}
			}
			else
			{
				break;
			}
		case RECV_FINDLEN:
			readSize = ProtocolMng_RxRead(&m_UartDataBuf.Buf[m_UartDataBuf.Len], m_SizeNeedtoRead);
			m_UartDataBuf.Len += readSize;
			m_SizeNeedtoRead -= readSize;
			if(m_SizeNeedtoRead == 0)
			{
				m_SizeNeedtoRead = (m_UartDataBuf.Buf[2] << 8) + m_UartDataBuf.Buf[3];
				m_RecvStage = RECV_RECVING;
			}
			else
			{
				break;
			}
		case RECV_RECVING:
			readSize = ProtocolMng_RxRead(&m_UartDataBuf.Buf[m_UartDataBuf.Len], m_SizeNeedtoRead);
			m_UartDataBuf.Len += readSize;
			m_SizeNeedtoRead -= readSize;
			if(m_SizeNeedtoRead == 0)
			{
				m_RecvStage = RECV_END;
			}
			else
			{
				break;
			}
		case RECV_END:					
			dataCheck = ProtocolMng_DataCheck(&m_UartDataBuf.Buf[0],m_UartDataBuf.Len);
			if(dataCheck == True)
			{
				ret = True;
			}
			else 
			{
				ProtocolMng_NAK();				
				ret = False;
			}
			m_RecvStage = RECV_GETSTART;			
			break;
		default:
			m_RecvStage = RECV_GETSTART;
			break;
	}
	return ret;
}

void Logger_string(char *string)
{
	ProtocolMng_TxWrite(string, strlen(string));
}
char m_LogBuff[1024];
void Logger_long(long num)
{
	sprintf(m_LogBuff, "%d", num);
	Logger_string(m_LogBuff);
}
void Logger_float(float num)
{
	sprintf(m_LogBuff, "%.1f", num);
	Logger_string(m_LogBuff);
}
