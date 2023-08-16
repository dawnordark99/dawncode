#include "MITSUBISHI_FX2.h"

#include "usart.h"	
#include <string.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>

// 宏定义，用于将2字节分成4个
#define HIGH_12BYTE(x) (((x)&0xF000) >> 12)
#define HIGH_8BYTE(x) (((x)&0x0F00) >> 8)
#define HIGH_4BYTE(x) (((x)&0x00F0) >> 4)
#define HIGH_0BYTE(x) ((x)&0x000F) 

#define ISprintf 0  //数据是否打印

char resultArray[40]; 
int dataLength =40;
int find_03;
int vaild;

void extractNumbers(const char* data, int dataLength, char* resultArray) //输入是字符用这个
{
    int resultIndex = 0;
    int i = 0;
    int digit1,digit2;
    while (i < dataLength)
	{
        char tempStr[2] = {0}; // 用于存储拼接的两个数字和字符串结尾符 '\0'
            tempStr[0] = data[i];
            tempStr[1] = data[i + 1];
            i += 3; // 跳过拼接的两个数字字符
            digit1 = tempStr[0] - '0';
            digit2 = tempStr[1] - '0';
        // 将拼接或单独的两位数字字符串转换成整数并存储在结果数组中
        resultArray[resultIndex] = digit1 * 16 + digit2;
        resultIndex++;
    }
}

void extractNumbers2(const char* data, int dataLength, char* resultArray) //输入是纯数字用这个
{

    static int i = 0;
	for(i=0;i<dataLength;i++)
	{	
	    resultArray[i] = Rx_DMA_receive[i];
	}
}


// 校验相加
int addHexNumbers(void)
{	
	static int i =1;
    static int sum = 0;
	static int result = 0;
	while(resultArray[i]!= 03)
	{	
 	  sum += resultArray[i];
	  i++;
	}
	result = sum + 03;
	find_03 = i;
	i=1;
	sum=0;

    return result;
}


// 将一个16进制字符转换成对应的整数值
int hexCharToInt(char hexChar) {
    if (hexChar >= '0' && hexChar <= '9')
        return hexChar - '0';
    else if (hexChar >= 'A' && hexChar <= 'F')
        return hexChar - 'A' + 10;
    else if (hexChar >= 'a' && hexChar <= 'f')
        return hexChar - 'a' + 10;

}


// 将16进制数值转换为对应的ASCII码字符
char hex_to_ascii(unsigned int hex_value) {
    if (hex_value >= 0 && hex_value <= 9)
	{
        return hex_value + '0';
    } 
	else if (hex_value >= 10 && hex_value <= 15)
	{
        return hex_value - 10 + 'A';
    } 
}

void IScommandvalid(void) //判断发来的命令是否正确
{

	static int result;
	extractNumbers2((char*)Rx_DMA_receive, dataLength, resultArray);	//第一步将数据放入一个新的数组
	result = addHexNumbers();

#if	ISprintf	// 输出提取出的数字
		for (static int i = 0; i < find_03+3; i++) 
		{
			printf("%02X ", resultArray[i]);
		}
		    printf("\n");
            printf("Sum :%X %d %d %d %d\n", result, (result& 0xF0)>>4,result& 0x0F,hexCharToInt(resultArray[find_03+1]) ,hexCharToInt(resultArray[find_03+2]));	
	        printf("find_03:%02X\n ", find_03);		
#endif	
//命令格式如（表1）：
//          ①                ②              ③                    ④                
// 起始位 命令字          数据地址          数据个数     结束位     校验和				  							    
//STX    CMD        163 162	161	160        161 160  	ETX       161  160
 
//02H	‘ 0 ’30H                                      03H

//① 命令字‘ 0 ’表示从主板读取数据。
//② 数据地址是指读取数据的起始地址。
//③ 数据个数是指从起始地址开始读取数据的个数(取值范围为：01，02，04，08)。
//④ 校验和＝(命令字＋数据地址＋数据个数＋结束位)    取低两位 。

	if((result& 0xF0)>>4 == hexCharToInt(resultArray[find_03+1]) && ( result& 0x0F) == hexCharToInt(resultArray[find_03+2]) )  //校验判断
	{
       vaild =1;
	}
	else  vaild =0 ;
}


