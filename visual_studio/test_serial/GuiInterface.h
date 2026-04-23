#pragma once
#include "pch.h"
#include <serial/serial.h>
#include "imgui.h"






class GuiInterface
{
public:
	static GuiInterface* GUI;
	GuiInterface();
	~GuiInterface();

	void SetErrorMessage(const std::string _Error)
	{
		ErrorMessage = _Error;
	}

	void Instance(ImGuiIO& io);
	void Spinner(const char* label, float radius, float thickness, const ImVec4& color);
	void TextWaterMark(std::string& _Text, ImVec2 _Pos, ImVec2 _Padding);
	std::string executeCommand(std::string _Command);
	std::string ExtractFileName(std::string _FileName);
	std::string OpenFileDialog(const std::string& description, const std::vector<std::string>& extensions);
	std::string SaveFileDialog(const std::string& description, const std::vector<std::string>& extensions, const std::string& defaultExt);


	//리틀에디안 형식의 hex데이터를 int값으로 바꿔주는함수(비트시프트)
	template<typename T>
	T ParseLittleEndian(const std::vector<uint8_t>& data, size_t offset)
	{
		const size_t size = sizeof(T);
		if (offset + size > data.size())
			throw std::out_of_range("Offset + size out of bounds");

		uint64_t result = 0;

		for (size_t i = 0; i < size; ++i)
		{
			result |= static_cast<uint64_t>(data[offset + i]) << (8 * i);
		}

		// 반환 타입이 signed인 경우를 위해 처리
		if constexpr (std::is_signed<T>::value)
		{
			// result를 T 크기만큼 sign-extension 시키기 위해 캐스팅
			using SignedT = typename std::make_signed<T>::type;
			return static_cast<SignedT>(result);
		}
		else
		{
			return static_cast<T>(result);
		}
	}

	float ParseLittleEndian_Float(const std::vector<uint8_t>& data, size_t offset, size_t size);
protected:
	

private:
	//포트
	int SelectPort = 0;
	std::vector<serial::PortInfo> PortInfos;
	std::vector<const char*> AllPort;
	//Error Message
	std::string ErrorMessage = "NONE";
};