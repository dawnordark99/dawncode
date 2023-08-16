#ifndef SLAVE_RTU_H
#define SLAVE_RTU_H

#include "modbus_core_define.h"
#include <stdbool.h>

typedef enum {
	RSP_OK = 0x0,
	// 不支持的功能码
	RSP_ERR_CMD = 0x1,
	// 寄存器地址错误
	RSP_ERR_REG_ADDR = 0x2,
	// 数据值域错误
	RSP_ERR_VALUE = 0x3,
	// 写入失败
	RSP_ERR_WRITE = 0x4,
} Slave_Rsp_Code;


void Modbus_Poll_Init(uint8_t addr);

void Modbus_Poll(uint8_t *data, uint16_t len);

Slave_Rsp_Code Slave_ReadHoldReg(uint16_t regAddr, uint16_t regNum, uint8_t *retData, uint16_t num);

Slave_Rsp_Code Slave_ReadInputState(uint16_t regAddr, uint16_t regNum, bool *retData);

Slave_Rsp_Code Slave_WriteReg(uint8_t funcode ,uint16_t regAddr, uint16_t len, uint8_t *retData,uint8_t *data,uint16_t num);



#endif //PROCOTOCOLBUILDPROJ_SLAVE_RTU_H

