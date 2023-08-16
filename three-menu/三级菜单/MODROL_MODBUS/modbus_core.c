#include "modbus_core.h"

// �������ݻص�ָ��
SwSendDataCallBack modbusSendCallBack = NULL;
// �������ݻص�ָ��
SwRecDataCallBack modbusRecCallBack = NULL;

/**
 * @brief ע�ᷢ�����ݻص�
 * @param sendDataCallBack
 */
__attribute__((unused)) void Modbus_RegistrySendCallBack(SwSendDataCallBack sendDataCallBack) {
	modbusSendCallBack = sendDataCallBack;
}

/**
 * @brief ע��������ݻص�
 * @param recDataCallBack
 */
__attribute__((unused)) void Modbus_RegistryRecCallBack(SwRecDataCallBack recDataCallBack) {
	modbusRecCallBack = recDataCallBack;
}

uint16_t PackBoolArray(uint8_t *dst, const bool *data, uint16_t dataLen) {
	uint8_t retLen = 0;
	uint8_t curDat = 0;
	uint8_t bitIdx = 0;
	for (int i = 0; i < dataLen; ++i) {
		if (data[i]) {
			curDat |= (uint8_t) (1 << bitIdx);
		}
		if (++bitIdx == 8) {
			dst[retLen++] = curDat;
			curDat = 0;
			bitIdx = 0;
		}
	}
	if (bitIdx != 8) {
		dst[retLen++] = curDat;
	}
	return retLen;
}

/**
 * @brief bool��������ת�ֽ�����
 * @param dst �洢
 * @param cnt ת���ĸ���
 * @param data �ֽ�����
 * @param dataLen ���ݳ���
 */
void UnPackBool(bool *dst, uint16_t cnt, const uint8_t *data, uint16_t dataLen) {
	uint16_t retLen = 0;
	uint8_t curDat = 0;
	uint8_t bitIdx = 0;
	for (int i = 0; i < dataLen; ++i) {
		curDat = data[i];

		for (int j = 0; j < 8; ++j) {
			if (cnt == 0) { return; };
			*dst++ = (curDat & (1 << j)) != 0;
			cnt--;
		}
	}
}
