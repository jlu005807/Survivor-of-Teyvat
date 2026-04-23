#pragma once
#include <cmath>
#include <cstddef>
#include "Object.h"
#include "Animation.h"
#include "game.h"
#include "Widget.h"
#include "Bullet.h"

// 玩家对象：处理输入、移动、环绕子弹、受击/升级/冻结/眩晕状态和得分显示。
class Player : public Object
{
public:
	static Player* CreateInstance(Widget* widget, int type = 0);
	static Player* GetInstance();
	static void DestroyInstance();

	Player(const Player&) = delete;
	Player& operator=(const Player&) = delete;

	const int SPEED = 3; // 玩家移动速度。
	const int SHADOW_WIDTH = 32; // 阴影宽度。
	const int player_type; // 0 为派蒙，1 为战士。

	int score = 0; // 当前得分。

	// 状态标记，由敌人效果或升级逻辑触发。
	bool is_upgrade = false;
	bool is_frozen = false;
	bool is_hurt = false;
	bool is_dizziness = false;

	IMAGE* img_shadow = nullptr;
	IMAGE* img_dizziness = nullptr;
	Animation* anim_left = nullptr;
	Animation* anim_right = nullptr;
	Animation* anim_sketch_left = nullptr;
	Animation* anim_sketch_right = nullptr;
	Animation* anim_hurt_left = nullptr;
	Animation* anim_hurt_right = nullptr;

	Widget* m_widget = nullptr;
	std::vector<Bullet> bullet_list;

	// 方向键状态，允许按住键持续移动。
	bool is_move_up = false;
	bool is_move_down = false;
	bool is_move_left = false;
	bool is_move_right = false;

private:
	static Player* instance;

	int last_upgrade_score = 0; // 上一次触发升级时的分数。
	int enemy_spawn_counter = 0; // 敌人生成计时器，随玩家实例重置。
	int upgrade_counter = 0;
	int hurt_counter = 0;
	int dizziness_counter = 0;
	int frozen_timer = 0;
	int frozen_highlight_pos_y = 0;

	Player(Widget* widget, int type = 0) : Object(game::Getwidth() / 2, game::Getheight() / 2, 80, 80), player_type(type)
	{
		if (player_type == 0)
		{
			anim_left = new Animation(45, game::atlas_paimon_left);
			anim_right = new Animation(45, game::atlas_paimon_right);
			anim_sketch_left = new Animation(45, game::atlas_paimon_sketch_left);
			anim_sketch_right = new Animation(45, game::atlas_paimon_sketch_right);
			anim_hurt_left = new Animation(45, game::atlas_paimon_hurt_left);
			anim_hurt_right = new Animation(45, game::atlas_paimon_hurt_right);
		}
		else
		{
			anim_left = new Animation(45, game::atlas_warrior_left);
			anim_right = new Animation(45, game::atlas_warrior_right);
			anim_sketch_left = new Animation(45, game::atlas_warrior_sketch_left);
			anim_sketch_right = new Animation(45, game::atlas_warrior_sketch_right);
			anim_hurt_left = new Animation(45, game::atlas_warrior_hurt_left);
			anim_hurt_right = new Animation(45, game::atlas_warrior_hurt_right);
		}

		img_shadow = new IMAGE();
		loadimage(img_shadow, _T("img/shadow_player.png"));

		img_dizziness = new IMAGE();
		loadimage(img_dizziness, _T("img/dizziness.png"), 50, 25);

		bullet_list.resize(2, Bullet(player_type));
		m_widget = widget;
	}

public:
	~Player()
	{
		if (instance == this)
			instance = nullptr;

		delete anim_left;
		delete anim_right;
		delete anim_sketch_left;
		delete anim_sketch_right;
		delete anim_hurt_left;
		delete anim_hurt_right;
		delete img_shadow;
		delete img_dizziness;
	}

	// 处理方向键输入；非键盘消息不会清空移动状态，避免鼠标移动打断角色移动。
	void ProcessEvent(const ExMessage& msg)
	{
		if (!is_frozen && !is_dizziness)
		{
			switch (msg.message)
			{
			case WM_KEYDOWN:
			{
				switch (msg.vkcode)
				{
				case VK_UP:
					is_move_up = true;
					break;
				case VK_DOWN:
					is_move_down = true;
					break;
				case VK_LEFT:
					is_move_left = true;
					break;
				case VK_RIGHT:
					is_move_right = true;
					break;
				default:
					break;
				}
				break;
			}
			case WM_KEYUP:
			{
				switch (msg.vkcode)
				{
				case VK_UP:
					is_move_up = false;
					break;
				case VK_DOWN:
					is_move_down = false;
					break;
				case VK_LEFT:
					is_move_left = false;
					break;
				case VK_RIGHT:
					is_move_right = false;
					break;
				default:
					break;
				}
				break;
			}
			default:
				break;
			}
		}
		else
		{
			is_move_down = false;
			is_move_up = false;
			is_move_left = false;
			is_move_right = false;
		}
	}

