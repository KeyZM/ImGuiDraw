#include "OwnImGui.h"

/****************************************************
* Copyright (C): Liv
* @file		: OwnImGui.cpp
* @author	: Liv
* @email	: 1319923129@qq.com
* @version	: 1.1
* @date		: 2022/6/11	22:51
****************************************************/

bool CreateDeviceD3D(HWND hWnd);
void CleanupDeviceD3D();
void CreateRenderTarget();
void CleanupRenderTarget();
void UpdateWindowState(WindowData& Window);
bool Clean = false;
WNDCLASSEX W_class;

void OwnImGui::Start(const char* WindowClassName,const char* WindowName , CallBack MainFunction)
{
    int flag1 = -1, flag2 = -1;
    flag1 = strcmp(WindowName, "");
    flag2 = strcmp(WindowClassName, "");

    if (flag1 == 0 && flag2 == 0)
    {
        std::cout << "[error]: WindowInfo can't be empty." << std::endl;
        return;
    }
    if (reinterpret_cast<int>(MainFunction) == 0)
    {
        std::cout << "[error]: MainFunction can't be null." << std::endl;
        return;
    }

    if (flag1)
        strcpy_s(this->Window.WindowName, strlen(WindowName) + 1, WindowName);
    if (flag2)
        strcpy_s(this->Window.WindowClassName, strlen(WindowClassName) + 1, WindowClassName);

    if (flag1)
    {
        if (flag2)
            this->Window.hWindow = FindWindowA(this->Window.WindowClassName, this->Window.WindowName);
        else
            this->Window.hWindow = FindWindowA(NULL, this->Window.WindowName);
    }
    else if (flag2)
    {
        this->Window.hWindow = FindWindowA(this->Window.WindowClassName, NULL);
    }


    if (reinterpret_cast<int>(this->Window.hWindow) <= 0)
    {
        std::cout << "[error]: Window isn't exist."<<std::endl;
        Clean = true;
        return;
    }
    if (!CreateOwnWindow(this->Window.hWindow))
    {
        Clean = true;
        return;
    }
    Initialize();
    WindowMessageCycle(MainFunction);
}

OwnImGui::OwnImGui(const char* MenuName_)
{
    strcpy_s(MenuName, strlen(MenuName_) + 1, MenuName_);
}

OwnImGui::~OwnImGui()
{
    if (!Clean)
    {
        ImGui_ImplDX11_Shutdown();
        ImGui_ImplWin32_Shutdown();
        ImGui::DestroyContext();

        CleanupDeviceD3D();
        ::DestroyWindow(this->Window.hMyWindow);
        ::UnregisterClass(W_class.lpszClassName, W_class.hInstance);
    }
}

void OwnImGui::WindowMessageCycle(CallBack MainFunction)
{
    MSG msg;
    while (true)
    {
        while (::PeekMessage(&msg, NULL, 0U, 0U, PM_REMOVE))
        {
            if (msg.message == WM_QUIT)
                done = true;
            ::TranslateMessage(&msg);
            ::DispatchMessage(&msg);
        }
        if (done)
            break;       

        UpdateWindowState(this->Window);

        // Start ImGui frame
        ImGui_ImplDX11_NewFrame();
        ImGui_ImplWin32_NewFrame();
        ImGui::NewFrame();
        // Main
        //Open = ImGui::Begin(MenuName);
        MainFunction();
        ImGui::End();
        
        // Rendering
        ImGui::Render();

        const float clear_color_with_alpha[4] = { 0,0,0,0 };
        g_pd3dDeviceContext->OMSetRenderTargets(1, &g_mainRenderTargetView, NULL);
        g_pd3dDeviceContext->ClearRenderTargetView(g_mainRenderTargetView, clear_color_with_alpha);
        ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
        g_pSwapChain->Present(0, 0);
    }
    // Cleanup
    ImGui_ImplDX11_Shutdown();
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext();

    CleanupDeviceD3D();
    ::DestroyWindow(this->Window.hMyWindow);
    ::UnregisterClass(W_class.lpszClassName, W_class.hInstance);
    Clean = true;
}

void OwnImGui::Initialize()
{
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();(void)io;

    // 设置imgui
    //ImGui::StyleColorsDark();
    io.IniFilename = nullptr;
    io.LogFilename = nullptr;
    ImGui::GetStyle().WindowRounding = 4;
    this->Font1 = io.Fonts->AddFontFromFileTTF("C:\\Windows\\Fonts\\arialbd.ttf", FontSize);
    //  this->Font1 = io.Fonts->AddFontFromFileTTF("C:\\Windows\\Fonts\\msyh.ttc", FontSize,0, io.Fonts->GetGlyphRangesChineseFull());
    // Setup backends
    ImGui_ImplWin32_Init(this->Window.hMyWindow);
    ImGui_ImplDX11_Init(g_pd3dDevice, g_pd3dDeviceContext);
}

