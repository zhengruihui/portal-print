#ifndef PORTOCOL_MNG_H
#define PORTOCOL_MNG_H
#include "de_type.h"
#include "Queue.h"
#include "PrinterCtrl.h"
#include "globals.h"

extern u8 cmdhead;
extern u16 cmdtemp,cmdspeed;
extern void (*cmdCbFunc[EXTRUDER_MAX])(u8);		//料盘操作回调函数

#define		AGREEMENT_HEAD_H			0xAA
#define		AGREEMENT_HEAD_L			0x55
#define 	SENDMAXLEN	128		//发送数据最大字节数

#define		SENDQUEUELEN		8//最大发送队列存储个数

#define ACK 0x00
#define NAK 0x01

#define		SERIALDEBUG					//调试串行打印宏定义,定义后,printf函数会输出到串口


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
#define PROTOCOL_TIME_OUT 2000	//通讯超时1S ，重发时间间隔

//通信连接状态
typedef enum 
{
	LINK_PORT_COM,		//虚拟串口连接
	LINK_PORT_WIFI,		//wifi连接
	LINK_PORT_MAX,
}LINK_PORT;

//数据接收状态
typedef enum 
{
	PORT_STATUS_WAIT,		//等待接收
	PORT_STATUS_RCV,		//接收中
	PORT_STATUS_ACK_DEFEAT	//ACK连续校验失败
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
	uint8_t RxBuf[RX_BUF_SIZE];		//接收缓冲
	uint8_t TxBuf[TX_BUF_SIZE];		//发送缓冲
	QUEUE8_t RxQueue;			//接收队列
	QUEUE8_t TxQueue;			//发送队列
	Send_Data SendDataBuf[SENDQUEUELEN];//发送队列
	QUEUE_STRUCT_t	SendDataQueue;
	LINK_PORT linkport;			//当前连接
	PORT_STATUS status;			//当前状态
	u32 RcvTimeOutCnt;			//接收数据通信超时
	u32 HeartBeatTimeOutCnt;	//心跳超时时间
	AckRepetition AckRecord;//记录需要重发的ACK数据
	bool Check_HeartBeat;	//是否检测心跳超时
	bool CanSent;			//可以发送数据
}PortCtrl;

extern PortCtrl g_PortCtrl;

extern int heart_count;
extern int send_flag;	
/*******************************************
	输出接口
********************************************/
//初始化
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
