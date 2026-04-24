#include "fw_getdata.h"

#include <stdio.h>
#include <windows.h>

static uint64_t fw_getdata_now_ms(void)
{
    return (uint64_t)GetTickCount64();
}

static void fw_getdata_log(const FW_Callbacks* cbs, FW_LogLevel level, const char* text)
{
    if (cbs != 0 && cbs->log_cb != 0)
    {
        cbs->log_cb(cbs->user_data, level, text);
    }
}

static int fw_getdata_is_cancelled(const FW_Callbacks* cbs)
{
    if (cbs != 0 && cbs->cancel_cb != 0)
    {
        return cbs->cancel_cb(cbs->user_data);
    }
    return 0;
}

FW_Result fw_get_sensor_data(
    FW_CanHandle* can_handle,
    uint8_t sensor_id,
    FW_GetDataResponse* out_data,
    uint32_t timeout_ms,
    const FW_Callbacks* cbs)
{
    FW_CanFrame tx_frame;
    FW_CanFrame rx_frame;
    FW_Result ret;
    uint64_t deadline;

    if (can_handle == 0 || out_data == 0)
    {
        return FW_RESULT_INVALID_ARG;
    }

    ret = fw_build_getdata_request(sensor_id, &tx_frame);
    if (ret != FW_RESULT_OK)
    {
        return ret;
    }

    ret = fw_can_send(can_handle, &tx_frame);
    if (ret != FW_RESULT_OK)
    {
        fw_getdata_log(cbs, FW_LOG_ERROR, "getdata request send failed");
        return ret;
    }

    fw_getdata_log(cbs, FW_LOG_INFO, "getdata request sent");
    deadline = fw_getdata_now_ms() + (uint64_t)timeout_ms;

    while (fw_getdata_now_ms() < deadline)
    {
        FW_ResponseCommon common;
        uint32_t slice_ms;
        uint64_t remain = deadline - fw_getdata_now_ms();

        if (fw_getdata_is_cancelled(cbs))
        {
            return FW_RESULT_CANCELLED;
        }

        slice_ms = (remain > 50u) ? 50u : (uint32_t)remain;
        ret = fw_can_recv(can_handle, &rx_frame, slice_ms);

        if (ret == FW_RESULT_TIMEOUT)
        {
            continue;
        }
        if (ret != FW_RESULT_OK)
        {
            return ret;
        }

        if (rx_frame.is_error_frame)
        {
            continue;
        }

        ret = fw_parse_common_response(&rx_frame, &common);
        if (ret != FW_RESULT_OK)
        {
            continue;
        }

        if (common.cmd != FW_CMD_GETDATA_RES)
        {
            continue;
        }

        ret = fw_parse_getdata_response(&rx_frame, out_data);
        if (ret != FW_RESULT_OK)
        {
            return ret;
        }

        if (out_data->result != 0)
        {
            char log_buf[128];
            snprintf(log_buf, sizeof(log_buf), "getdata response error: %u", (unsigned int)out_data->result);
            fw_getdata_log(cbs, FW_LOG_WARN, log_buf);
            return FW_RESULT_FAIL;
        }

        fw_getdata_log(cbs, FW_LOG_INFO, "getdata response received");
        return FW_RESULT_OK;
    }

    return FW_RESULT_TIMEOUT;
}