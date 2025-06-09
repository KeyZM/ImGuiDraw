#pragma once
#include "OwnImGui.h"
#include "Struct.h"
#include "Memory.h"
#include <tuple>
ReadWrite Memory;
//void ShowRect(const CEntity* Entity, const float ThickNess, int Color);
BOOL WorldToScreen(Vec3D Enemy_Pos, ScreenPos* ReturnPos, HWND GameWnd, HANDLE hpro, int High);
void Line(ScreenPos From, ScreenPos To, int color, int thickness);
void Line_two(Vec2 From, Vec2 To, int color, int thickness);
void DrawLine(float start_x, float start_y, float End_x, float End_y, float px, float* color);
void DrawCrossHair(RECT rect, int width, int color);
ScreenPos RadarCalc(float AngleX, Vec3D OwnPos, Vec3D EnemyPos, int distance);
void DrawAimRange(RECT rect, int 范围, int color, int width);
void DrawCornerBox(int x, int y, int w, int h, int color);
void Rotating_normal_array(Vec3D Enemy_Pos, float radius, int thickness, HWND GameWnd, HANDLE hpro);
void CirCle3D_RainBow(Vec3D Enemy_Pos, const float Radius, HWND GameWnd, HANDLE hpro);
void ThreeDBox(Vec3D pos, float px, int H, float* color, HWND GameWnd, HANDLE hpro);
void LightChange(float* Color);
int COLOR_RGBA(const float* Color);
float g_ViewWorld[4][4] = { 0 };
void DrawBox(ScreenPos Pos, float* color, float thickness);
void Circle(ScreenPos Center, float radius, int color, float thickness);
void CircleFilled(ScreenPos Center, float radius, int color, int num=360);
float Get_Mydis(float MyDisX, float MyDisY, ScreenPos EnemyDis);
void AimBot_Algorithm(ScreenPos MyPos, ScreenPos EnemyPos, ScreenPos* Angle);
void GetRolePos(Vec3D* ReturnPos, Vec3D* MyPos = NULL, int i = NULL, float* health = NULL, ScreenPos* Angle = NULL, DWORD* Enemy_Group = NULL, DWORD* My_Group = NULL);
void WarningPoint(const Vec3D EntityPos, const Vec3D LocalPos,const float AngleX, Vec2 Center, float Radius, Vec2 Size, ImColor Color);
int Color(float* Color_);
DWORD ViewWidth;
DWORD ViewHigh;
ScreenPos Photo = { 0,0,0 };
int Time_difference = 0;
float angle = 0;



