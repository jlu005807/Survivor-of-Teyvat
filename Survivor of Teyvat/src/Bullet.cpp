#include "Bullet.h"

// 不同角色使用不同配色，方便玩家区分当前角色的攻击效果。
Bullet::Bullet(int n) : Object(0, 0, 0, 0)
{
	if (n == 0)
	{
		r = 200;
		g = 75;
		b = 10;
		r_ = 255;
		g_ = 255;
		b_ = 50;
	}
	else
	{
		r = 80;
		g = 0;
		b = 120;
		r_ = 255;
		g_ = 255;
		b_ = 255;
	}
}

void Bullet::load_object(const ExMessage& msg)
{
	(VOID)msg;
}

void Bullet::draw_object(int delta)
{
	(VOID)delta;
	// 子弹本身不处理透明贴图，直接绘制实心圆。
	setlinecolor(RGB(r_, g_, b_));
	setfillcolor(RGB(r, g, b));
	fillcircle(x, y, RADIUS);
}