	// 根据方向键状态移动玩家，并限制在窗口边界内。
	void Move()
	{
		int dir_x = is_move_right - is_move_left;
		int dir_y = is_move_down - is_move_up;

		double len_dir = std::sqrt(dir_x * dir_x + dir_y * dir_y);
		if (len_dir != 0)
		{
			double normalized_x = dir_x / len_dir;
			double normalized_y = dir_y / len_dir;
			this->x += static_cast<int>(SPEED * normalized_x);
			this->y += static_cast<int>(SPEED * normalized_y);
		}

		if (x < 0) x = 0;
		if (y < 0) y = 0;

		int wid = game::Getwidth();
		int hei = game::Getheight();
		if (x + m_width > wid) x = wid - m_width;
		if (y + m_height > hei) y = hei - m_height;
	}

	void load_object(const ExMessage& msg) override
	{
		ProcessEvent(msg);
	}

	void draw_object(int delta) override
	{
		TryGenerateEnemy();
		Move();
		UpdateBullets();
		Check();
		Check_score();

		int pos_shadow_x = x + (m_width / 2 - SHADOW_WIDTH / 2);
		int pos_shadow_y = y + m_height - 8;
		putimage_alpha(pos_shadow_x, pos_shadow_y, img_shadow);

		static bool facing_left = true;
		int dir_x = is_move_right - is_move_left;

		if (dir_x < 0)
			facing_left = true;
		else if (dir_x > 0)
			facing_left = false;

		if (is_upgrade)
			draw_upgrade(facing_left, delta);
		else if (is_hurt)
			draw_hurt(facing_left, delta);
		else if (is_frozen)
			draw_frozen(facing_left);
		else if (is_dizziness)
			draw_dizziness(facing_left, delta);
		else
			draw_common(facing_left, delta);

		DrawPlayerScore();
	}

	// 升级状态：短暂显示闪白动画，结束后恢复普通状态。
	void draw_upgrade(bool facing_left, int delta)
	{
		if ((++upgrade_counter) <= 100)
		{
			if (facing_left)
				anim_sketch_left->Play(x, y, delta);
			else
				anim_sketch_right->Play(x, y, delta);

			for (std::size_t i = 0; i < bullet_list.size(); i++)
				bullet_list[i].draw_object(delta);
		}
		else
		{
			upgrade_counter = 0;
			is_upgrade = false;
		}
	}

	// 受击状态：短暂显示受击色动画。
	void draw_hurt(bool facing_left, int delta)
	{
		if ((++hurt_counter) <= 150)
		{
			if (facing_left)
				anim_hurt_left->Play(x, y, delta);
			else
				anim_hurt_right->Play(x, y, delta);

			for (std::size_t i = 0; i < bullet_list.size(); i++)
				bullet_list[i].draw_object(delta);
		}
		else
		{
			hurt_counter = 0;
			is_hurt = false;
		}
	}

	// 普通状态：播放移动动画并绘制环绕子弹。
	void draw_common(bool facing_left, int delta)
	{
		if (facing_left)
			anim_left->Play(x, y, delta);
		else
			anim_right->Play(x, y, delta);

		for (std::size_t i = 0; i < bullet_list.size(); i++)
			bullet_list[i].draw_object(delta);
	}

	const POINT& GetPosition() const
	{
		static POINT pos;
		pos.x = x;
		pos.y = y;
		return pos;
	}

	// 让所有子弹以正弦半径围绕玩家旋转。
	void UpdateBullets()
	{
		if (bullet_list.empty())
			return;

		constexpr double RADIAL_SPEED = 0.0045; // 半径波动速度。
		constexpr double TANGENT_SPEED = 0.0055; // 切向旋转速度。
		constexpr double PI = 3.14159265358979323846;

		double radian_interval = 2 * PI / bullet_list.size();
		double radius = 100 + 25 * std::sin(GetTickCount() * RADIAL_SPEED);

		for (std::size_t i = 0; i < bullet_list.size(); i++)
		{
			double radian = GetTickCount() * TANGENT_SPEED + radian_interval * i;
			bullet_list[i].x = x + m_width / 2 + static_cast<int>(radius * std::sin(radian));
			bullet_list[i].y = y + m_height / 2 + static_cast<int>(radius * std::cos(radian));
		}
	}