bool OwnImGui::CreateOwnWindow(HWND g_hToWindow)
{
	// 创建窗口
    USES_CONVERSION;
    WNDCLASSEX
	W_class = { sizeof(WNDCLASSEX), CS_HREDRAW | CS_VREDRAW , (WNDPROC)WndProc, 0L, 0L, GetModuleHandle(NULL), NULL, NULL, (HBRUSH)RGB(0, 0, 0), NULL, _T("ImGui"), NULL };
	RegisterClassEx(&W_class);
    this->Window.hMyWindow = CreateWindowExA(WS_EX_LAYERED|WS_EX_TRANSPARENT, W2A(W_class.lpszClassName), "DrawWindow", WS_POPUP, 1, 1, 100, 100, 0, 0, 0, 0);
    SetLayeredWindowAttributes(this->Window.hMyWindow, 0, 255, LWA_ALPHA);
    UpdateWindow(this->Window.hMyWindow);
    ShowWindow(this->Window.hMyWindow, SW_SHOW);

    // 获取目标窗口宽高
    GetWindowRect(g_hToWindow, &this->Window.WindowRect);
    this->Window.Width = this->Window.WindowRect.right - this->Window.WindowRect.left;
    this->Window.Height = this->Window.WindowRect.bottom - this->Window.WindowRect.top;

	// 初始化d3d设备
	if (!CreateDeviceD3D(this->Window.hMyWindow))
	{
		CleanupDeviceD3D();
		::UnregisterClass(W_class.lpszClassName, W_class.hInstance);
        std::cout << "[error]: Create D3D Device failed." << std::endl;
        return false;
	}

	// 显示窗口
	::ShowWindow(this->Window.hMyWindow, SW_SHOWDEFAULT);
	::UpdateWindow(this->Window.hMyWindow);
    return true;
}

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

LRESULT __stdcall OwnImGui::WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    if (ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam))
        return true;

    switch (msg)
    {
    case WM_CREATE:
        {
            MARGINS     Margin = { -1 };
            DwmExtendFrameIntoClientArea(hWnd, &Margin);
            break;
        }
    case WM_SIZE:
        if (g_pd3dDevice != NULL && wParam != SIZE_MINIMIZED)
        {
            CleanupRenderTarget();
            g_pSwapChain->ResizeBuffers(0, (UINT)LOWORD(lParam), (UINT)HIWORD(lParam), DXGI_FORMAT_UNKNOWN, 0);
            CreateRenderTarget();
        }
        return 0;
    case WM_SYSCOMMAND:
        if ((wParam & 0xfff0) == SC_KEYMENU)
            return 0;
        break;
    case WM_DESTROY:
        ::PostQuitMessage(0);
        return 0;
    }
    return ::DefWindowProc(hWnd, msg, wParam, lParam);
}

bool CreateDeviceD3D(HWND hWnd)
{
    // Setup swap chain
    DXGI_SWAP_CHAIN_DESC sd;
    ZeroMemory(&sd, sizeof(sd));
    sd.BufferCount = 2;
    sd.BufferDesc.Width = 0;
    sd.BufferDesc.Height = 0;
    sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    sd.BufferDesc.RefreshRate.Numerator = 60;
    sd.BufferDesc.RefreshRate.Denominator = 1;
    sd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
    sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    sd.OutputWindow = hWnd;
    sd.SampleDesc.Count = 1;
    sd.SampleDesc.Quality = 0;
    sd.Windowed = TRUE;
    sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
    UINT createDeviceFlags = 0;
    D3D_FEATURE_LEVEL featureLevel;
    const D3D_FEATURE_LEVEL featureLevelArray[2] = { D3D_FEATURE_LEVEL_11_0, D3D_FEATURE_LEVEL_10_0, };
    if (D3D11CreateDeviceAndSwapChain(NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, createDeviceFlags, featureLevelArray, 2, D3D11_SDK_VERSION, &sd, &g_pSwapChain, &g_pd3dDevice, &featureLevel, &g_pd3dDeviceContext) != S_OK)
        return false;

    CreateRenderTarget();
    return true;
}

void CleanupDeviceD3D()
{
    CleanupRenderTarget();
    if (g_pSwapChain) { g_pSwapChain->Release(); g_pSwapChain = NULL; }
    if (g_pd3dDeviceContext) { g_pd3dDeviceContext->Release(); g_pd3dDeviceContext = NULL; }
    if (g_pd3dDevice) { g_pd3dDevice->Release(); g_pd3dDevice = NULL; }
}

