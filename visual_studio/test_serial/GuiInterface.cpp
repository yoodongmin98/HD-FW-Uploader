//std
#include "pch.h"


#include <imgui_internal.h>
#include "MyImGui.h"
#include "MyDefine.h"
#include "GuiInterface.h"


GuiInterface* GuiInterface::GUI = nullptr;

GuiInterface::GuiInterface()
{
	GUI = this;
}
GuiInterface::~GuiInterface()
{

}



void GuiInterface::Instance(ImGuiIO& io)
{
	//LoopStart
}


//shell 단에 명령어 보내주는 함수
std::string GuiInterface::executeCommand(std::string command)
{
	std::array<char, 128> buffer;
	std::string result;

	FILE* pipe = _popen(command.c_str(), "r");
	if (!pipe)
	{
		std::cerr << "Error: Unable to open pipe for command: " << command << std::endl;
		return result;
	}

	while (fgets(buffer.data(), static_cast<int>(buffer.size()), pipe) != nullptr)
	{
		result += buffer.data();
	}

	_pclose(pipe);

	return result;
}


//빙글빙글 돌아가는 스피너~(로딩)
void GuiInterface::Spinner(const char* label, float radius, float thickness, const ImVec4& color)
{
	ImGuiWindow* window = ImGui::GetCurrentWindow();
	if (window->SkipItems) return;

	ImGuiContext& g = *GImGui;
	const ImGuiID id = window->GetID(label);
	const ImVec2 pos = ImGui::GetCursorScreenPos();
	const float time = (float)ImGui::GetTime();

	ImDrawList* draw_list = ImGui::GetWindowDrawList();

	const int num_segments = 30;
	const float start = fabsf(sinf(time * 1.8f)) * (num_segments - 5);

	const float a_min = IM_PI * 2.0f * ((float)start) / (float)num_segments;
	const float a_max = IM_PI * 2.0f * ((float)num_segments - 3) / (float)num_segments;

	const ImVec2 centre = ImVec2(pos.x + radius, pos.y + radius);

	for (int i = 0; i < num_segments; i++)
	{
		const float a = a_min + ((float)i / (float)num_segments) * (a_max - a_min);
		ImVec2 p1 = ImVec2(centre.x + cosf(a) * radius, centre.y + sinf(a) * radius);
		ImVec2 p2 = ImVec2(centre.x + cosf(a + 0.1f) * radius, centre.y + sinf(a + 0.1f) * radius);

		draw_list->AddLine(p1, p2, ImColor(color), thickness);
	}

	ImGui::Dummy(ImVec2((radius) * 2, (radius) * 2));
}


//경로에서 파일이름 추출
std::string GuiInterface::ExtractFileName(std::string _Path)
{
	size_t pos = _Path.find_last_of("\\");
	std::string filenames = (pos == std::string::npos) ? _Path : _Path.substr(pos + 1);
	return filenames;
}


//워터마크 (TextMultiline뒤에 써야함)
void GuiInterface::TextWaterMark(std::string& _Text, ImVec2 _Pos, ImVec2 _Padding)
{
	ImVec2 textPos = ImVec2(_Pos.x + _Padding.x, _Pos.y + _Padding.y);
	ImGui::GetWindowDrawList()->AddText
	(
		textPos,
		IM_COL32(150, 150, 150, 255),
		_Text.c_str()
	);
}


//파일 오픈 함수
std::string GuiInterface::OpenFileDialog(const std::string& description, const std::vector<std::string>& extensions)
{
	std::vector<char> filter;

	// 설명 추가 (e.g. "Firmware Files\0")
	std::string desc = description + " Files";
	filter.insert(filter.end(), desc.begin(), desc.end());
	filter.push_back('\0');

	// 확장자 추가 (e.g. "*.bin;*.hex;*.elf\0")
	for (size_t i = 0; i < extensions.size(); ++i)
	{
		std::string ext = "*." + extensions[i];
		filter.insert(filter.end(), ext.begin(), ext.end());
		if (i != extensions.size() - 1)
			filter.push_back(';');
	}
	filter.push_back('\0');

	// All Files 추가
	std::string allDesc = "All Files";
	filter.insert(filter.end(), allDesc.begin(), allDesc.end());
	filter.push_back('\0');

	std::string allPattern = "*.*";
	filter.insert(filter.end(), allPattern.begin(), allPattern.end());
	filter.push_back('\0');  // 마지막 이중 널 문자

	char fileName[MAX_PATH] = { 0 };
	OPENFILENAME ofn;
	ZeroMemory(&ofn, sizeof(ofn));
	ofn.lStructSize = sizeof(ofn);
	ofn.hwndOwner = nullptr;
	ofn.lpstrFilter = filter.data(); 
	ofn.lpstrFile = fileName;
	ofn.nMaxFile = MAX_PATH;
	ofn.Flags = OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST;

	if (GetOpenFileName(&ofn))
	{
		return std::string(fileName);
	}
	return "";
}


//파일 저장 함수
std::string GuiInterface::SaveFileDialog(const std::string& description, const std::vector<std::string>& extensions, const std::string& defaultExt)
{
	std::vector<char> filter;

	// 설명 추가
	std::string desc = description + " Files";
	filter.insert(filter.end(), desc.begin(), desc.end());
	filter.push_back('\0');

	// 확장자 패턴 추가 (예: *.txt;*.log)
	for (size_t i = 0; i < extensions.size(); ++i)
	{
		std::string ext = "*." + extensions[i];
		filter.insert(filter.end(), ext.begin(), ext.end());
		if (i != extensions.size() - 1)
			filter.push_back(';');
	}
	filter.push_back('\0');

	// All Files 추가
	std::string allDesc = "All Files";
	filter.insert(filter.end(), allDesc.begin(), allDesc.end());
	filter.push_back('\0');

	std::string allPattern = "*.*";
	filter.insert(filter.end(), allPattern.begin(), allPattern.end());
	filter.push_back('\0');  // 마지막 이중 null 종료

	// 파일 저장 다이얼로그 구성
	OPENFILENAME ofn;
	char szFile[MAX_PATH] = { 0 };
	ZeroMemory(&ofn, sizeof(ofn));
	ofn.lStructSize = sizeof(ofn);
	ofn.hwndOwner = nullptr;
	ofn.lpstrFilter = filter.data();
	ofn.lpstrFile = szFile;
	ofn.nMaxFile = MAX_PATH;
	ofn.Flags = OFN_OVERWRITEPROMPT | OFN_PATHMUSTEXIST;
	ofn.lpstrDefExt = defaultExt.c_str();

	if (GetSaveFileName(&ofn))
	{
		std::wcout << L"파일 저장 경로: " << szFile << std::endl;
		return std::string(szFile);
	}
	else
	{
		std::wcout << L"파일 저장 취소됨" << std::endl;
		return "";
	}
}




//리틀에디안 형식의 hex데이터를 float형식으로 바꿔주는 함수(static_cast)
float GuiInterface::ParseLittleEndian_Float(const std::vector<uint8_t>& _data, size_t _offset , size_t _size)
{
	if (_offset + 4 > _data.size())
		throw std::out_of_range("Offset + size out of bounds");

	uint32_t temp = 0;
	for (size_t i = 0; i < _size; ++i)
	{
		temp |= static_cast<uint32_t>(_data[_offset + i]) << (8 * i);
	}

	float result;
	std::memcpy(&result, &temp, sizeof(float));
	return result;
}