#pragma once
#include<iostream>
#include<vector>
#include<easyx.h>
#include<cmath>

#pragma comment(lib,"Winmm.lib")
#pragma comment(lib,"MSIMG32.LIB")


class Widget;//游戏界面

class Atlas;


//游戏总框架
class game
{
public:
	
	//游戏属性
	static bool is_running;

	static int current_widget;

	static int widget_num;

	//界面属性
	static int width_game;

	static int height_game;

	//delta
	static double DELTA;

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

	//改变界面
	static void Change_widget(int idx)
	{
		if (idx >= 0 && idx < game::widget_num)game::current_widget = idx;
		else game::current_widget = 0;
	}

	static void Change_running(bool one)
	{
		game::is_running = one;
	}

	static int Getwidth() {
		return width_game;
	}

	static int Getheight()
	{
		return height_game;
	}


	//储存界面
	std::vector<Widget*> m_widgets;

	//（构造函数作为初始化）
	game(int width,int height,int widget_num)
	{
		init_game(width,height,widget_num);
	}

	//运行游戏
	void running_game();

	//关闭游戏可以放置在析构函数
	~game()
	{
		close_game();
	}

private:
	//初始化
	void init_game(int width,int height,int widget_num);


	//关闭游戏
	void close_game();


};

int  game::current_widget = 0;
int game::widget_num = widget_num;
bool game::is_running = true;
int game::width_game = 1280;
int game::height_game = 720;
double game::DELTA = 1000 / 144;

 Atlas* game::atlas_paimon_left=NULL;
 Atlas* game::atlas_paimon_right = NULL;
 Atlas* game::atlas_warrior_left = NULL;
 Atlas* game::atlas_warrior_right = NULL;
 Atlas* game::atlas_boar_left = NULL;
 Atlas* game::atlas_boar_right = NULL;
 Atlas* game::atlas_paimon_sketch_left=NULL;
 Atlas* game::atlas_paimon_sketch_right=NULL;
 Atlas* game::atlas_paimon_hurt_left = NULL;
 Atlas* game::atlas_paimon_hurt_right = NULL;
 Atlas* game::atlas_warrior_sketch_left = NULL;
 Atlas* game::atlas_warrior_sketch_right = NULL;
 Atlas* game::atlas_warrior_hurt_left = NULL;
 Atlas* game::atlas_warrior_hurt_right = NULL;
 Atlas* game::atlas_bee_left = NULL;
 Atlas* game::atlas_bee_right = NULL;
 Atlas* game::atlas_snail_left = NULL;
 Atlas* game::atlas_snail_right = NULL;