#include "EnemyEffect.h"
#include "BoarEffect.h"
#include "BeeEffect.h"
#include "SnailEffect.h"

// 各敌人效果使用函数内静态对象，生命周期覆盖整个程序运行期。
const EnemyEffect& GetBoarEffect()
{
	static BoarEffect effect;
	return effect;
}

const EnemyEffect& GetBeeEffect()
{
	static BeeEffect effect;
	return effect;
}

const EnemyEffect& GetSnailEffect()
{
	static SnailEffect effect;
	return effect;
}

// 按敌人类型解析对应效果，未知类型默认按野猪处理。
const EnemyEffect& ResolveEnemyEffect(EnemyType type)
{
	switch (type)
	{
	case EnemyType::Bee:
		return GetBeeEffect();
	case EnemyType::Snail:
		return GetSnailEffect();
	case EnemyType::Boar:
	default:
		return GetBoarEffect();
	}
}
