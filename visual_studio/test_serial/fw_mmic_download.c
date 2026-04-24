#include "fw_mmic_download.h"
#include "fw_protocol.h"

#include <stdio.h>
#include <string.h>
#include <windows.h>



static uint64_t fw_mmic_now_ms(void)
{
    return (uint64_t)GetTickCount64();
}

static void fw_mmic_log(const FW_Callbacks* cbs, FW_LogLevel level, const char* text)
{
    if (cbs != 0 && cbs->log_cb != 0)
    {
        cbs->log_cb(cbs->user_data, level, text);
    }
}

static int fw_mmic_is_cancelled(const FW_Callbacks* cbs)
{
    if (cbs != 0 && cbs->cancel_cb != 0)
    {
        return cbs->cancel_cb(cbs->user_data);
    }
    return 0;
}

static void fw_mmic_report_progress(
    const FW_Callbacks* cbs,
    uint32_t total_bytes,
    uint32_t sent_bytes,
    uint16_t packet_seq,
    uint8_t sensor_id)
{
    FW_Progress p;

    if (cbs == 0 || cbs->progress_cb == 0)
    {
        return;
    }

    memset(&p, 0, sizeof(p));
    p.total_bytes = total_bytes;
    p.sent_bytes = sent_bytes;
    p.packet_seq = packet_seq;
    p.sensor_id = sensor_id;
    p.percent = (total_bytes == 0u) ? 100u : (uint8_t)((sent_bytes * 100u) / total_bytes);

    cbs->progress_cb(cbs->user_data, &p);
}

static FW_Result fw_mmic_get_file_size(FILE* fp, uint32_t* out_size)
{
    long pos;
    long end_pos;

    if (fp == 0 || out_size == 0)
    {
        return FW_RESULT_INVALID_ARG;
    }

    pos = ftell(fp);
    if (pos < 0)
    {
        return FW_RESULT_FILE_ERROR;
    }

    if (fseek(fp, 0, SEEK_END) != 0)
    {
        return FW_RESULT_FILE_ERROR;
    }

    end_pos = ftell(fp);
    if (end_pos < 0)
    {
        return FW_RESULT_FILE_ERROR;
    }

    if (fseek(fp, pos, SEEK_SET) != 0)
    {
        return FW_RESULT_FILE_ERROR;
    }

    *out_size = (uint32_t)end_pos;
    return FW_RESULT_OK;
}

static FW_Result fw_mmic_wait_response(
    FW_CanHandle* can_handle,
    uint8_t expected_cmd,
    uint32_t timeout_ms,
    FW_ResponseCommon* out_res)
{
    uint64_t deadline = fw_mmic_now_ms() + (uint64_t)timeout_ms;

    while (fw_mmic_now_ms() < deadline)
    {
        FW_CanFrame rx;
        FW_Result ret;
        uint64_t remain = deadline - fw_mmic_now_ms();
        uint32_t slice_ms = (remain > 20u) ? 20u : (uint32_t)remain;

        ret = fw_can_recv(can_handle, &rx, slice_ms);
        if (ret == FW_RESULT_TIMEOUT)
        {
            continue;
        }
        if (ret != FW_RESULT_OK)
        {
            return ret;
        }

        if (rx.is_error_frame)
        {
            continue;
        }

        ret = fw_parse_common_response(&rx, out_res);
        if (ret != FW_RESULT_OK)
        {
            continue;
        }

        if (out_res->cmd == FW_CMD_MMIC_DOWNLOAD_WAIT_RES)
        {
            continue;
        }

        if (out_res->cmd == expected_cmd)
        {
            return FW_RESULT_OK;
        }
    }

    return FW_RESULT_TIMEOUT;
}

