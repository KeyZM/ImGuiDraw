#pragma once
#include "imgui/imgui.h"
#include "imgui/imgui_impl_win32.h"
#include "imgui/imgui_impl_dx11.h"
#include "imgui/imgui_internal.h"
#include "Struct.h"
#include <d3d11.h>
#include <iostream>
#include <tchar.h>
#include <dwmapi.h>
#include <atlstr.h>
#pragma comment(lib,"d3d11.lib")
#pragma comment(lib,"winmm.lib")
#pragma comment(lib,"dwmapi.lib")

/****************************************************
* Copyright (C): Liv
* @file		: OwnImGui.h
* @author	: Liv
* @email	: 1319923129@qq.com
* @version	: 1.1
* @date		: 2022/6/11	22:51
****************************************************/

// Datas
static ID3D11Device* g_pd3dDevice;
static ID3D11DeviceContext* g_pd3dDeviceContext;
static IDXGISwapChain* g_pSwapChain;
static ID3D11RenderTargetView* g_mainRenderTargetView;
typedef void(*CallBack)();
extern bool Clean;
extern WNDCLASSEX W_class;

struct WindowData {
	HWND hWindow;		// Ŀ�괰�ھ��
	HWND hMyWindow;		// ���ƴ��ھ��
	RECT WindowRect;	// Ŀ�괰������
	char WindowName[256];
	char WindowClassName[256];
	int  Width, Height; // Ŀ�괰�ڴ�С
};

class OwnImGui
{
private:
	// ��ʼ��
	void Initialize();
	// ��������
	bool CreateOwnWindow(HWND ToWindow);
	// ������Ϣѭ��
	void WindowMessageCycle(CallBack MainFunction);
	// ���ڻص�
	static LRESULT __stdcall WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
	// ��ʼ�����С
	size_t FontSize = 15;
	// ����
	bool done = false;
public:
	OwnImGui(const char* MenuName_);
	OwnImGui() { strcpy_s(MenuName, 4, "Menu"); }
	~OwnImGui();
	ImFont* Font1 = nullptr;
	ImFont* Font2 = nullptr;

	// �˵�����
	char MenuName[256]{};
	// �˵�״̬
	bool Open = true;
	// ������Ϣ
	WindowData Window;
	// ����������
	void Start(const char* WindowClassName, const char* WindowName, CallBack MainFunction);
	// �˳�����
	void End() { done = true; }
	// ���������С
	void ReSetFontSize(size_t FontSize_);
	// ��������(����ǰ����)
	void MyStyle();
	// Բ��ѡ���
	void CheckboxEx(const char* Text, bool* Point);
	// �����ı����Ƴ���
	int  CalcTextWidth(const char* Text);
	// �ı�
	void Text(const char* Text, Vec2 Pos,int color);
	// ����
	void Rectangle(Vec2 Pos, int width, int height, int color, int thickness);
	// ��
	void Line(Vec2 From, Vec2 To,int color,int thickness);
	// Բ
	void Circle(Vec2 Center, float radius, int color, float thickness);
	// ���Բ
	void CircleFilled(Vec2 Center, float radius, int color, int num = 360);
	// ������
	void RectangleFilled(Vec2 Pos, int width, int height, int color);
	// ����ı�
	void StrokeText(const char* Text, Vec2 Pos, int color_bg, int color);
	// ���ӵ�
	void ConnectPoint(std::vector<Vec2> Points, int color, float thickness);
	// ��ɫת��
	int  Color(float* Color_);

	// ��ѡ��
	void MyCheckBox(const char* str_id, bool* v);
	void MyCheckBox2(const char* str_id, bool* v);
	void MyCheckBox3(const char* str_id, bool* v);
	void MyCheckBox4(const char* str_id, bool* v);

	// Բͷ������
	bool SliderIntEx(const char* label, int* v, int v_min, int v_max, const char* format = "%.3f", ImGuiSliderFlags flags = 0);
	bool SliderFloatEx(const char* label, float* v, float v_min, float v_max, const char* format = "%.3f", ImGuiSliderFlags flags = 0);
	// ��们����
	bool SliderIntEx2(const char* label, int* v, int v_min, int v_max, const char* format = "%.3f", ImGuiSliderFlags flags = 0);
	bool SliderFloatEx2(const char* label, float* v, float v_min, float v_max, const char* format = "%.3f", ImGuiSliderFlags flags = 0);
};