void CreateRenderTarget()
{
    ID3D11Texture2D* pBackBuffer;
    g_pSwapChain->GetBuffer(0, IID_PPV_ARGS(&pBackBuffer));
    g_pd3dDevice->CreateRenderTargetView(pBackBuffer, NULL, &g_mainRenderTargetView);
    pBackBuffer->Release();
}

void CleanupRenderTarget()
{
    if (g_mainRenderTargetView) { g_mainRenderTargetView->Release(); g_mainRenderTargetView = NULL; }
}

int  OwnImGui::CalcTextWidth(const char* Text)
{
    if (strcmp(Text, "") == 0)
        return 0;
    return ImGui::GetFont()->CalcTextSizeA(this->FontSize, FLT_MAX, 0, Text).x;
}

void OwnImGui::Text(const char* Text, Vec2 Pos, int color)
{
    ImGui::GetForegroundDrawList()->AddText(ImVec2(Pos.x, Pos.y), color, Text);
}

void OwnImGui::Rectangle(Vec2 Pos, int width, int height,int color, int thickness)
{
    ImGui::GetForegroundDrawList()->AddRect(ImVec2(Pos.x, Pos.y), ImVec2(Pos.x+width, Pos.y+height), color, 0, 15, thickness);
}

void OwnImGui::Line(Vec2 From, Vec2 To, int color, int thickness)
{
    ImGui::GetForegroundDrawList()->AddLine(ImVec2(From.x,From.y), ImVec2(To.x,To.y), color, thickness);
}

void OwnImGui::Circle(Vec2 Center,float radius,int color,float thickness)
{

    ImGui::GetForegroundDrawList()->AddCircle(ImVec2(Center.x,Center.y), radius, color, max(radius, 50), thickness);
}

void OwnImGui::CircleFilled(Vec2 Center, float radius, int color, int num)
{
    ImGui::GetForegroundDrawList()->AddCircleFilled(ImVec2(Center.x, Center.y), radius, color, num);
}

void OwnImGui::RectangleFilled(Vec2 Pos, int width, int height, int color)
{
    ImGui::GetForegroundDrawList()->AddRectFilled(ImVec2(Pos.x, Pos.y), ImVec2(Pos.x + width, Pos.y + height), color, 0, 0);
}

void OwnImGui::StrokeText(const char* Text, Vec2 Pos, int color_b, int color)
{
    this->Text(Text, Vec2(Pos.x - 1, Pos.y + 1), color_b);
    this->Text(Text, Vec2(Pos.x - 1, Pos.y - 1), color_b);
    this->Text(Text, Vec2(Pos.x + 1, Pos.y + 1), color_b);
    this->Text(Text, Vec2(Pos.x + 1, Pos.y - 1), color_b);
    this->Text(Text, Pos, color);
}

void UpdateWindowState(WindowData& Window)
{
    ImGuiIO& io = ImGui::GetIO();
    // 更新窗口数据
    POINT TempPoint{ 0,0 };
    RECT TempRect;
    GetClientRect(Window.hWindow, &TempRect);

    ClientToScreen(Window.hWindow, &TempPoint);
    Window.WindowRect.left = TempPoint.x;
    Window.WindowRect.right = Window.WindowRect.left + TempRect.right;
    Window.WindowRect.top = TempPoint.y;
    Window.WindowRect.bottom = Window.WindowRect.top + TempRect.bottom;

    Window.Width = Window.WindowRect.right - Window.WindowRect.left;
    Window.Height = Window.WindowRect.bottom - Window.WindowRect.top;
    SetWindowPos(Window.hMyWindow, HWND_TOPMOST, Window.WindowRect.left, Window.WindowRect.top, Window.Width, Window.Height, SWP_SHOWWINDOW);

    // 检查鼠标状态
    if (io.WantCaptureMouse)
        SetWindowLong(Window.hMyWindow, GWL_EXSTYLE, GetWindowLong(Window.hMyWindow, GWL_EXSTYLE) & (~WS_EX_LAYERED));
    else
        SetWindowLong(Window.hMyWindow, GWL_EXSTYLE, GetWindowLong(Window.hMyWindow, GWL_EXSTYLE)|WS_EX_LAYERED);
}

void OwnImGui::ReSetFontSize(size_t FontSize_)
{
    FontSize = FontSize_;
}

void OwnImGui::ConnectPoint(std::vector<Vec2> Points, int color, float thickness)
{
    if (Points.size() <= 0)
        return;
    for (int i = 0; i < Points.size() - 1; i++)
    {
        Line(Points[i], Points[i + 1], color, thickness);
        if (i == Points.size() - 2)
            Line(Points[i + 1], Points[0], color, thickness);
    }
}

