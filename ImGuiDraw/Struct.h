#pragma once
#include <Windows.h>
#include <vector>

/****************************************************
* Copyright (C): Liv
* @file		: Struct.h
* @author	: Liv
* @email		: 1319923129@qq.com
* @version	: 1.0
* @date		: 2022/4/17	15:57
****************************************************/

using std::vector;

struct Vec3 {
	float x, y, z;
	Vec3() { x = 0; y = 0; z = 0; }
	Vec3(float _x, float _y, float _z) { x = _x; y = _y; z = _z; }
	bool operator==(const float _t){
		return x == _t && y == _t && z == _t;
	}
	bool operator<(const float _t) {
		return x < _t&& y < _t&& z < _t;
	}
	bool operator>(const float _t) {
		return x > _t&& y > _t&& z > _t;
	}
	bool operator>=(const float _t) {
		return x >= _t && y >= _t && z >= _t;
	}
	bool operator<=(const float _t) {
		return x <= _t && y <= _t && z <= _t;
	}
	bool operator!=(const float _t) {
		return x != _t && y != _t && z != _t;
	}
};

struct Vec2 {
	float x, y;
	Vec2() { x = 0; y = 0; }
	Vec2(float _x, float _y) { x = _x; y = _y; }
	Vec2(Vec3 _t) { x = _t.x; y = _t.y; }
	bool operator==(const float _t) {
		return x == _t && y == _t;
	}
	bool operator<(const float _t) {
		return x < _t&& y < _t;
	}
	bool operator>(const float _t) {
		return x > _t && y > _t;
	}
	bool operator>=(const float _t) {
		return x >= _t && y >= _t;
	}
	bool operator<=(const float _t) {
		return x <= _t && y <= _t;
	}
	bool operator!=(const float _t) {
		return x != _t && y != _t;
	}
};
struct ��ַ
{
	unsigned int ModuleHandle;
	unsigned int CameraHandle;
	int CrossX64;
	int PlayerArray;
	int �ӽǵ�ַ;
	int ��������;
	int �����ַ;
	int �����С;
	int CameraAddr;
	int SelfAddr;
	int �ж�ģʽ;
	int ����͸��;
	int �Ƿ���Ϸ;
};
extern ��ַ GameBase;
extern int �ϴξ���, �������, ����BoneId, ��ǰ����, ����pawn, ��׼��Χ, GameCentX, GameCentY, Game_height, Game_wIde;
struct Vec3D
{
	int x;
	int y;
	int z;
	int H;
};
struct ScreenPos
{
	float x;
	float y;
	float z;
	float h;
	float w;
};
// 3D���������
struct RECT_3DPOS {
	vector<Vec3> Pos;
	vector<Vec2> ScreenPos;
};

// ���νṹ
struct RECTINFO {
	float  width, height;
	Vec2  point;
};

// ����ṹ
struct View_Matrix {
	float Matrix_[4][4];
	float* operator[](int index)
	{
		return Matrix_[index];
	}
};