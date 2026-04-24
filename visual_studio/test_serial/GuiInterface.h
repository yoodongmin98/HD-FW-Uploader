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
protected:
	

private:
	std::string ErrorMessage = "NONE";
};