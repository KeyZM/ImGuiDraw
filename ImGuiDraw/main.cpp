#define _USE_MATH_DEFINES
#include "OwnImGui.h"
#include <math.h>
#include <string>
#include <map>
#include <vector>
#include <thread>
#include "Draw.hpp"
#include <Psapi.h>
/****************************************************
* Copyright (C): Liv
* @file		: main.cpp
* @author	: Liv
* @email	: 1319923129@qq.com
* @version	: 1.2
* @date		: 2022/6/11	23:01
****************************************************/

OwnImGui gui("Menu");

static int Tab1 = 0;
static int Tab2 = 0;
bool MenuOpen = true;


HWND GameWnd = FindWindowA(NULL, "迷你世界");
DWORD GamePID = 0;
基址 GameBase;
HANDLE hpro;
ReadWrite Read;
RECT g_ClientRect;
int GameCentX, GameCentY, Game_height, Game_wIde;

void RefreshWindow()
{
	GetClientRect(GameWnd, &g_ClientRect);					//获取游戏窗口客户区矩形
	Game_height = g_ClientRect.bottom - g_ClientRect.top;
	Game_wIde = g_ClientRect.right - g_ClientRect.left;
	GameCentX = Game_wIde / 2;
	GameCentY = Game_height / 2;
}


void TabSelect1(ImVec2 TextPos, const char* Text, int Index)
{
	ImDrawList* Draw = ImGui::GetWindowDrawList();
	ImColor C_TabActive = ImColor(255, 255, 255, 255);
	ImColor C_TabHovered = ImColor(220, 220, 220, 225);
	ImColor C_TabDefault = ImColor(180, 180, 180, 225);
	ImColor C_Text;

	ImVec2 MousePos = ImGui::GetMousePos();
	ImVec2 RectPos, RectSize;
	ImVec2 MenuPos = ImGui::GetWindowPos();

	RectPos = { TextPos.x - 30,TextPos.y - 7 };
	RectSize = { 120,37 };

	ImGui::SetCursorPos({ RectPos.x - MenuPos.x  ,RectPos.y - MenuPos.y });
	if (ImGui::Button((std::string("##") + Text).c_str(), RectSize))
	{
		Tab1 = Index;
		Tab2 = 0;
	}

	if (MousePos.x > RectPos.x && MousePos.x<RectPos.x + RectSize.x && MousePos.y>RectPos.y && MousePos.y < RectPos.y + RectSize.y)
	{
		if (Tab1 != Index)
		{
			C_Text = C_TabHovered;
		}
		else
		{
			C_Text = C_TabActive;
		}
	}
	else if (Tab1 == Index)
	{
		C_Text = C_TabActive;
	}
	else
	{
		C_Text = C_TabDefault;
	}
	
	if(Tab1==Index)
		Draw->AddRectFilled({ RectPos.x,RectPos.y }, { RectPos.x + RectSize.x, RectPos.y + RectSize.y }, ImColor(65, 75, 90, 255), 10);
	Draw->AddText(gui.Font1, 20, TextPos, C_Text, Text);
}

void TabSelect2(ImVec2 RectPos, const char* Text, int Index)
{
	ImDrawList* Draw = ImGui::GetWindowDrawList();
	ImColor C_TabActive = ImColor(255, 255, 255, 255);
	ImColor C_TabHovered = ImColor(220, 220, 220, 225);
	ImColor C_TabDefault = ImColor(180, 180, 180, 225);
	ImColor C_Text;

	ImVec2 MousePos = ImGui::GetMousePos();
	ImVec2 TextPos, RectSize;
	ImVec2 MenuPos = ImGui::GetWindowPos();
	ImVec2 TextSize = gui.Font1->CalcTextSizeA(15, FLT_MAX, 0, Text);

	RectSize = { 70,30 };
	// 居中
	TextPos = { RectPos.x + (RectSize.x - TextSize.x) / 2 ,RectPos.y + (RectSize.y - TextSize.y) / 2 };


	ImGui::SetCursorPos({ RectPos.x - MenuPos.x  ,RectPos.y - MenuPos.y });
	if (ImGui::Button((std::string("##") + Text).c_str(), RectSize))
	{
		Tab2 = Index;
	}

	if (MousePos.x > RectPos.x && MousePos.x<RectPos.x + RectSize.x && MousePos.y>RectPos.y && MousePos.y < RectPos.y + RectSize.y)
	{
		if (Tab2 != Index)
		{
			C_Text = C_TabHovered;
		}
		else
		{
			C_Text = C_TabActive;
		}
	}
	else if (Tab2 == Index)
	{
		C_Text = C_TabActive;
	}
	else
	{
		C_Text = C_TabDefault;
	}

	if (Tab2 == Index)
		Draw->AddLine({TextPos.x,TextPos.y+20}, {TextPos.x + TextSize.x,TextPos.y+20}, ImColor(65, 75, 90, 255), 2);
	Draw->AddText(gui.Font1, 15, TextPos, C_Text, Text);
}

