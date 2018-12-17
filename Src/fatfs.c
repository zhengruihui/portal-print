/**
  ******************************************************************************
  * @file   fatfs.c
  * @brief  Code for fatfs applications
  ******************************************************************************
  * This notice applies to any and all portions of this file
  * that are not between comment pairs USER CODE BEGIN and
  * USER CODE END. Other portions of this file, whether 
  * inserted by the user or by software development tools
  * are owned by their respective copyright owners.
  *
  * Copyright (c) 2017 STMicroelectronics International N.V. 
  * All rights reserved.
  *
  * Redistribution and use in source and binary forms, with or without 
  * modification, are permitted, provided that the following conditions are met:
  *
  * 1. Redistribution of source code must retain the above copyright notice, 
  *    this list of conditions and the following disclaimer.
  * 2. Redistributions in binary form must reproduce the above copyright notice,
  *    this list of conditions and the following disclaimer in the documentation
  *    and/or other materials provided with the distribution.
  * 3. Neither the name of STMicroelectronics nor the names of other 
  *    contributors to this software may be used to endorse or promote products 
  *    derived from this software without specific written permission.
  * 4. This software, including modifications and/or derivative works of this 
  *    software, must execute solely and exclusively on microcontroller or
  *    microprocessor devices manufactured by or for STMicroelectronics.
  * 5. Redistribution and use of this software other than as permitted under 
  *    this license is void and will automatically terminate your rights under 
  *    this license. 
  *
  * THIS SOFTWARE IS PROVIDED BY STMICROELECTRONICS AND CONTRIBUTORS "AS IS" 
  * AND ANY EXPRESS, IMPLIED OR STATUTORY WARRANTIES, INCLUDING, BUT NOT 
  * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR A 
  * PARTICULAR PURPOSE AND NON-INFRINGEMENT OF THIRD PARTY INTELLECTUAL PROPERTY
  * RIGHTS ARE DISCLAIMED TO THE FULLEST EXTENT PERMITTED BY LAW. IN NO EVENT 
  * SHALL STMICROELECTRONICS OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
  * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
  * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, 
  * OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF 
  * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING 
  * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
  * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************
  */

#include "fatfs.h"

uint8_t retSD;    /* Return value for SD */
char SD_Path[4];  /* SD logical drive path */

/* USER CODE BEGIN Variables */
#include "de_type.h"
#include "macros.h"

#define SD_CARD_READ_RETRY_COUNT			3		//读失败重试次数
#define FILE_OPEN_RETRY_COUNT				3		//打开文件重试次数

char USBH_Path[4];  /* SD logical drive path */

FATFS UDISK_FS;
FATFS SDCARD_FS;

/* USER CODE END Variables */    

void MX_FATFS_Init(void) 
{
  /*## FatFS: Link the SD driver ###########################*/
  retSD = FATFS_LinkDriver(&SD_Driver, SD_Path);

  /* USER CODE BEGIN Init */
  /* additional user code for init */     
  /* USER CODE END Init */
}

/**
  * @brief  Gets Time from RTC 
  * @param  None
  * @retval Time in DWORD
  */
DWORD get_fattime(void)
{
  /* USER CODE BEGIN get_fattime */
  return 0;
  /* USER CODE END get_fattime */  
}

/* USER CODE BEGIN Application */

unsigned  char FATFS_MountUdisk(void)
{
	unsigned  char res;
	res = f_mount(&UDISK_FS,USBH_Path,0);
	return res;
}
unsigned  char FATFS_UmountUdisk(void)
{
	unsigned  char res;
	res = f_mount(&UDISK_FS,NULL,0);
	return res;
}
unsigned  char FATFS_MountSDdisk(void)
{
	unsigned  char res = FR_DISK_ERR;
	if(MSD_OK == BSP_SD_Init())
	{
		res = f_mount(&SDCARD_FS,SD_Path,0);
	}
	return res;
}
unsigned  char FATFS_UmountSDdisk(void)
{
	unsigned  char res;
	res = f_mount(NULL,SD_Path,0);
	return res;
}

FRESULT FATFS_OpenFile(FIL *this, const TCHAR *path, BYTE mode)
{
	FRESULT res;
	int tryCount;
	
	for(tryCount = 0; tryCount < FILE_OPEN_RETRY_COUNT; tryCount++)
	{
		DISABLE_TIMER_INT
		res = f_open(this, path, mode);
		ENABLE_TIMER_INT
		
		if(res == FR_OK)	break;
	}
	return res;
}


/******************************************
功能：从目标文件读数捿
作成者：cao-dl
作成日期＿2015/2/2
参数:
	r_data:读出的数捿
	length:数据长度
返回值：
	True:  读取成功
	False：读取失贿
*******************************************/
char *FATFS_GetOneLine( FIL *this, char *r_data)
{
	return f_gets(r_data, 1024, this);
}

