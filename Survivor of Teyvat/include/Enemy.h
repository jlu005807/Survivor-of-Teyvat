#pragma once
#include <cmath>
#include <cstdlib>
#include "EnemyEffect.h"

// 敌人对象：负责出生位置、追踪玩家、碰撞检测和动画绘制。
class Enemy : public Object
{
public:
	Enemy(Player* player = nullptr, const EnemyEffect& effect = ResolveEnemyEffect(EnemyType::Boar)) : Object(0, 0, 80, 80)
	{
		anim_boar_left = new Animation(45, game::atlas_boar_left);
		anim_boar_right = new Animation(45, game::atlas_boar_right);
		anim_bee_left = new Animation(45, game::atlas_bee_left);
		anim_bee_right = new Animation(45, game::atlas_bee_right);
		anim_snail_left = new Animation(45, game::atlas_snail_left);
		anim_snail_right = new Animation(45, game::atlas_snail_right);

		img_shadow = new IMAGE();
		loadimage(img_shadow, _T("img/shadow_enemy.png"));

		Bind(player, effect);
	}

	// 对象池复用敌人时，重新绑定目标玩家和敌人效果。
	void Bind(Player* player, const EnemyEffect& effect)
	{
		this->player = player;
		this->effect = &effect;
		alive = true;
		facing_left = false;

		ConfigureVisual();
		SetSpawnPosition();
	}

	// 在窗口四条边外随机生成，随后向玩家移动。
	void SetSpawnPosition()
	{
		enum class SpawnEdge
		{
			Up = 0,
			Down,
			Left,
			Right
		};

		SpawnEdge edge = static_cast<SpawnEdge>(std::rand() % 4);

		switch (edge)
		{
		case SpawnEdge::Up:
			x = std::rand() % game::Getwidth();
			y = -m_height;
			break;
		case SpawnEdge::Down:
			x = std::rand() % game::Getwidth();
			y = game::Getheight();
			break;
		case SpawnEdge::Left:
			x = -m_width;
			y = std::rand() % game::Getheight();
			break;
		case SpawnEdge::Right:
			x = game::Getwidth();
			y = std::rand() % game::Getheight();
			break;
		default:
			break;
		}
	}

	// 使用目标对象中心点判断是否进入敌人矩形范围。
	bool CheckCollision(const Object* obj) const
	{
		POINT check_position = { obj->x + obj->m_width / 2, obj->y + obj->m_height / 2 };
		return x < check_position.x && check_position.x < x + m_width && y < check_position.y && check_position.y < y + m_height;
	}

	void load_object(const ExMessage& msg) override
	{
		(VOID)msg;
	}

	// 处理敌人与玩家/子弹的碰撞逻辑。
	void Check()
	{
		if (!alive || player == nullptr || effect == nullptr)
			return;

		if (CheckCollision(player))
		{
			if (!player->is_hurt && !player->is_upgrade)
			{
				effect->OnPlayerCollision(*player);
				Hurt();
			}
			else if (player->is_hurt)
			{
				mciSendString(_T("play hit from 0"), NULL, 0, NULL);
				Hurt();
			}

			if (player->bullet_list.empty())
			{
				TCHAR text[64];
				_stprintf_s(text, _T("最终得分：%d"), player->score);
				MessageBox(GetHWnd(), text, _T("游戏结束"), MB_OK);
				game::Change_running(false);
			}
		}
		else
		{
			for (size_t i = 0; i < player->bullet_list.size(); i++)
			{
				if (player->is_frozen)
					break;

				if (CheckCollision(&player->bullet_list[i]))
				{
					effect->OnBulletCollision(*player);
					if (effect->DestroyOnBulletCollision())
						Hurt();
				}
			}
		}
	}

	// 向玩家当前位置做单位向量移动。
	void Move()
	{
		if (player == nullptr || effect == nullptr)
			return;

		int dir_x = player->x - x;
		int dir_y = player->y - y;

		double len_dir = std::sqrt(dir_x * dir_x + dir_y * dir_y);
		if (len_dir != 0)
		{
			double normalized_x = dir_x / len_dir;
			double normalized_y = dir_y / len_dir;
			this->x += static_cast<int>(effect->MoveSpeed() * normalized_x);
			this->y += static_cast<int>(effect->MoveSpeed() * normalized_y);
		}

		if (dir_x < 0)
			facing_left = true;
		else if (dir_x > 0)
			facing_left = false;
	}

	void draw_object(int delta) override
	{
		Move();
		Check();

		if (alive)
		{
			int pos_shadow_x = x + (m_width / 2 - SHADOW_WIDTH / 2) + 10;
			int pos_shadow_y = y + m_height + effect->ShadowOffsetY();
			putimage_alpha(pos_shadow_x, pos_shadow_y, img_shadow);

			if (facing_left)
				anim_left->Play(x, y, delta);
			else
				anim_right->Play(x, y, delta);
		}
	}

	const POINT& GetPosition() const
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

	void Deactivate()
	{
		alive = false;
		player = nullptr;
	}

	bool GetAlive() const
	{
		return alive;
	}

	EnemyType GetType() const
	{
		return effect == nullptr ? EnemyType::Boar : effect->Type();
	}

	~Enemy()
	{
		delete anim_boar_left;
		delete anim_boar_right;
		delete anim_bee_left;
		delete anim_bee_right;
		delete anim_snail_left;
		delete anim_snail_right;
		delete img_shadow;
	}

private:
	// 按敌人类型切换左右朝向动画。
	void ConfigureVisual()
	{
		if (effect == nullptr)
		{
			anim_left = anim_boar_left;
			anim_right = anim_boar_right;
			return;
		}

		switch (effect->Type())
		{
		case EnemyType::Bee:
			anim_left = anim_bee_left;
			anim_right = anim_bee_right;
			break;
		case EnemyType::Snail:
			anim_left = anim_snail_left;
			anim_right = anim_snail_right;
			break;
		case EnemyType::Boar:
		default:
			anim_left = anim_boar_left;
			anim_right = anim_boar_right;
			break;
		}
	}

	const int SHADOW_WIDTH = 48;

	IMAGE* img_shadow = nullptr;
	Animation* anim_left = nullptr;
	Animation* anim_right = nullptr;
	Animation* anim_boar_left = nullptr;
	Animation* anim_boar_right = nullptr;
	Animation* anim_bee_left = nullptr;
	Animation* anim_bee_right = nullptr;
	Animation* anim_snail_left = nullptr;
	Animation* anim_snail_right = nullptr;
	Player* player = nullptr;
	const EnemyEffect* effect = nullptr;
	bool facing_left = false;
	bool alive = true;
};
