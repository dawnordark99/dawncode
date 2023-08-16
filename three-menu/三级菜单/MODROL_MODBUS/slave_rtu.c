#include "slave_rtu.h"
#include "usart.h"	
#include "modbus_core_define.h"
#include "modbus_core_crc.h"

#define FIX_MIN_DATA_LEN 8  //����modbus�����С����
 
typedef struct {
	uint8_t slaveAddr;
	uint8_t funcCode;
	uint8_t regAddr;
	uint8_t regNum;
	uint8_t *data;
	uint16_t dataLen;
} slave_packet_t;

static uint8_t slaveAddr = 1; //�ӻ���ַ
static uint8_t modbus_slave_buf[256] = {0}; //����ȡ�������ݱ�����������
static slave_packet_t packet; 

static void core_slave_data_parse(uint8_t *data, uint16_t len, slave_packet_t *ptr); //����ԭʼ����

static uint16_t core_build_err_resp(uint8_t code, slave_packet_t *pkt, uint8_t *retData); //���ݴ�������

void Modbus_Poll_Init(uint8_t addr) { slaveAddr = addr; }   // ��ʼ���ӻ���ַ

/***************��ڲ���     ������       ����***********************************************/
uint16_t add_crc(uint8_t *retData, uint16_t len) //���CRC16У���� 
{
	uint16_t crc = Modbus_Crc_Compute(modbus_slave_buf, len);
	modbus_slave_buf[len++] = crc;
	modbus_slave_buf[len++] = crc >> 8;
	return len;
}

void Modbus_Poll(uint8_t *data, uint16_t len)  //���� modbusЭ��������
{
	if (len < FIX_MIN_DATA_LEN) return;
	if (slaveAddr != data[0] || Modbus_Crc_Compute(data, len) != 0) return;   //��һ�����жϴӻ���ַ��CRC16У����
	
	core_slave_data_parse(data, len, &packet);  //����ԭʼ����
	uint16_t rspLen = 0;
	uint8_t respCode = RSP_OK;


	// ��ȡ������
	switch (packet.funcCode) 
	{ 
//��ȡ		
//��վ��ַ    	������	       ��ʼ��ַ	              �Ĵ�������	     CRC
//1�ֽ�(1~147)	1�ֽ�(0x3)	2�ֽ�(0x0000~0xFFFF)	2�ֽ�( 1~125)	2�ֽ�   

//��վ��ַ	      ������	 �ֽ���	    �Ĵ���ֵ	CRC
//1�ֽ�(1~147)	1�ֽ�(0x3)	1�ֽ�(N)	2*N�ֽ�	    2�ֽ�

		
		case ReadHoldReg: // 0x3
		case ReadInputReg: // 0x4 
			modbus_slave_buf[rspLen++] = packet.slaveAddr;          //�����ݱ��浽������ �ӻ���ַ
			modbus_slave_buf[rspLen++] = packet.funcCode;               //������
			modbus_slave_buf[rspLen++] = packet.regNum *2;             //���ݳ���
            Slave_ReadHoldReg(packet.regAddr, packet.regNum, modbus_slave_buf, rspLen );  //��ȡ������Ϣ
			rspLen += (packet.regNum * 2);                 
			rspLen = add_crc(modbus_slave_buf, rspLen);		  //���CRC16У����  	
					  for (int i = 0; i < rspLen; i++) 
				{				
					  while((USART1->SR&0X40)==0) ; 
					  USART1->DR = (u8) modbus_slave_buf[i];
				}		
		break;

		case WriteOneReg: // 0x06	 06��ķ��ͺͽ�����ͬ��
		case WriteMulReg: // 0x10
//����			
//���ͣ�
//��վ��ַ	     ������	      ��ʼ��ַ	               д��Ĵ�������	   д���ֽ���	   д��ֵ	    CRC
//1�ֽ�(1~147)	1�ֽ�(0x10)	2�ֽ�(0x0000~0xFFFF)	2�ֽ�( 0x0001~0x07B0)	1�ֽ�(N)	  2*N�ֽ�	   2�ֽ�

//���գ�
//��վ��ַ       	������     	��ʼ��ַ	                   д��ֵ	          CRC
//1�ֽ�(1~147)	1�ֽ�(0x10)	2�ֽ�(0x0000~0xFFFF)	       2*N�ֽ�		      2�ֽ�		
				
			modbus_slave_buf[rspLen++] = packet.slaveAddr;   //�����ݱ��浽������ �ӻ���ַ
			modbus_slave_buf[rspLen++] = packet.funcCode;		 //������	
			modbus_slave_buf[rspLen++] = data[2];            //���ݵ�ַ
		    modbus_slave_buf[rspLen++] = data[3];           //���ݵ�ַ
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
	// ������Ӧ
	if (respCode != RSP_OK)
	{
		rspLen = core_build_err_resp(respCode, &packet, modbus_slave_buf);
	}

}


static void core_slave_data_parse(uint8_t *data, uint16_t len, slave_packet_t *ptr) //����ԭʼ����
{
	ptr->slaveAddr = data[0];                                  //�ӻ���ַ��һ�ֽ�
	ptr->funcCode = data[1];                                   //������ڶ��ֽ�
	ptr->regAddr = modbus_u8_to_u16(data[2], data[3]);        //���ݵ�ַ�������ֽ�
		switch (ptr->funcCode)
{
		case ReadHoldReg: //0x3
		case ReadInputReg: //0x4
		     ptr->regNum = modbus_u8_to_u16(data[4], data[5]);	//	 ���ݳ���     
		break;
		case WriteOneReg:  //0x6		
			 ptr->dataLen = 2;		                          //һ�����ݳ���
		break; 
		case WriteMulReg:   //0x10
			 ptr->regNum = modbus_u8_to_u16(data[4], data[5]);
			 ptr->dataLen = data[6];		                  //�������ݳ���              
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

extern uint8_t uint8_array[];  //��2���ֽڵ�������1���ֽڱ���

Slave_Rsp_Code Slave_ReadHoldReg(uint16_t regAddr, uint16_t regNum, uint8_t *retData,uint16_t num)   //��ȡ
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

Slave_Rsp_Code Slave_WriteReg(uint8_t funcode ,uint16_t regAddr, uint16_t len, uint8_t *retData,uint8_t *data,uint16_t num) //д��
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