#define M_PI       3.14159265358979323846   // pi
BOOL WorldToScreen(Vec3D Enemy_Pos, ScreenPos* ReturnPos, HWND GameWnd, HANDLE hpro,int High)
{
	ReadProcessMemory(hpro, (void*)GameBase.CameraAddr, g_ViewWorld, sizeof(g_ViewWorld), NULL);
	ViewWidth = Memory.GetwindowW(GameWnd) / 2;
	ViewHigh = Memory.GetwindowH(GameWnd) / 2;

	Photo.z = g_ViewWorld[0][3] * Enemy_Pos.x + g_ViewWorld[1][3] * Enemy_Pos.y + g_ViewWorld[2][3] * Enemy_Pos.z + g_ViewWorld[3][3];

	if (Photo.z <= 0)return false;
	Photo.x = ViewWidth + (g_ViewWorld[0][0] * Enemy_Pos.x + g_ViewWorld[1][0] * Enemy_Pos.y + g_ViewWorld[2][0] * Enemy_Pos.z + g_ViewWorld[3][0]) / Photo.z * ViewWidth;
	Photo.y = ViewHigh - (g_ViewWorld[0][1] * Enemy_Pos.x + g_ViewWorld[1][1] * Enemy_Pos.y + g_ViewWorld[2][1] * Enemy_Pos.z + g_ViewWorld[3][1]) / Photo.z * ViewHigh;
	Photo.h = ViewHigh - (g_ViewWorld[0][1] * Enemy_Pos.x + g_ViewWorld[1][1] * (Enemy_Pos.y + High)+ g_ViewWorld[2][1] * Enemy_Pos.z + g_ViewWorld[3][1]) / Photo.z * ViewHigh;
	Photo.w = Photo.h - Photo.y;

	ReturnPos->x = Photo.x;
	ReturnPos->y = Photo.y;
	ReturnPos->z = Photo.h;
	ReturnPos->h = Photo.w;
	return true;
}
void AimBot_Algorithm(ScreenPos MyPos, ScreenPos EnemyPos, ScreenPos* Angle)
{
	ScreenPos Enemy;
	
	Enemy.x = EnemyPos.x - MyPos.x;
	Enemy.y = EnemyPos.y - MyPos.y;
	Enemy.z = EnemyPos.z - MyPos.z - 10;
	if (Enemy.x > 0 && Enemy.y > 0)Angle->x= 90 - atan(Enemy.y / Enemy.x) / M_PI * 180;
	if (Enemy.x < 0 && Enemy.y >= 0)Angle->x = 270 - atan(Enemy.y / Enemy.x) / M_PI * 180;
	if (Enemy.x <= 0 && Enemy.y <= 0)Angle->x = 270 - atan(Enemy.y / Enemy.x) / M_PI * 180;
	if (Enemy.x >= 0 && Enemy.y < 0)Angle->x = 90 - atan(Enemy.y / Enemy.x) / M_PI * 180;
	Angle->y = atan(Enemy.z / sqrt(Enemy.x * Enemy.x + Enemy.y * Enemy.y)) / M_PI * 180;

}

 
float Get_Mydis(float MyDisX, float MyDisY, ScreenPos EnemyDis)
{
	ScreenPos Dis;
	float RetDis;
	Dis.x= MyDisX - EnemyDis.x;
	Dis.y = MyDisY - EnemyDis.y;
	RetDis = sqrt(Dis.x * Dis.x + Dis.y * Dis.y);
	return RetDis;
}

int Color(float* Color_)
{
	return IM_COL32(Color_[0] * 255, Color_[1] * 255, Color_[2] * 255, Color_[3] * 255);
}
void LightChange(float* Color)
{
	float speed = 10;
	Color[0] = sin(speed / 1500 * clock()) * 0.5 + 0.5;
	Color[1] = sin(speed / 1500 * clock() + 2 * M_PI / 3) * 0.5 + 0.5;
	Color[2] = sin(speed / 1500 * clock() + 4 * M_PI / 3) * 0.5 + 0.5;
}
void Line(ScreenPos From, ScreenPos To, int color, int thickness)
{
	ImGui::GetForegroundDrawList()->AddLine(ImVec2(From.x, From.y), ImVec2(To.x, To.y), color, thickness);
}
void Line_two(Vec2 From, Vec2 To, int color, int thickness)
{
	ImGui::GetForegroundDrawList()->AddLine(ImVec2(From.x, From.y), ImVec2(To.x, To.y), color, thickness);
}
// 雷达算法
ScreenPos RadarCalc(float AngleX, Vec3D OwnPos, Vec3D EnemyPos,int distance)
{
	float PosX = (OwnPos.x - EnemyPos.x) / distance;
	float PosY = (OwnPos.z - EnemyPos.z) / distance;
	float Mydistance= sqrt(PosX* PosX+ PosY* PosY);
	float angle;
	ScreenPos result;
	// 雷达坐标(自定)
	ScreenPos RadarPos;

	if (PosX >= 0 && PosY >= 0)angle = atan(PosX / PosY) / M_PI * 180 + 180 - AngleX;
	if (PosX <= 0 && PosY >= 0)angle = atan(PosX / PosY) / M_PI * 180 + 180 - AngleX;
	if (PosX <= 0 && PosY <= 0)angle = atan(PosX / PosY) / M_PI * 180  -  AngleX;
	if (PosX >= 0 && PosY <= 0)angle = atan(PosX / PosY) / M_PI * 180 + 360 - AngleX;

	RadarPos.x =  cos(angle* M_PI/  180)* Mydistance;
	RadarPos.y =  sin(angle * M_PI / 180)* Mydistance;
	// 计算结果坐标点
	result.x = RadarPos.y;
	result.y = RadarPos.x*-1;
	return result;
}