void Tab()
{
	ImVec2 MenuPos = ImGui::GetWindowPos();
	ImDrawList* Draw = ImGui::GetWindowDrawList();

	// 相对坐标
	auto Calc = [&](float x, float y)->ImVec2
	{
		return ImVec2(MenuPos.x + x, MenuPos.y + y);
	};

	// Tab1
	TabSelect1(Calc(45, 30), "Visual", 0);
	TabSelect1(Calc(45, 90), "Aimbot", 1);
	TabSelect1(Calc(45, 150), "Memory", 2);
	TabSelect1(Calc(45, 210), "Debug", 3);
	TabSelect1(Calc(45, 270), "Any1", 4);
	TabSelect1(Calc(45, 330), "Any2", 5);
	TabSelect1(Calc(45, 390), "Any3", 6);
	/*
	Sample:

	TabSelect1(Calc(45, 30), "Any", 0);
	TabSelect1(Calc(45, 90), "Any", 1);

	TabSelect1(Calc(45, 30+i*60), "Any", i);

	*/

	// Tab2
	switch (Tab1)
	{
	case 0:
	{
		// #Visual
		TabSelect2(Calc(170, 25), "Esp", 0);
		TabSelect2(Calc(240, 25), "Radar", 1);
		TabSelect2(Calc(310, 25), "Color", 2);
		break;
	}
	case 1:
	{
		// #Aimbot
		TabSelect2(Calc(170, 25), "Aim", 0);
		break;
	}
	case 2:
	{
		// #Memory
		/*
		Sample:

		TabSelect2(Calc(170, 25), "Any", 0);
		TabSelect2(Calc(240, 25), "Any", 1);
		TabSelect2(Calc(310, 25), "Any", 2);

		TabSelect2(Calc(170+i*70, 25), "Any", i);

		*/
		break;
	}
	case 3:
	{
		// #Debug
		// ...

		break;
	}
	default:
		break;
	}
}

void StyleSet()
{
	ImGui::GetStyle().FrameRounding = 7;
	ImGui::GetStyle().PopupRounding = 7;

	ImVec4* Colors = ImGui::GetStyle().Colors;

	Colors[ImGuiCol_Button] = ImColor(0, 0, 0, 0);
	Colors[ImGuiCol_ButtonActive] = ImColor(0, 0, 0, 0);
	Colors[ImGuiCol_ButtonHovered] = ImColor(0, 0, 0, 0);
	Colors[ImGuiCol_PopupBg] = ImColor(15, 20, 35, 230);
	Colors[ImGuiCol_FrameBg] = ImColor(40, 45, 55, 240);
	Colors[ImGuiCol_FrameBgHovered] = ImColor(35, 40, 50, 240);
	Colors[ImGuiCol_FrameBgActive] = ImColor(43, 48, 58, 240);
	Colors[ImGuiCol_Header] = ImColor(40, 45, 55, 240);
	Colors[ImGuiCol_HeaderActive] = ImColor(43, 48, 58, 240);
	Colors[ImGuiCol_HeaderHovered] = ImColor(35, 40, 50, 240);
	Colors[ImGuiCol_Border] = ImColor(15, 20, 35, 230);
}


std::string replaceBackslashes(const std::string& str)
{
	std::string result;
	for (char ch : str)
	{
		if (ch == '\\')
			result += "\\\\";
		else
			result += ch;
	}
	return result;
}

