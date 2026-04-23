#pragma once


#define MYThreadCount 1


//기본 색상
#define Im4Red         ImVec4(1.0f, 0.0f, 0.0f, 1.0f)
#define Im4Green       ImVec4(0.0f, 1.0f, 0.0f, 1.0f)
#define Im4Blue        ImVec4(0.0f, 0.0f, 1.0f, 1.0f)
#define Im4Cyan        ImVec4(0.0f, 1.0f, 1.0f, 1.0f)
#define Im4Magenta     ImVec4(1.0f, 0.0f, 1.0f, 1.0f)
#define Im4Yellow      ImVec4(1.0f, 1.0f, 0.0f, 1.0f)
#define Im4Orange      ImVec4(1.0f, 0.55f, 0.0f, 1.0f)
#define Im4Purple      ImVec4(0.5f, 0.0f, 0.5f, 1.0f)

//무채색 계열
#define Im4White       ImVec4(1.0f, 1.0f, 1.0f, 1.0f)
#define Im4Black       ImVec4(0.0f, 0.0f, 0.0f, 1.0f)
#define Im4Gray        ImVec4(0.5f, 0.5f, 0.5f, 1.0f)
#define Im4LightGray   ImVec4(0.75f, 0.75f, 0.75f, 1.0f)
#define Im4DarkGray    ImVec4(0.25f, 0.25f, 0.25f, 1.0f)

//파스텔 톤
#define Im4PastelPink     ImVec4(1.0f, 0.8f, 0.86f, 1.0f)
#define Im4PastelBlue     ImVec4(0.68f, 0.85f, 0.9f, 1.0f)
#define Im4PastelGreen    ImVec4(0.6f, 0.98f, 0.6f, 1.0f)
#define Im4PastelPurple   ImVec4(0.8f, 0.7f, 0.9f, 1.0f)
#define Im4PastelYellow   ImVec4(1.0f, 1.0f, 0.8f, 1.0f)
#define Im4PastelOrange   ImVec4(1.0f, 0.85f, 0.7f, 1.0f)
#define Im4Lavender       ImVec4(0.902f, 0.902f, 0.980f, 1.0f)
#define Im4PastelLilac       ImVec4(0.78f, 0.64f, 0.78f, 1.0f) // 연보라
#define Im4Mint           ImVec4(0.74f, 0.98f, 0.78f, 1.0f)
#define Im4PastelMint        ImVec4(0.7f, 1.0f, 0.85f, 1.0f)   // 민트색
#define Im4SkyBlue        ImVec4(0.53f, 0.81f, 0.92f, 1.0f)
#define Im4Peach          ImVec4(1.0f, 0.9f, 0.8f, 1.0f)
#define Im4PastelCoral       ImVec4(1.0f, 0.7f, 0.6f, 1.0f)    // 연한 코랄
#define Im4PastelLemon       ImVec4(1.0f, 1.0f, 0.75f, 1.0f)   // 연노랑
#define Im4PastelAqua        ImVec4(0.64f, 0.94f, 0.96f, 1.0f) // 아쿠아톤


//딥톤
#define Im4Navy         ImVec4(0.0f, 0.0f, 0.5f, 1.0f)
#define Im4Maroon       ImVec4(0.5f, 0.0f, 0.0f, 1.0f)
#define Im4Olive        ImVec4(0.5f, 0.5f, 0.0f, 1.0f)
#define Im4Teal         ImVec4(0.0f, 0.5f, 0.5f, 1.0f)
#define Im4DarkCyan     ImVec4(0.0f, 0.55f, 0.55f, 1.0f)
#define Im4SteelBlue    ImVec4(0.27f, 0.51f, 0.71f, 1.0f)
#define Im4Indigo       ImVec4(0.29f, 0.0f, 0.51f, 1.0f)
#define Im4Crimson      ImVec4(0.86f, 0.08f, 0.24f, 1.0f)


