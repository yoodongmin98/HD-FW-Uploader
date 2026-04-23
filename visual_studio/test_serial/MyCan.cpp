#include "pch.h"

#include "MyCan.h"
#include "GuiInterface.h"
#include "MyDefine.h"


MyCan* MyCan::Cans = nullptr;

MyCan::MyCan()
{
	Cans = this;
}

MyCan::~MyCan()
{
	
}



void MyCan::CAN_Instance(const int _PacketID_idx, char* _Params, const int _IDsize)
{
	//ID가 있는 인덱스 위치 설정
	PACKET_ID_IDX = _PacketID_idx;
	//파라미터 설정
	params = _Params;

	//사용할 ID만큼 resize
	AllPacketData.resize(_IDsize);
	for (auto i = 0; i < AllPacketData.size(); ++i)
		AllPacketData[i].resize(64);
}



void MyCan::Send(TPCANHandle _BusID, std::vector<uint8_t> _Packet)
{
	status = 0;
	if (InitializeErrorCheck(status))
	{
		int DLCSize = LenToDlc(_Packet.size());

		TPCANMsgFD msg;
		msg.ID = _Packet[2];
		msg.DLC = DLCSize;
		msg.MSGTYPE = PCAN_MESSAGE_FD | PCAN_MESSAGE_BRS;
		//패킷 값 msg에 복사
		for (int i = 0; i < static_cast<int>(_Packet.size()); ++i)
			msg.DATA[i] = _Packet[i];

		{
			std::lock_guard<std::mutex> lock(CanWriteMutex);
			status = CAN_WriteFD(_BusID, &msg);
		}

		InitializeErrorCheck(status);

		std::vector<uint8_t> RecieveData;
		RecieveData = ReadData(_BusID, _Packet);
		{
			std::lock_guard<std::mutex> lock(CanDataMutex);
			AllPacketData[RecieveData[PACKET_ID_IDX - 1]] = RecieveData;
		}
	}
}



std::vector<uint8_t> MyCan::ReadData(TPCANHandle _BusID, std::vector<uint8_t> _Packet)
{
	std::vector<uint8_t> ReadData;
	TPCANMsgFD receivedMsg;
	receivedMsg.MSGTYPE = PCAN_MESSAGE_FD | PCAN_MESSAGE_BRS;
	for (int i = 0; i < 3; ++i)
	{
		status = CAN_ReadFD(_BusID, &receivedMsg, &timestamp);
		if (InitializeErrorCheck(status))
		{
			//받은 메세지가 요청했던 ID의 메세지가 아니라면 return
			if (_Packet[PACKET_ID_IDX] != receivedMsg.DATA[PACKET_ID_IDX])
				return ReadData;

			uint8_t actualSize = CAN_DLC_TO_LENGTH(receivedMsg.DLC);
			if (IsDebugPacket)
			{
				std::cout << "응답 수신됨! DLC = " << (int)receivedMsg.DLC << ", 전체 바이트 = ";
				for (int i = 0; i < actualSize; ++i)
					std::cout << std::hex << std::setw(2) << std::setfill('0') << (int)receivedMsg.DATA[i] << " ";
				std::cout << std::endl;
			}
			for (int i = 0; i < actualSize; ++i)
				ReadData.emplace_back(receivedMsg.DATA[i]);
			break;
		}
	}
	return ReadData;
}



void MyCan::ThreadRead(TPCANHandle bus)
{
	for (;;) {
		TPCANMsgFD rx{};
		TPCANTimestampFD ts{};
		TPCANStatus st = CAN_ReadFD(bus, &rx, &ts);
		if (st != 0) {
			// 비어있음: 천천히 폴링
			std::this_thread::sleep_for(std::chrono::milliseconds(1));
			continue;
		}

		const uint8_t len = CAN_DLC_TO_LENGTH(rx.DLC);
		std::vector<uint8_t> frame(rx.DATA, rx.DATA + len);

		// PACKET_ID_IDX 유효성
		if (PACKET_ID_IDX < 0 || static_cast<size_t>(PACKET_ID_IDX) >= frame.size()) {
			continue; // 잘못된 프레임
		}

		size_t logical_idx = frame[PACKET_ID_IDX];
		if (logical_idx == 0) continue; // 1-based라면 0은 무시
		if (logical_idx - 1 >= AllPacketData.size()) continue; // 경계

		{
			std::lock_guard<std::mutex> lock(CanDataMutex);
			AllPacketData[logical_idx - 1] = std::move(frame);
		}
	}
}



inline uint8_t MyCan::LenToDlc(size_t n) 
{
	if (n <= 8)  return static_cast<uint8_t>(n);
	if (n <= 12) return 9;
	if (n <= 16) return 10;
	if (n <= 20) return 11;
	if (n <= 24) return 12;
	if (n <= 32) return 13;
	if (n <= 48) return 14;
	return 15; // up to 64
}

