#pragma once
#include"Object.h"
#include"Animation.h"
#include"game.h"
#include"Widget.h"


class Bullet:public Object
{
public:

	Bullet(int n=0):Object(0, 0, 0, 0)
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
		}
	}
	~Bullet() = default;

	void load_object(const ExMessage & msg)
	{

	}

	void draw_object(int delta)
	{
		setlinecolor(RGB(r_, g_, b_));
		setfillcolor(RGB(r, g, b));
		fillcircle(x,y, RADIUS);
	} 


private:
	const int RADIUS = 10;
	int r;
	int g;
	int b;
	int r_;
	int g_;
	int b_;
};


class Player :public Object
{
public:
	const int SPEED = 3;//人物速度
	const int SHADOW_WIDTH = 32;//阴影宽度
	const int player_type;

	//得分
	int score = 0;
	
	//状态
	bool is_upgrade = false;
	bool is_frozen = false;
	bool is_hurt = false;
	bool is_dizziness = false;

	IMAGE* img_shadow;
	IMAGE* img_dizziness;
	Animation* anim_left;
	Animation* anim_right;
	Animation* anim_sketch_left;
	Animation* anim_sketch_right;
	Animation* anim_hurt_left;
	Animation* anim_hurt_right;


	Widget* m_widget;
	std::vector<Bullet> bullet_list;
	

	//移动标志
	bool is_move_up = false;
	bool is_move_down = false;
	bool is_move_left = false;
	bool is_move_right = false;

public:
	Player(Widget* widget,int type=0) :Object(game::Getwidth()/2, game::Getheight()/2, 80, 80), player_type(type)
	{
		if(player_type==0)
		{
			anim_left = new Animation(45, game::atlas_paimon_left);
			anim_right = new Animation(45, game::atlas_paimon_right);

			anim_sketch_left = new Animation(45, game::atlas_paimon_sketch_left);
			anim_sketch_right = new Animation(45, game::atlas_paimon_sketch_right);

			anim_hurt_left = new Animation(45, game::atlas_paimon_hurt_left);
			anim_hurt_right = new Animation(45, game::atlas_paimon_hurt_right);
		}
		else if (player_type == 1)
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
		loadimage(img_dizziness, _T("img/dizziness.png"),50,25);

		bullet_list.resize(2,Bullet(player_type));

		m_widget = widget;

	}

	~Player()
	{
		delete img_shadow;
		delete img_dizziness;
	}


	//处理消息
	void ProcessEvent(const ExMessage& msg)
	{
		if(!is_frozen&&!is_dizziness)
		{
			switch (msg.message)
			{
				//start
			case WM_KEYDOWN: {

				switch (msg.vkcode)
				{
				case VK_UP: {
					is_move_up = true;
					break;
				}
				case VK_DOWN: {
					is_move_down = true;
					break;
				}
				case VK_LEFT: {
					is_move_left = true;
					break;
				}
				case VK_RIGHT: {
					is_move_right = true;
					break;
				}
				}
				break;
			}//end
			case WM_KEYUP: {
				switch (msg.vkcode)
				{
				case VK_UP: {
					is_move_up = false;
					break;
				}
				case VK_DOWN: {
					is_move_down = false;
					break;
				}
				case VK_LEFT: {
					is_move_left = false;
					break;
				}
				case VK_RIGHT: {
					is_move_right = false;
					break;
				}
				}
				break;
			}
			default:
				is_move_down = false;
				is_move_up = false;
				is_move_left = false;
				is_move_right = false;
				break;
			}
		}
		else if (is_frozen||is_dizziness)
		{
			is_move_down = false;
			is_move_up = false;
			is_move_left = false;
			is_move_right = false;
		}
	}

	void Move()
	{
		int dir_x = is_move_right - is_move_left;

		int dir_y = is_move_down - is_move_up;

		double len_dir = sqrt(dir_x * dir_x + dir_y * dir_y);

		if (len_dir != 0)
		{
			double normalized_x = dir_x / len_dir;
			double normalized_y = dir_y / len_dir;
			this->x += (int)(SPEED * normalized_x);
			this->y += (int)(SPEED * normalized_y);
		}

		//边界
		if (x < 0)x = 0;
		if (y < 0)y =0;
		int wid = game::Getwidth();
		int hei = game::Getheight();
		if (x + m_width > wid)x = wid - m_width;
		if (y + m_height > hei)y = hei - m_height;


	}

	void load_object(const ExMessage& msg)
	{

		ProcessEvent(msg);
	}

	void draw_object(int delta)
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

	void draw_upgrade(bool facing_left, int delta)
	{
		static int counter = 0;

		if((++counter)<=100)
		{
			if (facing_left)
				anim_sketch_left->Play(x, y, delta);
			else
				anim_sketch_right->Play(x, y, delta);

			for (int i = 0; i < bullet_list.size(); i++)
				bullet_list[i].draw_object(delta);
		}
		else
		{
			counter = 0;
			is_upgrade = false;
		}
	}

	void draw_hurt(bool facing_left, int delta)
	{
		static int counter = 0;

		if ((++counter) <= 150)
		{
			if (facing_left)
				anim_hurt_left->Play(x, y, delta);
			else
				anim_hurt_right->Play(x, y, delta);

			for (int i = 0; i < bullet_list.size(); i++)
				bullet_list[i].draw_object(delta);
		}
		else
		{
			counter = 0;
			is_hurt = false;
		}
	}

	void draw_common(bool facing_left,int delta)
	{
		if (facing_left)
			anim_left->Play(x, y, delta);
		else
			anim_right->Play(x, y, delta);

		for (int i = 0; i < bullet_list.size(); i++)
			bullet_list[i].draw_object(delta);
	}