//비비드 생동감 컬러
#define Im4VividRed          ImVec4(0.95f, 0.15f, 0.15f, 1.0f)
#define Im4VividGreen        ImVec4(0.0f, 0.8f, 0.2f, 1.0f)
#define Im4VividBlue         ImVec4(0.2f, 0.4f, 1.0f, 1.0f)
#define Im4VividOrange       ImVec4(1.0f, 0.45f, 0.1f, 1.0f)
#define Im4VividPurple       ImVec4(0.6f, 0.3f, 1.0f, 1.0f)
#define Im4VividPink         ImVec4(1.0f, 0.2f, 0.6f, 1.0f)
#define Im4VividTurquoise    ImVec4(0.0f, 0.85f, 0.8f, 1.0f)

struct MyWindow
{
	MyWindow() = default;

	MyWindow& Size(const ImVec2& size, ImGuiCond cond = 0)
	{
		ImGui::SetNextWindowSize(size, cond);
		return *this;
	}

	MyWindow& Pos(const ImVec2& pos, ImGuiCond cond = 0)
	{
		ImGui::SetNextWindowPos(pos, cond);
		return *this;
	}

	MyWindow& BgAlpha(float alpha)
	{
		ImGui::SetNextWindowBgAlpha(alpha);
		return *this;
	}
};


struct ImGuiBegin 
{
	bool opened = false;

	ImGuiBegin(const char* name, bool movable = false, bool* p_open = nullptr) 
	{
		ImGuiWindowFlags flags = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoCollapse;

		if (movable)
			flags &= ~ImGuiWindowFlags_NoMove;

		opened = ImGui::Begin(name, p_open, flags);
	}

	~ImGuiBegin() 
	{
		if (opened)
			ImGui::End();
	}

	operator bool() const { return opened; }
};


struct ImGuiBeginChild
{
	bool opened = false;

	ImGuiWindowFlags flags = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoCollapse;

	ImGuiBeginChild(const char* str_id, ImVec2 size = ImVec2(0, 0), bool border = false)
	{
		opened = ImGui::BeginChild(str_id, size, border, flags);
	}

	~ImGuiBeginChild()
	{
		if (opened)
			ImGui::EndChild();
	}

	operator bool() const { return opened; }
};


//ImGuiWindowFlags_None		플래그 없음(기본값)
//ImGuiWindowFlags_NoTitleBar		창 타이틀바(상단 제목줄)를 숨김
//ImGuiWindowFlags_NoResize		사용자가 창을 사이즈 조절할 수 없게 함
//ImGuiWindowFlags_NoMove		사용자가 창을 드래그로 이동할 수 없게 함
//ImGuiWindowFlags_NoScrollbar		스크롤바를 표시하지 않음(스크롤은 가능함)
//ImGuiWindowFlags_NoScrollWithMouse		마우스 휠로도 스크롤을 못하게 함
//ImGuiWindowFlags_NoCollapse		창의 축소 / 확장 버튼을 숨김(더블클릭으로 접는 거 비활성화됨)
//ImGuiWindowFlags_AlwaysAutoResize		매 프레임마다 콘텐츠 크기에 맞게 창 크기를 자동 조절함
//ImGuiWindowFlags_NoBackground		창 배경을 그리지 않음(투명해 보임)
//ImGuiWindowFlags_NoSavedSettings.ini	파일에 창 위치 / 크기 등을 저장하지 않음
//ImGuiWindowFlags_NoMouseInputs		창이 마우스 입력을 받지 않음(마우스 클릭 / 호버 등 무시)
//ImGuiWindowFlags_MenuBar		메뉴바 영역을 추가함(BeginMenuBar() 사용 가능)
//ImGuiWindowFlags_HorizontalScrollbar		수평 스크롤바를 활성화함
//ImGuiWindowFlags_NoFocusOnAppearing		처음 보일 때 포커스를 가져가지 않음
//ImGuiWindowFlags_NoBringToFrontOnFocus		클릭해도 다른 창 위로 올라오지 않음
//ImGuiWindowFlags_AlwaysVerticalScrollbar		수직 스크롤바를 항상 표시(필요 없어도)
//ImGuiWindowFlags_AlwaysHorizontalScrollbar		수평 스크롤바를 항상 표시
//ImGuiWindowFlags_NoNavInputs		키보드 / 게임패드 내비게이션 입력을 막음
//ImGuiWindowFlags_NoNavFocus		내비게이션 포커스를 받을 수 없음
//ImGuiWindowFlags_UnsavedDocument		제목 옆에 점(dot)을 표시(예: 저장 안 된 문서 표시)


