#include "Widget.h"

// 清屏后绘制背景和所有对象，最后刷新批量绘制缓冲。
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
