#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "fw_common.h"
#include "fw_can_pcan.h"
#include "fw_protocol.h"

    FW_Result fw_get_sensor_data(
        FW_CanHandle* can_handle,
        uint8_t sensor_id,
        FW_GetDataResponse* out_data,
        uint32_t timeout_ms,
        const FW_Callbacks* cbs);

#ifdef __cplusplus
}
#endif