void OwnImGui::CheckboxEx(const char* Text, bool* Point)
{
    ImVec2 p = ImGui::GetCursorScreenPos();
    ImDrawList* draw_list = ImGui::GetWindowDrawList();

    float height = ImGui::GetFrameHeight();
    float width = height * 1.55f;
    float radius = height * 0.50f;

    ImGui::InvisibleButton(Text, ImVec2(width, height));
    if (ImGui::IsItemClicked())
        *Point = !*Point;

    float t = *Point ? 1.0f : 0.0f;

    ImGuiContext& g = *GImGui;
    float ANIM_SPEED = 0.08f;
    if (g.LastActiveId == g.CurrentWindow->GetID(Text))
    {
        float t_anim = ImSaturate(g.LastActiveIdTimer / ANIM_SPEED);
        t = *Point ? (t_anim) : (1.0f - t_anim);
    }

    ImU32 col_bg;
    if (ImGui::IsItemHovered())
        col_bg = ImGui::GetColorU32(ImLerp(ImVec4(0.78f, 0.78f, 0.78f, 1.0f), ImVec4(0.86f, 0.07f, 0.23f, 1.0f), t));
    else
        col_bg = ImGui::GetColorU32(ImLerp(ImVec4(0.85f, 0.85f, 0.85f, 1.0f), ImVec4(0.78f, 0.07f, 0.15f, 1.0f), t));

    draw_list->AddRectFilled(p, ImVec2(p.x + width, p.y + height), col_bg, height * 0.5f);
    draw_list->AddCircleFilled(ImVec2(p.x + radius + t * (width - radius * 2.0f), p.y + radius), radius - 1.5f, IM_COL32(255, 255, 255, 255));
    ImGui::SameLine();
    ImGui::Text(Text);
}

int  OwnImGui::Color(float* Color_)
{
    return IM_COL32(Color_[0] * 255, Color_[1] * 255, Color_[2] * 255, Color_[3] * 255);
}

void OwnImGui::MyCheckBox(const char* str_id, bool* v)
{
    ImVec2 p = ImGui::GetCursorScreenPos();
    ImDrawList* DrawList = ImGui::GetWindowDrawList();
    float Height = ImGui::GetFrameHeight();
    float Width = Height * 1.7;
    float Radius = Height / 2 - 2;

    ImGui::InvisibleButton(str_id, ImVec2(Width, Height));
    if (ImGui::IsItemClicked())
        *v = !(*v);
    // 组件移动动画
    float t = *v ? 1.0f : 0.f;
    ImGuiContext& g = *GImGui;
    float AnimationSpeed = 0.08f;
    if (g.LastActiveId == g.CurrentWindow->GetID(str_id))
    {
        float T_Animation = ImSaturate(g.LastActiveIdTimer / AnimationSpeed);
        t = *v ? (T_Animation) : (1.0f - T_Animation);
    }
    // 鼠标悬停颜色
    ImU32 Color;
    if (ImGui::IsItemHovered())
        Color = ImGui::GetColorU32(ImLerp(ImVec4(0.85f, 0.24f, 0.15f, 1.0f), ImVec4(0.55f, 0.85f, 0.13, 1.000f), t));
    else
        Color = ImGui::GetColorU32(ImLerp(ImVec4(0.90f, 0.29f, 0.20f, 1.0f), ImVec4(0.60f, 0.90f, 0.18, 1.000f), t));
    // 组件绘制
    DrawList->AddRectFilled(ImVec2(p.x, p.y), ImVec2(p.x + Width, p.y + Height), Color, Height);
    DrawList->AddCircleFilled(ImVec2(p.x + Radius + t * (Width - Radius * 2) + (t == 0 ? 2 : -2), p.y + Radius + 2), Radius, IM_COL32(255, 255, 255, 255), 360);
    DrawList->AddCircle(ImVec2(p.x + Radius + t * (Width - Radius * 2) + (t == 0 ? 2 : -2), p.y + Radius + 2), Radius, IM_COL32(20, 20, 20, 80), 360, 1);

    ImGui::SameLine();
    ImGui::Text(str_id);
}