u16 getdataadress(void)
{
    static int Adress = 0 ;
	Adress =(hexCharToInt(resultArray[2])<<12) +(hexCharToInt(resultArray[3])<<8) +(hexCharToInt(resultArray[4])<<4) +hexCharToInt(resultArray[5]) ;
//	printf("%d %d %d %d",(hexCharToInt(resultArray[2])<<12) ,( (resultArray[3] - 48) << 8 )&0x0F00,( (resultArray[4] - 48) << 4)&0x00F0,((resultArray[5] - 48 )& 0x000f));
    return Adress;
}

u16 getdataCount(void)   //数据个数计算
{
	    static int Count = 0 ;
		static int i = 6;
		static int sum = 0;

		for(i= 6;i<8;i++)
		{
		sum += hexCharToInt(resultArray[i]);
		}
		
		Count = sum ;
		sum = 0;
		i = 6;
	return Count;
}


// 按要求输出返回指令
void getRequestedData(u16 dataAddress, unsigned char dataCount, unsigned char* responseData)
{
	 unsigned char i = 0;
//************************** 是读取命令的话	*****************************************************************/
	if(resultArray[1] == '0')   
	{    
	 static int sum = 0;
   /***********帧头02*************/		
	 responseData[0] = 02;      
   /**************返回读取数据***********/		
	if (dataCount <2)                                  //因为是将一个字节分成两个再转换成ASCLL码的，我读取的数组是UINT16_T的，两个字节，要分成4个ASCLL码，所以小于两个字节的话就读前两半
	{
        responseData[1] = hex_to_ascii(HIGH_12BYTE(dataStorage[dataAddress ]));
		responseData[2] = hex_to_ascii(HIGH_8BYTE(dataStorage[dataAddress ]));
	}
	else if (dataCount >= 2)
	{
		for (i = 0; i < dataCount/2; i++)    
		{
			responseData[i*4+1] = hex_to_ascii(HIGH_12BYTE(dataStorage[dataAddress + i]));
			responseData[i*4+2] = hex_to_ascii(HIGH_8BYTE(dataStorage[dataAddress + i]));	
			responseData[i*4+3] = hex_to_ascii(HIGH_4BYTE(dataStorage[dataAddress + i]));
			responseData[i*4+4] = hex_to_ascii(HIGH_0BYTE(dataStorage[dataAddress + i]));	
	#if	ISprintf	// 输出提取出的数字
		 printf("DATA1234 : %02X %d %d %d %d\n",(dataStorage[dataAddress + i]),responseData[i*4+1], responseData[i*4+2],responseData[i*4+3],responseData[i*4+4] );	
	#endif		
		}	
	 }
	 /*************帧03***********/
	 responseData[dataCount*2+1] = 03;
	 /************校验位***********/
	 for (i = 1; i < dataCount*2+2; i++)  //添加校验位
	{
		sum+=responseData[i];
	}		
		 responseData[dataCount*2+2]= hex_to_ascii(((sum & 0x00F0)>>4));
		 responseData[dataCount*2+3]= hex_to_ascii((sum & 0x000F));
		 sum = 0;	
   /***************输出数据***********************/
		 for (i = 0; i < dataCount*2+4; i++) 
		{			
			while((USART1->SR&0X40)==0); 
              USART1->DR = (u8) responseData[i];
		}
		
		#if	ISprintf	// 输出提取出的数字
		//		printf("%02x",dataAddress);
		#endif
	}
	
	
	
	
	
//************************** 是改写命令的话	**************************************************************/	
	  else if(resultArray[1] == '1')
	{
//		printf("count: %d  \n", dataCount);
 
//起始位   命令字           数据地址                  数据个数           数据1	          数据2          EXT	        校验和
//STX        CMD   ‘ 0 ’‘ 1 ’‘ A ’‘ 1 ’     ‘ 0 ’‘ 2     ’‘ 4 ’‘ 0 ’   ‘ 5 ’‘A’
//02H	                                                                                                 03H

		 switch( dataCount )		 
		 {
			case 0:
		  /***************输出15********************/   
		     while((USART1->SR&0X40)==0); 
             USART1->DR = (u8) 0x15;
			break;
			
			case 1:     //修改1个字节          //从数组第8个字节开始将要改写的数据装入数组
             dataStorage[dataAddress] =  (hexCharToInt(resultArray[8])<<12) +(hexCharToInt(resultArray[9])<<8) +HIGH_4BYTE(dataStorage[dataAddress]) +HIGH_0BYTE(dataStorage[dataAddress]);
			 while((USART1->SR&0X40)==0); 
             USART1->DR = (u8) 0x06;
			break;
			
			case 2:     //修改2个字节
				
             dataStorage[dataAddress] =  (hexCharToInt(resultArray[8])<<12) +(hexCharToInt(resultArray[9])<<8) +(hexCharToInt(resultArray[10])<<4) +hexCharToInt(resultArray[11]);
			/***************输出06********************/
		    while((USART1->SR&0X40)==0); 
            USART1->DR = (u8) 0x06;
//			 printf("DATACHANGE: %X  \n", dataStorage[dataAddress]); 
			
			break;
			
			case 4:     //修改4个字节
             dataStorage[dataAddress] =  (hexCharToInt(resultArray[8])<<12) +(hexCharToInt(resultArray[9])<<8) +(hexCharToInt(resultArray[10])<<4) +hexCharToInt(resultArray[11]);
			 dataStorage[dataAddress+1] =  (hexCharToInt(resultArray[12])<<12) +(hexCharToInt(resultArray[13])<<8) +(hexCharToInt(resultArray[14])<<4) +hexCharToInt(resultArray[15]);
			/***************输出06********************/
		    while((USART1->SR&0X40)==0); 
            USART1->DR = (u8) 0x06;
			break;
			
			case 8:     //修改8个字节

			dataStorage[dataAddress] =  (hexCharToInt(resultArray[8])<<12) +(hexCharToInt(resultArray[9])<<8) +(hexCharToInt(resultArray[10])<<4) +hexCharToInt(resultArray[11]);
			dataStorage[dataAddress+1] =  (hexCharToInt(resultArray[12])<<12) +(hexCharToInt(resultArray[13])<<8) +(hexCharToInt(resultArray[14])<<4) +hexCharToInt(resultArray[15]);
			dataStorage[dataAddress+2] =  (hexCharToInt(resultArray[16])<<12) +(hexCharToInt(resultArray[17])<<8) +(hexCharToInt(resultArray[18])<<4) +hexCharToInt(resultArray[19]);
			dataStorage[dataAddress+3] =  (hexCharToInt(resultArray[20])<<12) +(hexCharToInt(resultArray[21])<<8) +(hexCharToInt(resultArray[22])<<4) +hexCharToInt(resultArray[23]);
			
	    	/***************输出06********************/
		    while((USART1->SR&0X40)==0); 
            USART1->DR = (u8) 0x06;
			break;

		 
		 }

	}

}


// 函数：主板应答处理
void processResponse( u16 dataAddress, u16 dataCount) 
{
	unsigned char responseData[256] = {0}; // 存放上位机要求的数据		
#if	ISprintf	// 输出提取出的数字	
	printf("RIGHT? ADRESS COUNT: %X %X %X \n", vaild, dataAddress,dataCount); 
#endif		
	
    if ((vaild ==1)&& dataCount>0) 
	{
		  getRequestedData(dataAddress, dataCount, responseData);
	}
	
	  else
	{
		  while((USART1->SR&0X40)==0); 
           USART1->DR = (u8) 0x15;
	} 

}

