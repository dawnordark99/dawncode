#ifndef MODBUS_CORE_H
#define MODBUS_CORE_H

#include "string.h"
#include "stdbool.h"
#include "modbus_core_define.h"
#include "modbus_core_crc.h"
//#include "modbus_core_master.h"
#include "slave_rtu.h"

extern SwSendDataCallBack modbusSendCallBack;
extern SwRecDataCallBack modbusRecCallBack;

__attribute__((unused)) void Modbus_RegistrySendCallBack(SwSendDataCallBack sendDataCallBack);

__attribute__((unused)) void Modbus_RegistryRecCallBack(SwRecDataCallBack recDataCallBack);

/**
 * @brief ��bool��ָ��ת�ֽ�����
 * @param dst �洢ת�����ֽ�����
 * @param data bool����
 * @param dataLen ���ݳ���
 * @return
 */
uint16_t PackBoolArray(uint8_t *dst, const bool *data, uint16_t dataLen);

/**
 * @brief bool��������ת�ֽ�����
 * @param dst �洢
 * @param cnt ת���ĸ���
 * @param data �ֽ�����
 * @param dataLen ���ݳ���
 */
void UnPackBool(bool *dst, uint16_t cnt, const uint8_t *data, uint16_t dataLen);


#endif //PROCOTOCOLBUILDPROJ_MODBUS_CORE_H

