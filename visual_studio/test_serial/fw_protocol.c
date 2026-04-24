#include "fw_protocol.h"
#include "fw_crc16.h"

#include <string.h>

#define FW_PROTO_FIXED_BODY_LEN   62u
#define FW_PROTO_FIXED_FRAME_LEN  64u
#define FW_PROTO_DEFAULT_CAN_ID   0u

static void fw_proto_init_frame(FW_CanFrame* frame)
{
    memset(frame, 0, sizeof(*frame));
    frame->can_id = FW_PROTO_DEFAULT_CAN_ID;
    frame->is_fd = 1;
    frame->bitrate_switch = 1;
    frame->is_error_frame = 0;
}

static void fw_proto_write_crc(uint8_t* dst, size_t crc_pos, const uint8_t* src, size_t src_len)
{
    uint16_t crc = fw_crc16_modbus(0xFFFFu, src, src_len);
    dst[crc_pos + 0] = (uint8_t)(crc >> 8);
    dst[crc_pos + 1] = (uint8_t)(crc & 0xFF);
}

static FW_Result fw_proto_build_fixed_62_payload_frame(FW_CanFrame* out_frame)
{
    if (out_frame == 0)
    {
        return FW_RESULT_INVALID_ARG;
    }

    fw_proto_init_frame(out_frame);
    memset(out_frame->data, 0, FW_PROTO_FIXED_FRAME_LEN);
    out_frame->data_len = FW_PROTO_FIXED_FRAME_LEN;
    return FW_RESULT_OK;
}

FW_Result fw_build_get_sensor_id_request(FW_CanFrame* out_frame)
{
    FW_Result ret = fw_proto_build_fixed_62_payload_frame(out_frame);
    if (ret != FW_RESULT_OK)
    {
        return ret;
    }

    out_frame->data[0] = FW_CMD_GET_SENSOR_ID_REQ;
    out_frame->data[1] = 0x40;
    out_frame->data[2] = 0xFF;
    out_frame->data[3] = FW_CMD_GET_SENSOR_ID_REQ;
    out_frame->data[4] = FW_CMD_GET_SENSOR_ID_REQ;

    fw_proto_write_crc(out_frame->data, FW_PROTO_FIXED_BODY_LEN, out_frame->data, FW_PROTO_FIXED_BODY_LEN);
    return FW_RESULT_OK;
}

FW_Result fw_build_getdata_request(uint8_t sensor_id, FW_CanFrame* out_frame)
{
    FW_Result ret = fw_proto_build_fixed_62_payload_frame(out_frame);
    if (ret != FW_RESULT_OK)
    {
        return ret;
    }

    out_frame->data[0] = FW_CMD_GETDATA_REQ;
    out_frame->data[1] = 0x40;
    out_frame->data[2] = sensor_id;
    out_frame->data[4] = 0x01;
    out_frame->data[5] = 0x00;

    fw_proto_write_crc(out_frame->data, FW_PROTO_FIXED_BODY_LEN, out_frame->data, FW_PROTO_FIXED_BODY_LEN);
    return FW_RESULT_OK;
}

FW_Result fw_build_mcu_start_request(uint8_t sensor_id, uint32_t file_size, FW_CanFrame* out_frame)
{
    FW_Result ret = fw_proto_build_fixed_62_payload_frame(out_frame);
    if (ret != FW_RESULT_OK)
    {
        return ret;
    }

    out_frame->data[0] = FW_CMD_MCU_DOWNLOAD_START_REQ;
    out_frame->data[1] = 0x40;
    out_frame->data[2] = sensor_id;
    out_frame->data[3] = (uint8_t)((file_size >> 24) & 0xFF);
    out_frame->data[4] = (uint8_t)((file_size >> 16) & 0xFF);
    out_frame->data[5] = (uint8_t)((file_size >> 8) & 0xFF);
    out_frame->data[6] = (uint8_t)(file_size & 0xFF);

    fw_proto_write_crc(out_frame->data, FW_PROTO_FIXED_BODY_LEN, out_frame->data, FW_PROTO_FIXED_BODY_LEN);
    return FW_RESULT_OK;
}

FW_Result fw_build_mcu_chunk_request(
    uint8_t sensor_id,
    uint32_t offset,
    uint16_t seq,
    const uint8_t* chunk,
    uint8_t chunk_size,
    FW_CanFrame* out_frame)
{
    size_t total_len;

    if (out_frame == 0 || (chunk == 0 && chunk_size > 0))
    {
        return FW_RESULT_INVALID_ARG;
    }

    total_len = (size_t)9 + (size_t)chunk_size + (size_t)2;
    if (total_len > FW_MAX_CAN_DATA_LEN)
    {
        return FW_RESULT_INVALID_ARG;
    }

    fw_proto_init_frame(out_frame);
    memset(out_frame->data, 0, FW_MAX_CAN_DATA_LEN);

    out_frame->data[0] = FW_CMD_MCU_DOWNLOAD_FILE_REQ;
    out_frame->data[1] = (uint8_t)(chunk_size + 5u);
    out_frame->data[2] = sensor_id;
    out_frame->data[3] = (uint8_t)((offset >> 24) & 0xFF);
    out_frame->data[4] = (uint8_t)((offset >> 16) & 0xFF);
    out_frame->data[5] = (uint8_t)((offset >> 8) & 0xFF);
    out_frame->data[6] = (uint8_t)(offset & 0xFF);
    out_frame->data[7] = (uint8_t)((seq >> 8) & 0xFF);
    out_frame->data[8] = (uint8_t)(seq & 0xFF);

    if (chunk_size > 0)
    {
        memcpy(&out_frame->data[9], chunk, chunk_size);
    }

    /* 원본 파이썬 기준으로 MCU data packet CRC는 실제 파일 chunk 바이트만 계산 */
    fw_proto_write_crc(out_frame->data, 9u + chunk_size, chunk, chunk_size);

    out_frame->data_len = (uint8_t)total_len;
    return FW_RESULT_OK;
}

