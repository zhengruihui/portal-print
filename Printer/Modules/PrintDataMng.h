#ifndef PRINT_DATA_MNG_H
#define PRINT_DATA_MNG_H
#include "de_type.h"
#include "Queue.h"
#include "fatfs.h"


#define PRINT_DATA_BUF_SIZE	 (1024*8+64)
// #define ONEPARME_DATA_SIZE	1024*15

#define QUEUEEMPTY_TIME	20000	//打印中缓存空异常时间

typedef enum 
{
	PRINTDATA_1,
	PRINTDATA_2,
	PRINTDATA_3,
	
	PRINTDATA_MAX
	
}PRINTDATA_SEL;

typedef enum
{
	DATASOURCE_COM,
	DATASOURCE_LOCAL,
	
	DATASOURCE_MAX
}DATASOURCE_TYPE;

typedef struct
{
	DATASOURCE_TYPE dataSourceType;
	
	bool isLocalStart;
	char *filename;
	FIL prtFile;
	int diskErrCount;
	int intErrCount;
	long totalReadSize;
	
	u32 printByteSkip_Count;
	u32 printDataFarme;
	QUEUE8_t printDataQueue;
	uint8_t  printDataBuf[PRINT_DATA_BUF_SIZE];
	u32 printData_RxIndex;	   //缓冲区地址读取索引
	u32 queueEmptyTime;
}PRINTDATAMNG;

// extern PRINTDATAMNG g_PrintDataMng[PRINTDATA_MAX];

extern u16 PrintDataMng_GetQueueAvalilableSize( PRINTDATAMNG *this );
extern void PrintDataMng_ReceDataToQueue( PRINTDATAMNG *this, char* buf ,u16 len);
extern bool PrintDataMng_QueueIsFull( PRINTDATAMNG *this ) ;
extern bool PrintDataMng_GetOneLine( PRINTDATAMNG *this, char *r_data, u8* length );
extern void PrintDataMng_SetSkipByte(PRINTDATAMNG *this, u32 count);
extern void PrintDataMng_NoPrintDataCheck(PRINTDATAMNG *this);

extern bool PrintDataMng_LocalStart(PRINTDATAMNG *this);
extern void PrintDataMng_SetDataSource(PRINTDATAMNG *this, DATASOURCE_TYPE type);
extern void PrintDataMng_SetDataFileName(PRINTDATAMNG *this, char *filename);
extern void PrintDataMng_Stop(PRINTDATAMNG *this);
extern u32 PrintDataMng_GetTotalSize(PRINTDATAMNG *this);

extern void PrintDataMng_DataClear(PRINTDATAMNG *this);
extern void PrintDataMng_Init(PRINTDATAMNG *this);
extern void PrintDataMng_Pro(PRINTDATAMNG *this);

#endif