struct MyStyle
{
	int colorCount = 0;
	int varCount = 0;
	int fontCount = 0;

	MyStyle() = default;

	MyStyle& PushColor(ImGuiCol idx, const ImVec4& col)
	{
		ImGui::PushStyleColor(idx, col);
		colorCount++;
		return *this;
	}

	MyStyle& PushVar(ImGuiStyleVar idx, float val)
	{
		ImGui::PushStyleVar(idx, val);
		varCount++;
		return *this;
	}

	MyStyle& PushVar(ImGuiStyleVar idx, const ImVec2& val)
	{
		ImGui::PushStyleVar(idx, val);
		varCount++;
		return *this;
	}

	MyStyle& PushFont(ImFont* font)
	{
		if (font)
		{
			ImGui::PushFont(font);
			fontCount++;
		}
		return *this;
	}

	void SetCur(const float _X, const float _Y)
	{
		ImGui::SetCursorPos(ImVec2{ _X,_Y });
	}

	~MyStyle()
	{
		if (colorCount) ImGui::PopStyleColor(colorCount);
		if (varCount) ImGui::PopStyleVar(varCount);
		if (fontCount) ImGui::PopFont();
	}
};

///////////////////////////////////////////////ImCol//////////////////////////////////////////////
//																								// 	
//		ImGuiCol_Text,                    // 일반 텍스트 색상										//
//		ImGuiCol_TextDisabled,           // 비활성화된 텍스트 색상 (예: 체크 해제 상태 등)			//
//																								//
//		ImGuiCol_WindowBg,               // 일반 윈도우 배경 색상									//
//		ImGuiCol_ChildBg,                // 자식 윈도우 배경 색상 (ChildWindow, ChildFrame 등)	//
//		ImGuiCol_PopupBg,                // 팝업, 메뉴, 툴팁 등의 배경 색상					    //
//																								//
//		ImGuiCol_Border,                 // 테두리 색상											//
//		ImGuiCol_BorderShadow,           // 테두리 그림자 색상 (현재 거의 사용되지 않음)		    //
//																								//
//		ImGuiCol_FrameBg,                // 프레임 배경 색상 (체크박스, 슬라이더, 텍스트 입력 등)	//
//		ImGuiCol_FrameBgHovered,         // 프레임 위에 마우스가 올라갔을 때 색상					//
//		ImGuiCol_FrameBgActive,          // 프레임이 활성 상태일 때 색상						    //
//																								//
//		ImGuiCol_TitleBg,                // 윈도우 타이틀바 기본 배경								//
//		ImGuiCol_TitleBgActive,          // 타이틀바가 포커스를 가졌을 때							//
//		ImGuiCol_TitleBgCollapsed,       // 타이틀바가 접혀 있을 때								//
//																								//
//		ImGuiCol_MenuBarBg,              // 메뉴 바 배경 색상										//
//																								//
//		ImGuiCol_ScrollbarBg,            // 스크롤바 배경											//
//		ImGuiCol_ScrollbarGrab,          // 스크롤바 핸들 기본 색									//
//		ImGuiCol_ScrollbarGrabHovered,   // 마우스 오버 상태의 스크롤바 핸들					    //
//		ImGuiCol_ScrollbarGrabActive,    // 클릭한 상태의 스크롤바 핸들						    //
//																								//
//		ImGuiCol_CheckMark,              // 체크박스 체크 표시 색상								//
//		ImGuiCol_SliderGrab,             // 슬라이더 핸들 기본 색									//
//		ImGuiCol_SliderGrabActive,       // 클릭한 슬라이더 핸들 색								//
//																								//
//		ImGuiCol_Button,                 // 버튼 기본 색상										//
//		ImGuiCol_ButtonHovered,          // 버튼 위에 마우스가 올라갔을 때							//
//		ImGuiCol_ButtonActive,           // 버튼 클릭 상태										//
//																								//
//		ImGuiCol_Header,                 // 헤더 영역 기본 색상 (CollapsingHeader, TreeNode 등)   //
//		ImGuiCol_HeaderHovered,          // 헤더에 마우스 오버 상태								//
//		ImGuiCol_HeaderActive,           // 헤더 클릭 상태										//
//																								//
//		ImGuiCol_Separator,              // 구분선 기본 색										//
//		ImGuiCol_SeparatorHovered,       // 구분선 마우스 오버								    //
//		ImGuiCol_SeparatorActive,        // 구분선 클릭 상태										//
//																								//
//		ImGuiCol_ResizeGrip,             // 리사이즈 핸들 기본 색 (하단/우측 모서리)				//
//		ImGuiCol_ResizeGripHovered,      // 마우스 오버 상태										//
//		ImGuiCol_ResizeGripActive,       // 클릭 상태											//
//																								//
//		ImGuiCol_TabHovered,             // 탭 위에 마우스 오버 시								//
//		ImGuiCol_Tab,                    // 기본 탭 색상 (선택되지 않은 탭)					    //
//		ImGuiCol_TabSelected,            // 선택된 탭 색상										//
//		ImGuiCol_TabSelectedOverline,    // 선택된 탭의 상단 강조선								//
//		ImGuiCol_TabDimmed,              // 포커스를 잃은 상태의 탭								//
//		ImGuiCol_TabDimmedSelected,      // 포커스를 잃은 상태에서 선택된 탭					    //
//		ImGuiCol_TabDimmedSelectedOverline, // 위와 같은 상태에서 강조선 색상						//
//																								//
//		ImGuiCol_PlotLines,              // 선 그래프 기본 선 색									//
//		ImGuiCol_PlotLinesHovered,       // 선 그래프에 마우스 오버								//
//		ImGuiCol_PlotHistogram,          // 히스토그램 바 색상								    //
//		ImGuiCol_PlotHistogramHovered,   // 히스토그램 바 마우스 오버								//
//																								//
//		ImGuiCol_TableHeaderBg,          // 테이블 헤더 배경										//
//		ImGuiCol_TableBorderStrong,      // 테이블 외곽선 (두꺼운 테두리)							//
//		ImGuiCol_TableBorderLight,       // 테이블 내부선 (얇은 테두리)							//
//		ImGuiCol_TableRowBg,             // 테이블 짝수 행 배경								    //
//		ImGuiCol_TableRowBgAlt,          // 테이블 홀수 행 배경								    //
//																								//
//		ImGuiCol_TextLink,               // 하이퍼링크 색상 (예: URL 등)							//
//		ImGuiCol_TextSelectedBg,         // 텍스트 선택 시 배경 색								//
//																								//
//		ImGuiCol_DragDropTarget,         // 드래그&드롭 대상 강조 색								//
//																								//
//		ImGuiCol_NavHighlight,           // 키보드/게임패드 내비게이션 포커스 항목 강조				//
//		ImGuiCol_NavWindowingHighlight,  // Ctrl+Tab 시 선택된 윈도우 강조						//
//		ImGuiCol_NavWindowingDimBg,      // Ctrl+Tab 창 리스트 외곽 어둡게 처리					//
//																								//
//		ImGuiCol_ModalWindowDimBg,       // 모달 윈도우 표시 중 배경 어둡게 처리					//
//																								//
//////////////////////////////////////////////////////////////////////////////////////////////////



