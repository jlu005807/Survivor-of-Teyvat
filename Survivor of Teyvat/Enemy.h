#pragma once
#include <cstdlib>
#include<ctime>
#include"Player.h"

class Enemy :public Object
{
public:

	Enemy(Player* player,int enemy_type=0):Object(0,0,80,80)
	{
		if(enemy_type==0)
		{
			anim_left = new Animation(45, game::atlas_boar_left);
			anim_right = new Animation(45, game::atlas_boar_right);
			SPEED = 3;
		}
		else if (enemy_type == 1)
		{
			anim_left = new Animation(45, game::atlas_bee_left);
			anim_right = new Animation(45, game::atlas_bee_right);
			SPEED = 4;
		}
		else if(enemy_type==2)
		{
			anim_left = new Animation(45, game::atlas_snail_left);
			anim_right = new Animation(45, game::atlas_snail_right);
			SPEED = 2;
		}
		img_shadow = new IMAGE();
		loadimage(img_shadow, _T("img/shadow_enemy.png"));
		Set_locate(); 
		this->player = player;
		this->enemy_type = enemy_type;
	}

	void Set_locate()
	{

		//敌人生成边界
		enum class SpawnEdge
		{
			Up = 0,
			Down,
			Left,
			Right

		};

		std::srand(static_cast<unsigned int>(std::time(nullptr)));
		std::cout << rand() % 4;
		//随机生成于边界
		SpawnEdge edge = (SpawnEdge)(rand() % 4);


		switch (edge)
		{
		case SpawnEdge::Up:
			x = rand() % game::Getwidth();
			y = -m_height;
			break;
		case SpawnEdge::Down:
			x = rand() % game::Getwidth();
			y = m_height;
			break;
		case SpawnEdge::Left:
			x = -m_width;
			y = rand() % game::Getheight();
			break;
		case SpawnEdge::Right:
			x = m_width;
			y = rand() % game::Getheight();
			break;
		default:
			break;
		}

	}

	bool CheckCollison(const Object* obj)
	{
		POINT check_position = { obj->x + obj->m_width / 2,obj->y + obj->m_height/2 };

		if (x < check_position.x && check_position.x < x + m_width && y < check_position.y && check_position.y < y + m_height)
			return true;

		return false;
	}

	void load_object(const ExMessage& msg)
	{

	}

	void Check()
	{
		if (CheckCollison(player))
		{
			if(player->is_hurt==false&&player->is_upgrade==false)
			{
				if(enemy_type==0)
				{
					player->bullet_list.pop_back();
					player->is_hurt = true;
					player->is_frozen = false;
					player->is_dizziness = false;
				}
				else if (enemy_type == 1)
				{
					player->bullet_list.pop_back();
					if(player->is_frozen==false)
					   player->is_dizziness = true;
				}
				else if (enemy_type == 2)
				{
					player->is_frozen = true;
					player->score+= 9;

				}
				mciSendString(_T("play hit from 0"), NULL, 0, NULL);
				this->Hurt();

			}
			else if (player->is_hurt)
			{
				mciSendString(_T("play hit from 0"), NULL, 0, NULL);
				this->Hurt();
			}
			if(player->bullet_list.size()==0)
			{
				static TCHAR test[64];
				_stprintf_s(test, _T("本局玩家得分： %d"), player->score);
				MessageBox(GetHWnd(), test, _T("游戏结束"), MB_OK);
				game::Change_running(false);
			}

		}
		else {
			for (int i = 0; i < (player->bullet_list.size()); i++)
			{
				if (CheckCollison(&player->bullet_list[i])&&player->is_frozen==false)
				{
					if (enemy_type == 0)player->score++;
					else if (enemy_type == 1)player->score += 5;
					if (enemy_type != 2)
					{
						Hurt();
						mciSendString(_T("play hit from 0"), NULL, 0, NULL);
					}
				}
			}
		}
	}

	void Move()
	{
		int dir_x = player->x - x;
		int dir_y = player->y - y;

		double len_dir = sqrt(dir_x * dir_x + dir_y * dir_y);

		if (len_dir != 0)
		{
			double normalized_x = dir_x / len_dir;
			double normalized_y = dir_y / len_dir;
			this->x += (int)(SPEED * normalized_x);
			this->y += (int)(SPEED * normalized_y);
		}

		if (dir_x < 0)
			facing_left = true;
		else if (dir_x > 0)
			facing_left = false;

	}

	void draw_object(int delta)
	{
		Move();

		Check();


		if(alive)
		{
			int pos_shadow_x = x + (m_width / 2 - SHADOW_WIDTH / 2)+10;
			int pos_shadow_y = y + m_height - (15-enemy_type*15);
			if (enemy_type == 2)pos_shadow_y -= 30;
			putimage_alpha(pos_shadow_x, pos_shadow_y, img_shadow);


			if (facing_left)
				anim_left->Play(x, y, delta);
			else
				anim_right->Play(x, y, delta);
		}

	}

	const POINT& GetPosition()const
	{
		static POINT pos;
		pos.x = x;
		pos.y = y;
		return pos;
	}


	void Hurt()
	{
		alive = false;
	}


	bool GetAlive()
	{
		return alive;
	}


	~Enemy()
	{
		delete img_shadow;
	}
private:

	int SPEED;
	const int SHADOW_WIDTH = 48;


	IMAGE* img_shadow;
	Animation* anim_left;
	Animation* anim_right;
	Player* player;
	bool facing_left = false;
	bool alive = true;
	//类型
	int enemy_type;
};