ImColor CrossColor = ImColor(0, 0, 0, 255);
std::vector<std::tuple<Vec2, ImColor, int, float>> Points;
float PointRadius = 4;
// 雷达范围
float RenderRange = 150;
// 比例
float Proportion = 2680;
// 本地Yaw数据
float LocalYaw = 0.0f;
// 箭头尺寸
float ArrowSize = 11;
void Render(Vec2  Pos, float Width, float AngleX);

void Render(Vec2  Pos, float Width,float AngleX)
{
	if (Width <= 0)
		return;
	LocalYaw = AngleX;
	// Cross
	std::pair<Vec2, Vec2> Line1;
	std::pair<Vec2, Vec2> Line2;
	std::pair<Vec2, Vec2> Line3;
	std::pair<Vec2, Vec2> Line4;

	Line1.first = Vec2(Pos.x - Width / 2,Pos.y);
	Line1.second = Vec2(Pos.x + Width / 2, Pos.y);

	Line2.first = Vec2(Pos.x, Pos.y - Width / 2);
	Line2.second = Vec2(Pos.x, Pos.y + Width / 2);

	Line3.second = Vec2(Pos.x - Width / 2, Pos.y - Width / 2);
	Line3.first = Vec2(Pos.x, Pos.y );

	Line4.second = Vec2(Pos.x + Width / 2, Pos.y - Width / 2);
	Line4.first = Vec2(Pos.x, Pos.y);

		Line_two(Line1.first, Line1.second, CrossColor, 1);
		Line_two(Line2.first, Line2.second, CrossColor, 1);
		Line_two(Line3.first, Line3.second, CrossColor, 1);
		Line_two(Line4.first, Line4.second, CrossColor, 1);

	if (Points.size() > 0)
		Points.clear();
}


