#pragma once
#include <iostream>



class MyTime;
class MyImGui;
class EngineCore
{
public:
	EngineCore();
	~EngineCore();

	void Instance();
protected:
private:
	std::shared_ptr<MyImGui> ImGui_s = nullptr;
	std::shared_ptr<MyTime> Times = nullptr;
};