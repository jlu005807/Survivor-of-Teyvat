#pragma once
#include <easyx.h>

class Atlas; // 图集类型前置声明，避免在基类中引入完整图集定义。

// 所有游戏对象的抽象基类，统一保存位置、尺寸和每帧更新/绘制接口。
class Object
{
public:
	// 对象左上角坐标。
	int x;
	int y;

	// 对象碰撞与绘制尺寸。
	int m_width;
	int m_height;

	Object(int x, int y, int width, int height) : x(x), y(y), m_width(width), m_height(height)
	{
	}

	virtual ~Object() = default;

	// 处理一条 EasyX 输入消息。
	virtual void load_object(const ExMessage& msg) = 0;

	// 按当前帧间隔绘制对象。
	virtual void draw_object(int delta) = 0;
};
