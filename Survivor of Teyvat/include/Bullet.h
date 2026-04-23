#pragma once
#include "Object.h"

// 围绕玩家旋转的子弹，颜色由玩家类型决定。
class Bullet : public Object
{
public:
	Bullet(int n = 0);
	~Bullet() = default;

	void load_object(const ExMessage& msg) override;
	void draw_object(int delta) override;

private:
	const int RADIUS = 10; // 子弹半径。
	int r; // 填充色红色分量。
	int g; // 填充色绿色分量。
	int b; // 填充色蓝色分量。
	int r_; // 描边色红色分量。
	int g_; // 描边色绿色分量。
	int b_; // 描边色蓝色分量。
};
