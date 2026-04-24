#pragma once

////////////////////////////////////////////////
/// 공통 타입 , 결과 코드 , 콜백 , 공통 매크로  ///
////////////////////////////////////////////////

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stddef.h>

#define FW_MAX_CAN_DATA_LEN      64
#define FW_FIXED_PAYLOAD_LEN     62
#define FW_FIXED_FRAME_LEN       64
#define FW_MAX_SENSOR_COUNT      32
#define FW_MCU_CHUNK_SIZE        32
#define FW_MMIC_CHUNK_SIZE       30

    typedef enum FW_Result
    {
        FW_RESULT_OK = 0,
        FW_RESULT_FAIL = -1,
        FW_RESULT_INVALID_ARG = -2,
        FW_RESULT_OPEN_FAIL = -3,
        FW_RESULT_SEND_FAIL = -4,
        FW_RESULT_RECV_FAIL = -5,
        FW_RESULT_TIMEOUT = -6,
        FW_RESULT_PROTOCOL_ERROR = -7,
        FW_RESULT_CRC_ERROR = -8,
        FW_RESULT_SEQ_ERROR = -9,
        FW_RESULT_FILE_ERROR = -10,
        FW_RESULT_CANCELLED = -11
    } FW_Result;

    typedef enum FW_LogLevel
    {
        FW_LOG_INFO = 0,
        FW_LOG_WARN,
        FW_LOG_ERROR,
        FW_LOG_DEBUG
    } FW_LogLevel;

    typedef struct FW_Progress
    {
        uint32_t total_bytes;
        uint32_t sent_bytes;
        uint16_t packet_seq;
        uint8_t  sensor_id;
        uint8_t  percent;
    } FW_Progress;

    typedef void (*FW_LogCallback)(void* user, FW_LogLevel level, const char* text);
    typedef void (*FW_ProgressCallback)(void* user, const FW_Progress* progress);
    typedef int  (*FW_CancelCallback)(void* user);

    typedef struct FW_Callbacks
    {
        FW_LogCallback      log_cb;
        FW_ProgressCallback progress_cb;
        FW_CancelCallback   cancel_cb;
        void* user_data;
    } FW_Callbacks;

    typedef struct FW_SensorList
    {
        uint8_t ids[FW_MAX_SENSOR_COUNT];
        uint32_t count;
    } FW_SensorList;

#ifdef __cplusplus
}
#endif