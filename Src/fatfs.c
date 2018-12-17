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

#define SD_CARD_READ_RETRY_COUNT			3		//��ʧ�����Դ���
#define FILE_OPEN_RETRY_COUNT				3		//���ļ����Դ���

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
���ܣ���Ŀ���ļ�������
�����ߣ�cao-dl
�������ڣ�2015/2/2
����:
	r_data:����������
	length:���ݳ���
����ֵ��
	True:  ��ȡ�ɹ�
	False����ȡʧ��
*******************************************/
char *FATFS_GetOneLine( FIL *this, char *r_data)
{
	return f_gets(r_data, 1024, this);
}

/******************************************
���ܣ��õ�����ʣ�����Y
�����ߣ�liu-cy
�������ڣ�2017/3/31
����:
	drv:���̱��("0:"/"1:")
����ֵ����������
*******************************************/
u32 FATFS_GetFree(u8 *drv)
{
	FATFS *fs1;
	static u8 res;
    u32 fre_clust=0, fre_sect=0;
    //�õ����̿��д����Y
    res =(u32)f_getfree((const TCHAR*)drv, (DWORD*)&fre_clust, &fs1);
    if(res==0)
	{
	    fre_sect=fre_clust*fs1->csize;			//�õ�����������
#if _MAX_SS!=512				  				//������С����512�ֽ�,��ת��Ϊ512�ֽ�
		fre_sect*=fs1->ssize/512;
#endif	  
 	}
	return (fre_sect>>1);//��λΪKB 
}

/******************************************
���ܣ�Ŀ���ļ��Ƿ��]
�����ߣ�cao-dl
�������ڣ�2015/2/2
����:
	filepath:�ļ�·��
����ֵ��
	True:  �ļ�����
	False���ļ�������
*******************************************/
bool FATFS_IsExist(char * filepath )
{
	bool	bIsExist;
	FILINFO finfo;
	FRESULT res;
	bIsExist = False;
	res = f_stat( (const TCHAR *)filepath, &finfo );
	//���ļ����Կ���
	if( res == FR_OK )
	{
		bIsExist = True;
	}
	return bIsExist;
}

/******************************************
���ܣ��ж��Ƿ�����ļ�β
�����ߣ�liu-cy
�������ڣ�2016/10/24
����:
����ֵ��
	True:  �ѵ���β
	False��δ������
*******************************************/
bool FATFS_IsReadEnd(FIL *this)
{
	if(this->fptr == this->fsize)	//�ļ�ָ���ѵ��c��
	{
		return True;			//���ض����c��
	}
	return False;
}

/******************************************
���ܣ��ļ�ָ���Ƶ����һ��֮��(�����жϖc��һ���Ƿ�ΪM25)
�����ߣ�liu-cy
�������ڣ�2016/10/25
����:��
����ֵ����
*******************************************/
void FATFS_LseekEnd(FIL *this)
{
	// f_lseek(this, this->fsize - LAST_LINE_LENGTH);
}

/******************************************
���ܣ��ļ�ָ���Ƶ�����
�����ߣ�liu-cy
�������ڣ�2016/3/30
����:��
����ֵ����
*******************************************/
void FATFS_LseekStart(FIL *this)
{
	f_lseek(this, 0);
}

/******************************************
���ܣ��ļ�ָ�붨λ��ָ����λ�Z
�����ߣ�liu-cy
�������ڣ�2016/10/25
����:��
����ֵ���J
*******************************************/
void FATFS_Lseek(FIL *this, DWORD fsize)
{
	f_lseek(this, fsize);
}

/******************************************
���ܣ���ȡ�ļ��S���e
�����ߣ�liu-cy
�������ڣ�2016/3/30
����:��
����ֵ���J
*******************************************/
DWORD FATFS_GetFileSize(FIL *this)
{
	return this->fsize;
}

/******************************************
���ܣ���ȡ�ļ��S���e
�����ߣ�liu-cy
�������ڣ�2016/3/30
����:��
����ֵ���J
*******************************************/
DWORD FATFS_GetFileIndex(FIL *this)
{
	return this->fptr;
}

/******************************************
���ܣ���Ŀ���ļ�������
�����ߣ�cao-dl
�������ڣ�2015/2/2
����:
	r_data:����������
	length:���ݳ���
����ֵ��
	True:  ��ȡ�ɹ�
	False����ȡʧ��
*******************************************/
FRESULT FATFS_ReadFile( FIL *this, char *r_data, u32 btr, u32 *br)
{
	FRESULT   myres;
	u8	i = 0;				//�ض�����

	//��ȡ�ļ�
	while(i++ < SD_CARD_READ_RETRY_COUNT)	
	{
		CRITICAL_SECTION_START
		myres = f_read( this, r_data, btr, br );
		CRITICAL_SECTION_END
		if(myres == FR_OK)	//��ȡ�ɹ�
		{
			break;		//��ȡ�ɹ��T��
		}
	}
	return myres;
}

/******************************************
���ܣ�Ŀ���ļ�ɾ�
�����ߣ�cao-dl
�������ڣ�2015/2/2
����:
	filepath:�ļ�·��
����ֵ��
	True:  �ļ�ɾ���ɹ�
	False���ļ�ɾ��ʧ��
*******************************************/
bool FATFS_DeleteFile( char *filepath )
{
	FRESULT   res;
	bool	  bIsOK;
	bIsOK = True;
	//�ļ����ڣ�ɾ�
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
���ܣ���Ŀ���ļ�д����
�����ߣ�cao-dl
�������ڣ�2015/2/2
����:
	w_data:д���ļ��е�����
	length:���ݳ���
����ֵ��
	True:  д��ɹ�
	False��д��ʧ��
*******************************************/
FRESULT FATFS_WriteFile( FIL *this, u8 *w_data, u32 length )
{
	static FRESULT   myres;
	UINT writesize = 0;
	UINT ActualSize = 0;
	//д���ļ�
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
���ܣ��ر��ķ�
�����ߣ�liu-cy
�������ڣ�2016/3/30
����:��
����ֵ���J
*******************************************/
void FATFS_CloseFile(FIL *this)
{
	f_close(this);
}
/* USER CODE END Application */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
