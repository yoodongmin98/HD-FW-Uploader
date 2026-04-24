//std
#include "pch.h"

#include "GuiInterface.h"

// c ///////////////////////////////////////
#include <cstdio>
#include <atomic>

#include "PCANBasic.h"

extern "C"
{
#include "fw_common.h"
#include "fw_can_pcan.h"
#include "fw_sensor_query.h"
#include "fw_getdata.h"
#include "fw_mcu_download.h"
#include "fw_mmic_download.h"
}
////////////////////////////////////////////

static std::atomic<bool> g_Cancel = false;
GuiInterface* GuiInterface::GUI = nullptr;
static void MyLogCallback(void* user, FW_LogLevel level, const char* text)
{
    const char* levelStr = "INFO";

    switch (level)
    {
    case FW_LOG_INFO:  levelStr = "INFO"; break;
    case FW_LOG_WARN:  levelStr = "WARN"; break;
    case FW_LOG_ERROR: levelStr = "ERROR"; break;
    case FW_LOG_DEBUG: levelStr = "DEBUG"; break;
    default: break;
    }

    std::printf("[%s] %s\n", levelStr, text ? text : "");
}

static void MyProgressCallback(void* user, const FW_Progress* progress)
{
    if (progress == nullptr)
    {
        return;
    }

    std::printf("sensor=%u, seq=%u, sent=%u/%u, %u%%\n",
        (unsigned int)progress->sensor_id,
        (unsigned int)progress->packet_seq,
        (unsigned int)progress->sent_bytes,
        (unsigned int)progress->total_bytes,
        (unsigned int)progress->percent);
}

static int MyCancelCallback(void* user)
{
    return g_Cancel.load() ? 1 : 0;
}
GuiInterface::GuiInterface()
{
	GUI = this;
}
GuiInterface::~GuiInterface()
{

}




void GuiInterface::Instance(ImGuiIO& io)
{
    static FW_CanHandle* CanHandle = nullptr;
    static FW_SensorList SensorList = {};
    static uint8_t SelectedSensorId = 1;
    static FW_GetDataResponse LastDataRes = {};
    static bool IsConnected = false;

    FW_Callbacks callbacks = {};
    callbacks.log_cb = MyLogCallback;
    callbacks.progress_cb = MyProgressCallback;
    callbacks.cancel_cb = MyCancelCallback;
    callbacks.user_data = nullptr;

    if (ImGui::Button("Open"))
    {
        if (CanHandle == nullptr)
        {
            FW_CanConfig canConfig = {};
            canConfig.channel = PCAN_USBBUS1;
            canConfig.nominal_bitrate = 1000000;
            canConfig.fd_enable = 1;

            canConfig.f_clock_mhz = 40;
            canConfig.nom_brp = 2;
            canConfig.nom_tseg1 = 10;
            canConfig.nom_tseg2 = 9;
            canConfig.nom_sjw = 8;

            canConfig.data_brp = 2;
            canConfig.data_tseg1 = 5;
            canConfig.data_tseg2 = 4;
            canConfig.data_sjw = 3;

            FW_Result result = fw_can_open(&CanHandle, &canConfig, &callbacks);
            if (result == FW_RESULT_OK)
            {
                IsConnected = true;
                std::printf("PCAN Open Success\n");
            }
            else
            {
                std::printf("fw_can_open failed : %d\n", result);
            }
        }
    }

    ImGui::SameLine();

    if (ImGui::Button("Scan"))
    {
        if (CanHandle != nullptr)
        {
            FW_Result result = fw_query_sensor_ids(CanHandle, &SensorList, 2000, &callbacks);
            if (result == FW_RESULT_OK)
            {
                std::printf("Sensor Count : %u\n", (unsigned int)SensorList.count);

                if (SensorList.count > 0)
                {
                    SelectedSensorId = SensorList.ids[0];
                    std::printf("Selected Sensor ID : %u\n", (unsigned int)SelectedSensorId);
                }
            }
            else
            {
                std::printf("fw_query_sensor_ids failed : %d\n", result);
            }
        }
    }

    ImGui::SameLine();

    if (ImGui::Button("GetData"))
    {
        if (CanHandle != nullptr)
        {
            FW_Result result = fw_get_sensor_data(CanHandle, 14, &LastDataRes, 2000, &callbacks);
            if (result == FW_RESULT_OK)
            {
                std::printf("GetData OK - Sensor=%u, SensorOnOff=%u, StaticObj=%u\n",
                    (unsigned int)LastDataRes.sensor_id,
                    (unsigned int)LastDataRes.sensor_onoff,
                    (unsigned int)LastDataRes.static_obj_onoff);
            }
            else
            {
                std::printf("fw_get_sensor_data failed : %d\n", result);
            }
        }
    }

    if (ImGui::Button("MCU Download"))
    {
        if (CanHandle != nullptr)
        {
            FW_McuDownloadOption mcuOpt = {};
            mcuOpt.sensor_id = 14;
            mcuOpt.file_path = "brs.bin";
            mcuOpt.send_timeout_ms = 2000;
            mcuOpt.recv_timeout_ms = 3000;
            mcuOpt.retry_count = 4;

            FW_Result result = fw_download_mcu_firmware(CanHandle, &mcuOpt, &callbacks);
            if (result == FW_RESULT_OK)
            {
                std::printf("MCU Download Success\n");
            }
            else
            {
                std::printf("fw_download_mcu_firmware failed : %d\n", result);
            }
        }
    }

    ImGui::SameLine();

   if (ImGui::Button("MMIC Download"))
    {
        if (CanHandle != nullptr)
        {
            FW_MmicDownloadOption mmicOpt = {};
            mmicOpt.sensor_id = 14;
            mmicOpt.file_path = "mmic.bin";
            mmicOpt.send_timeout_ms = 2000;
            mmicOpt.recv_timeout_ms = 4000;
            mmicOpt.retry_count = 3;

            FW_Result result = fw_download_mmic_firmware(CanHandle, &mmicOpt, &callbacks);
            if (result == FW_RESULT_OK)
            {
                std::printf("MMIC Download Success\n");
            }
            else
            {
                std::printf("fw_download_mmic_firmware failed : %d\n", result);
            }
        }
    }

    ImGui::SameLine();
    
    if (ImGui::Button("Disconnect"))
    {
        if (CanHandle != nullptr)
        {
            fw_can_close(CanHandle);
            CanHandle = nullptr;
            IsConnected = false;
            std::printf("PCAN Closed\n");
        }
    }

    ImGui::Text("Connected : %s", IsConnected ? "True" : "False");
    ImGui::Text("Sensor Count : %u", (unsigned int)SensorList.count);
    ImGui::Text("Selected Sensor ID : %u", (unsigned int)SelectedSensorId);
    ImGui::Text("SensorOnOff : %u", (unsigned int)LastDataRes.sensor_onoff);
    ImGui::Text("StaticObjOnOff : %u", (unsigned int)LastDataRes.static_obj_onoff);
}