	const POINT& GetPosition()const
	{
		static POINT pos;
		pos.x = x;
		pos.y = y;
		return pos;
	}

	void UpdateBullets()
	{

		const double RADIAL_SPEED = 0.0045;//径向波动速度

		const double TANGENT_SPEED = 0.0055;//切向波动速度

		double radian_interval = 2 * 3.14159 / bullet_list.size();//子弹间隔

		POINT player_position = { x,y };

		double radius = 100 + 25 * sin(GetTickCount() * RADIAL_SPEED);

		for (size_t i = 0; i < bullet_list.size(); i++)
		{
			double radian = GetTickCount() * TANGENT_SPEED + radian_interval * i;//当前子弹弧度

			bullet_list[i].x = x + m_width / 2 + (int)(radius * sin(radian));

			bullet_list[i].y = y + m_height / 2 + (int)(radius * cos(radian));
		}


	}

	void TryGenerateEnemy();

	void Check();

	/*void Check_socre()
	{
		static int flag = 0;
		if(score%30==0&&score!=flag)
		{
			mciSendString(_T("play upgrade from 0"), NULL, 0, NULL);
			bullet_list.push_back(Bullet());
			is_upgrade = true;
			flag = score;
		}
	}*/

	void Check_score()
	{   
		int current_score = score;
		static int last_score = 0; // 记录上一次检查分数时的分数
		static int score_threshold = 20; // 初始分数阈值

		// 检查当前分数是否大于上一次检查的分数
		if (current_score > last_score)
		{
			// 计算分数增加的量
			int score_increase = current_score - last_score;

			// 根据当前子弹数量调整分数阈值
			score_threshold =bullet_list.size() * 10; // 例如，每增加一个子弹，分数阈值增加10

			// 检查分数增加是否达到了新的阈值
			if(score_increase >= score_threshold)
			{
				// 播放升级音效
				mciSendString(_T("play upgrade"), NULL, 0, NULL);

				// 增加一个子弹
				bullet_list.push_back(Bullet(player_type));
				is_upgrade = true;

				// 更新分数阈值和子弹数量
				score_increase -= score_threshold;
				// 更新上一次检查的分数
				last_score = current_score;
			}
		}
	}

	//绘制得分
	void DrawPlayerScore()
	{
		static TCHAR test[64];
		_stprintf_s(test, _T("当前玩家得分： %d"), score);

		setbkmode(TRANSPARENT);
		settextcolor(RGB(255, 58, 285));
		outtextxy(10, 10, test);
	}

	void draw_dizziness(bool facing_left,int delta)
	{
		static int counter = 0;
		if((++counter)!=75)
		{
			if (facing_left)
				anim_left->Play(x, y, delta);
			else
				anim_right->Play(x, y, delta);

			int d_x = x + m_width / 2 - 25;
			int d_y = y-10;

			putimage_alpha(d_x, d_y, img_dizziness);
		}
		else
		{
			is_dizziness = false;
			counter = 0;
		}
	}

	void draw_frozen(bool facing_left)
	{
		static  POINT position ;

		position.x = this->x;
		position.y = this->y;
		
		static int frozen_timer = 0;//冰冻状态计时器
		static const int THICKNESS = 5; // 扫描线宽度
		static int highlight_pos_y = 0; // 扫描线竖直坐标

		//更新冻结状态计时器并重置扫描线位置
		if (++frozen_timer % 150 == 0)
		{
			is_frozen = !is_frozen;
			highlight_pos_y = -THICKNESS;
		}
		 
		IMAGE img_current_frame;
		if (facing_left)
			img_current_frame=(this->anim_left->Getimg());
		else
			img_current_frame=(this->anim_right->Getimg());

		int width=img_current_frame.getwidth();
		int height = img_current_frame.getheight();

		static IMAGE* img_ice = new IMAGE();
		loadimage(img_ice, _T("img/img_ice.png"), width, height);
		
		//更新高亮扫描线竖直坐标
		highlight_pos_y = (highlight_pos_y + 2) % height;

		//遍历
		DWORD* color_buffer_ice_img = GetImageBuffer(img_ice);
		DWORD* color_buffer_frame_img = GetImageBuffer(&img_current_frame);

		for (int y = 0; y < height; y++)
		{
			for (int x = 0; x < width; x++)
			{
				int idx = y * width + x;

				static const float RATIO = 0.25f;//混叠比率
				static const float THRESHOLD = 0.84f;

				DWORD color_ice_img = color_buffer_ice_img[idx];
				DWORD color_frame_img = color_buffer_frame_img[idx];

				if ((color_frame_img & 0xFF000000) >> 24)
				{
					//注意！由于COLOREF中颜色为BGR顺序，所以要交换R B位置
					BYTE r = (BYTE)(GetBValue(color_frame_img) * RATIO + GetBValue(color_ice_img) * (1 - RATIO));
					BYTE g = (BYTE)(GetGValue(color_frame_img) * RATIO + GetGValue(color_ice_img) * (1 - RATIO));
					BYTE b = (BYTE)(GetRValue(color_frame_img) * RATIO + GetRValue(color_ice_img) * (1 - RATIO));
				
					if ((y >= highlight_pos_y) && y <= highlight_pos_y + THICKNESS && (((r / 255.0f) * 0.2126f + (g / 255.0f) * 0.7152f + (b / 255.0f) * 0.0722f) >= THRESHOLD))
					{
						std::cout << 0;
						color_buffer_frame_img[idx] = BGR(RGB(255, 255, 255)) | (((DWORD)(BYTE)(255)) << 24);
						continue;
					}
					color_buffer_frame_img[idx] = BGR(RGB(r, g, b)) | (((DWORD)(BYTE)(255)) << 24);
				}

			}
		}
		putimage_alpha(position.x, position.y, &img_current_frame);
	}

};