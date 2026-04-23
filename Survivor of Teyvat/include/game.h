#pragma once
#include <easyx.h>
#include <vector>
#include "Widget.h"

#pragma comment(lib,"Winmm.lib")
#pragma comment(lib,"MSIMG32.LIB")

class Atlas;

// game 是全局游戏管理类，集中维护窗口、场景、资源图集和主循环。
class game
{
public:
	game() = delete;
	~game() = delete;

	// 游戏是否继续运行。
	static bool is_running;

	// 当前显示的场景索引。
	static int current_widget;

	// 场景总数。
	static int widget_num;

	// 游戏窗口尺寸。
	static int width_game;
	static int height_game;

	// 固定帧间隔，默认按 144 FPS 计算。
	static double DELTA;

	// 玩家、敌人和状态特效的全局图集资源。
	static Atlas* atlas_paimon_left;
	static Atlas* atlas_paimon_right;
	static Atlas* atlas_warrior_left;
	static Atlas* atlas_warrior_right;
	static Atlas* atlas_boar_left;
	static Atlas* atlas_boar_right;
	static Atlas* atlas_paimon_sketch_left;
	static Atlas* atlas_paimon_sketch_right;
	static Atlas* atlas_paimon_hurt_left;
	static Atlas* atlas_paimon_hurt_right;
	static Atlas* atlas_warrior_sketch_left;
	static Atlas* atlas_warrior_sketch_right;
	static Atlas* atlas_warrior_hurt_left;
	static Atlas* atlas_warrior_hurt_right;
	static Atlas* atlas_bee_left;
	static Atlas* atlas_bee_right;
	static Atlas* atlas_snail_left;
	static Atlas* atlas_snail_right;

	// 切换场景；索引非法时回到主菜单。
	static void Change_widget(int idx)
	{
		if (idx >= 0 && idx < game::widget_num)
			game::current_widget = idx;
		else
			game::current_widget = 0;
	}

	static void Change_running(bool one)
	{
		game::is_running = one;
	}

	static int Getwidth()
	{
		return width_game;
	}

	static int Getheight()
	{
		return height_game;
	}

	// 初始化窗口尺寸、资源和场景。
	static void init_game(int width, int height, int widget_num);

	// 进入游戏主循环。
	static void running_game();

	// 释放图集、场景和音频资源。
	static void close_game();

	// 场景列表。
	static std::vector<Widget*> m_widgets;
};
