#pragma once
#include "EnemyEffect.h"

// 蜜蜂敌人：速度较快，碰到玩家会造成眩晕并扣除一颗子弹。
class BeeEffect : public EnemyEffect
{
public:
	EnemyType Type() const override
	{
		return EnemyType::Bee;
	}

	int MoveSpeed() const override
	{
		return 4;
	}

	int ShadowOffsetY() const override
	{
		return 0;
	}

	void OnPlayerCollision(Player& player) const override
	{
		mciSendString(_T("play dizze from 0"), NULL, 0, NULL);

		if (!player.bullet_list.empty())
			player.bullet_list.pop_back();

		if (!player.is_frozen)
			player.is_dizziness = true;
	}

	void OnBulletCollision(Player& player) const override
	{
		player.score += 5;
		mciSendString(_T("play hit from 0"), NULL, 0, NULL);
	}

	bool DestroyOnBulletCollision() const override
	{
		return true;
	}
};
