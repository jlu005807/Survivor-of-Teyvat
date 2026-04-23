#pragma once
#include "EnemyEffect.h"

// 野猪敌人：基础敌人，碰到玩家会触发受击状态。
class BoarEffect : public EnemyEffect
{
public:
	EnemyType Type() const override
	{
		return EnemyType::Boar;
	}

	int MoveSpeed() const override
	{
		return 3;
	}

	int ShadowOffsetY() const override
	{
		return -15;
	}

	void OnPlayerCollision(Player& player) const override
	{
		mciSendString(_T("play hit from 0"), NULL, 0, NULL);

		if (!player.bullet_list.empty())
			player.bullet_list.pop_back();

		player.is_hurt = true;
		player.is_frozen = false;
		player.is_dizziness = false;
	}

	void OnBulletCollision(Player& player) const override
	{
		player.score++;
		mciSendString(_T("play hit from 0"), NULL, 0, NULL);
	}

	bool DestroyOnBulletCollision() const override
	{
		return true;
	}
};
