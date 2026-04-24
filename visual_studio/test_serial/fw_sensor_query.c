#include "fw_sensor_query.h"
#include "fw_protocol.h"

#include <stdio.h>
#include <string.h>
#include <windows.h>

static uint64_t fw_sensor_now_ms(void)
{
    return (uint64_t)GetTickCount64();
}

static void fw_sensor_log(const FW_Callbacks* cbs, FW_LogLevel level, const char* text)
{
    if (cbs != 0 && cbs->log_cb != 0)
    {
        cbs->log_cb(cbs->user_data, level, text);
    }
}

static int fw_sensor_is_cancelled(const FW_Callbacks* cbs)
{
    if (cbs != 0 && cbs->cancel_cb != 0)
    {
        return cbs->cancel_cb(cbs->user_data);
    }
    return 0;
}

static int fw_sensor_contains(const FW_SensorList* list, uint8_t id)
{
    uint32_t i;
    for (i = 0; i < list->count; ++i)
    {
        if (list->ids[i] == id)
        {
            return 1;
        }
    }
    return 0;
}

FW_Result fw_query_sensor_ids(
    FW_CanHandle* can_handle,
    FW_SensorList* out_list,
    uint32_t timeout_ms,
    const FW_Callbacks* cbs)
{
    FW_CanFrame tx_frame;
    FW_CanFrame rx_frame;
    FW_Result ret;
    uint64_t deadline;
    int got_any = 0;

    if (can_handle == 0 || out_list == 0)
    {
        return FW_RESULT_INVALID_ARG;
    }

    memset(out_list, 0, sizeof(*out_list));

    ret = fw_build_get_sensor_id_request(&tx_frame);
    if (ret != FW_RESULT_OK)
    {
        return ret;
    }

    ret = fw_can_send(can_handle, &tx_frame);
    if (ret != FW_RESULT_OK)
    {
        fw_sensor_log(cbs, FW_LOG_ERROR, "sensor id request send failed");
        return ret;
    }

    fw_sensor_log(cbs, FW_LOG_INFO, "sensor id request sent");

    deadline = fw_sensor_now_ms() + (uint64_t)timeout_ms;

    while (fw_sensor_now_ms() < deadline)
    {
        FW_ResponseCommon res;
        uint32_t slice_ms;
        uint64_t remain = deadline - fw_sensor_now_ms();

        if (fw_sensor_is_cancelled(cbs))
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

        ret = fw_parse_common_response(&rx_frame, &res);
        if (ret != FW_RESULT_OK)
        {
            continue;
        }

        if (res.cmd != FW_CMD_GET_SENSOR_ID_RES)
        {
            continue;
        }

        if (res.result != 0)
        {
            continue;
        }

        if (!fw_sensor_contains(out_list, res.sensor_id))
        {
            if (out_list->count < FW_MAX_SENSOR_COUNT)
            {
                char log_buf[128];

                out_list->ids[out_list->count] = res.sensor_id;
                out_list->count += 1;
                got_any = 1;

                snprintf(log_buf, sizeof(log_buf), "sensor found: %u", (unsigned int)res.sensor_id);
                fw_sensor_log(cbs, FW_LOG_INFO, log_buf);

                if (out_list->count >= FW_MAX_SENSOR_COUNT)
                {
                    return FW_RESULT_OK;
                }
            }
        }
    }

    return got_any ? FW_RESULT_OK : FW_RESULT_TIMEOUT;
}