#include "game.h"

// 程序入口：初始化游戏、运行主循环，并在退出时释放资源。
int main()
{
	game::init_game(1280, 720, 4);
	game::running_game();
	game::close_game();
	return 0;
}
