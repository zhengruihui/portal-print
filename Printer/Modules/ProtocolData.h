/********************** (C) COPYRIGHT 2017 DediBot ******************************
  * @file    ProtocolData.h
  * @author  Dedibot
  * @version V
  * @date    2017-05-24
  * @brief   ͨ�����ݱ���
*********************************************************************************/
#ifndef PORTOCOLDATA_H
#define PORTOCOLDATA_H


extern char g_ControlBuf[ONELINE_GCODE_MAXSIZE+1];

/* ���ż�ѡ�� */
extern u8 g_PortalFrameSelect;

/* ��ӡ�ļ����ѡ�� */
extern u8 g_GcodeFileSelect;

/* �������ż�Y��ѡ�� */
extern u8 g_PortalFrame_Y_Operation;

#endif	/* PORTOCOLDATA_H */
