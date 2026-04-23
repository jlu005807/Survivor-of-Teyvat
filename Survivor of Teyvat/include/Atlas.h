#pragma once
#include <vector>
#include <easyx.h>

// 图集类：负责批量加载和保存一组 IMAGE 帧，供 Animation 按帧播放。
class Atlas
{
public:
	// 按路径模板加载图片，例如 "img/paimon_left_%d.png"。
	Atlas(LPCTSTR path, int num);

	// 从已有图集生成描边/受击色图集，color 为 0 时生成白色剪影，否则生成红色剪影。
	Atlas(const Atlas* other, int color = 0);
	~Atlas();

	// 动画帧列表，Atlas 负责释放其中的 IMAGE 指针。
	std::vector<IMAGE*> frame_list;
};
