#pragma once
#include "Player.h"

// 敌人类型枚举，用于决定移动速度、碰撞效果和动画资源。
enum class EnemyType
{
	Boar = 0,
	Bee,
	Snail
};

// 敌人效果基类：不同敌人在碰到玩家或子弹时有不同表现。
class EnemyEffect
{
public:
	virtual ~EnemyEffect() = default;

	virtual EnemyType Type() const = 0;
	virtual int MoveSpeed() const = 0;
	virtual int ShadowOffsetY() const = 0;
	virtual void OnPlayerCollision(Player& player) const = 0;
	virtual void OnBulletCollision(Player& player) const = 0;
	virtual bool DestroyOnBulletCollision() const = 0;
};

// 提供全局静态效果对象，避免为每个 Enemy 重复创建策略对象。
const EnemyEffect& GetBoarEffect();
const EnemyEffect& GetBeeEffect();
const EnemyEffect& GetSnailEffect();
const EnemyEffect& ResolveEnemyEffect(EnemyType type);