HMODULE GetModuleHandleByName(DWORD processId, const wchar_t* moduleName) {
	HMODULE hModule = NULL;
	HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE | TH32CS_SNAPMODULE32, processId);
	if (snapshot != INVALID_HANDLE_VALUE) {
		MODULEENTRY32W moduleEntry = { sizeof(MODULEENTRY32W) };
		if (Module32FirstW(snapshot, &moduleEntry)) {
			do {
				if (_wcsicmp(moduleEntry.szModule, moduleName) == 0) {
					hModule = moduleEntry.hModule;
					break;
				}
			} while (Module32NextW(snapshot, &moduleEntry));
		}
		CloseHandle(snapshot);
	}
	return hModule;
}
void BaseAddInitial()
{
	GetWindowThreadProcessId(GameWnd, &GamePID);
	Read.InitProcessHandleForWindow("WinPlayer", NULL);
	hpro = Read.GetProcessHandle();


	//CHAR fullPath[MAX_PATH] = { 0 };
	//unsigned long len = _countof(fullPath);
	//::QueryFullProcessImageNameA(hpro, 0, fullPath, &len);
	//	std::cout << "文件路径：" << fullPath << std::endl;
	//	std::string pathString(fullPath);
	//	//std::string pattern = "minigameapp.exe";
	//	pathString.replace(pathString.find("minigameapp.exe"), 15, "");

	//	std::string libPath = pathString + "libengine.dll";
	//	libPath = replaceBackslashes(libPath);
	//	HMODULE hModule = GetModuleHandleA("libengine.dll");

	//	cout << hModule << endl;


	//	FARPROC functionAddress = GetProcAddress(hModule, "libEngine.BTXW2okqbV3vkCYSv75rJ1hmZSzX4K9mIXE8qzAmZ4");
	//	std::cout << "函数指针地址：" << functionAddress << std::endl;
	//	FreeLibrary(hModule);
	//HMODULE hModule = GetModuleHandleByName(GamePID, L"libEngine.dll");



	GameBase.CameraHandle = Read.GetModuleAddress((char*)"libEngine.dll");
	GameBase.ModuleHandle = Read.GetModuleAddress((char*)"libiworld.dll");
	if (GameBase.ModuleHandle == 0)
	{
		MessageBoxW(NULL, L"数据读取出错，即将退出！", L"温馨提示", 0);
		exit(0);
	}
	//GameBase.CrossX64 = (unsigned __int64)GetModuleHandleW(L"crossfire.exe");

	//GameBase.CameraAddr = Read.ReadMemoryDword((Read.GetAddress((char*)"8D 45 ?? 64 A3 00 00 00 00 8B F1 89 75 ?? 8B 3D", GameBase.ModuleHandle, 16)));//1
	GameBase.CameraAddr = Read.ReadMemoryDword(Read.GetAddress((char*)"01 7E 14 5F 5E 5B 5D C2 08 00 CC A1", GameBase.CameraHandle, 12));
	GameBase.CameraAddr = Read.Linked_Integer(GameBase.CameraAddr, 0x2C, 0x98, 0xC4);

	//GameBase.人物数据 = Read.ReadMemoryDword((Read.GetAddress((char*)"55 8B EC FF 75 14 8A 45", GameBase.ModuleHandle, 25)));//1
	//GameBase.人物数据 = Read.Linked_Integer(GameBase.人物数据, 0x3C0);
	GameBase.PlayerArray = Read.ReadMemoryDword(Read.GetAddress((char*)"FF D6 50 FF 15 ?? ?? ?? ?? 83 C4 ?? 5F 5E C3 A1", GameBase.ModuleHandle, 16));//1
	GameBase.SelfAddr = Read.ReadMemoryDword((Read.GetAddress((char*)"57 89 45 ?? 8B 89 ?? ?? ?? ?? 83 C1 ?? 8B 11", GameBase.ModuleHandle, -4)));//1
	cout << GameBase.CameraAddr << endl;
	// 关闭进程句柄
	//CloseHandle(hpro);
	
}
void GetRolePos(Vec3D* ReturnPos, Vec3D* MyPos, int i, float* health, ScreenPos* Angle, DWORD* Enemy_Group, DWORD* My_Group)
{
	ReturnPos->x = Read.ReadMemoryDword(Read.Linked_Integer(GameBase.PlayerArray, 0x40, 0x58, i * 4, 0x14));
	ReturnPos->y = Read.ReadMemoryDword(Read.Linked_Integer(GameBase.PlayerArray, 0x40, 0x58, i * 4, 0x18));
	ReturnPos->z = Read.ReadMemoryDword(Read.Linked_Integer(GameBase.PlayerArray, 0x40, 0x58, i * 4, 0x1C));
	//*health = Read.ReadMemory<float>(Read.Linked_Integer(GameBase.PlayerArray, 0x40, 0x48, i * 4, 0x10));
	Angle->x = Read.ReadMemory<float>(Read.Linked_Integer2(GameBase.SelfAddr, 0x918, 0));
	MyPos->x = Read.ReadMemory<DWORD>(Read.Linked_Integer2(GameBase.SelfAddr, 0x3E8, 0xD8));
	MyPos->y = Read.ReadMemory<DWORD>(Read.Linked_Integer2(GameBase.SelfAddr, 0x3E8, 0xDC));
	MyPos->z = Read.ReadMemory<DWORD>(Read.Linked_Integer2(GameBase.SelfAddr, 0x3E8, 0xE0));
	*Enemy_Group = Read.ReadMemoryDword(Read.Linked_Integer(GameBase.PlayerArray, 0x40, 0x58, i * 4, 0x24));
	//*My_Group = Read.ReadMemory<DWORD>(GameBase.ModuleHandle + 0x25E05CC);
}


