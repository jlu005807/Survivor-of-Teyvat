#include "Player.h"
#include "EnemyPool.h"
#include <algorithm>
#include <cstdlib>

Player* Player::instance = nullptr;

// 创建单例玩家；切换角色时先释放旧玩家，避免战斗场景持有过期状态。
Player* Player::CreateInstance(Widget* widget, int type)
{
	if (instance != nullptr)
	{
		delete instance;
		instance = nullptr;
	}

	instance = new Player(widget, type);
	return instance;
}

// 获取当前玩家实例。
Player* Player::GetInstance()
{
	return instance;
}

// 释放当前玩家实例。
void Player::DestroyInstance()
{
	delete instance;
	instance = nullptr;
}

// 随分数提升逐步缩短生成间隔，并限制同屏敌人数量。
void Player::TryGenerateEnemy()
{
	int interval = 100;
	if (m_widget == nullptr)
		return;

	int num = static_cast<int>(m_widget->m_objects.size());
	interval -= this->score / 10;
	if (interval <= 25) interval = 25;
	if ((++enemy_spawn_counter) % interval == 0 && num < 10 + this->score / 10)
	{
		int r = std::rand() % 10;
		EnemyType type = EnemyType::Boar;

		// 蜜蜂会造成眩晕，蜗牛会造成冻结，按分数和当前状态控制出现概率。
		if (r >= 0 && r <= 2 && is_dizziness == false)
			type = EnemyType::Bee;
		else if (r >= 3 && r <= 4 && score >= 30 && is_frozen == false)
			type = EnemyType::Snail;

		m_widget->m_objects.push_back(EnemyPool::Instance().Acquire(this, type));
	}
}

// 回收已经死亡的敌人，玩家对象固定保留在 m_objects[0]。
void Player::Check()
{
	if (m_widget == nullptr)
		return;

	for (size_t i = 1; i < m_widget->m_objects.size();)
	{
		Enemy* obj = dynamic_cast<Enemy*>(m_widget->m_objects[i]);
		if (obj && !obj->GetAlive())
		{
			std::swap(m_widget->m_objects[i], m_widget->m_objects.back());
			m_widget->m_objects.pop_back();
			EnemyPool::Instance().Release(obj);
			continue;
		}
		++i;
	}
}
