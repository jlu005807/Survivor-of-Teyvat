#include "game.h"
#include "Widget.h"
#include "Object.h"
#include "Button.h"
#include "Animation.h"
#include "Player.h"
#include "EnemyPool.h"
#include <cmath>
#include <cstdlib>
#include <ctime>
#include <vector>

namespace
{
	// 批量加载按钮多状态贴图，并按状态生成轻微缩放效果。
	void mulLoad(LPCTSTR path, int num, std::vector<IMAGE*>& frame_list)
	{
		TCHAR path_file[256];
		double scale = 1.0;

		_stprintf_s(path_file, path, 0);

		IMAGE* frame = new IMAGE();
		loadimage(frame, path_file);
		frame_list.push_back(frame);

		int width = frame_list[0]->getwidth();
		int height = frame_list[0]->getheight();

		for (int i = 1; i < num; i++)
		{
			if (i == 1) scale = 1.1;
			else if (i == 2) scale = 0.9;

			int scaledWidth = static_cast<int>(std::lround(width * scale));
			int scaledHeight = static_cast<int>(std::lround(height * scale));

			_stprintf_s(path_file, path, i);

			IMAGE* one = new IMAGE();
			loadimage(one, path_file, scaledWidth, scaledHeight);
			frame_list.push_back(one);
		}
	}

	// 角色选择时替换战斗场景中的玩家对象。
	void ReplaceBattlePlayer(Widget* battle_widget, int player_type)
	{
		if (battle_widget == nullptr)
			return;

		Player* current_player = Player::GetInstance();
		if (current_player != nullptr)
		{
			for (size_t i = 0; i < battle_widget->m_objects.size(); i++)
			{
				if (battle_widget->m_objects[i] == current_player)
				{
					battle_widget->m_objects.erase(battle_widget->m_objects.begin() + i);
					break;
				}
			}

			Player::DestroyInstance();
		}

		battle_widget->m_objects.push_back(Player::CreateInstance(battle_widget, player_type));
	}

	// 统一释放图集指针，避免关闭后留下悬空指针。
	void DeleteAtlas(Atlas*& atlas)
	{
		delete atlas;
		atlas = nullptr;
	}
}

int game::current_widget = 0;
int game::widget_num = 0;
bool game::is_running = true;
int game::width_game = 1280;
int game::height_game = 720;
double game::DELTA = 1000.0 / 144.0;

Atlas* game::atlas_paimon_left = NULL;
Atlas* game::atlas_paimon_right = NULL;
Atlas* game::atlas_warrior_left = NULL;
Atlas* game::atlas_warrior_right = NULL;
Atlas* game::atlas_boar_left = NULL;
Atlas* game::atlas_boar_right = NULL;
Atlas* game::atlas_paimon_sketch_left = NULL;
Atlas* game::atlas_paimon_sketch_right = NULL;
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
std::vector<Widget*> game::m_widgets;

