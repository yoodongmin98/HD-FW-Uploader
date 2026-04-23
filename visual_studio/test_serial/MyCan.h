#pragma once
#include "pch.h"
#include "PCANBasic.h"



class MyCan
{
public:
	static MyCan* Cans;
	MyCan();
	~MyCan();

	//설정한 채널로 패킷을 보냅니다.
	void Send(TPCANHandle _BusID , std::vector<uint8_t> _Packet);
	void OpenHandle(TPCANHandle _handle);
	void CloseHandle(TPCANHandle _handle);
	void ThreadRead(TPCANHandle _BusID);

	//필수로 1회 호출해야합니다(CANID가 있는 IDX값 , 파라미터 , 사용할ID의Size)
	void CAN_Instance(const int _PacketID_idx , char* _Params, const int _IDsize);

	//디버그 패킷을 콘솔로 확인할지를 결정합니다.
	const void SetDebugPacket(bool _Debug)
	{
		IsDebugPacket = _Debug;
	}
protected:
	uint8_t CAN_DLC_TO_LENGTH(uint8_t _dlc);
	inline uint8_t LenToDlc(size_t n);
	bool InitializeErrorCheck(const int _ErrorNum);
	std::vector<uint8_t> ReadData(TPCANHandle _BusID, std::vector<uint8_t> _Packet);

private:
	//패킷의 데이터를 모아놓는 vector
	std::vector<std::vector<uint8_t>> AllPacketData;
	//파라미터 값을 담고있는 char*
	char* params = 0;
	//패킷ID의 IDX위치
	uint8_t PACKET_ID_IDX = 0;
	//오류 검수를 위한 Value
	TPCANStatus status = 0;
	TPCANTimestampFD timestamp = 0;

	//open , write mutex
	std::mutex CanDataMutex;
	std::mutex CanWriteMutex;

	//패킷을 디버깅할건지(콘솔로)
	bool IsDebugPacket = false;
};
