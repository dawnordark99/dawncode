#ifndef MODBUS_CORE_CRC_H
#define MODBUS_CORE_CRC_H

#include "modbus_core_define.h"

uint16_t Modbus_Crc_Compute(const uint8_t *buf, uint16_t bufLen);

#endif //PROCOTOCOLBUILDPROJ_MODBUS_CORE_CRC_H