FW_Result fw_download_mmic_firmware(
    FW_CanHandle* can_handle,
    const FW_MmicDownloadOption* opt,
    const FW_Callbacks* cbs)
{
    FILE* fp;
    FW_Result ret;
    uint32_t file_size = 0;
    uint32_t sent_size = 0;
    uint16_t packet_seq = 1;
    uint8_t chunk[FW_MMIC_CHUNK_SIZE];
    char log_buf[256];
    uint32_t retry_index;

    if (can_handle == 0 || opt == 0 || opt->file_path == 0)
    {
        return FW_RESULT_INVALID_ARG;
    }

    fp = fopen(opt->file_path, "rb");
    if (fp == 0)
    {
        fw_mmic_log(cbs, FW_LOG_ERROR, "failed to open MMIC firmware file");
        return FW_RESULT_FILE_ERROR;
    }

    ret = fw_mmic_get_file_size(fp, &file_size);
    if (ret != FW_RESULT_OK)
    {
        fclose(fp);
        return ret;
    }

    snprintf(log_buf, sizeof(log_buf), "MMIC download start: sensor=%u size=%u",
        (unsigned int)opt->sensor_id, (unsigned int)file_size);
    fw_mmic_log(cbs, FW_LOG_INFO, log_buf);

    /* 1) START */
    FILE* fps = fopen("log.bin", "wb"); // a = append
    if (fps == NULL)
    {
        return;
    }
    for (retry_index = 0; retry_index <= opt->retry_count; ++retry_index)
    {
        FW_CanFrame tx;
        FW_ResponseCommon res;

        if (fw_mmic_is_cancelled(cbs))
        {
            fclose(fp);
            return FW_RESULT_CANCELLED;
        }

        ret = fw_build_mmic_start_request(opt->sensor_id, file_size, &tx);
        if (ret != FW_RESULT_OK)
        {
            fclose(fp);
            return ret;
        }

        ret = fw_can_send(can_handle, &tx);
        if (ret != FW_RESULT_OK)
        {
            if (retry_index >= opt->retry_count)
            {
                fclose(fp);
                return ret;
            }
            continue;
        }

        ret = fw_mmic_wait_response(
            can_handle,
            FW_CMD_MMIC_DOWNLOAD_START_RES,
            opt->recv_timeout_ms,
            &res);

        if (ret != FW_RESULT_OK)
        {
            if (retry_index >= opt->retry_count)
            {
                fclose(fp);
                return ret;
            }
            continue;
        }

        if (res.result == 0)
        {
            fw_mmic_log(cbs, FW_LOG_INFO, "MMIC start response OK");
            break;
        }

        snprintf(log_buf, sizeof(log_buf), "MMIC start rejected: result=%u", (unsigned int)res.result);
        fw_mmic_log(cbs, FW_LOG_WARN, log_buf);

        if (retry_index >= opt->retry_count)
        {
            fclose(fp);
            return FW_RESULT_FAIL;
        }
    }

    fw_mmic_report_progress(cbs, file_size, 0u, packet_seq, opt->sensor_id);

    /* 2) CHUNK LOOP */
    while (1)
    {
        int read_bytes = fread(chunk, 1, sizeof(chunk), fp);

        if (read_bytes == 0)
        {
            if (feof(fp))
            {
                break;
            }

            fclose(fp);
            return FW_RESULT_FILE_ERROR;
        }

        for (retry_index = 0; retry_index <= opt->retry_count; ++retry_index)
        {
            FW_CanFrame tx;
            FW_ResponseCommon res;

            if (fw_mmic_is_cancelled(cbs))
            {
                fclose(fp);
                return FW_RESULT_CANCELLED;
            }

            ret = fw_build_mmic_chunk_request(
                opt->sensor_id,
                chunk,
                (uint8_t)read_bytes,
                &tx);

            if (ret != FW_RESULT_OK)
            {
                fclose(fp);
                return ret;
            }

            ret = fw_can_send(can_handle, &tx);
            fwrite(chunk, 1, read_bytes, fps);
            if (ret != FW_RESULT_OK)
            {
                if (retry_index >= opt->retry_count)
                {
                    fclose(fp);
                    return ret;
                }
                continue;
            }

            ret = fw_mmic_wait_response(
                can_handle,
                FW_CMD_MMIC_DOWNLOAD_FILE_RES,
                opt->recv_timeout_ms,
                &res);

            if (ret != FW_RESULT_OK)
            {
                if (retry_index >= opt->retry_count)
                {
                    fclose(fp);
                    return ret;
                }
                continue;
            }

            if (res.result == 0)
            {
                sent_size += (uint32_t)read_bytes;
                packet_seq += 1;
                fw_mmic_report_progress(cbs, file_size, sent_size, packet_seq, opt->sensor_id);
                break;
            }

            snprintf(log_buf, sizeof(log_buf), "MMIC chunk failed: result=%u", (unsigned int)res.result);
            fw_mmic_log(cbs, FW_LOG_WARN, log_buf);

            if (retry_index >= opt->retry_count)
            {
                fclose(fp);
                return FW_RESULT_FAIL;
            }
        }
    }
    fclose(fps);
    fclose(fp);

    /* 3) END */
    for (retry_index = 0; retry_index <= opt->retry_count; ++retry_index)
    {
        FW_CanFrame tx;
        FW_ResponseCommon res;

        if (fw_mmic_is_cancelled(cbs))
        {
            return FW_RESULT_CANCELLED;
        }

        ret = fw_build_mmic_end_request(opt->sensor_id, &tx);
        if (ret != FW_RESULT_OK)
        {
            return ret;
        }

        ret = fw_can_send(can_handle, &tx);
        if (ret != FW_RESULT_OK)
        {
            if (retry_index >= opt->retry_count)
            {
                return ret;
            }
            continue;
        }

        ret = fw_mmic_wait_response(
            can_handle,
            FW_CMD_MMIC_DOWNLOAD_END_RES,
            opt->recv_timeout_ms,
            &res);

        if (ret != FW_RESULT_OK)
        {
            if (retry_index >= opt->retry_count)
            {
                return ret;
            }
            continue;
        }

        if (res.result == 0)
        {
            fw_mmic_report_progress(cbs, file_size, file_size, packet_seq, opt->sensor_id);
            fw_mmic_log(cbs, FW_LOG_INFO, "MMIC download complete");
            return FW_RESULT_OK;
        }

        snprintf(log_buf, sizeof(log_buf), "MMIC end failed: result=%u", (unsigned int)res.result);
        fw_mmic_log(cbs, FW_LOG_WARN, log_buf);

        if (retry_index >= opt->retry_count)
        {
            return FW_RESULT_FAIL;
        }
    }

    return FW_RESULT_FAIL;
}