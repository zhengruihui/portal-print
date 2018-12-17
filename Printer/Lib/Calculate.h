#ifndef CALCULATE_H
#define CALCULATE_H
#include "de_type.h"

// Macros to contrain values
#define NOLESS(v,n) do{ if (v < n) v = n; }while(0)
#define NOMORE(v,n) do{ if (v > n) v = n; }while(0)

//ͨ����ѧ��������
#define limit(value, min, max)	((value < max)?((value < min)? min: value): max)
#define max(x, y)				((x >= y)? x: y)
#define min(x, y)				((x <= y)? x: y)
#define square(x)				((x)*(x))

//λ��������
#define _BV(n) 					(1<<(n))
#define TEST(n,b) 				(((n)&_BV(b))!=0)		//�ж�ĳЩλ�Ƿ�Ϊ1
#define SETBIT(n,b,v) 			((n) ^= ((-v)^(n)) & (_BV(b)))		//����ĳλ��ֵ
#define SBI(n,b) 				((n) |= _BV(b))			//��ĳЩλ��1
#define CBI(n,b) 				((n) &= ~_BV(b))		//��ĳЩλ��0

#define PI						(3.1415926)


//����ͨ��У��
extern u16 GetCRC16(u8* pchMsg, u16 wDataLen);
#endif
