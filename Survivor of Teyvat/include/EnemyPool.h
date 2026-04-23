#pragma once
#include <vector>
#include "Enemy.h"

// 敌人对象池：复用 Enemy 对象，减少战斗中频繁 new/delete 的开销。
class EnemyPool
{
public:
	static EnemyPool& Instance()
	{
		static EnemyPool pool;
		return pool;
	}

	// 获取一个可用敌人，并按指定类型重新绑定玩家和效果。
	Enemy* Acquire(Player* player, EnemyType type)
	{
		Enemy* enemy = nullptr;
		if (!available_list.empty())
		{
			enemy = available_list.back();
			available_list.pop_back();
		}
		else
		{
			enemy = new Enemy();
		}

		enemy->Bind(player, ResolveEnemyEffect(type));
		return enemy;
	}

	// 回收已经失活的敌人，等待下一次生成时复用。
	void Release(Enemy* enemy)
	{
		if (enemy == nullptr)
			return;

		enemy->Deactivate();
		available_list.push_back(enemy);
	}

	// 关闭游戏时释放池中闲置的敌人。
	void Clear()
	{
		for (Enemy* enemy : available_list)
			delete enemy;

		available_list.clear();
	}

private:
	EnemyPool() = default;
	~EnemyPool() = default;

	EnemyPool(const EnemyPool&) = delete;
	EnemyPool& operator=(const EnemyPool&) = delete;

	std::vector<Enemy*> available_list;
};
