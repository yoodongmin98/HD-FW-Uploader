#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "fw_common.h"
#include <stdint.h>

    typedef struct FW_CanHandle FW_CanHandle;

    typedef struct FW_CanConfig
    {
        uint16_t channel;

        uint32_t nominal_bitrate;
        int fd_enable;

        uint32_t f_clock_mhz;
        uint16_t nom_brp;
        uint16_t nom_tseg1;
        uint16_t nom_tseg2;
        uint16_t nom_sjw;

        uint16_t data_brp;
        uint16_t data_tseg1;
        uint16_t data_tseg2;
        uint16_t data_sjw;
    } FW_CanConfig;

    typedef struct FW_CanFrame
    {
        uint32_t can_id;
        uint8_t  data[FW_MAX_CAN_DATA_LEN];
        uint8_t  data_len;
        int      is_fd;
        int      bitrate_switch;
        int      is_error_frame;
    } FW_CanFrame;

    FW_Result fw_can_open(FW_CanHandle** out_handle, const FW_CanConfig* config, const FW_Callbacks* cbs);
    void      fw_can_close(FW_CanHandle* handle);

    FW_Result fw_can_send(FW_CanHandle* handle, const FW_CanFrame* frame);
    FW_Result fw_can_recv(FW_CanHandle* handle, FW_CanFrame* out_frame, uint32_t timeout_ms);

#ifdef __cplusplus
}
#endif