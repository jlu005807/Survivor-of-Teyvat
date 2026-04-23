#pragma once
#include <cmath>
#include <functional>
#include <map>
#include <vector>
#include "Object.h"

// 绘制指定尺寸的透明按钮图片。
inline void putimage_alpha_button(int x, int y, int width, int height, IMAGE* img)
{
	if (img == nullptr)
		return;

	AlphaBlend(GetImageHDC(NULL), x, y, width, height, GetImageHDC(img), 0, 0, width, height, { AC_SRC_OVER,0,255,AC_SRC_ALPHA });
}

// 图片按钮：根据鼠标悬停/按下状态切换贴图，并在释放时触发回调。
class Button : public Object
{
private:
	float scale = 1.0f; // 按钮缩放比例。
	std::function<void()> onClick; // 点击回调。

	enum class Status
	{
		Idle = 0,
		Hovered,
		Pushed
	};

	std::map<Status, IMAGE*> img;
	Status status = Status::Idle; // 当前按钮状态。

public:
	Button(int x, int y, int width, int height, const std::function<void()>& onClick, std::vector<IMAGE*> contents)
		: Object(x, y, width, height), onClick(onClick)
	{
		for (size_t i = 0; i < contents.size(); i++)
		{
			Status key = static_cast<Status>(i);
			img[key] = contents[i];
		}
	}

	void Draw_button()
	{
		int scaledWidth = static_cast<int>(std::lround(m_width * scale));
		int scaledHeight = static_cast<int>(std::lround(m_height * scale));
		int scaledX = x + (m_width - scaledWidth) / 2;
		int scaledY = y + (m_height - scaledHeight) / 2;

		putimage_alpha_button(scaledX, scaledY, scaledWidth, scaledHeight, img[status]);
	}

	// 检测鼠标是否落在按钮范围内。
	bool CheckCursorHit(int mouse_x, int mouse_y)
	{
		return mouse_x >= x && mouse_x <= x + m_width && mouse_y >= y && mouse_y <= y + m_height;
	}

	void ProcessEvent(const ExMessage& msg)
	{
		switch (msg.message)
		{
		case WM_MOUSEMOVE:
		{
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
		case WM_LBUTTONDOWN:
		{
			if (CheckCursorHit(msg.x, msg.y))
			{
				mciSendString(_T("stop button"), NULL, 0, NULL);
				status = Status::Pushed;
				scale = 0.9f;
			}
			break;
		}
		case WM_LBUTTONUP:
		{
			if (status == Status::Pushed)
			{
				const bool hit = CheckCursorHit(msg.x, msg.y);
				status = hit ? Status::Hovered : Status::Idle;
				scale = hit ? 1.1f : 1.0f;

				if (hit)
				{
					mciSendString(_T("play button from 0"), NULL, 0, NULL);
					onClick();
				}
			}
			break;
		}
		default:
			break;
		}
	}

	void load_object(const ExMessage& msg) override
	{
		ProcessEvent(msg);
	}

	void draw_object(int delta) override
	{
		(VOID)delta;
		Draw_button();
	}

	~Button()
	{
		for (auto& entry : img)
			delete entry.second;

		img.clear();
	}
};
