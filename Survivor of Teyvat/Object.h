#pragma once
#include<iostream>
#include<easyx.h>
#include<vector>

class Atlas;//图集类

//抽象物体类
class Object
{
public:
	//坐标(左上角
	int x;
	int y;

	//大小
	int m_width;
	int m_height;

	
	//物体运行逻辑

	Object(int x, int y, int width, int height) :x(x), y(y), m_width(width), m_height(height)
	{
	}

	virtual void load_object(const ExMessage& msg) = 0;

	virtual void draw_object(int delta) = 0;

};