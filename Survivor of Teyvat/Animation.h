#pragma once
#include<iostream>
#include<vector>
#include<easyx.h>


//图片边缘透明化
void putimage_alpha(int x, int y, IMAGE* img)
{
	int w = img->getwidth();
	int h = img->getheight();
	AlphaBlend(GetImageHDC(NULL), x, y, w, h, GetImageHDC(img), 0, 0, w, h, { AC_SRC_OVER,0,255,AC_SRC_ALPHA });
}

//图集
class Atlas
{
public:
	Atlas(LPCTSTR path, int num)
	{
		TCHAR path_file[256];
		for (size_t i = 0; i < num; i++)
		{
			_stprintf_s(path_file, path, i);

			IMAGE* frame = new IMAGE();

			loadimage(frame, path_file);

			frame_list.push_back(frame);

		}

	}
	//生成剪影
	Atlas(const Atlas* other,int color=0)
	{
		int r, g, b;
		if (color == 0) {
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

		for (int i = 0; i < 6; i++)
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


	~Atlas()
	{
		for (size_t i = 1; i < frame_list.size(); i++)
			delete frame_list[i];
	}

	std::vector<IMAGE*> frame_list;
};

class Animation 
{
private:
	int interval_ms = 0;//动画帧间隔
	int idx_frame = 0;//动画帧索引
	int timer = 0;//动画计时器
	Atlas* anim_atlas;

public:

	Animation(int interval,Atlas* atlas) :interval_ms(interval)
	{
		anim_atlas = atlas;
	}

	~Animation(){};
	

	IMAGE Getimg()
	{
		return *(this->anim_atlas->frame_list[idx_frame]);
	}

	void Play(int x, int y, int delta)
	{
		timer += delta;

		if (timer >= interval_ms)
		{
			idx_frame = (idx_frame + 1) % anim_atlas->frame_list.size();
			timer = 0;
		}
		putimage_alpha(x, y, anim_atlas->frame_list[idx_frame]);
	}

};