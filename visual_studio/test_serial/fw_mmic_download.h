#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "fw_common.h"
#include "fw_can_pcan.h"

    typedef struct FW_MmicDownloadOption
    {
        uint8_t  sensor_id;
        const char* file_path;
        uint32_t send_timeout_ms;
        uint32_t recv_timeout_ms;
        uint32_t retry_count;
    } FW_MmicDownloadOption;

    FW_Result fw_download_mmic_firmware(
        FW_CanHandle* can_handle,
        const FW_MmicDownloadOption* opt,
        const FW_Callbacks* cbs);

#ifdef __cplusplus
}
#endif