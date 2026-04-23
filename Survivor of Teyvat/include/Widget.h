#pragma once
#include <easyx.h>
#include <vector>
#include "Object.h"

// Widget 表示一个游戏场景，例如菜单、战斗场景或角色选择界面。
class Widget
{
public:
	// 场景尺寸。
	int width_widget;
	int height_widget;

	// 当前帧读取到的输入消息。
	ExMessage m_msg;

	// 场景背景图，Widget 拥有并负责释放。
	IMAGE* page = nullptr;

	// 场景中的对象列表，Widget 拥有并负责释放。
	std::vector<Object*> m_objects;

	// 初始化场景对象与背景。
	void init_widget(int Width, int height, std::vector<Object*> objects, IMAGE* page);

	// 释放背景图和对象列表，允许析构或提前关闭时重复安全调用。
	void close_widget()
	{
		delete page;
		page = nullptr;

		for (Object* object : m_objects)
			delete object;

		m_objects.clear();
	}

	Widget(int width, int height, std::vector<Object*> objects, IMAGE* page) : width_widget(width), height_widget(height)
	{
		init_widget(width, height, objects, page);
	}

	~Widget()
	{
		close_widget();
	}

	// 更新场景输入与对象状态。
	void run_widget();

	// 绘制场景背景和所有对象。
	void draw_widget();

	// 运行一帧：处理输入、绘制画面，并限制到约 144 FPS。
	void show_widget()
	{
		DWORD start_time = GetTickCount();

		run_widget();
		draw_widget();

		DWORD end_time = GetTickCount();
		DWORD gap_time = end_time - start_time;
		constexpr DWORD frame_time_ms = 1000 / 144;

		if (gap_time < frame_time_ms)
			Sleep(frame_time_ms - gap_time);
	}
};