void OwnImGui::MyCheckBox2(const char* str_id, bool* v)
{
    ImVec2 p = ImGui::GetCursorScreenPos();
    ImDrawList* DrawList = ImGui::GetWindowDrawList();
    float Height = ImGui::GetFrameHeight();
    float Width = Height * 1.7;
    float Radius = Height / 2 - 2;

    ImGui::InvisibleButton(str_id, ImVec2(Width, Height));
    if (ImGui::IsItemClicked())
        *v = !(*v);
    // 组件移动动画
    float t = *v ? 1.0f : 0.f;
    ImGuiContext& g = *GImGui;
    float AnimationSpeed = 0.15f;
    if (g.LastActiveId == g.CurrentWindow->GetID(str_id))
    {
        float T_Animation = ImSaturate(g.LastActiveIdTimer / AnimationSpeed);
        t = *v ? (T_Animation) : (1.0f - T_Animation);
    }
    // 鼠标悬停颜色
    ImU32 Color;
    if (ImGui::IsItemHovered())
        Color = ImGui::GetColorU32(ImLerp(ImVec4(0.08f, 0.18f, 0.21f, 1.0f), ImVec4(0.10f, 0.48f, 0.68f, 1.000f), t));
    else
        Color = ImGui::GetColorU32(ImLerp(ImVec4(0.12f, 0.22f, 0.25f, 1.0f), ImVec4(0.14f, 0.52f, 0.72f, 1.000f), t));
    // 组件绘制
    DrawList->AddRectFilled(ImVec2(p.x, p.y), ImVec2(p.x + Width, p.y + Height), Color, 360);
    DrawList->AddCircleFilled(ImVec2(p.x + Radius + 2 + t * (Width - (Radius + 2) * 2), p.y + Radius + 2), Radius + 2, IM_COL32(255, 255, 255, 255), 360);
    DrawList->AddCircleFilled(ImVec2(p.x + Radius + t * (Width - Radius * 2) + (t == 0 ? 2 : -2), p.y + Radius + 2), Radius, IM_COL32(230, 230, 230, 255), 360);
    if (*v)
        DrawList->AddText(ImVec2(p.x + 45, p.y + 2), ImColor{ 255,255,255,255 }, str_id);
    else
        DrawList->AddText(ImVec2(p.x + 45, p.y + 2), ImColor{ 185,185,185,255 }, str_id);

}

void OwnImGui::MyCheckBox3(const char* str_id, bool* v)
{
    ImVec2 p = ImGui::GetCursorScreenPos();
    ImDrawList* DrawList = ImGui::GetWindowDrawList();
    float Height = ImGui::GetFrameHeight();
    float Width = Height;
    float Left = 8;
    float Right = Left * 1.5;
    ImGui::InvisibleButton(str_id, ImVec2(Width, Height));

    if (ImGui::IsItemClicked())
        *v = !(*v);
    // 组件移动动画
    float t = *v ? 1.0f : 0.f;
    ImGuiContext& g = *GImGui;
    float AnimationSpeed = 0.12f;
    if (g.LastActiveId == g.CurrentWindow->GetID(str_id))
    {
        float T_Animation = ImSaturate(g.LastActiveIdTimer / AnimationSpeed);
        t = *v ? (T_Animation) : (1.0f - T_Animation);
    }
    // 鼠标悬停颜色
    ImU32 Color;
    ImU32 TickColor1, TickColor2;
    if (ImGui::IsItemHovered())
        Color = ImGui::GetColorU32(ImLerp(ImVec4(0.75f, 0.75f, 0.75f, 1.0f), ImVec4(0.05f, 0.85f, 0.25f, 1.000f), t));
    else
        Color = ImGui::GetColorU32(ImLerp(ImVec4(0.8f, 0.8f, 0.8f, 1.0f), ImVec4(0.1f, 0.9f, 0.3f, 1.000f), t));

    TickColor1 = IM_COL32(255, 255, 255, 255 * t);
    TickColor2 = IM_COL32(180, 180, 180, 255 * (1 - t));

    float Size = Width;
    float Scale = (float)(Size) / 20.0f;
    // 底色
    DrawList->AddRectFilled(ImVec2(p.x, p.y), ImVec2(p.x + Width, p.y + Height), Color, 5, 15);
    // 选中勾
    DrawList->AddLine(ImVec2(p.x + 3 * Scale, p.y + Size / 2 - 2 * Scale), ImVec2(p.x + Size / 2 - 1 * Scale, p.y + Size - 5 * Scale), TickColor1, 3 * Scale);
    DrawList->AddLine(ImVec2(p.x + Size - 3 * Scale - 1, p.y + 3 * Scale + 1), ImVec2(p.x + Size / 2 - 1 * Scale, p.y + Size - 5 * Scale), TickColor1, 3 * Scale);
    // 未选中勾
    DrawList->AddLine(ImVec2(p.x + 3 * Scale, p.y + Size / 2 - 2 * Scale), ImVec2(p.x + Size / 2 - 1 * Scale, p.y + Size - 5 * Scale), TickColor2, 3 * Scale);
    DrawList->AddLine(ImVec2(p.x + Size - 3 * Scale - 1, p.y + 3 * Scale + 1), ImVec2(p.x + Size / 2 - 1 * Scale, p.y + Size - 5 * Scale), TickColor2, 3 * Scale);
    ImGui::SameLine();
    ImGui::Text(str_id);
}