	void TryGenerateEnemy();

	void Check();

	// 根据得分增长升级子弹数量；阈值随当前子弹数提升。
	void Check_score()
	{
		int current_score = score;
		int score_threshold = static_cast<int>(bullet_list.size()) * 10;
		if (score_threshold <= 0)
			return;

		if (current_score - last_upgrade_score >= score_threshold)
		{
			mciSendString(_T("play upgrade from 0"), NULL, 0, NULL);

			bullet_list.push_back(Bullet(player_type));
			is_upgrade = true;
			last_upgrade_score = current_score;
		}
	}

	// 绘制玩家得分。
	void DrawPlayerScore()
	{
		TCHAR text[64];
		_stprintf_s(text, _T("当前玩家得分: %d"), score);

		setbkmode(TRANSPARENT);
		settextcolor(RGB(255, 58, 255));
		outtextxy(10, 10, text);
	}

	// 眩晕状态：玩家暂停移动，并在头顶绘制眩晕图标。
	void draw_dizziness(bool facing_left, int delta)
	{
		if ((++dizziness_counter) <= 75)
		{
			if (facing_left)
				anim_left->Play(x, y, delta);
			else
				anim_right->Play(x, y, delta);

			int d_x = x + m_width / 2 - 25;
			int d_y = y - 10;

			putimage_alpha(d_x, d_y, img_dizziness);
		}
		else
		{
			is_dizziness = false;
			dizziness_counter = 0;
		}
	}

	// 冻结状态：混合冰层贴图，并绘制一条扫光线。
	void draw_frozen(bool facing_left)
	{
		POINT position = { this->x, this->y };
		constexpr int THICKNESS = 5;

		if (++frozen_timer % 150 == 0)
		{
			is_frozen = false;
			frozen_highlight_pos_y = -THICKNESS;
		}

		IMAGE img_current_frame;
		if (facing_left)
			img_current_frame = this->anim_left->Getimg();
		else
			img_current_frame = this->anim_right->Getimg();

		int width = img_current_frame.getwidth();
		int height = img_current_frame.getheight();
		if (width <= 0 || height <= 0)
			return;

		static IMAGE* img_ice = new IMAGE();
		static int cached_width = 0;
		static int cached_height = 0;
		if (cached_width != width || cached_height != height)
		{
			loadimage(img_ice, _T("img/img_ice.png"), width, height);
			cached_width = width;
			cached_height = height;
		}

		frozen_highlight_pos_y = (frozen_highlight_pos_y + 2) % height;

		DWORD* color_buffer_ice_img = GetImageBuffer(img_ice);
		DWORD* color_buffer_frame_img = GetImageBuffer(&img_current_frame);

		for (int y = 0; y < height; y++)
		{
			for (int x = 0; x < width; x++)
			{
				int idx = y * width + x;

				constexpr float RATIO = 0.25f; // 原图与冰层混合比例。
				constexpr float THRESHOLD = 0.84f;

				DWORD color_ice_img = color_buffer_ice_img[idx];
				DWORD color_frame_img = color_buffer_frame_img[idx];

				if ((color_frame_img & 0xFF000000) >> 24)
				{
					// 注意：COLORREF 在缓冲区中是 BGR 顺序。
					BYTE r = static_cast<BYTE>(GetBValue(color_frame_img) * RATIO + GetBValue(color_ice_img) * (1 - RATIO));
					BYTE g = static_cast<BYTE>(GetGValue(color_frame_img) * RATIO + GetGValue(color_ice_img) * (1 - RATIO));
					BYTE b = static_cast<BYTE>(GetRValue(color_frame_img) * RATIO + GetRValue(color_ice_img) * (1 - RATIO));

					if ((y >= frozen_highlight_pos_y) && y <= frozen_highlight_pos_y + THICKNESS && (((r / 255.0f) * 0.2126f + (g / 255.0f) * 0.7152f + (b / 255.0f) * 0.0722f) >= THRESHOLD))
					{
						color_buffer_frame_img[idx] = BGR(RGB(255, 255, 255)) | (static_cast<DWORD>(static_cast<BYTE>(255)) << 24);
						continue;
					}

					color_buffer_frame_img[idx] = BGR(RGB(r, g, b)) | (static_cast<DWORD>(static_cast<BYTE>(255)) << 24);
				}
			}
		}

		putimage_alpha(position.x, position.y, &img_current_frame);
	}
};
