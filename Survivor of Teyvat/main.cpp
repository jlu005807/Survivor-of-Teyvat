#pragma once
#include"game.h"
#include"Widget.h"
#include"Object.h"
#include"Button.h"
#include"Animation.h"
#include"Player.h"
#include"Enemy.h"
//完善函数

void mulLoad(LPCTSTR path, int num,std::vector<IMAGE*>& frame_list)
{
	TCHAR path_file[256];
	double scale = 1.0f;

	_stprintf_s(path_file, path, 0);

	IMAGE* frame = new IMAGE();

	loadimage(frame, path_file);

	frame_list.push_back(frame);

	int width = frame_list[0]->getwidth();
	int height = frame_list[0]->getheight();

	for (int i = 1; i < num; i++)
	{
		if (i == 1)scale = 1.1f;
		else if (i == 2)scale = 0.9f;
		int scaledWidth = width * scale; // 缩放后的按钮宽度
		int scaledHeight = height * scale; // 缩放后的按钮高度

		_stprintf_s(path_file, path, i);

		IMAGE* frame = new IMAGE();

		loadimage(frame, path_file,scaledWidth,scaledHeight);

		frame_list.push_back(frame);

	}
}

//游戏初始化
void game::init_game(int width, int height,int widget_num)
{
	game::current_widget = 0;
	game::widget_num = widget_num;
	game::is_running = true;
	game::width_game = width;
	game::height_game = height;
	game::DELTA = 1000 / 144;
	//加载图片

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

	 game::atlas_paimon_sketch_left = new Atlas(game::atlas_paimon_left);
	 game::atlas_paimon_sketch_right = new Atlas(game::atlas_paimon_right);

	 game::atlas_warrior_sketch_left = new Atlas(game::atlas_warrior_left);
	 game::atlas_warrior_sketch_right = new Atlas(game::atlas_warrior_right);

	 game::atlas_paimon_hurt_left = new Atlas(game::atlas_paimon_left,1);
	 game::atlas_paimon_hurt_right = new Atlas(game::atlas_paimon_right,1);

	 game::atlas_warrior_hurt_left = new Atlas(game::atlas_warrior_left, 1);
	 game::atlas_warrior_hurt_right = new Atlas(game::atlas_warrior_right, 1);

	//音乐
	mciSendString(_T("open mus/hit.wav alias hit"), NULL, 0, NULL);
	mciSendString(_T("open mus/bgm.mp3 alias bgm"), NULL, 0, NULL);
	mciSendString(_T("open mus/begin.mp3 alias begin"), NULL, 0, NULL);
	mciSendString(_T("open mus/Push_Button.mp3 alias button"), NULL, 0, NULL);
	mciSendString(_T("open mus/upgrade.wav alias upgrade"), NULL, 0, NULL);
	mciSendString(_T("open mus/ice.mp3 alias ice"), NULL, 0, NULL);
	mciSendString(_T("open mus/dizze.wav alias dizze"), NULL, 0, NULL);

	std::vector<std::vector<Object*>> widgets_obj;
	widgets_obj.resize(widget_num);
	std::vector<IMAGE*> pages;
	pages.resize(widget_num);
	pages[0] = new IMAGE();
	pages[1] = new IMAGE();
	pages[2] = new IMAGE();
	pages[3] = new IMAGE();
	//加载物体
	loadimage(pages[0], _T("img/menu.png"), width_game, height_game);
	loadimage(pages[1], _T("img/background.png"), width_game, height_game);
	loadimage(pages[2], _T("img/background.png"), width_game, height_game);
	loadimage(pages[3], _T("img/select.png"), width_game, height_game);
	
	std::vector<IMAGE*> start, quit,select_paimon,select_warrior;
	mulLoad(_T("img/ui_start_%d.png"), 3, start);
	mulLoad(_T("img/ui_quit_%d.png"), 3, quit);
	mulLoad(_T("img/avatar_paimon_%d.png"), 3, select_paimon);
	mulLoad(_T("img/avatar_warrior_%d.png"), 3, select_warrior);


	
	Button* button0_0 = new Button(width_game / 2 - 96, 500, 192, 75, [&]() {game::Change_widget(3); }, start);
	Button* button0_1 = new Button(width_game / 2 - 96, 600, 192, 75, [&]() { mciSendString(_T("stop all"), NULL, 0, NULL);MessageBox(GetHWnd(), _T("提示"),_T("退出成功"), MB_OK); game::Change_running(false); }, quit);
	Button* button3_0 = new Button(width_game / 2 - 84 - 50, 400, 84, 84, [&]() {game::Change_widget(2); mciSendString(_T("stop begin"), NULL, 0, NULL); mciSendString(_T("play bgm repeat from 0"), NULL, 0, NULL); }, select_warrior);
	Button* button3_1 = new Button(width_game / 2 + 50, 400, 84, 84, [&]() {game::Change_widget(1); mciSendString(_T("stop begin"), NULL, 0, NULL); mciSendString(_T("play bgm repeat from 0"), NULL, 0, NULL); }, select_paimon);

	widgets_obj[0].push_back(button0_0);
	widgets_obj[0].push_back(button0_1);
	widgets_obj[3].push_back(button3_0);
	widgets_obj[3].push_back(button3_1);

	for (int i = 0; i < widget_num; i++)
	{
		Widget* one = new Widget(width, height, widgets_obj[i],pages[i]);
		m_widgets.push_back(one);
	}

	Player* player = new Player(m_widgets[1],0);

	Player* warrior = new Player(m_widgets[2], 1);

	m_widgets[1]->m_objects.push_back(player);
	m_widgets[2]->m_objects.push_back(warrior);
};