void OwnImGui::MyCheckBox4(const char* str_id, bool* v)
{
    ImVec2 p = ImGui::GetCursorScreenPos();
    ImDrawList* DrawList = ImGui::GetWindowDrawList();
    float Height = ImGui::GetFrameHeight();
    float Width = Height;
    ImGui::InvisibleButton(str_id, ImVec2(Width, Height));

    if (ImGui::IsItemClicked())
        *v = !(*v);
    // 组件动画
    float t = *v ? 1.0f : 0.f;
    ImGuiContext& g = *GImGui;
    float AnimationSpeed = 0.12f;
    if (g.LastActiveId == g.CurrentWindow->GetID(str_id))
    {
        float T_Animation = ImSaturate(g.LastActiveIdTimer / AnimationSpeed);
        t = *v ? (T_Animation) : (1.0f - T_Animation);
    }
    // bg 0.74 0.72 0.81-> 0.69 0.77 0.76
    ImU32 BgColor;
    if (ImGui::IsItemHovered())
        BgColor = ImGui::GetColorU32(ImVec4(0.69f, 0.69f, 0.69f, 1.0f));
    else
        BgColor = ImGui::GetColorU32(ImVec4(0.74f, 0.74f, 0.74f, 1.0f));
    DrawList->AddRectFilled(ImVec2(p.x, p.y), ImVec2(p.x + Width, p.y + Width), BgColor);

    ImU32 FrColor;
    //FrColor = ImGui::GetColorU32(ImVec4(0.58f, 1.0f, 0.59f, 1.0f*t));
    FrColor = ImGui::GetColorU32(ImVec4(0.f, 0.f, 0.f, 0.5f * t));
    DrawList->AddRectFilled(ImVec2(p.x + Width / 5, p.y + Width / 5), ImVec2(p.x + Width - Width / 5, p.y + Width - Width / 5), FrColor);

    ImGui::SameLine();
    ImGui::Text(str_id);
}

static const char* PatchFormatStringFloatToInt(const char* fmt)
{
    if (fmt[0] == '%' && fmt[1] == '.' && fmt[2] == '0' && fmt[3] == 'f' && fmt[4] == 0) // Fast legacy path for "%.0f" which is expected to be the most common case.
        return "%d";
    const char* fmt_start = ImParseFormatFindStart(fmt);    // Find % (if any, and ignore %%)
    const char* fmt_end = ImParseFormatFindEnd(fmt_start);  // Find end of format specifier, which itself is an exercise of confidence/recklessness (because snprintf is dependent on libc or user).
    if (fmt_end > fmt_start && fmt_end[-1] == 'f')
    {
#ifndef IMGUI_DISABLE_OBSOLETE_FUNCTIONS
        if (fmt_start == fmt && fmt_end[0] == 0)
            return "%d";
        ImGuiContext& g = *GImGui;
        ImFormatString(g.TempBuffer, IM_ARRAYSIZE(g.TempBuffer), "%.*s%%d%s", (int)(fmt_start - fmt), fmt, fmt_end); // Honor leading and trailing decorations, but lose alignment/precision.
        return g.TempBuffer;
#else
        IM_ASSERT(0 && "DragInt(): Invalid format string!"); // Old versions used a default parameter of "%.0f", please replace with e.g. "%d"
#endif
    }
    return fmt;
}

