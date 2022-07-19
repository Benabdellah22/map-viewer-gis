static void set_imgui_font( mv::string filename ) {
  ImGuiIO& io = ImGui::GetIO(); (void)io;
  io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;    //Enable Docking

  ImFont* ftest = io.Fonts->AddFontFromFileTTF( filename.c_str(), 16.0f );
}

static void set_imgui_style_dark( void ) {
  ImGuiStyle* style = &ImGui::GetStyle();
  style->FrameRounding = 2.0f;
  style->TabRounding = style->FrameRounding;
  style->WindowMenuButtonPosition = -1;
  style->ColorButtonPosition = 0;

  //Set UI 
  ImVec4* colors = ImGui::GetStyle().Colors;

  //Dark blue-----------------------------------------------------------------------------------------------------
  colors[ImGuiCol_WindowBg] = ImVec4( 0.10f, 0.10f, 0.12f, 1.00f );
  colors[ImGuiCol_Text] = ImVec4( 1.00f, 0.98f, 0.95f, 1.00f );
  colors[ImGuiCol_TextDisabled] = ImVec4( 0.18f, 0.18f, 0.18f, 1.00f );
  //Window Title
  colors[ImGuiCol_TitleBg] = ImVec4( 0.00f, 0.00f, 0.00f, 1.00f );  //colors[ImGuiCol_TitleBg] = ImVec4(0.03f, 0.03f, 0.03f, 1.00f);
  colors[ImGuiCol_TitleBgActive] = ImVec4( 0.07f, 0.07f, 0.07f, 1.00f );
  //Frame
  colors[ImGuiCol_FrameBg] = ImVec4(0.27f, 0.27f, 0.27f, 1.00f);
  colors[ImGuiCol_FrameBgHovered] = ImVec4( 0.05f, 0.58f, 0.71f, 0.31f );
  colors[ImGuiCol_FrameBgActive] = ImVec4( 0.05f, 0.58f, 0.71f, 0.31f );
  //Tab
  colors[ImGuiCol_Tab] = ImVec4( 0.05f, 0.58f, 0.71f, 0.35f );
  colors[ImGuiCol_TabActive] = ImVec4( 0.05f, 0.58f, 0.71f, 1.00f );
  colors[ImGuiCol_TabUnfocused] = ImVec4( 0.05f, 0.58f, 0.71f, 0.35f );
  colors[ImGuiCol_TabUnfocusedActive] = ImVec4( 0.05f, 0.58f, 0.71f, 1.00f );
  colors[ImGuiCol_TabHovered] = ImVec4( 0.05f, 0.58f, 0.71f, 1.00f );
  //Buttons
  colors[ImGuiCol_Button] = ImVec4( 0.05f, 0.58f, 0.71f, 1.00f );
  colors[ImGuiCol_ButtonHovered] = ImVec4( 0.39f, 0.88f, 1.00f, 1.00f );
  colors[ImGuiCol_ButtonActive] = ImVec4(0.39f, 0.88f, 1.00f, 1.00f); //colors[ImGuiCol_ButtonActive] = ImVec4(0.05f, 0.58f, 0.71f, 1.00f);
  //Table data
  colors[ImGuiCol_TableHeaderBg] = ImVec4( 0.05f, 0.05f, 0.05f, 1.00f );
  colors[ImGuiCol_TableRowBg] = ImVec4( 0.14f, 0.14f, 0.14f, 1.00f );
  colors[ImGuiCol_TableRowBgAlt] = ImVec4( 0.13f, 0.13f, 0.13f, 1.00f );
  colors[ImGuiCol_TableBorderLight] = ImVec4(0.22f, 0.22f, 0.22f, 1.00f);;
  //Header
  colors[ImGuiCol_Header] = ImVec4(0.05f, 0.58f, 0.71f, 0.31f); //zz
  colors[ImGuiCol_HeaderHovered] = ImVec4( 0.05f, 0.58f, 0.71f, 1.00f );
  colors[ImGuiCol_HeaderActive] = ImVec4( 0.05f, 0.58f, 0.71f, 1.00f );
  colors[ImGuiCol_ScrollbarBg] = ImVec4( 0.01f, 0.01f, 0.01f, 0.53f );
  colors[ImGuiCol_CheckMark] = ImVec4( 0.05f, 0.58f, 0.71f, 1.00f );
  colors[ImGuiCol_SliderGrab] = ImVec4( 0.05f, 0.58f, 0.71f, 1.00f );
  colors[ImGuiCol_SliderGrabActive] = ImVec4( 0.05f, 0.58f, 0.71f, 1.00f );
  colors[ImGuiCol_SeparatorHovered] = ImVec4( 0.05f, 0.58f, 0.71f, 1.00f );
  colors[ImGuiCol_SeparatorActive] = ImVec4( 0.05f, 0.58f, 0.71f, 1.00f );
  colors[ImGuiCol_ResizeGrip] = ImVec4( 0.05f, 0.58f, 0.71f, 1.00f );
  colors[ImGuiCol_ResizeGripHovered] = ImVec4( 0.05f, 0.58f, 0.71f, 1.00f );
  colors[ImGuiCol_ResizeGripActive] = ImVec4( 0.05f, 0.58f, 0.71f, 1.00f );
  colors[ImGuiCol_TextSelectedBg] = ImVec4( 0.05f, 0.58f, 0.71f, 0.74f );
  colors[ImGuiCol_NavHighlight] = ImVec4( 0.05f, 0.58f, 0.71f, 1.00f );
  colors[ImGuiCol_ChildBg] = ImVec4( 0.00f, 0.00f, 0.00f, 0.00f );
  colors[ImGuiCol_PopupBg] = ImVec4(0.13f, 0.13f, 0.13f, 1.00f);
  colors[ImGuiCol_Border] = ImVec4( 0.00f, 0.00f, 0.00f, 0.50f );
  colors[ImGuiCol_BorderShadow] = ImVec4( 0.02f, 0.02f, 0.02f, 0.00f );
  colors[ImGuiCol_TitleBgCollapsed] = ImVec4( 0.00f, 0.00f, 0.00f, 0.51f );
  colors[ImGuiCol_MenuBarBg] = ImVec4( 0.14f, 0.14f, 0.14f, 1.00f );
  colors[ImGuiCol_ScrollbarGrab] = ImVec4( 0.15f, 0.16f, 0.20f, 1.00f );
  colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4( 0.22f, 0.24f, 0.30f, 1.00f );
  colors[ImGuiCol_ScrollbarGrabActive] = ImVec4( 0.22f, 0.24f, 0.30f, 1.00f );
  colors[ImGuiCol_Separator] = ImVec4( 0.06f, 0.06f, 0.06f, 0.50f );
  colors[ImGuiCol_DockingEmptyBg] = ImVec4( 0.12f, 0.12f, 0.12f, 1.00f );
  colors[ImGuiCol_PlotLines] = ImVec4( 0.62f, 0.62f, 0.62f, 1.00f );
  colors[ImGuiCol_PlotLinesHovered] = ImVec4( 1.00f, 0.43f, 0.35f, 1.00f );
  colors[ImGuiCol_PlotHistogram] = ImVec4( 0.90f, 0.70f, 0.00f, 1.00f );
  colors[ImGuiCol_PlotHistogramHovered] = ImVec4( 1.00f, 0.60f, 0.00f, 1.00f );
  colors[ImGuiCol_DragDropTarget] = ImVec4( 1.00f, 1.00f, 0.00f, 0.90f );
  colors[ImGuiCol_NavWindowingHighlight] = ImVec4( 1.00f, 1.00f, 1.00f, 0.70f );
  colors[ImGuiCol_NavWindowingDimBg] = ImVec4( 0.80f, 0.80f, 0.80f, 0.20f );
  colors[ImGuiCol_ModalWindowDimBg] = ImVec4( 0.80f, 0.80f, 0.80f, 0.35f );
  colors[ImGuiCol_TableBorderStrong] = ImVec4( 0.18f, 0.18f, 0.18f, 1.00f );
  //docking
  colors[ImGuiCol_DockingPreview] = ImVec4( 0.05f, 0.58f, 0.71f, 0.46f );
}