/******************************************
功能：得到磁盘剩余容釿
作成者：liu-cy
作成日期＿2017/3/31
参数:
	drv:磁盘编号("0:"/"1:")
返回值：空闲容量
*******************************************/
u32 FATFS_GetFree(u8 *drv)
{
	FATFS *fs1;
	static u8 res;
    u32 fre_clust=0, fre_sect=0;
    //得到磁盘空闲簇数釿
    res =(u32)f_getfree((const TCHAR*)drv, (DWORD*)&fre_clust, &fs1);
    if(res==0)
	{
	    fre_sect=fre_clust*fs1->csize;			//得到空闲扇区敿
#if _MAX_SS!=512				  				//扇区大小不是512字节,则转换为512字节
		fre_sect*=fs1->ssize/512;
#endif	  
 	}
	return (fre_sect>>1);//单位为KB 
}

/******************************************
功能：目标文件是否存圿
作成者：cao-dl
作成日期＿2015/2/2
参数:
	filepath:文件路径
返回值：
	True:  文件存在
	False：文件不存在
*******************************************/
bool FATFS_IsExist(char * filepath )
{
	bool	bIsExist;
	FILINFO finfo;
	FRESULT res;
	bIsExist = False;
	res = f_stat( (const TCHAR *)filepath, &finfo );
	//该文件属性可诿
	if( res == FR_OK )
	{
		bIsExist = True;
	}
	return bIsExist;
}

/******************************************
功能：判断是否读到文件尾
作成者：liu-cy
作成日期＿2016/10/24
参数:
返回值：
	True:  已到结尾
	False：未到结尿
*******************************************/
bool FATFS_IsReadEnd(FIL *this)
{
	if(this->fptr == this->fsize)	//文件指针已到朿吿
	{
		return True;			//返回读到朿吿
	}
	return False;
}

/******************************************
功能：文件指针移到最后一行之剿(用于判断朿后一行是否为M25)
作成者：liu-cy
作成日期＿2016/10/25
参数:无
返回值：无
*******************************************/
void FATFS_LseekEnd(FIL *this)
{
	// f_lseek(this, this->fsize - LAST_LINE_LENGTH);
}

/******************************************
功能：文件指针移到开姿
作成者：liu-cy
作成日期＿2016/3/30
参数:无
返回值：无
*******************************************/
void FATFS_LseekStart(FIL *this)
{
	f_lseek(this, 0);
}

/******************************************
功能：文件指针定位到指定的位罿
作成者：liu-cy
作成日期＿2016/10/25
参数:无
返回值：旿
*******************************************/
void FATFS_Lseek(FIL *this, DWORD fsize)
{
	f_lseek(this, fsize);
}

/******************************************
功能：获取文件濻字芿
作成者：liu-cy
作成日期＿2016/3/30
参数:无
返回值：旿
*******************************************/
DWORD FATFS_GetFileSize(FIL *this)
{
	return this->fsize;
}

/******************************************
功能：获取文件濻字芿
作成者：liu-cy
作成日期＿2016/3/30
参数:无
返回值：旿
*******************************************/
DWORD FATFS_GetFileIndex(FIL *this)
{
	return this->fptr;
}

/******************************************
功能：从目标文件读数捿
作成者：cao-dl
作成日期＿2015/2/2
参数:
	r_data:读出的数捿
	length:数据长度
返回值：
	True:  读取成功
	False：读取失贿
*******************************************/
FRESULT FATFS_ReadFile( FIL *this, char *r_data, u32 btr, u32 *br)
{
	FRESULT   myres;
	u8	i = 0;				//重读次数

	//读取文件
	while(i++ < SD_CARD_READ_RETRY_COUNT)	
	{
		CRITICAL_SECTION_START
		myres = f_read( this, r_data, btr, br );
		CRITICAL_SECTION_END
		if(myres == FR_OK)	//读取成功
		{
			break;		//读取成功逿凿
		}
	}
	return myres;
}

/******************************************
功能：目标文件删陿
作成者：cao-dl
作成日期＿2015/2/2
参数:
	filepath:文件路径
返回值：
	True:  文件删除成功
	False：文件删除失贿
*******************************************/
bool FATFS_DeleteFile( char *filepath )
{
	FRESULT   res;
	bool	  bIsOK;
	bIsOK = True;
	//文件存在，删陿
	if( FATFS_IsExist(filepath) )
	{
		res = f_unlink( (const TCHAR *)filepath );
		if( res != FR_OK )
		{
			bIsOK = False;		
		}
	}
	return bIsOK;
}

/******************************************
功能：向目标文件写数捿
作成者：cao-dl
作成日期＿2015/2/2
参数:
	w_data:写入文件中的数据
	length:数据长度
返回值：
	True:  写入成功
	False：写入失贿
*******************************************/
FRESULT FATFS_WriteFile( FIL *this, u8 *w_data, u32 length )
{
	static FRESULT   myres;
	UINT writesize = 0;
	UINT ActualSize = 0;
	//写入文件
	while(ActualSize < length)
	{
		if(length - ActualSize > 512)
		{
			myres = f_write(this,w_data+ActualSize,512,&writesize);
		}
		else
		{
			myres = f_write(this,w_data+ActualSize,length - ActualSize,&writesize);
		}
		if(FR_OK == myres)
		{
			ActualSize+= writesize;
		}
		else
		{
			break;
		}
	}
	return myres;
}

/******************************************
功能：关闭文仿
作成者：liu-cy
作成日期＿2016/3/30
参数:无
返回值：旿
*******************************************/
void FATFS_CloseFile(FIL *this)
{
	f_close(this);
}
/* USER CODE END Application */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