FW_Result fw_build_mcu_end_request(uint8_t sensor_id, FW_CanFrame* out_frame)
{
    FW_Result ret = fw_proto_build_fixed_62_payload_frame(out_frame);
    if (ret != FW_RESULT_OK)
    {
        return ret;
    }

    out_frame->data[0] = FW_CMD_MCU_DOWNLOAD_END_REQ;
    out_frame->data[1] = 0x40;
    out_frame->data[2] = sensor_id;

    fw_proto_write_crc(out_frame->data, FW_PROTO_FIXED_BODY_LEN, out_frame->data, FW_PROTO_FIXED_BODY_LEN);
    return FW_RESULT_OK;
}

FW_Result fw_build_mmic_start_request(uint8_t sensor_id, uint32_t file_size, FW_CanFrame* out_frame)
{
    FW_Result ret = fw_proto_build_fixed_62_payload_frame(out_frame);
    if (ret != FW_RESULT_OK)
    {
        return ret;
    }

    out_frame->data[0] = FW_CMD_MMIC_DOWNLOAD_START_REQ;
    out_frame->data[1] = 0x40;
    out_frame->data[2] = sensor_id;
    out_frame->data[3] = (uint8_t)((file_size >> 24) & 0xFF);
    out_frame->data[4] = (uint8_t)((file_size >> 16) & 0xFF);
    out_frame->data[5] = (uint8_t)((file_size >> 8) & 0xFF);
    out_frame->data[6] = (uint8_t)(file_size & 0xFF);

    fw_proto_write_crc(out_frame->data, FW_PROTO_FIXED_BODY_LEN, out_frame->data, FW_PROTO_FIXED_BODY_LEN);
    return FW_RESULT_OK;
}

FW_Result fw_build_mmic_chunk_request(
    uint8_t sensor_id,
    const uint8_t* chunk,
    uint8_t chunk_size,
    FW_CanFrame* out_frame)
{
    size_t body_len;
    size_t total_len;

    if (out_frame == 0 || (chunk == 0 && chunk_size > 0))
    {
        return FW_RESULT_INVALID_ARG;
    }

    body_len = (size_t)3 + (size_t)chunk_size;
    total_len = body_len + (size_t)2;

    if (total_len > FW_MAX_CAN_DATA_LEN)
    {
        return FW_RESULT_INVALID_ARG;
    }

    fw_proto_init_frame(out_frame);
    memset(out_frame->data, 0, FW_MAX_CAN_DATA_LEN);

    out_frame->data[0] = FW_CMD_MMIC_DOWNLOAD_FILE_REQ;
    out_frame->data[1] = (uint8_t)(chunk_size + 5u);
    out_frame->data[2] = sensor_id;

    if (chunk_size > 0)
    {
        memcpy(&out_frame->data[3], chunk, chunk_size);
    }

    /* 원본 파이썬 기준으로 MMIC data packet CRC는 header+chunk 전체 */
    fw_proto_write_crc(out_frame->data, body_len, out_frame->data, body_len);

    out_frame->data_len = (uint8_t)total_len;
    return FW_RESULT_OK;
}

FW_Result fw_build_mmic_end_request(uint8_t sensor_id, FW_CanFrame* out_frame)
{
    FW_Result ret = fw_proto_build_fixed_62_payload_frame(out_frame);
    if (ret != FW_RESULT_OK)
    {
        return ret;
    }

    out_frame->data[0] = FW_CMD_MMIC_DOWNLOAD_END_REQ;
    out_frame->data[1] = 0x40;
    out_frame->data[2] = sensor_id;

    fw_proto_write_crc(out_frame->data, FW_PROTO_FIXED_BODY_LEN, out_frame->data, FW_PROTO_FIXED_BODY_LEN);
    return FW_RESULT_OK;
}

FW_Result fw_parse_common_response(const FW_CanFrame* frame, FW_ResponseCommon* out_res)
{
    if (frame == 0 || out_res == 0)
    {
        return FW_RESULT_INVALID_ARG;
    }

    if (frame->data_len < 4)
    {
        return FW_RESULT_PROTOCOL_ERROR;
    }

    out_res->cmd = frame->data[0];
    out_res->sensor_id = frame->data[2];
    out_res->result = frame->data[3];
    return FW_RESULT_OK;
}

FW_Result fw_parse_getdata_response(const FW_CanFrame* frame, FW_GetDataResponse* out_res)
{
    if (frame == 0 || out_res == 0)
    {
        return FW_RESULT_INVALID_ARG;
    }

    if (frame->data_len < 62)
    {
        return FW_RESULT_PROTOCOL_ERROR;
    }

    out_res->sensor_id = frame->data[2];
    out_res->result = frame->data[3];
    out_res->sensor_onoff = frame->data[60];
    out_res->static_obj_onoff = frame->data[61];
    return FW_RESULT_OK;
}