void Circle(ScreenPos Center, float radius, int color, float thickness)
{

	ImGui::GetForegroundDrawList()->AddCircle(ImVec2(Center.x, Center.y), radius, color, max(radius, 50), thickness);
}
void CircleFilled(ScreenPos Center, float radius, int color, int num)
{
	ImGui::GetForegroundDrawList()->AddCircleFilled(ImVec2(Center.x, Center.y), radius, color, num);
}
int COLOR_RGBA(const float* Color)
{
	return ImColor(Color[0], Color[1], Color[2], Color[3]);
}
void Rotating_normal_array(Vec3D Enemy_Pos, float radius,int thickness, HWND GameWnd, HANDLE hpro)
{
	int Sides;
	Vec3D bottom[9];
	ScreenPos Bottom_point_screen_coordinates;//底点屏幕坐标
	ScreenPos Temporary_bottom;//临时底点坐标
	static float MagicCircleColor[4]{ 0,0,0,1 };
	//int i;
	LightChange(MagicCircleColor);
	float sine;
	for (int i = 0; i < 9; i++)
	{
		//GetTickCount() -
		if (GetTickCount() - Time_difference >= 8)
		{
			angle = angle + 1;
			Time_difference = GetTickCount();
		}
		if (angle == 360)
		{
			angle = 0;
		}
		sine = (i * 135 + angle) * M_PI / 180;
		bottom[i].x = Enemy_Pos.x + cos(sine) * radius;
		bottom[i].z = Enemy_Pos.z + sin(sine) * radius;
		bottom[i].y = Enemy_Pos.y;
		if(WorldToScreen(bottom[i], &Temporary_bottom, GameWnd, hpro, 220)==false)return;
		if (Temporary_bottom.x + Temporary_bottom.y <= 0)
		{
			break;
		}
		if (i != 0)
		{
			Line(Bottom_point_screen_coordinates, Temporary_bottom, COLOR_RGBA(MagicCircleColor), thickness);
		}
		Bottom_point_screen_coordinates = Temporary_bottom;
	}

}
void CirCle3D_RainBow(Vec3D Enemy_Pos, const float Radius, HWND GameWnd, HANDLE hpro)
{
	float flPoint = M_PI * 2.0f / 97;
	float R = 255.0f; float G = 0.0f; float B = 0.0f;
	int Counts = 0;

	for (float flAngle = 0; flAngle < (M_PI * 2.0f); flAngle += flPoint)
	{
		G += (Counts == 0 ? 15 : 0);
		if (Counts == 0 && G >= 255) Counts++;
		R -= (Counts == 1 ? 15 : 0);
		if (Counts == 1 && R <= 0)   Counts++;
		B += (Counts == 2 ? 15 : 0);
		if (Counts == 2 && B >= 255) Counts++;
		G -= (Counts == 3 ? 15 : 0);
		if (Counts == 3 && G <= 0)   Counts++;
		R += (Counts == 4 ? 15 : 0);
		if (Counts == 4 && R >= 255) Counts++;
		B -= (Counts == 5 ? 15 : 0);
		if (Counts == 5 && B <= 0)   Counts++;
		Vec3D Start;
		Vec3D End;
		Start.x = Radius * cosf(flAngle) + Enemy_Pos.x;
		Start.z = Radius * sinf(flAngle) + Enemy_Pos.z;
		Start.y = Enemy_Pos.y;

		End.x = Radius * cosf(flAngle + flPoint) + Enemy_Pos.x;
		End.z = Radius * sinf(flAngle + flPoint) + Enemy_Pos.z;
		End.y = Enemy_Pos.y;

		ScreenPos StartSc;
		ScreenPos EndSc;

		 if(WorldToScreen(Start, &StartSc, GameWnd, hpro, 220)==false)return;
		 WorldToScreen(End,&EndSc, GameWnd, hpro,220);
		Line(StartSc, EndSc, IM_COL32(R, G, B, 255),2);//下层圆彩色
	}

}
void ThreeDBox(Vec3D pos, float px, int H, float* color, HWND GameWnd, HANDLE hpro)
{

	ScreenPos Camera1 = { 0 };
	ScreenPos Camera2 = { 0 };
	ScreenPos Camera3 = { 0 };
	ScreenPos Camera4 = { 0 };
	ScreenPos Camera5 = { 0 };
	ScreenPos Camera6 = { 0 };
	ScreenPos Camera7 = { 0 };
	ScreenPos Camera8 = { 0 };


	 if(WorldToScreen({ pos.x - H / 4, pos.y - H / 2, pos.z - H / 4 }, &Camera1, GameWnd, hpro, 90)==false)return;
	 WorldToScreen({ pos.x + H / 4, pos.y - H / 2, pos.z - H / 4 }, &Camera2,GameWnd, hpro, 90);
	 WorldToScreen({ pos.x + H / 4, pos.y - H / 2, pos.z + H / 4 }, &Camera3, GameWnd, hpro, 90);
	 WorldToScreen({ pos.x - H / 4, pos.y - H / 2, pos.z + H / 4 }, &Camera4, GameWnd, hpro, 90);

	 WorldToScreen({ pos.x - H / 4, pos.y + H / 2, pos.z - H / 4 }, &Camera5, GameWnd, hpro, 90);
	 WorldToScreen({ pos.x + H / 4, pos.y + H / 2, pos.z - H / 4 }, &Camera6, GameWnd, hpro, 90);
	 WorldToScreen({ pos.x + H / 4, pos.y + H / 2, pos.z + H / 4 }, &Camera7, GameWnd, hpro, 90);
	 WorldToScreen({ pos.x - H / 4, pos.y + H / 2, pos.z + H / 4 }, &Camera8, GameWnd, hpro, 90);

	DrawLine(Camera1.x, Camera1.z, Camera5.x, Camera5.z, px, color);
	DrawLine(Camera2.x, Camera2.z, Camera6.x, Camera6.z, px, color);
	DrawLine(Camera3.x, Camera3.z, Camera7.x, Camera7.z, px, color);
	DrawLine(Camera4.x, Camera4.z, Camera8.x, Camera8.z, px, color);

	DrawLine(Camera1.x, Camera1.z, Camera2.x, Camera2.z, px, color);
	DrawLine(Camera3.x, Camera3.z, Camera4.x, Camera4.z, px, color);

	DrawLine(Camera1.x, Camera1.z, Camera4.x, Camera4.z, px, color);
	DrawLine(Camera2.x, Camera2.z, Camera3.x, Camera3.z, px, color);

	DrawLine(Camera5.x, Camera5.z, Camera6.x, Camera6.z, px, color);
	DrawLine(Camera7.x, Camera7.z, Camera8.x, Camera8.z, px, color);

	DrawLine(Camera5.x, Camera5.z, Camera8.x, Camera8.z, px, color);
	DrawLine(Camera6.x, Camera6.z, Camera7.x, Camera7.z, px, color);
}

