#include "Atlas.h"

// 按照路径模板依次加载一组图片帧。
Atlas::Atlas(LPCTSTR path, int num)
{
	TCHAR path_file[256];
	for (int i = 0; i < num; i++)
	{
		_stprintf_s(path_file, path, i);
		IMAGE* frame = new IMAGE();
		loadimage(frame, path_file);
		frame_list.push_back(frame);
	}
}

// 基于已有图集生成闪白/受击等效果图集，保留原图的透明通道。
Atlas::Atlas(const Atlas* other, int color)
{
	if (other == nullptr)
		return;

	int r, g, b;
	if (color == 0)
	{
		r = 255;
		g = 255;
		b = 255;
	}
	else
	{
		r = 139;
		g = 0;
		b = 0;
	}

	for (size_t i = 0; i < other->frame_list.size(); i++)
	{
		IMAGE* one;
		if (i % 2 == 0)
		{
			int width = other->frame_list[i]->getwidth();
			int height = other->frame_list[i]->getheight();
			one = new IMAGE(width, height);

			DWORD* color_buffer_raw_img = GetImageBuffer(other->frame_list[i]);
			DWORD* color_buffer_sketch_img = GetImageBuffer(one);

			for (int y = 0; y < height; y++)
			{
				for (int x = 0; x < width; x++)
				{
					int idx = y * width + x;
					if ((color_buffer_raw_img[idx] & 0xFF000000) >> 24)
						color_buffer_sketch_img[idx] = BGR(RGB(r, g, b)) | (((DWORD)(BYTE)(255)) << 24);
				}
			}
		}
		else
		{
			one = new IMAGE(*other->frame_list[i]);
		}

		this->frame_list.push_back(one);
	}
}

Atlas::~Atlas()
{
	for (IMAGE* frame : frame_list)
		delete frame;

	frame_list.clear();
}
