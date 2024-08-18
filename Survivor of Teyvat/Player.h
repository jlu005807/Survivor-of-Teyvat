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
	const int SPEED = 3;//�����ٶ�
	const int SHADOW_WIDTH = 32;//��Ӱ���
	const int player_type;

	//�÷�
	int score = 0;
	
	//״̬
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
	

	//�ƶ���־
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


	//������Ϣ
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

		//�߽�
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

		const double RADIAL_SPEED = 0.0045;//���򲨶��ٶ�

		const double TANGENT_SPEED = 0.0055;//���򲨶��ٶ�

		double radian_interval = 2 * 3.14159 / bullet_list.size();//�ӵ����

		POINT player_position = { x,y };

		double radius = 100 + 25 * sin(GetTickCount() * RADIAL_SPEED);

		for (size_t i = 0; i < bullet_list.size(); i++)
		{
			double radian = GetTickCount() * TANGENT_SPEED + radian_interval * i;//��ǰ�ӵ�����

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
		static int last_score = 0; // ��¼��һ�μ�����ʱ�ķ���
		static int score_threshold = 20; // ��ʼ������ֵ

		// ��鵱ǰ�����Ƿ������һ�μ��ķ���
		if (current_score > last_score)
		{
			// ����������ӵ���
			int score_increase = current_score - last_score;

			// ���ݵ�ǰ�ӵ���������������ֵ
			score_threshold =bullet_list.size() * 10; // ���磬ÿ����һ���ӵ���������ֵ����10

			// �����������Ƿ�ﵽ���µ���ֵ
			if(score_increase >= score_threshold)
			{
				// ����������Ч
				mciSendString(_T("play upgrade"), NULL, 0, NULL);

				// ����һ���ӵ�
				bullet_list.push_back(Bullet(player_type));
				is_upgrade = true;

				// ���·�����ֵ���ӵ�����
				score_increase -= score_threshold;
				// ������һ�μ��ķ���
				last_score = current_score;
			}
		}
	}

	//���Ƶ÷�
	void DrawPlayerScore()
	{
		static TCHAR test[64];
		_stprintf_s(test, _T("��ǰ��ҵ÷֣� %d"), score);

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
		
		static int frozen_timer = 0;//����״̬��ʱ��
		static const int THICKNESS = 5; // ɨ���߿��
		static int highlight_pos_y = 0; // ɨ������ֱ����

		//���¶���״̬��ʱ��������ɨ����λ��
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
		
		//���¸���ɨ������ֱ����
		highlight_pos_y = (highlight_pos_y + 2) % height;

		//����
		DWORD* color_buffer_ice_img = GetImageBuffer(img_ice);
		DWORD* color_buffer_frame_img = GetImageBuffer(&img_current_frame);

		for (int y = 0; y < height; y++)
		{
			for (int x = 0; x < width; x++)
			{
				int idx = y * width + x;

				static const float RATIO = 0.25f;//�������
				static const float THRESHOLD = 0.84f;

				DWORD color_ice_img = color_buffer_ice_img[idx];
				DWORD color_frame_img = color_buffer_frame_img[idx];

				if ((color_frame_img & 0xFF000000) >> 24)
				{
					//ע�⣡����COLOREF����ɫΪBGR˳������Ҫ����R Bλ��
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