////////////////////////////////////////////////////////ImVar///////////////////////////////////////////////////////
//																												  //
//		ImGuiStyleVar_Alpha,                    // float     전체 UI 투명도 (0.0~1.0)							  //	
//		ImGuiStyleVar_DisabledAlpha,            // float     비활성화된 요소의 투명도 (0.0~1.0)					  //
//		ImGuiStyleVar_WindowPadding,            // ImVec2    창 내부 여백 (컨텐츠와 창 테두리 사이의 간격)			  //
//		ImGuiStyleVar_WindowRounding,           // float     창 모서리 둥글기 반지름								  //
//		ImGuiStyleVar_WindowBorderSize,         // float     창 테두리 두께										  //
//		ImGuiStyleVar_WindowMinSize,            // ImVec2    창 최소 크기										  //
//		ImGuiStyleVar_WindowTitleAlign,         // ImVec2    제목 정렬 위치 (0.0: 왼쪽, 0.5: 중앙, 1.0: 오른쪽)	  //
//		ImGuiStyleVar_ChildRounding,            // float     Child 윈도우 모서리 둥글기							  //
//		ImGuiStyleVar_ChildBorderSize,          // float     Child 윈도우 테두리 두께								  //
//		ImGuiStyleVar_PopupRounding,            // float     팝업 창의 모서리 둥글기								  //
//		ImGuiStyleVar_PopupBorderSize,          // float     팝업 창의 테두리 두께								  //
//		ImGuiStyleVar_FramePadding,             // ImVec2    프레임 내부 패딩 (버튼, 입력창 내부 여백)				  //
//		ImGuiStyleVar_FrameRounding,            // float     프레임 모서리 둥글기 (버튼 등)						  //
//		ImGuiStyleVar_FrameBorderSize,          // float     프레임 테두리 두께									  //
//		ImGuiStyleVar_ItemSpacing,              // ImVec2    요소 간 간격 (버튼 간 거리 등)						  //
//		ImGuiStyleVar_ItemInnerSpacing,         // ImVec2    내부 요소 간 간격 (텍스트와 체크박스 등)				  //
//		ImGuiStyleVar_IndentSpacing,            // float     들여쓰기 간격 (TreeNode, Indent 등)					  //
//		ImGuiStyleVar_CellPadding,              // ImVec2    테이블 셀 내부 여백									  //
//		ImGuiStyleVar_ScrollbarSize,            // float     스크롤바 두께										  //
//		ImGuiStyleVar_ScrollbarRounding,        // float     스크롤바 모서리 둥글기								  //
//		ImGuiStyleVar_GrabMinSize,              // float     슬라이더, 스크롤바 등에서 드래그 가능한 최소 크기		  //
//		ImGuiStyleVar_GrabRounding,             // float     드래그 핸들의 둥글기									  //
//		ImGuiStyleVar_TabRounding,              // float     탭 모서리 둥글기										  //
//		ImGuiStyleVar_TabBorderSize,            // float     탭 테두리 두께										  //
//		ImGuiStyleVar_TabBarBorderSize,         // float     탭바 전체의 테두리 두께								  //
//		ImGuiStyleVar_TabBarOverlineSize,       // float     선택된 탭의 윗줄 두께 (강조선)						  //
//		ImGuiStyleVar_TableAngledHeadersAngle,  // float     각진 테이블 헤더의 기울기 각도						  //
//		ImGuiStyleVar_TableAngledHeadersTextAlign, // ImVec2 텍스트 정렬 (각진 헤더 내부에서의 위치)				  //
//		ImGuiStyleVar_ButtonTextAlign,          // ImVec2    버튼 안에서의 텍스트 정렬 위치						  //
//		ImGuiStyleVar_SelectableTextAlign,      // ImVec2    Selectable 요소 안의 텍스트 정렬 위치				  //
//		ImGuiStyleVar_SeparatorTextBorderSize,  // float     구분선 텍스트의 외곽선 크기							  //
//		ImGuiStyleVar_SeparatorTextAlign,       // ImVec2    구분선 텍스트의 정렬 (좌우)							  //
//		ImGuiStyleVar_SeparatorTextPadding,     // ImVec2    구분선 텍스트의 패딩									  //
//																												  //
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


struct TextLay
{
	ImVec2 pos;
	float spacing;


	TextLay(ImVec2 start) : pos(start)
	{
		spacing = ImGui::GetTextLineHeightWithSpacing();
	}

	template<typename T>
	void Row(const char* label, ImVec4 color, T value)
	{
		ImGui::SetCursorPos(pos);

		if constexpr (std::is_integral_v<T>)
			ImGui::TextColored(color, "%s : %d", label, value);
		else if constexpr (std::is_floating_point_v<T>)
			ImGui::TextColored(color, "%s : %.2f", label, value);
		else if constexpr (std::is_same_v<T, std::string>)
			ImGui::TextColored(color, "%s : %s", label, value.c_str());
		else
			static_assert(sizeof(T) == 0, "Unsupported type for TextLay::Row");

		pos.y += spacing;
	}
};