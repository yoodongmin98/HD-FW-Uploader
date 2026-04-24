#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "fw_common.h"
#include "fw_can_pcan.h"
#include <stdint.h>

    enum
    {
        FW_CMD_GETDATA_REQ = 0x40,
        FW_CMD_GETDATA_RES = 0x41,

        FW_CMD_GET_SENSOR_ID_REQ = 0x45,
        FW_CMD_GET_SENSOR_ID_RES = 0x46,

        FW_CMD_MCU_DOWNLOAD_START_REQ = 0x50,
        FW_CMD_MCU_DOWNLOAD_START_RES = 0x51,
        FW_CMD_MCU_DOWNLOAD_FILE_REQ = 0x60,
        FW_CMD_MCU_DOWNLOAD_FILE_RES = 0x61,
        FW_CMD_MCU_DOWNLOAD_END_REQ = 0x70,
        FW_CMD_MCU_DOWNLOAD_END_RES = 0x71,

        FW_CMD_MMIC_DOWNLOAD_START_REQ = 0x80,
        FW_CMD_MMIC_DOWNLOAD_START_RES = 0x81,
        FW_CMD_MMIC_DOWNLOAD_FILE_REQ = 0x90,
        FW_CMD_MMIC_DOWNLOAD_FILE_RES = 0x91,
        FW_CMD_MMIC_DOWNLOAD_WAIT_RES = 0x92,
        FW_CMD_MMIC_DOWNLOAD_END_REQ = 0x95,
        FW_CMD_MMIC_DOWNLOAD_END_RES = 0x96
    };

    typedef struct FW_ResponseCommon
    {
        uint8_t cmd;
        uint8_t sensor_id;
        uint8_t result;
    } FW_ResponseCommon;

    typedef struct FW_GetDataResponse
    {
        uint8_t sensor_id;
        uint8_t result;
        uint8_t sensor_onoff;
        uint8_t static_obj_onoff;
    } FW_GetDataResponse;

    FW_Result fw_build_get_sensor_id_request(FW_CanFrame* out_frame);
    FW_Result fw_build_getdata_request(uint8_t sensor_id, FW_CanFrame* out_frame);

    FW_Result fw_build_mcu_start_request(uint8_t sensor_id, uint32_t file_size, FW_CanFrame* out_frame);
    FW_Result fw_build_mcu_chunk_request(
        uint8_t sensor_id,
        uint32_t offset,
        uint16_t seq,
        const uint8_t* chunk,
        uint8_t chunk_size,
        FW_CanFrame* out_frame);
    FW_Result fw_build_mcu_end_request(uint8_t sensor_id, FW_CanFrame* out_frame);

    FW_Result fw_build_mmic_start_request(uint8_t sensor_id, uint32_t file_size, FW_CanFrame* out_frame);
    FW_Result fw_build_mmic_chunk_request(
        uint8_t sensor_id,
        const uint8_t* chunk,
        uint8_t chunk_size,
        FW_CanFrame* out_frame);
    FW_Result fw_build_mmic_end_request(uint8_t sensor_id, FW_CanFrame* out_frame);

    FW_Result fw_parse_common_response(const FW_CanFrame* frame, FW_ResponseCommon* out_res);
    FW_Result fw_parse_getdata_response(const FW_CanFrame* frame, FW_GetDataResponse* out_res);

#ifdef __cplusplus
}
#endif