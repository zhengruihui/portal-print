#ifndef CALCULATE_H
#define CALCULATE_H
#include "de_type.h"

// Macros to contrain values
#define NOLESS(v,n) do{ if (v < n) v = n; }while(0)
#define NOMORE(v,n) do{ if (v > n) v = n; }while(0)

//通用数学函数定义
#define limit(value, min, max)	((value < max)?((value < min)? min: value): max)
#define max(x, y)				((x >= y)? x: y)
#define min(x, y)				((x <= y)? x: y)
#define square(x)				((x)*(x))

//位操作定义
#define _BV(n) 					(1<<(n))
#define TEST(n,b) 				(((n)&_BV(b))!=0)		//判断某些位是否为1
#define SETBIT(n,b,v) 			((n) ^= ((-v)^(n)) & (_BV(b)))		//设置某位的值
#define SBI(n,b) 				((n) |= _BV(b))			//将某些位置1
#define CBI(n,b) 				((n) &= ~_BV(b))		//将某些位置0

#define PI						(3.1415926)


//串行通信校验
extern u16 GetCRC16(u8* pchMsg, u16 wDataLen);
#endif
