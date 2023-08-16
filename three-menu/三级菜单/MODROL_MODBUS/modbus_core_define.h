#ifndef MODBUS_CORE_DEFINE_H
#define MODBUS_CORE_DEFINE_H

#include <stdint.h>
#include "stdio.h"

typedef enum {
	/**
	 * @brief ����Ȧ״̬
	 */
	ReadCoilState = 0x1,
	/**
	 * @brief ������״̬
	 */
	ReadInputState = 0x2,
	/**
	 * @brief �����ּĴ���
	 */
	ReadHoldReg = 0x3,
	/**
	 * @brief ������Ĵ���
	 */
	ReadInputReg = 0x4,
	/**
	 * @brief дһ����Ȧ
	 */
	WriteOneCoil = 0x05,
	/**
	 * @brief дһ���Ĵ���
	 */
	WriteOneReg = 0x06,
	/**
	 * @brief д����Ĵ���
	 */
	WriteMulReg = 0x10,
	/**
	 * @brief д�����Ȧ
	 */
	WriteMulCoilsReg = 0x0F,
} Modbus_FuncCode_TypeDef;

typedef enum {
	// �Ƿ�����
	InvalidDataErrorCode = 0x2,
	// �������ݳ��Ȳ���ȷ
	RecDataLenErrorCode = 0xFF,

} Modbus_RecCode_TypeDef;
#define modbus_u8_to_u16(a, b) (uint16_t)((uint16_t)((a)<<8)+(b))


#endif //PROCOTOCOLBUILDPROJ_MODBUS_CORE_DEFINE_H