void game::init_game(int width, int height, int widget_num)
{
	if (widget_num < 4)
		widget_num = 4;

	std::srand(static_cast<unsigned int>(std::time(nullptr)));
	game::current_widget = 0;
	game::widget_num = widget_num;
	game::is_running = true;
	game::width_game = width;
	game::height_game = height;
	game::DELTA = 1000.0 / 144.0;

	// 加载角色与敌人的基础动画图集。
	game::atlas_paimon_left = new Atlas(_T("img/paimon_left_%d.png"), 6);
	game::atlas_paimon_right = new Atlas(_T("img/paimon_right_%d.png"), 6);
	game::atlas_warrior_left = new Atlas(_T("img/warrior_walk_left_%d.png"), 8);
	game::atlas_warrior_right = new Atlas(_T("img/warrior_walk_right_%d.png"), 8);
	game::atlas_boar_left = new Atlas(_T("img/boar_left_%d.png"), 6);
	game::atlas_boar_right = new Atlas(_T("img/boar_right_%d.png"), 6);
	game::atlas_bee_left = new Atlas(_T("img/bee_left_%d.png"), 4);
	game::atlas_bee_right = new Atlas(_T("img/bee_right_%d.png"), 4);
	game::atlas_snail_left = new Atlas(_T("img/snail_left_%d.png"), 4);
	game::atlas_snail_right = new Atlas(_T("img/snail_right_%d.png"), 4);

	// 基于基础图集生成升级闪白与受击变色图集。
	game::atlas_paimon_sketch_left = new Atlas(game::atlas_paimon_left);
	game::atlas_paimon_sketch_right = new Atlas(game::atlas_paimon_right);
	game::atlas_warrior_sketch_left = new Atlas(game::atlas_warrior_left);
	game::atlas_warrior_sketch_right = new Atlas(game::atlas_warrior_right);
	game::atlas_paimon_hurt_left = new Atlas(game::atlas_paimon_left, 1);
	game::atlas_paimon_hurt_right = new Atlas(game::atlas_paimon_right, 1);
	game::atlas_warrior_hurt_left = new Atlas(game::atlas_warrior_left, 1);
	game::atlas_warrior_hurt_right = new Atlas(game::atlas_warrior_right, 1);

	// 预打开音效，运行时通过别名播放。
	mciSendString(_T("open mus/hit.wav alias hit"), NULL, 0, NULL);
	mciSendString(_T("open mus/bgm.mp3 alias bgm"), NULL, 0, NULL);
	mciSendString(_T("open mus/begin.mp3 alias begin"), NULL, 0, NULL);
	mciSendString(_T("open mus/Push_Button.mp3 alias button"), NULL, 0, NULL);
	mciSendString(_T("open mus/upgrade.wav alias upgrade"), NULL, 0, NULL);
	mciSendString(_T("open mus/ice.mp3 alias ice"), NULL, 0, NULL);
	mciSendString(_T("open mus/dizze.wav alias dizze"), NULL, 0, NULL);

	std::vector<std::vector<Object*>> widgets_obj(widget_num);
	std::vector<IMAGE*> pages(widget_num, nullptr);
	pages[0] = new IMAGE();
	pages[1] = new IMAGE();
	pages[2] = new IMAGE();
	pages[3] = new IMAGE();

	loadimage(pages[0], _T("img/menu.png"), width_game, height_game);
	loadimage(pages[1], _T("img/background.png"), width_game, height_game);
	loadimage(pages[2], _T("img/background.png"), width_game, height_game);
	loadimage(pages[3], _T("img/select.png"), width_game, height_game);

	std::vector<IMAGE*> start, quit, select_paimon, select_warrior;
	mulLoad(_T("img/ui_start_%d.png"), 3, start);
	mulLoad(_T("img/ui_quit_%d.png"), 3, quit);
	mulLoad(_T("img/avatar_paimon_%d.png"), 3, select_paimon);
	mulLoad(_T("img/avatar_warrior_%d.png"), 3, select_warrior);

	Button* button0_0 = new Button(width_game / 2 - 96, 500, 192, 75, []() {
		game::Change_widget(3);
	}, start);

	Button* button0_1 = new Button(width_game / 2 - 96, 600, 192, 75, []() {
		mciSendString(_T("stop all"), NULL, 0, NULL);
		MessageBox(GetHWnd(), _T("退出成功"), _T("提示"), MB_OK);
		game::Change_running(false);
	}, quit);

	Button* button3_0 = new Button(width_game / 2 - 84 - 50, 400, 84, 84, []() {
		if (game::m_widgets.size() > 1)
			ReplaceBattlePlayer(game::m_widgets[1], 1);

		game::Change_widget(1);
		mciSendString(_T("stop begin"), NULL, 0, NULL);
		mciSendString(_T("play bgm repeat from 0"), NULL, 0, NULL);
	}, select_warrior);

	Button* button3_1 = new Button(width_game / 2 + 50, 400, 84, 84, []() {
		if (game::m_widgets.size() > 1)
			ReplaceBattlePlayer(game::m_widgets[1], 0);

		game::Change_widget(1);
		mciSendString(_T("stop begin"), NULL, 0, NULL);
		mciSendString(_T("play bgm repeat from 0"), NULL, 0, NULL);
	}, select_paimon);

	widgets_obj[0].push_back(button0_0);
	widgets_obj[0].push_back(button0_1);
	widgets_obj[3].push_back(button3_0);
	widgets_obj[3].push_back(button3_1);

	for (int i = 0; i < widget_num; i++)
	{
		Widget* one = new Widget(width, height, widgets_obj[i], pages[i]);
		game::m_widgets.push_back(one);
	}
}

void game::running_game()
{
	if (game::m_widgets.empty())
		return;

	initgraph(width_game, height_game);
	BeginBatchDraw();
	mciSendString(_T("play begin from 0"), NULL, 0, NULL);

	while (is_running)
	{
		game::m_widgets[current_widget]->show_widget();
	}

	EndBatchDraw();
}

void game::close_game()
{
	mciSendString(_T("stop all"), NULL, 0, NULL);

	for (Widget* widget : game::m_widgets)
		delete widget;

	game::m_widgets.clear();

	EnemyPool::Instance().Clear();
	Player::DestroyInstance();

	DeleteAtlas(game::atlas_paimon_left);
	DeleteAtlas(game::atlas_paimon_right);
	DeleteAtlas(game::atlas_warrior_left);
	DeleteAtlas(game::atlas_warrior_right);
	DeleteAtlas(game::atlas_boar_left);
	DeleteAtlas(game::atlas_boar_right);
	DeleteAtlas(game::atlas_paimon_sketch_left);
	DeleteAtlas(game::atlas_paimon_sketch_right);
	DeleteAtlas(game::atlas_paimon_hurt_left);
	DeleteAtlas(game::atlas_paimon_hurt_right);
	DeleteAtlas(game::atlas_warrior_sketch_left);
	DeleteAtlas(game::atlas_warrior_sketch_right);
	DeleteAtlas(game::atlas_warrior_hurt_left);
	DeleteAtlas(game::atlas_warrior_hurt_right);
	DeleteAtlas(game::atlas_bee_left);
	DeleteAtlas(game::atlas_bee_right);
	DeleteAtlas(game::atlas_snail_left);
	DeleteAtlas(game::atlas_snail_right);

	mciSendString(_T("close all"), NULL, 0, NULL);
	closegraph();
}
