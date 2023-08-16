#ifndef _MITSUBISHI_H
#define _MITSUBISHI_H
#include "usart.h"	

void extractNumbers(const char* data, int dataLength, char* resultArray) ;
int addHexNumbers(void);
extern char resultArray[40];
void IScommandvalid(void);
u16 getdataadress(void);
u16 getdataCount(void);
void processResponse( u16 dataAddress, u16 dataCount)  ;

#endif