static void set_imgui_style_light( void ) {
  ImGuiStyle* style = &ImGui::GetStyle();
  style->FrameRounding = 2.0f;
  style->TabRounding = style->FrameRounding;
  style->WindowMenuButtonPosition = -1;
  style->ColorButtonPosition = 0;

  //Set UI 
  ImVec4* colors = ImGui::GetStyle().Colors;

  colors[ImGuiCol_Text] = ImVec4( 0.00f, 0.00f, 0.00f, 1.00f );
  colors[ImGuiCol_TextDisabled] = ImVec4( 0.60f, 0.60f, 0.60f, 1.00f );
  colors[ImGuiCol_WindowBg] = ImVec4( 0.94f, 0.94f, 0.94f, 1.00f );
  colors[ImGuiCol_Border] = ImVec4( 0.85f, 0.85f, 0.85f, 1.00f );
  colors[ImGuiCol_ScrollbarBg] = ImVec4( 0.82f, 0.82f, 0.82f, 0.53f );
  colors[ImGuiCol_ResizeGripActive] = ImVec4( 0.64f, 0.75f, 0.38f, 1.00f );
  colors[ImGuiCol_Tab] = ImVec4( 0.89f, 0.89f, 0.89f, 0.93f );
  colors[ImGuiCol_FrameBgHovered] = ImVec4( 1.00f, 1.00f, 1.00f, 1.00f );
  colors[ImGuiCol_FrameBgActive] = ImVec4( 1.00f, 1.00f, 1.00f, 1.00f );
  colors[ImGuiCol_TitleBgCollapsed] = ImVec4( 0.57f, 0.75f, 0.72f, 0.51f );
  colors[ImGuiCol_CheckMark] = ImVec4( 0.24f, 0.51f, 0.60f, 1.00f );
  colors[ImGuiCol_SliderGrab] = ImVec4( 0.38f, 0.61f, 0.75f, 1.00f );
  colors[ImGuiCol_Button] = ImVec4( 0.38f, 0.66f, 0.75f, 1.00f );
  colors[ImGuiCol_ButtonHovered] = ImVec4( 0.33f, 0.74f, 0.91f, 1.00f );
  colors[ImGuiCol_ButtonActive] = ImVec4( 0.38f, 0.75f, 0.63f, 1.00f );
  colors[ImGuiCol_HeaderHovered] = ImVec4( 0.38f, 0.68f, 0.75f, 1.00f );
  colors[ImGuiCol_HeaderActive] = ImVec4( 0.26f, 0.71f, 0.88f, 1.00f );
  colors[ImGuiCol_SeparatorHovered] = ImVec4( 0.38f, 0.68f, 0.75f, 1.00f );
  colors[ImGuiCol_SeparatorActive] = ImVec4( 0.38f, 0.68f, 0.75f, 1.00f );
  colors[ImGuiCol_ResizeGripHovered] = ImVec4( 0.38f, 0.68f, 0.75f, 1.00f );
  colors[ImGuiCol_TextSelectedBg] = ImVec4( 0.38f, 0.75f, 0.71f, 1.00f );
  colors[ImGuiCol_DragDropTarget] = ImVec4( 0.38f, 0.64f, 0.75f, 1.00f );
  colors[ImGuiCol_NavHighlight] = ImVec4( 0.38f, 0.63f, 0.75f, 1.00f );
  colors[ImGuiCol_PopupBg] = ImVec4( 1.00f, 1.00f, 1.00f, 0.94f );
  colors[ImGuiCol_TabActive] = ImVec4( 1.00f, 1.00f, 1.00f, 1.00f );
  colors[ImGuiCol_TabUnfocusedActive] = ImVec4( 0.86f, 0.86f, 0.86f, 1.00f );
  colors[ImGuiCol_DockingPreview] = ImVec4( 0.00f, 0.87f, 0.59f, 0.71f );
  colors[ImGuiCol_SliderGrabActive] = ImVec4( 0.38f, 0.61f, 0.75f, 1.00f );
  colors[ImGuiCol_FrameBg] = ImVec4( 1.00f, 1.00f, 1.00f, 1.00f );
  colors[ImGuiCol_TitleBg] = ImVec4( 0.24f, 0.51f, 0.60f, 1.00f );
  colors[ImGuiCol_TitleBgActive] = ImVec4( 0.20f, 0.62f, 0.76f, 1.00f );
  colors[ImGuiCol_MenuBarBg] = ImVec4( 1.00f, 1.00f, 1.00f, 1.00f );
  colors[ImGuiCol_Header] = ImVec4( 0.76f, 0.76f, 0.76f, 0.31f );
  colors[ImGuiCol_Separator] = ImVec4( 0.92f, 0.92f, 0.92f, 1.00f );
  colors[ImGuiCol_TabHovered] = ImVec4( 1.00f, 1.00f, 1.00f, 1.00f );
  colors[ImGuiCol_Header] = ImVec4( 0.56f, 0.62f, 0.64f, 0.31f );
  colors[ImGuiCol_TableHeaderBg] = ImVec4( 0.38f, 0.75f, 0.63f, 1.00f );
  //Main
  style->WindowPadding = ImVec2{ 4.0f, 4.0f };
  style->FramePadding = ImVec2{ 4.0f, 3.0f };
  style->CellPadding = ImVec2{ 4.0f, 2.0f };
  style->ItemSpacing = ImVec2{ 7.0f, 9.0f };
  style->ItemInnerSpacing = ImVec2{ 6.0f, 4.0f };
  style->TouchExtraPadding = ImVec2{ 4.0f, 9.0f };
  style->IndentSpacing = 21.0f;
  style->ScrollbarSize = 14.0f;
  style->GrabMinSize = 17.0f;
  //Bordes
  style->WindowBorderSize = 1.0f;
  style->ChildBorderSize = 0.0f;
  style->PopupBorderSize = 1.0f;
  style->FrameBorderSize = 1.0f;
  style->TabBorderSize = 1.0f;
  //Rounding
  style->WindowRounding = 0.0f;
  style->ChildRounding = 0.0f;
  style->FrameRounding = 4.0f;
  style->PopupRounding = 0.0f;
  style->ScrollbarRounding = 4.0f;
  style->GrabRounding = 5.0f;
  style->LogSliderDeadzone = 4.0f;
  style->TabRounding = 4.0f;

  style->WindowMenuButtonPosition = -1;
  style->ColorButtonPosition = 0;
}

