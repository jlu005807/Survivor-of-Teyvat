#pragma once
#include <cstddef>
#include <easyx.h>
#include "Atlas.h"

// 绘制带透明通道的图片，统一处理 EasyX 的 AlphaBlend 调用。
void putimage_alpha(int x, int y, IMAGE* img);

// 动画播放器：根据时间间隔从图集中取帧并绘制到窗口。
class Animation
{
private:
	int interval_ms = 0; // 动画帧间隔，单位毫秒。
	std::size_t idx_frame = 0; // 当前帧索引。
	int timer = 0; // 帧计时器。
	Atlas* anim_atlas = nullptr;

public:
	Animation(int interval, Atlas* atlas);
	~Animation();

	// 返回当前帧的副本，用于冻结等需要像素处理的特效。
	IMAGE Getimg();

	// 推进动画计时并绘制当前帧。
	void Play(int x, int y, int delta);
};
