#include "fw_can_pcan.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>

#include "PCANBasic.h"

struct FW_CanHandle
{
    TPCANHandle channel;
    FW_Callbacks callbacks;
};

static uint64_t fw_can_now_ms(void)
{
    return (uint64_t)GetTickCount64();
}

static void fw_can_log(FW_CanHandle* handle, FW_LogLevel level, const char* text)
{
    if (handle != 0 && handle->callbacks.log_cb != 0)
    {
        handle->callbacks.log_cb(handle->callbacks.user_data, level, text);
    }
}

static void fw_can_make_error_text(TPCANStatus status, char* out_buf, size_t out_buf_size)
{
    char tmp[256];

    if (out_buf == 0 || out_buf_size == 0)
    {
        return;
    }

    memset(tmp, 0, sizeof(tmp));
    memset(out_buf, 0, out_buf_size);

    if (CAN_GetErrorText(status, 0, tmp) == PCAN_ERROR_OK)
    {
        strncpy(out_buf, tmp, out_buf_size - 1);
    }
    else
    {
        snprintf(out_buf, out_buf_size, "PCAN error: 0x%08X", (unsigned int)status);
    }
}

static uint8_t fw_can_fd_len_to_dlc(uint8_t len)
{
    if (len <= 8u)  return len;
    if (len <= 12u) return 9u;
    if (len <= 16u) return 10u;
    if (len <= 20u) return 11u;
    if (len <= 24u) return 12u;
    if (len <= 32u) return 13u;
    if (len <= 48u) return 14u;
    return 15u;
}

static uint8_t fw_can_fd_dlc_to_len(uint8_t dlc)
{
    static const uint8_t dlc_map[16] =
    {
        0u, 1u, 2u, 3u, 4u, 5u, 6u, 7u,
        8u, 12u, 16u, 20u, 24u, 32u, 48u, 64u
    };

    return dlc_map[dlc & 0x0Fu];
}

FW_Result fw_can_open(FW_CanHandle** out_handle, const FW_CanConfig* config, const FW_Callbacks* cbs)
{
    FW_CanHandle* handle;
    char bitrate_fd[256];
    TPCANStatus sts;

    if (out_handle == 0 || config == 0)
    {
        return FW_RESULT_INVALID_ARG;
    }

    *out_handle = 0;

    handle = (FW_CanHandle*)calloc(1, sizeof(FW_CanHandle));
    if (handle == 0)
    {
        return FW_RESULT_FAIL;
    }

    if (cbs != 0)
    {
        handle->callbacks = *cbs;
    }

    handle->channel = (TPCANHandle)config->channel;

    if (!config->fd_enable)
    {
        free(handle);
        return FW_RESULT_INVALID_ARG;
    }

    snprintf(
        bitrate_fd,
        sizeof(bitrate_fd),
        "f_clock_mhz=%u,nom_brp=%u,nom_tseg1=%u,nom_tseg2=%u,nom_sjw=%u,data_brp=%u,data_tseg1=%u,data_tseg2=%u,data_sjw=%u",
        (unsigned int)config->f_clock_mhz,
        (unsigned int)config->nom_brp,
        (unsigned int)config->nom_tseg1,
        (unsigned int)config->nom_tseg2,
        (unsigned int)config->nom_sjw,
        (unsigned int)config->data_brp,
        (unsigned int)config->data_tseg1,
        (unsigned int)config->data_tseg2,
        (unsigned int)config->data_sjw);

    sts = CAN_InitializeFD(handle->channel, bitrate_fd);
    if (sts != PCAN_ERROR_OK)
    {
        char err[256];
        fw_can_make_error_text(sts, err, sizeof(err));
        fw_can_log(handle, FW_LOG_ERROR, err);
        free(handle);
        return FW_RESULT_OPEN_FAIL;
    }

    *out_handle = handle;
    fw_can_log(handle, FW_LOG_INFO, "PCAN FD opened");
    return FW_RESULT_OK;
}

void fw_can_close(FW_CanHandle* handle)
{
    if (handle == 0)
    {
        return;
    }

    CAN_Uninitialize(handle->channel);
    free(handle);
}

FW_Result fw_can_send(FW_CanHandle* handle, const FW_CanFrame* frame)
{
    TPCANMsgFD msg;
    TPCANStatus sts;
    uint8_t payload_len;
    char err[256];

    if (handle == 0 || frame == 0)
    {
        return FW_RESULT_INVALID_ARG;
    }

    memset(&msg, 0, sizeof(msg));

    msg.ID = frame->can_id;
    msg.MSGTYPE = PCAN_MESSAGE_FD;

    if (frame->bitrate_switch)
    {
        msg.MSGTYPE |= PCAN_MESSAGE_BRS;
    }

    msg.DLC = fw_can_fd_len_to_dlc(frame->data_len);
    payload_len = fw_can_fd_dlc_to_len(msg.DLC);

    memset(msg.DATA, 0, sizeof(msg.DATA));
    if (frame->data_len > 0u)
    {
        memcpy(msg.DATA, frame->data, frame->data_len);
    }

    if (payload_len > frame->data_len)
    {
        memset(&msg.DATA[frame->data_len], 0, payload_len - frame->data_len);
    }

    sts = CAN_WriteFD(handle->channel, &msg);
    if (sts != PCAN_ERROR_OK)
    {
        fw_can_make_error_text(sts, err, sizeof(err));
        fw_can_log(handle, FW_LOG_ERROR, err);
        return FW_RESULT_SEND_FAIL;
    }

    return FW_RESULT_OK;
}

FW_Result fw_can_recv(FW_CanHandle* handle, FW_CanFrame* out_frame, uint32_t timeout_ms)
{
    uint64_t deadline;
    char err[256];

    if (handle == 0 || out_frame == 0)
    {
        return FW_RESULT_INVALID_ARG;
    }

    deadline = fw_can_now_ms() + (uint64_t)timeout_ms;

    while (1)
    {
        TPCANMsgFD msg;
        TPCANTimestampFD ts;
        TPCANStatus sts;

        memset(&msg, 0, sizeof(msg));
        memset(&ts, 0, sizeof(ts));

        sts = CAN_ReadFD(handle->channel, &msg, &ts);
        if (sts == PCAN_ERROR_OK)
        {
            memset(out_frame, 0, sizeof(*out_frame));
            out_frame->can_id = msg.ID;
            out_frame->data_len = fw_can_fd_dlc_to_len(msg.DLC);
            out_frame->is_fd = 1;
            out_frame->bitrate_switch = ((msg.MSGTYPE & PCAN_MESSAGE_BRS) != 0) ? 1 : 0;
            out_frame->is_error_frame = ((msg.MSGTYPE & PCAN_MESSAGE_ERRFRAME) != 0) ? 1 : 0;
            memcpy(out_frame->data, msg.DATA, out_frame->data_len);
            return FW_RESULT_OK;
        }

        if (sts == PCAN_ERROR_QRCVEMPTY)
        {
            if (fw_can_now_ms() >= deadline)
            {
                return FW_RESULT_TIMEOUT;
            }

            Sleep(1);
            continue;
        }

        fw_can_make_error_text(sts, err, sizeof(err));
        fw_can_log(handle, FW_LOG_ERROR, err);
        return FW_RESULT_RECV_FAIL;
    }
}