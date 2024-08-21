#pragma once
#include"Object.h"
#include <functional>
#include <string>
#include<map>
#include<climits>


//图片边缘透明化
void putimage_alpha_button(int x, int y, int width,int height,IMAGE* img)
{
	AlphaBlend(GetImageHDC(NULL), x, y, width, height, GetImageHDC(img), 0, 0, width, height, { AC_SRC_OVER,0,255,AC_SRC_ALPHA });
}

class Button : public Object
{
private:

	float scale=1.0f;//缩放比例
	std::function<void()> onClick;//点击按钮触发函数

	enum class Status
	{
		Idle=0,
		Hovered,
		Pushed
	};

    std::map<Status,IMAGE*> img;
	//状态
	Status status = Status::Idle;

public:
	Button(int x,int y,int width,int height,const std::function<void()>& onClick,std::vector<IMAGE*>contents):Object(x,y,width,height),onClick(onClick)
	{
		for (int i = 0; i < contents.size(); i++)
		{
			Status k = static_cast<Status>(i);
			img[k] = contents[i];
		}
		this->status = Status::Idle;

	}

	void Draw_button()
	{
		int scaledWidth = m_width * scale; // 缩放后的按钮宽度
		int scaledHeight = m_height * scale; // 缩放后的按钮高度
		int scaledX = x + (m_width - scaledWidth) / 2; // 缩放后的按钮x坐标
		int scaledY = y + (m_height - scaledHeight) / 2; // 缩放后的按钮y坐标
	
		putimage_alpha_button(scaledX,scaledY,scaledWidth,scaledHeight , img[status]);

	}

	// 检查鼠标是否在按钮上方
	bool CheckCursorHit(int mouse_x, int mouse_y)
	{
		return mouse_x >= x && mouse_x <= x + m_width && mouse_y >= y && mouse_y <= y + m_height;
	}

	void ProcessEvent(const ExMessage& msg)
	{
		switch (msg.message)
		{
		case WM_MOUSEMOVE: {
			if (status == Status::Idle && CheckCursorHit(msg.x, msg.y))
			{
				mciSendString(_T("play hurt from 0"), NULL, 0, NULL);
				status = Status::Hovered;
				scale = 1.1f;
			}
			else if (status == Status::Hovered && !CheckCursorHit(msg.x, msg.y))
			{
				status = Status::Idle;
				scale = 1.0f;
			}
			break;
		}
		case WM_LBUTTONDOWN:{
			if (CheckCursorHit(msg.x, msg.y))
			{
				mciSendString(_T("stop button"), NULL, 0, NULL);
				status = Status::Pushed;
				scale = 0.9f;
			}
			break;
		}
		case WM_LBUTTONUP: {
			if (status == Status::Pushed)
			{
				mciSendString(_T("play button from 0"), NULL, 0, NULL);
				onClick();
			}
			break;
		}
		default:
			status = Status::Idle;
			break;
		}
	}

	virtual void load_object(const ExMessage& msg)
	{
		ProcessEvent(msg);
	}

	virtual void draw_object(int delta)
	{
		Draw_button();
	}
	
	~Button()
	{
		for (int i = 0; i < img.size(); i++)
		{
			Status k = static_cast<Status>(i);
			delete img[k];
		}

	}
};