//界面初始化
void Widget::init_widget(int Width, int height, std::vector<Object*> objects,IMAGE* page)
{
	for (int i = 0; i < objects.size(); i++)
		m_objects.push_back(objects[i]);
	this->page = page;

}


void game::running_game()
{

	initgraph(width_game, height_game);

	BeginBatchDraw();

	mciSendString(_T("play begin from 0"), NULL, 0, NULL);

	while(is_running)
	{
		m_widgets[current_widget]->show_widget();
	}

	EndBatchDraw();
}

void Widget::run_widget()
{

	while(peekmessage(&m_msg))
	{
		for (int i = 0; i < m_objects.size(); i++)
		{
			m_objects[i]->load_object(m_msg);
		}
	}
}

void Widget::draw_widget()
{
	cleardevice();

	putimage(0, 0, page);


	for (size_t i = 0; i < m_objects.size(); i++)
	{
		m_objects[i]->draw_object(1000 / 144);
	}

	FlushBatchDraw();
}

void game::close_game()
{


	delete game::atlas_paimon_left ;
	delete game::atlas_paimon_right ;
	delete game::atlas_warrior_left ;
	delete game::atlas_warrior_right ;
	delete game::atlas_boar_left ;
	delete game::atlas_boar_right ;
	delete game::atlas_paimon_sketch_left ;
	delete game::atlas_paimon_sketch_right ;
	delete game::atlas_paimon_hurt_left;
	delete game::atlas_paimon_hurt_right ;
	delete game::atlas_warrior_sketch_left ;
	delete game::atlas_warrior_sketch_right;
	delete game::atlas_warrior_hurt_left ;
	delete game::atlas_warrior_hurt_right;
	delete game::atlas_bee_left ;
	delete game::atlas_bee_right;
	delete game::atlas_snail_left ;
	delete game::atlas_snail_right ;

}

void Player::TryGenerateEnemy()
{
	int INTERVAL = 100;
	static int counter = 0;
	int num = m_widget->m_objects.size();
	INTERVAL -= this->score / 10;
	if (INTERVAL <= 25)INTERVAL = 25;
	if ((++counter) % INTERVAL == 0&&num<10+this->score / 10)
	{
		std::srand(static_cast<unsigned int>(std::time(nullptr)));
		int r = rand() % 10;
		if (r >= 0 && r <= 2 && is_dizziness == false)r = 1;
		else if (r>=3 && r<=4 && score >= 30  && is_frozen == false)r = 2;
		else r = 0;
		m_widget->m_objects.push_back(new Enemy(this,r));
	}
}

void Player::Check()
{
	//检查被击杀的敌人并删除
	for (int i = 1; i < m_widget->m_objects.size(); i++)
	{
		Enemy* obj = dynamic_cast<Enemy*>(m_widget->m_objects[i]);
		if (!obj->GetAlive()&&obj)
		{
			std::swap(m_widget->m_objects[i], m_widget->m_objects.back());
			m_widget->m_objects.pop_back();
			delete obj;
		}
	}
}

int main()
{

	game my_game(1280, 720,4);
	my_game.running_game();

	return 0;

}