static void set_imgui_style_dark_shadow( void ) {
  ImGuiStyle* style = &ImGui::GetStyle();
  style->FrameRounding = 2.0f;
  style->TabRounding = style->FrameRounding;
  style->WindowMenuButtonPosition = -1;
  style->ColorButtonPosition = 0;

  //Set UI 
  ImVec4* colors = ImGui::GetStyle().Colors;

  colors[ImGuiCol_WindowBg] = ImVec4( 0.20f, 0.20, 0.20f, 1.00f );
//Window Title
  colors[ImGuiCol_TitleBg] = ImVec4( 0.07f, 0.07f, 0.07f, 1.00f );
  colors[ImGuiCol_TitleBgActive] = ImVec4( 0.07f, 0.07f, 0.07f, 1.00f );
  //Frame
  colors[ImGuiCol_FrameBg] = ImVec4( 0.28f, 0.28f, 0.28f, 1.00f );
  colors[ImGuiCol_FrameBgHovered] = ImVec4( 0.05f, 0.58f, 0.71f, 0.31f );
  colors[ImGuiCol_FrameBgActive] = ImVec4( 0.05f, 0.58f, 0.71f, 0.31f );
  //Tab
  colors[ImGuiCol_Tab] = ImVec4( 0.31f, 0.31f, 0.31f, 1.00f );
  colors[ImGuiCol_TabHovered] = ImVec4( 0.05f, 0.58f, 0.71f, 1.00f );
  colors[ImGuiCol_TabActive] = ImVec4( 0.00f, 0.42f, 0.52f, 1.00f );
  colors[ImGuiCol_TabUnfocused] = ImVec4( 0.30f, 0.30f, 0.30f, 0.50f );
  colors[ImGuiCol_TabUnfocusedActive] = ImVec4( 0.28f, 0.50f, 0.55f, 1.00f );
  //Table data
  colors[ImGuiCol_TableHeaderBg] = ImVec4( 0.04f, 0.10f, 0.13f, 1.00f );
  colors[ImGuiCol_TableRowBg] = ImVec4( 0.41f, 0.41f, 0.41f, 1.00f );
  colors[ImGuiCol_TableRowBgAlt] = ImVec4( 0.41f, 0.41f, 0.41f, 1.00f );
  colors[ImGuiCol_TableBorderLight] = ImVec4( 0.70f, 0.70f, 0.70f, 1.00f );
  //Header
  colors[ImGuiCol_Header] = ImVec4( 0.13f, 0.13f, 0.13f, 1.00f );
  colors[ImGuiCol_HeaderHovered] = ImVec4( 0.05f, 0.58f, 0.71f, 1.00f );
  colors[ImGuiCol_HeaderActive] = ImVec4( 0.05f, 0.58f, 0.71f, 1.00f );
  colors[ImGuiCol_ScrollbarBg] = ImVec4( 0.13f, 0.13f, 0.13f, 0.53f );
  colors[ImGuiCol_CheckMark] = ImVec4( 0.05f, 0.58f, 0.71f, 1.00f );
  colors[ImGuiCol_SliderGrab] = ImVec4( 0.05f, 0.58f, 0.71f, 1.00f );
  colors[ImGuiCol_SliderGrabActive] = ImVec4( 0.05f, 0.58f, 0.71f, 1.00f );
  colors[ImGuiCol_Button] = ImVec4( 0.05f, 0.58f, 0.71f, 1.00f );
  colors[ImGuiCol_ButtonHovered] = ImVec4( 0.53f, 0.91f, 1.00f, 1.00f );
  colors[ImGuiCol_ButtonActive] = ImVec4( 0.05f, 0.58f, 0.71f, 1.00f );
  colors[ImGuiCol_SeparatorHovered] = ImVec4( 0.05f, 0.58f, 0.71f, 1.00f );
  colors[ImGuiCol_SeparatorActive] = ImVec4( 0.05f, 0.58f, 0.71f, 1.00f );
  colors[ImGuiCol_ResizeGrip] = ImVec4( 0.05f, 0.58f, 0.71f, 1.00f );
  colors[ImGuiCol_ResizeGripHovered] = ImVec4( 0.05f, 0.58f, 0.71f, 1.00f );
  colors[ImGuiCol_ResizeGripActive] = ImVec4( 0.05f, 0.58f, 0.71f, 1.00f );
  colors[ImGuiCol_TextSelectedBg] = ImVec4( 0.05f, 0.58f, 0.71f, 0.74f );
  colors[ImGuiCol_NavHighlight] = ImVec4( 0.05f, 0.58f, 0.71f, 1.00f );
  colors[ImGuiCol_Text] = ImVec4( 1.00f, 1.00f, 1.00f, 1.00f );
  colors[ImGuiCol_TextDisabled] = ImVec4( 0.50f, 0.50f, 0.50f, 1.00f );
  colors[ImGuiCol_ChildBg] = ImVec4( 0.00f, 0.00f, 0.00f, 0.00f );
  colors[ImGuiCol_PopupBg] = ImVec4( 0.08f, 0.08f, 0.08f, 0.94f );
  colors[ImGuiCol_Border] = ImVec4( 0.43f, 0.43f, 0.50f, 0.50f );
  colors[ImGuiCol_BorderShadow] = ImVec4( 0.00f, 0.00f, 0.00f, 0.00f );
  colors[ImGuiCol_TitleBgCollapsed] = ImVec4( 0.00f, 0.00f, 0.00f, 0.51f );
  colors[ImGuiCol_MenuBarBg] = ImVec4( 0.14f, 0.14f, 0.14f, 1.00f );
  colors[ImGuiCol_ScrollbarGrab] = ImVec4( 0.31f, 0.31f, 0.31f, 1.00f );
  colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4( 0.41f, 0.41f, 0.41f, 1.00f );
  colors[ImGuiCol_ScrollbarGrabActive] = ImVec4( 0.51f, 0.51f, 0.51f, 1.00f );
  colors[ImGuiCol_Separator] = colors[ImGuiCol_Border];
  colors[ImGuiCol_DockingEmptyBg] = ImVec4( 0.12f, 0.12f, 0.12f, 1.00f );
  colors[ImGuiCol_PlotLines] = ImVec4( 0.61f, 0.61f, 0.61f, 1.00f );
  colors[ImGuiCol_PlotLinesHovered] = ImVec4( 1.00f, 0.43f, 0.35f, 1.00f );
  colors[ImGuiCol_PlotHistogram] = ImVec4( 0.90f, 0.70f, 0.00f, 1.00f );
  colors[ImGuiCol_PlotHistogramHovered] = ImVec4( 1.00f, 0.60f, 0.00f, 1.00f );
  colors[ImGuiCol_DragDropTarget] = ImVec4( 1.00f, 1.00f, 0.00f, 0.90f );
  colors[ImGuiCol_NavWindowingHighlight] = ImVec4( 1.00f, 1.00f, 1.00f, 0.70f );
  colors[ImGuiCol_NavWindowingDimBg] = ImVec4( 0.80f, 0.80f, 0.80f, 0.20f );
  colors[ImGuiCol_ModalWindowDimBg] = ImVec4( 0.80f, 0.80f, 0.80f, 0.35f );
}