bool SliderScalarEx(const char* label, ImGuiDataType data_type, void* p_data, const void* p_min, const void* p_max, const char* format, ImGuiSliderFlags flags)
{
    ImGuiWindow* window = ImGui::GetCurrentWindow();
    if (window->SkipItems)
        return false;

    ImGuiContext& g = *GImGui;
    const ImGuiStyle& style = g.Style;
    const ImGuiID id = window->GetID(label);
    const float w = 150;

    const ImVec2 label_size = ImGui::CalcTextSize(label, NULL, true);
    ImVec2 temp = ImVec2(w, label_size.y + style.FramePadding.y * 2.0f);
    ImRect frame_bb(window->DC.CursorPos, { window->DC.CursorPos.x + temp.x,window->DC.CursorPos.y + temp.y });
    ImVec2 temp_2 = ImVec2(label_size.x > 0.0f ? style.ItemInnerSpacing.x + label_size.x : 0.0f, 0.0f);
    ImRect total_bb(frame_bb.Min, { frame_bb.Max.x + temp_2.x ,frame_bb.Max.y + temp_2.y });

    ImGui::ItemSize(total_bb, style.FramePadding.y);
    if (!ImGui::ItemAdd(total_bb, id, &frame_bb))
        return false;
    if (format == NULL)
        format = ImGui::DataTypeGetInfo(data_type)->PrintFmt;
    else if (data_type == ImGuiDataType_S32 && strcmp(format, "%d") != 0)
        format = PatchFormatStringFloatToInt(format);

    const bool hovered = ImGui::ItemHoverable(frame_bb, id);
    const bool temp_input_allowed = (flags & ImGuiSliderFlags_NoInput) == 0;
    bool temp_input_is_active = temp_input_allowed && ImGui::TempInputIsActive(id);

    char value_buf[64];
    const char* value_buf_end = value_buf + ImGui::DataTypeFormatString(value_buf, IM_ARRAYSIZE(value_buf), data_type, p_data, format);

    if (!temp_input_is_active)
    {
        const bool focus_requested = temp_input_allowed && ImGui::FocusableItemRegister(window, id);
        const bool clicked = (hovered && g.IO.MouseClicked[0]);
        if (focus_requested || clicked || g.NavActivateId == id || g.NavInputId == id)
        {
            ImGui::SetActiveID(id, window);
            ImGui::SetFocusID(id, window);
            ImGui::FocusWindow(window);
            g.ActiveIdUsingNavDirMask |= (1 << ImGuiDir_Left) | (1 << ImGuiDir_Right);
            if (temp_input_allowed && (focus_requested || (clicked && g.IO.KeyCtrl) || g.NavInputId == id))
            {
                temp_input_is_active = true;
                ImGui::FocusableItemUnregister(window);
            }
        }
    }

    if (temp_input_is_active)
    {
        const bool is_clamp_input = (flags & ImGuiSliderFlags_AlwaysClamp) != 0;
        return ImGui::TempInputScalar(frame_bb, id, label, data_type, p_data, format, is_clamp_input ? p_min : NULL, is_clamp_input ? p_max : NULL);
    }

    // 绘制滑动条框
    const ImU32 frame_col = ImGui::GetColorU32(g.ActiveId == id ? ImGuiCol_FrameBgActive : g.HoveredId == id ? ImGuiCol_FrameBgHovered : ImGuiCol_FrameBg);
    ImGui::RenderNavHighlight(frame_bb, id);
    frame_bb.Min.y += 7.5;
    frame_bb.Max.y -= 7.5;
    window->DrawList->AddRectFilled(frame_bb.Min, frame_bb.Max, frame_col, 15);

    // 滑动
    ImRect grab_bb;

    const bool value_changed = ImGui::SliderBehavior(frame_bb, id, data_type, p_data, p_min, p_max, format, flags, &grab_bb);
    if (value_changed)
        ImGui::MarkItemEdited(id);

    // 绘制滑动头
    ImVec2 center = { grab_bb.Min.x + (grab_bb.Max.x - grab_bb.Min.x) / 2 ,grab_bb.Min.y + (grab_bb.Max.y - grab_bb.Min.y) / 2 };

    if (grab_bb.Max.x > grab_bb.Min.x)
        window->DrawList->AddCircleFilled(center, 6.5, (g.ActiveId == id ? ImColor(0.10f, 0.48f, 0.68f, 1.000f) : ImColor(0.14f, 0.52f, 0.72f, 1.000f)), 360);

    // 激活时显示
    if (ImGui::IsItemActive())
        ImGui::RenderTextClipped({ frame_bb.Min.x,frame_bb.Min.y-8 }, frame_bb.Max, value_buf, value_buf_end, NULL, ImVec2(0.5f, 0.5f));

    if (label_size.x > 0.0f)
        ImGui::RenderText(ImVec2(frame_bb.Max.x + style.ItemInnerSpacing.x, frame_bb.Min.y - 3), label);

    IMGUI_TEST_ENGINE_ITEM_INFO(id, label, window->DC.ItemFlags);
    return value_changed;
}

