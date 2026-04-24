#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "fw_common.h"
#include "fw_can_pcan.h"

    FW_Result fw_query_sensor_ids(
        FW_CanHandle* can_handle,
        FW_SensorList* out_list,
        uint32_t timeout_ms,
        const FW_Callbacks* cbs);

#ifdef __cplusplus
}
#endif