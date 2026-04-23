#include "pch.h"
#include "EngineCore.h"
#include "MyImGui.h"
#include "MyTime.h"




EngineCore::EngineCore()
{
	Times = std::make_shared<MyTime>();
	ImGui_s = std::make_shared<MyImGui>();
}

EngineCore::~EngineCore()
{

}

void EngineCore::Instance()
{
	ImGui_s->Instance();
}