bool SliderScalarEx2(const char* label, ImGuiDataType data_type, void* p_data, const void* p_min, const void* p_max, const char* format, ImGuiSliderFlags flags)
{
    ImGuiWindow* window = ImGui::GetCurrentWindow();
    if (window->SkipItems)
        return false;

    ImGuiContext& g = *GImGui;
    const ImGuiStyle& style = g.Style;
    const ImGuiID id = window->GetID(label);
    const float w = 150;

    const ImVec2 label_size = ImGui::CalcTextSize(label, NULL, true);
    ImVec2 temp = ImVec2(w, label_size.y + style.FramePadding.y * 2.0f);
    ImRect frame_bb(window->DC.CursorPos, { window->DC.CursorPos.x + temp.x,window->DC.CursorPos.y + temp.y });
    ImVec2 temp_2 = ImVec2(label_size.x > 0.0f ? style.ItemInnerSpacing.x + label_size.x : 0.0f, 0.0f);
    ImRect total_bb(frame_bb.Min, { frame_bb.Max.x + temp_2.x ,frame_bb.Max.y + temp_2.y });

    ImGui::ItemSize(total_bb, style.FramePadding.y);
    if (!ImGui::ItemAdd(total_bb, id, &frame_bb))
        return false;
    if (format == NULL)
        format = ImGui::DataTypeGetInfo(data_type)->PrintFmt;
    else if (data_type == ImGuiDataType_S32 && strcmp(format, "%d") != 0)
        format = PatchFormatStringFloatToInt(format);

    const bool hovered = ImGui::ItemHoverable(frame_bb, id);
    const bool temp_input_allowed = (flags & ImGuiSliderFlags_NoInput) == 0;
    bool temp_input_is_active = temp_input_allowed && ImGui::TempInputIsActive(id);

    char value_buf[64];
    const char* value_buf_end = value_buf + ImGui::DataTypeFormatString(value_buf, IM_ARRAYSIZE(value_buf), data_type, p_data, format);

    if (!temp_input_is_active)
    {
        const bool focus_requested = temp_input_allowed && ImGui::FocusableItemRegister(window, id);
        const bool clicked = (hovered && g.IO.MouseClicked[0]);
        if (focus_requested || clicked || g.NavActivateId == id || g.NavInputId == id)
        {
            ImGui::SetActiveID(id, window);
            ImGui::SetFocusID(id, window);
            ImGui::FocusWindow(window);
            g.ActiveIdUsingNavDirMask |= (1 << ImGuiDir_Left) | (1 << ImGuiDir_Right);
            if (temp_input_allowed && (focus_requested || (clicked && g.IO.KeyCtrl) || g.NavInputId == id))
            {
                temp_input_is_active = true;
                ImGui::FocusableItemUnregister(window);
            }
        }
    }

    if (temp_input_is_active)
    {
        const bool is_clamp_input = (flags & ImGuiSliderFlags_AlwaysClamp) != 0;
        return ImGui::TempInputScalar(frame_bb, id, label, data_type, p_data, format, is_clamp_input ? p_min : NULL, is_clamp_input ? p_max : NULL);
    }

    // 绘制滑动条框
    const ImU32 frame_col = ImGui::GetColorU32(g.ActiveId == id ? ImGuiCol_FrameBgActive : g.HoveredId == id ? ImGuiCol_FrameBgHovered : ImGuiCol_FrameBg);
    ImGui::RenderNavHighlight(frame_bb, id);
    frame_bb.Min.y += 7.5;
    frame_bb.Max.y -= 7.5;
    window->DrawList->AddRectFilled(frame_bb.Min, frame_bb.Max, frame_col, 15);

    // 滑动
    ImRect grab_bb;

    const bool value_changed = ImGui::SliderBehavior(frame_bb, id, data_type, p_data, p_min, p_max, format, flags, &grab_bb);
    if (value_changed)
        ImGui::MarkItemEdited(id);
    grab_bb.Min.y += 5;
    grab_bb.Min.x += 2;
    grab_bb.Max.y += 2;
    grab_bb.Max.x += 2;

    // 绘制滑动条
    if (grab_bb.Max.x > grab_bb.Min.x)
        window->DrawList->AddRectFilled(frame_bb.Min, grab_bb.Max, (g.ActiveId == id ? ImColor(0.10f, 0.48f, 0.68f, 1.000f) : ImColor(0.14f, 0.52f, 0.72f, 1.000f)), 15);

    // 激活时显示
    if (ImGui::IsItemActive())
        ImGui::RenderTextClipped({ frame_bb.Min.x,frame_bb.Min.y - 8 }, frame_bb.Max, value_buf, value_buf_end, NULL, ImVec2(0.5f, 0.5f));

    if (label_size.x > 0.0f)
        ImGui::RenderText(ImVec2(frame_bb.Max.x + style.ItemInnerSpacing.x, frame_bb.Min.y - 3), label);

    IMGUI_TEST_ENGINE_ITEM_INFO(id, label, window->DC.ItemFlags);
    return value_changed;
}


bool OwnImGui::SliderFloatEx(const char* label, float* v, float v_min, float v_max, const char* format, ImGuiSliderFlags flags)
{
    return SliderScalarEx(label, ImGuiDataType_Float, v, &v_min, &v_max, format, flags);
}

bool OwnImGui::SliderIntEx(const char* label, int* v, int v_min, int v_max, const char* format, ImGuiSliderFlags flags)
{
    return SliderScalarEx(label, ImGuiDataType_S32, v, &v_min, &v_max, format, flags);
}

bool OwnImGui::SliderFloatEx2(const char* label, float* v, float v_min, float v_max, const char* format, ImGuiSliderFlags flags)
{
    return SliderScalarEx2(label, ImGuiDataType_Float, v, &v_min, &v_max, format, flags);
}

bool OwnImGui::SliderIntEx2(const char* label, int* v, int v_min, int v_max, const char* format, ImGuiSliderFlags flags)
{
    return SliderScalarEx2(label, ImGuiDataType_S32, v, &v_min, &v_max, format, flags);
}


