#include "slave_rtu.h"
#include "usart.h"	
#include "modbus_core_define.h"
#include "modbus_core_crc.h"

#define FIX_MIN_DATA_LEN 8  //定义modbus码的最小长度
 
typedef struct {
	uint8_t slaveAddr;
	uint8_t funcCode;
	uint8_t regAddr;
	uint8_t regNum;
	uint8_t *data;
	uint16_t dataLen;
} slave_packet_t;

static uint8_t slaveAddr = 1; //从机地址
static uint8_t modbus_slave_buf[256] = {0}; //将读取到的数据保存进这个数组
static slave_packet_t packet; 

static void core_slave_data_parse(uint8_t *data, uint16_t len, slave_packet_t *ptr); //处理原始数据

static uint16_t core_build_err_resp(uint8_t code, slave_packet_t *pkt, uint8_t *retData); //数据错误处理函数

void Modbus_Poll_Init(uint8_t addr) { slaveAddr = addr; }   // 初始化从机地址

/***************入口参数     数组名       长度***********************************************/
uint16_t add_crc(uint8_t *retData, uint16_t len) //添加CRC16校验码 
{
	uint16_t crc = Modbus_Crc_Compute(modbus_slave_buf, len);
	modbus_slave_buf[len++] = crc;
	modbus_slave_buf[len++] = crc >> 8;
	return len;
}

void Modbus_Poll(uint8_t *data, uint16_t len)  //调用 modbus协议主函数
{
	if (len < FIX_MIN_DATA_LEN) return;
	if (slaveAddr != data[0] || Modbus_Crc_Compute(data, len) != 0) return;   //第一步先判断从机地址和CRC16校验码
	
	core_slave_data_parse(data, len, &packet);  //处理原始数据
	uint16_t rspLen = 0;
	uint8_t respCode = RSP_OK;


	// 获取功能码
	switch (packet.funcCode) 
	{ 
//读取		
//从站地址    	功能码	       起始地址	              寄存器数量	     CRC
//1字节(1~147)	1字节(0x3)	2字节(0x0000~0xFFFF)	2字节( 1~125)	2字节   

//从站地址	      功能码	 字节数	    寄存器值	CRC
//1字节(1~147)	1字节(0x3)	1字节(N)	2*N字节	    2字节

		
		case ReadHoldReg: // 0x3
		case ReadInputReg: // 0x4 
			modbus_slave_buf[rspLen++] = packet.slaveAddr;          //将数据保存到新数组 从机地址
			modbus_slave_buf[rspLen++] = packet.funcCode;               //功能码
			modbus_slave_buf[rspLen++] = packet.regNum *2;             //数据长度
            Slave_ReadHoldReg(packet.regAddr, packet.regNum, modbus_slave_buf, rspLen );  //读取数据信息
			rspLen += (packet.regNum * 2);                 
			rspLen = add_crc(modbus_slave_buf, rspLen);		  //添加CRC16校验码  	
					  for (int i = 0; i < rspLen; i++) 
				{				
					  while((USART1->SR&0X40)==0) ; 
					  USART1->DR = (u8) modbus_slave_buf[i];
				}		
		break;

		case WriteOneReg: // 0x06	 06码的发送和接收相同。
		case WriteMulReg: // 0x10
//接收			
//发送：
//从站地址	     功能码	      起始地址	               写入寄存器个数	   写入字节数	   写入值	    CRC
//1字节(1~147)	1字节(0x10)	2字节(0x0000~0xFFFF)	2字节( 0x0001~0x07B0)	1字节(N)	  2*N字节	   2字节

//接收：
//从站地址       	功能码     	起始地址	                   写入值	          CRC
//1字节(1~147)	1字节(0x10)	2字节(0x0000~0xFFFF)	       2*N字节		      2字节		
				
			modbus_slave_buf[rspLen++] = packet.slaveAddr;   //将数据保存到新数组 从机地址
			modbus_slave_buf[rspLen++] = packet.funcCode;		 //功能码	
			modbus_slave_buf[rspLen++] = data[2];            //数据地址
		    modbus_slave_buf[rspLen++] = data[3];           //数据地址
            respCode = Slave_WriteReg(packet.funcCode,packet.regAddr, packet.dataLen,modbus_slave_buf, data, rspLen );
	        rspLen += packet.dataLen;
		    rspLen = add_crc(modbus_slave_buf, rspLen);	
		
					  for (int i = 0; i < rspLen; i++) 
				{				
					  while((USART1->SR&0X40)==0) ; 
					  USART1->DR = (u8) modbus_slave_buf[i];
				}			
		break;
	}
	// 错误响应
	if (respCode != RSP_OK)
	{
		rspLen = core_build_err_resp(respCode, &packet, modbus_slave_buf);
	}

}


static void core_slave_data_parse(uint8_t *data, uint16_t len, slave_packet_t *ptr) //处理原始数据
{
	ptr->slaveAddr = data[0];                                  //从机地址第一字节
	ptr->funcCode = data[1];                                   //功能码第二字节
	ptr->regAddr = modbus_u8_to_u16(data[2], data[3]);        //数据地址第三四字节
		switch (ptr->funcCode)
{
		case ReadHoldReg: //0x3
		case ReadInputReg: //0x4
		     ptr->regNum = modbus_u8_to_u16(data[4], data[5]);	//	 数据长度     
		break;
		case WriteOneReg:  //0x6		
			 ptr->dataLen = 2;		                          //一个数据长度
		break; 
		case WriteMulReg:   //0x10
			 ptr->regNum = modbus_u8_to_u16(data[4], data[5]);
			 ptr->dataLen = data[6];		                  //几个数据长度              
		break;
	}
}

/**
 * @brief 
 * @param code 
 * @param pkt 
 * @param retData 
 * @return 
 */
static uint16_t core_build_err_resp(uint8_t code, slave_packet_t *pkt, uint8_t *retData) 
{
	uint8_t idx = 0;
	retData[idx++] = pkt->slaveAddr;
	retData[idx++] = pkt->funcCode | 0x80;
	retData[idx++] = 2;
	uint16_t crc = Modbus_Crc_Compute(retData, idx);
	retData[idx++] = crc;
	retData[idx++] = crc >> 8;
	return idx;
}

uint16_t reg_start_addr = 0x000;

extern uint8_t uint8_array[];  //将2个字节的数组拆成1个字节保存

Slave_Rsp_Code Slave_ReadHoldReg(uint16_t regAddr, uint16_t regNum, uint8_t *retData,uint16_t num)   //读取
{
	if (regAddr < reg_start_addr) {
		return RSP_ERR_REG_ADDR;
	} 
	else 
	{
		for(int i=0;i<regNum * 2;i++)
		{
		  retData[num +i]= uint8_array[regAddr + i];		
		}
		return RSP_OK;
	}

}

Slave_Rsp_Code Slave_WriteReg(uint8_t funcode ,uint16_t regAddr, uint16_t len, uint8_t *retData,uint8_t *data,uint16_t num) //写入
{
		for(int i=0;i<len;i++)
	{
		if (funcode == WriteOneReg)
		{uint8_array[regAddr + i] = data[4+i];}
		else if (funcode == WriteMulReg)
		{uint8_array[regAddr + i] = data[7+i];}
		retData[num +i]= uint8_array[regAddr + i];	
		
	}
       return RSP_OK;
}