inline uint8_t MyCan::CAN_DLC_TO_LENGTH(uint8_t dlc)
{
	static const uint8_t L[16] = { 0,1,2,3,4,5,6,7,8,12,16,20,24,32,48,64 };
	return (dlc <= 15) ? L[dlc] : 0;
}




void MyCan::OpenHandle(TPCANHandle _handle)
{
	status = CAN_InitializeFD(_handle, params);

	if (InitializeErrorCheck(status))
	{
		std::cout << "The handle has been successfully initialized." << std::endl;
		GuiInterface::GUI->SetErrorMessage("The handle has been successfully initialized.");
	}
}

void MyCan::CloseHandle(TPCANHandle _handle)
{
		status = CAN_Uninitialize(_handle);

	if (!status)
		GuiInterface::GUI->SetErrorMessage("The handle has been successfully Closed");
	else
		InitializeErrorCheck(status);
}


bool MyCan::InitializeErrorCheck(const int _ErrorNum)
{
	switch (_ErrorNum)
	{
	case 0x00000U:
	{
		return true;
	}
	case 0x00001U:
	{
		GuiInterface::GUI->SetErrorMessage("Transmit buffer in CAN controller is full");
		return false;
	}
	case 0x00002U:
	{
		GuiInterface::GUI->SetErrorMessage("CAN controller was read too late");
		return false;
	}
	case 0x00004U:
	{
		GuiInterface::GUI->SetErrorMessage("Bus error : an error counter reached the 'light' limit");
		return false;
	}
	case 0x00008U:
	{
		GuiInterface::GUI->SetErrorMessage("Bus error: an error counter reached the 'heavy' limit");
		return false;
	}
	case 0x40000U:
	{
		GuiInterface::GUI->SetErrorMessage("Bus error: the CAN controller is error passive");
		return false;
	}
	case 0x00010U:
	{
		GuiInterface::GUI->SetErrorMessage("Bus error : the CAN controller is in bus - off state");
		return false;
	}
	case 0x00020U:
	{
		GuiInterface::GUI->SetErrorMessage("Receive queue is empty");
		return false;
	}
	case 0x00040U:
	{
		GuiInterface::GUI->SetErrorMessage("Receive queue was read too late");
		return false;
	}
	case 0x00080U:
	{
		GuiInterface::GUI->SetErrorMessage("Transmit queue is full");
		return false;
	}
	case 0x00100U:
	{
		GuiInterface::GUI->SetErrorMessage("Test of the CAN controller hardware registers failed (no hardware found)");
		return false;
	}
	case 0x00200U:
	{
		GuiInterface::GUI->SetErrorMessage("Driver not loaded");
		return false;
	}
	case 0x00400U:
	{
		GuiInterface::GUI->SetErrorMessage("Hardware already in use by a Net");
		return false;
	}
	case 0x00800U:
	{
		GuiInterface::GUI->SetErrorMessage("A Client is already connected to the Net");
		return false;
	}
	case 0x01400U:
	{
		GuiInterface::GUI->SetErrorMessage("Hardware handle is invalid");
		return false;
	}
	case 0x01800U:
	{
		GuiInterface::GUI->SetErrorMessage("Net handle is invalid");
		return false;
	}
	case 0x01C00U:
	{
		GuiInterface::GUI->SetErrorMessage("Client handle is invalid");
		return false;
	}
	case 0x02000U:
	{
		GuiInterface::GUI->SetErrorMessage("Resource (FIFO, Client, timeout) cannot be created");
		return false;
	}
	case 0x04000U:
	{
		GuiInterface::GUI->SetErrorMessage("Invalid parameter");
		return false;
	}
	case 0x08000U:
	{
		GuiInterface::GUI->SetErrorMessage("Invalid parameter value");
		return false;
	}
	case 0x10000U:
	{
		GuiInterface::GUI->SetErrorMessage("Unknown error");
		return false;
	}
	case 0x20000U:
	{
		GuiInterface::GUI->SetErrorMessage("Invalid data, function, or action");
		return false;
	}
	case 0x80000U:
	{
		GuiInterface::GUI->SetErrorMessage("Driver object state is wrong for the attempted operation");
		return false;
	}
	case 0x2000000U:
	{
		GuiInterface::GUI->SetErrorMessage("An operation was successfully carried out, however, irregularities were registered");
		return false;
	}
	case 0x4000000U:
	{
		GuiInterface::GUI->SetErrorMessage("Channel is not initialized [Value was changed from 0x40000 to 0x4000000]");
		return false;
	}
	case 0x8000000U:
	{
		GuiInterface::GUI->SetErrorMessage("Invalid operation [Value was changed from 0x80000 to 0x8000000]");
		return false;
	}
	default:
	{
		GuiInterface::GUI->SetErrorMessage("Critical Error");
		return false;
	}
	}
	return false;
}