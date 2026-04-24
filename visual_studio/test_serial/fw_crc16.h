#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stddef.h>

	uint16_t fw_crc16_modbus(uint16_t init_crc, const uint8_t* data, size_t size);

#ifdef __cplusplus
}
#endif