static void set_imgui_style_silver( void ) {

  ImGuiStyle* style = &ImGui::GetStyle();

  style->FrameBorderSize = 1.0f;
  style->WindowMenuButtonPosition = -1;
  style->ColorButtonPosition = 0;

  //Set UI 
  ImVec4* colors = ImGui::GetStyle().Colors;

  colors[ImGuiCol_TextDisabled] = ImVec4(0.61f, 0.61f, 0.61f, 1.00f);
  colors[ImGuiCol_WindowBg] = ImVec4(0.93f, 0.92f, 0.89f, 1.00f);
  colors[ImGuiCol_Button] = ImVec4(0.80f, 0.79f, 0.77f, 1.00f);
  colors[ImGuiCol_ButtonActive] = ImVec4(0.46f, 0.45f, 0.44f, 1.00f);
  colors[ImGuiCol_ResizeGrip] = ImVec4(0.46f, 0.45f, 0.44f, 1.00f);
  colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.46f, 0.45f, 0.44f, 1.00f);
  colors[ImGuiCol_ResizeGripActive] = ImVec4(0.46f, 0.45f, 0.44f, 1.00f);
  colors[ImGuiCol_TabHovered] = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
  colors[ImGuiCol_Text] = ImVec4(0.18f, 0.17f, 0.16f, 1.00f);
  colors[ImGuiCol_Border] = ImVec4(0.76f, 0.76f, 0.76f, 1.00f);
  colors[ImGuiCol_ScrollbarBg] = ImVec4(0.52f, 0.52f, 0.52f, 0.53f);
  colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.25f, 0.25f, 0.25f, 1.00f);
  colors[ImGuiCol_HeaderActive] = ImVec4(0.88f, 0.85f, 0.81f, 1.00f);
  colors[ImGuiCol_Separator] = ImVec4(0.84f, 0.84f, 0.84f, 0.50f);
  colors[ImGuiCol_Tab] = ImVec4(0.89f, 0.88f, 0.86f, 1.00f);
  colors[ImGuiCol_TabUnfocused] = ImVec4(0.93f, 0.92f, 0.89f, 1.00f);
  colors[ImGuiCol_TabUnfocusedActive] = ImVec4(0.93f, 0.92f, 0.89f, 1.00f);
  colors[ImGuiCol_TableHeaderBg] = ImVec4(0.90f, 0.55f, 0.10f, 1.00f);
  colors[ImGuiCol_PopupBg] = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
  colors[ImGuiCol_BorderShadow] = ImVec4(0.74f, 0.74f, 0.74f, 0.00f);
  colors[ImGuiCol_FrameBg] = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
  colors[ImGuiCol_FrameBgHovered] = ImVec4(0.96f, 0.96f, 0.96f, 1.00f);
  colors[ImGuiCol_FrameBgActive] = ImVec4(1.00f, 0.83f, 0.67f, 1.00f);
  colors[ImGuiCol_TitleBg] = ImVec4(0.77f, 0.77f, 0.77f, 1.00f);
  colors[ImGuiCol_TitleBgActive] = ImVec4(0.89f, 0.89f, 0.89f, 1.00f);
  colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.38f, 0.38f, 0.38f, 0.51f);
  colors[ImGuiCol_MenuBarBg] = ImVec4(0.86f, 0.86f, 0.83f, 1.00f);
  colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(1.00f, 0.47f, 0.01f, 1.00f);
  colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(1.00f, 0.47f, 0.01f, 1.00f);
  colors[ImGuiCol_CheckMark] = ImVec4(0.11f, 0.11f, 0.11f, 1.00f);
  colors[ImGuiCol_SliderGrab] = ImVec4(0.91f, 0.87f, 0.82f, 1.00f);
  colors[ImGuiCol_SliderGrabActive] = ImVec4(1.00f, 0.47f, 0.01f, 1.00f);
  colors[ImGuiCol_ButtonHovered] = ImVec4(1.00f, 0.47f, 0.01f, 1.00f);
  colors[ImGuiCol_Header] = ImVec4(0.84f, 0.81f, 0.78f, 1.00f);
  colors[ImGuiCol_HeaderHovered] = ImVec4(1.00f, 0.47f, 0.01f, 1.00f);
  colors[ImGuiCol_SeparatorHovered] = ImVec4(1.00f, 0.47f, 0.01f, 1.00f);
  colors[ImGuiCol_SeparatorActive] = ImVec4(1.00f, 0.47f, 0.01f, 1.00f);
  colors[ImGuiCol_TabActive] = ImVec4(0.97f, 0.96f, 0.95f, 1.00f);
  colors[ImGuiCol_DockingPreview] = ImVec4(1.00f, 0.47f, 0.01f, 1.00f);
  colors[ImGuiCol_TableRowBg] = ImVec4(0.96f, 0.96f, 0.96f, 1.00f);
  colors[ImGuiCol_TableRowBgAlt] = ImVec4(0.94f, 0.94f, 0.94f, 1.00f);
  colors[ImGuiCol_TextSelectedBg] = ImVec4(1.00f, 0.47f, 0.01f, 1.00f);
  colors[ImGuiCol_NavHighlight] = ImVec4(1.00f, 0.47f, 0.01f, 1.00f);
  colors[ImGuiCol_TableBorderStrong] = ImVec4(0.34f, 0.34f, 0.34f, 1.00f);
  colors[ImGuiCol_TableBorderLight] = ImVec4(0.78f, 0.78f, 0.78f, 1.00f);
}