void DrawLine(float start_x, float start_y, float End_x, float End_y, float px, float* color)
{
	ImGui::GetForegroundDrawList()->AddLine(ImVec2(start_x, start_y), ImVec2(End_x, End_y), ImColor(Color(color)), px);
}
void DrawBox(ScreenPos Pos, float* color,float thickness)
{

	float BoxHigh = Pos.y - Pos.z;
	float BoxWide = BoxHigh * 0.526515151552;
	ImGui::GetForegroundDrawList()->AddRect(ImVec2(Pos.x - BoxWide / 2, Pos.z), ImVec2(Pos.x - BoxWide / 2 + BoxWide, Pos.z + BoxHigh), ImColor(Color(color)), 0.0F,15, thickness);

}
void WarningSingle(Vec2 Center, float Radius, float Angle, Vec2 Size, ImColor Color)
{
	Vec2 TopPoint = Center;

	TopPoint.x += (Radius + Size.y) * cos(Angle / 180 * M_PI);
	TopPoint.y += (Radius + Size.y) * sin(Angle / 180 * M_PI);

	ImDrawList* DrawList = ImGui::GetForegroundDrawList();

	DrawList->PathLineTo({ TopPoint.x,TopPoint.y });
	DrawList->PathArcTo({ Center.x,Center.y }, Radius, (Angle - Size.x) / 180 * M_PI, (Angle + Size.x) / 180 * M_PI, 30);

	DrawList->PathFillConvex(Color);

	DrawList->PathLineTo({ TopPoint.x,TopPoint.y });
	DrawList->PathArcTo({ Center.x,Center.y }, Radius, (Angle - Size.x) / 180 * M_PI, (Angle + Size.x) / 180 * M_PI, 30);

	DrawList->PathStroke(Color, true, 2.5);
}

void WarningPoint(const Vec3D EntityPos, const Vec3D LocalPos, const float AngleX, Vec2 Center, float Radius, Vec2 Size, ImColor Color)
{
	float Angle = 0.f;
	Angle = atan2f((EntityPos.y - LocalPos.y), (EntityPos.x - LocalPos.x)) * 180 / M_PI;
	Angle = (AngleX - Angle) - 90;

	WarningSingle(Center, Radius, Angle, Size, Color);
}

void DrawAimRange(RECT rect, int 范围, int color, int width)
{
	int x = rect.right / 2;
	int y = rect.bottom / 2;
	ImGui::GetForegroundDrawList()->AddCircle(ImVec2(x, y), 范围, color, 0, width);
}

void DrawCrossHair(RECT rect, int width, int color)
{
	int x = rect.right / 2;
	int y = rect.bottom / 2;
	//DrawLine(x - 5, y, x + 6, y, color);
	//DrawLine(x, y - 5, x, y + 6, color);
	ImGui::GetForegroundDrawList()->AddLine(ImVec2(x - 8, y), ImVec2(x + 9, y), color, width);
	ImGui::GetForegroundDrawList()->AddLine(ImVec2(x, y - 8), ImVec2(x, y + 9), color, width);
}
void DrawCornerBox(int x, int y, int w, int h, int color)	//绘制角框
{
	int length = 8, width = 1;

	ImGui::GetBackgroundDrawList()->AddLine(ImVec2(x, y), ImVec2(x + length, y), color, width);
	ImGui::GetBackgroundDrawList()->AddLine(ImVec2(x, y), ImVec2(x, y + length), color, width);					//左上角角框
	ImGui::GetBackgroundDrawList()->AddLine(ImVec2(w, y), ImVec2(w - length, y), color, width);
	ImGui::GetBackgroundDrawList()->AddLine(ImVec2(w, y), ImVec2(w, y + length), color, width);					//右上角角框
	ImGui::GetBackgroundDrawList()->AddLine(ImVec2(x, h), ImVec2(x, h - length), color, width);
	ImGui::GetBackgroundDrawList()->AddLine(ImVec2(x, h), ImVec2(x + length, h), color, width);					//左下角角框
	ImGui::GetBackgroundDrawList()->AddLine(ImVec2(w, h), ImVec2(w, h - length), color, width);
	ImGui::GetBackgroundDrawList()->AddLine(ImVec2(w, h), ImVec2(w - length, h), color, width);					//右下角角框
}

