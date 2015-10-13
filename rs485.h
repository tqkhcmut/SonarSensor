#ifndef _rs485_h_
#define _rs485_h_

#include "stm8s.h"

#ifndef BYTE_FORMAT

#define BIN     0
#define OCT     1
#define DEC     2
#define HEX     3
typedef unsigned char BYTE_FORMAT;

#endif

void RS485_Init(void);
void RS485_SendChar(char c);
void RS485_SendStr(char Str[]);
void RS485_SendNum(int num);
void RS485_SendFloat(float num);
void RS485_SendByte(uint8_t b, BYTE_FORMAT f);
int RS485_Available(void);
int RS485_GetData(char * buffer, int len);


#endif

