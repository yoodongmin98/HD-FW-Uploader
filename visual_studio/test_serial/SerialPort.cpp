#pragma once
#include "pch.h"
#include "SerialPort.h"
#include "MyDefine.h"
#include <imgui.h>
#include <serial/serial.h>

SerialPort* SerialPort::P_SerialPort = nullptr;

SerialPort::SerialPort()
{
	P_SerialPort = this;
}

SerialPort::~SerialPort()
{
}

void SerialPort::Instance(const bool _IsChild, ImVec2 _Size)
{
	//포트 리스트 GET
	std::vector<serial::PortInfo> PortInfos = serial::list_ports();

	//장치관리자 포트 리스트가 비어있으면 return
	if (!PortInfos.size())
		return;

	//프레임마다 도니까 전에 받은 포트의 데이터가 있으면 clear
	if (AllPort.size())
		AllPort.clear();

	//포트 리스트에서 COM부분만 추출해서 컨테이너에 push
	for (auto i = 0; i < PortInfos.size(); ++i)
	{
		AllPort.push_back(PortInfos[i].port.c_str());
	}

	//Interface생성
	MyWindow().Size(_Size).Pos(ImVec2{ 0,0 });
	if (_IsChild)
	{
		ImGuiBegin PortWindows("Port", true, nullptr);
	}
	else
	{
		ImGuiBeginChild PortWindows("Port");
	}
	//포트 선택하면 선택한 포트이름 대입
	if (ImGui::Combo("PortList", &SelectPort, AllPort.data(), AllPort.size()))
	{
		SerialPorts = AllPort[SelectPort];
	}

	if (ImGui::Combo("BaudRate", &SelectBaudRate, BaudrateArray.data(), BaudrateArray.size()))
	{
		BaudRate = std::stoi(BaudrateArray[SelectBaudRate]);
	}
}