#include "Animation.h"

// EasyX 的 putimage 不直接处理 PNG 透明通道，这里用 AlphaBlend 保留透明效果。
void putimage_alpha(int x, int y, IMAGE* img)
{
	if (img == nullptr)
		return;

	int w = img->getwidth();
	int h = img->getheight();
	AlphaBlend(GetImageHDC(NULL), x, y, w, h, GetImageHDC(img), 0, 0, w, h, { AC_SRC_OVER,0,255,AC_SRC_ALPHA });
}

Animation::Animation(int interval, Atlas* atlas) : interval_ms(interval), anim_atlas(atlas)
{
}

Animation::~Animation()
{
}

IMAGE Animation::Getimg()
{
	if (anim_atlas == nullptr || anim_atlas->frame_list.empty())
		return IMAGE();

	return *(this->anim_atlas->frame_list[idx_frame]);
}

void Animation::Play(int x, int y, int delta)
{
	if (anim_atlas == nullptr || anim_atlas->frame_list.empty())
		return;

	timer += delta;

	if (timer >= interval_ms)
	{
		idx_frame = (idx_frame + 1) % anim_atlas->frame_list.size();
		timer = 0;
	}

	putimage_alpha(x, y, anim_atlas->frame_list[idx_frame]);
}
