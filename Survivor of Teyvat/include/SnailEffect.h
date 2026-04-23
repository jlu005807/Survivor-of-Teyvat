#pragma once
#include "EnemyEffect.h"

// 蜗牛敌人：移动较慢，碰到玩家会冻结玩家；子弹碰撞不会直接消灭它。
class SnailEffect : public EnemyEffect
{
public:
	EnemyType Type() const override
	{
		return EnemyType::Snail;
	}

	int MoveSpeed() const override
	{
		return 2;
	}

	int ShadowOffsetY() const override
	{
		return -15;
	}

	void OnPlayerCollision(Player& player) const override
	{
		mciSendString(_T("play ice from 0"), NULL, 0, NULL);
		player.is_frozen = true;
		player.score += 9;
	}

	void OnBulletCollision(Player& player) const override
	{
		(VOID)player;
	}

	bool DestroyOnBulletCollision() const override
	{
		return false;
	}
};