void Menu()
{
	// 菜单配置项
	static bool Box = true;
	static bool ShowLine = true;
	static bool ShowMagic = true;
	static bool ShowMagicCircle = false;
	static bool ShowWarningPoint = false;
	static bool Aimbot = true;
	static float BoxColor[]{ 1,1,1,1 };
	static float LineColor[]{ 1,1,1,1 };
	static int BoxType = 0;
	static int  Int = 1;
	static int  Int2 = 1;
	static float Float = 1.0;
	static float Float2 = 1.0;

	StyleSet();
	ImGui::Begin("Menu", nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse | ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoTitleBar);
	if (!MenuOpen)
	{
		ImGui::SetWindowSize({ 0, 0 });
		//return;
	}

	const ImVec2 MenuSize{ 600,500 };// 高度可调，宽度调整后请自行校准其他组件位置
	ImGui::SetWindowSize(MenuSize);

	ImVec2 MenuPos = ImGui::GetWindowPos();
	ImDrawList* Draw = ImGui::GetWindowDrawList();

	// 相对坐标计算函数
	auto Calc = [&](float x, float y)->ImVec2
	{
		return ImVec2(MenuPos.x + x, MenuPos.y + y);
	};

	if (MenuOpen)
	{	
		// 菜单背景
		Draw->AddRectFilled(MenuPos, { MenuPos.x + MenuSize.x,MenuPos.y + MenuSize.y }, ImColor(15, 20, 35, 230), 12);

		ImGui::PushFont(gui.Font1);

		// 分隔符
		Draw->AddLine(Calc(150, 20), Calc(150, MenuSize.y - 20), ImColor(45, 55, 75, 240), 2);

		// Tab2背景
		Draw->AddRectFilled(Calc(165, 20), Calc(580, 60), ImColor(25, 30, 40, 240), 7);
		Draw->AddRectFilled(Calc(165, 70), Calc(580, MenuSize.y - 20), ImColor(25, 30, 40, 240), 7);

		Tab();

		if (Tab1 == 0)
		{
			// Visual
			switch (Tab2)
			{
			case 0:
			{
				// #Esp
				ImGui::SetCursorPos({ 175,80 });
				gui.MyCheckBox2("Box", &Box);

				ImGui::SetCursorPos({ 400,80 });
				const char* TypeCombo[]{
					"2D Normal","3D Normal","3D Dynamic"
				};
				ImGui::Combo("Type", &BoxType, TypeCombo, 3);

				ImGui::SetCursorPos({ 175,110 });
				gui.MyCheckBox2("Line", &ShowLine);

				ImGui::SetCursorPos({ 175,140 });
				gui.MyCheckBox2("Magic", &ShowMagic);
				
				ImGui::SetCursorPos({ 175,170 });
				gui.MyCheckBox2("MagicCircle", &ShowMagicCircle);

				ImGui::SetCursorPos({ 175,200 });
				gui.MyCheckBox2("WarningPoint", &ShowWarningPoint);
				

				break;
			}
			case 1:
			{
				// #Radar
				break;
			}
			case 2:
			{
				// #Color
				ImGui::SetCursorPos({ 175,80 });
				ImGui::ColorEdit3("Box", BoxColor, ImGuiColorEditFlags_NoInputs);

				ImGui::SetCursorPos({ 175,110 });
				ImGui::ColorEdit3("Line", LineColor, ImGuiColorEditFlags_NoInputs);

				ImGui::SetCursorPos({ 175,140 });
				gui.SliderIntEx("Int", &Int, 1, 10);

				ImGui::SetCursorPos({ 175,170 });
				gui.SliderFloatEx("Float", &Float, 1.0f, 10.0f, "%.1f");

				ImGui::SetCursorPos({ 175,200 });
				gui.SliderIntEx2("Int2", &Int2, 1, 10);

				ImGui::SetCursorPos({ 175,230 });
				gui.SliderFloatEx2("Float2", &Float2, 1.0f, 10.0f, "%.1f");
				break;
			}
			default:
				break;
			}
		}

		if (Tab1 == 1)
		{
			// Aimbot
			switch (Tab2)
			{
			case 0:
			{
				break;
			}
			case 1:
			{
				break;
			}
			case 2:
			{
				break;
			}
			default:
				break;
			}
		}
		ImGui::PopFont();

	}
	
	static bool isSearch = true;
	if (isSearch) { BaseAddInitial(); 
	isSearch = false;
	}

	RefreshWindow();
	int RoleNum = 40;
	float Enemy_MeDis;
	//float AimDis = AimSpeed;
	DWORD Enemy_Group, My_Group;
	Vec2 WindowCenter{ (float)GameCentX / 2,(float)GameCentY / 2 };
	Vec3D MyDisPos = { 0 };
	Vec3D RolePos = { 0 };
	ScreenPos EnemyPos = { 0 };
	ScreenPos CameraPos = { 0 };
	ScreenPos Camera = { 0 };
	float health;
	ScreenPos Angle = { 0 };
	ScreenPos AimAngle = { 0 };
	Vec3D MyPos = { 0 };
	char BloodText[15] = { NULL };
	int Ret;
	for (int i = 0; i < RoleNum - 1; i++)
	{
		GetRolePos(&RolePos, &MyPos, i, &health, &Angle, &Enemy_Group, &My_Group);
		if (RolePos.x == 0 || RolePos.y == 0 || RolePos.z == 0)return;

		if(WorldToScreen(RolePos, &CameraPos, GameWnd, hpro, 220) == false)return;

		if (Box) {
			switch (BoxType)
			{
			case 0:
			{
				DrawBox(CameraPos, BoxColor, 1.5);
				break;
			}
			case 1:
			{
				ThreeDBox(RolePos, 2, 220, BoxColor, GameWnd, hpro);
				break;
			}
			case 2:
			{
				ThreeDBox(RolePos, 2, 220, BoxColor, GameWnd, hpro);
				break;
			}

			}

		}

		if(ShowLine)ImGui::GetForegroundDrawList()->AddLine(ImVec2((float)GameCentX, 0), ImVec2(CameraPos.x, CameraPos.z), ImColor(Color(LineColor)) , 2);

		if(ShowMagic)Rotating_normal_array(RolePos, 100.0f, 2, GameWnd, hpro);

		if(ShowMagicCircle)CirCle3D_RainBow(RolePos, 100.0f, GameWnd, hpro);

		if(ShowWarningPoint)WarningPoint(RolePos, MyPos, Angle.x, WindowCenter, 120, { 15,15 }, ImColor(255, 52, 66, 170));
		
	}
}




void Draw_()
{
	if (GetAsyncKeyState(VK_HOME) & 0x8000)
	{
		// 定时事件
		static DWORD PreTimeCount = GetTickCount();
		DWORD CurrentTickCount = GetTickCount();
		// 170ms/count
		if (CurrentTickCount - PreTimeCount > 170)
		{
			PreTimeCount = CurrentTickCount;
			MenuOpen = !MenuOpen;
		}
	}
	
	Menu();
	
	// ... 
}
int main()
{
	//SetConsoleTitleA("Title");
	//Sleep(500);
	//std::thread t(GameDraw);
	gui.Start("", "迷你世界", Draw_);
	
	return 0;
}
