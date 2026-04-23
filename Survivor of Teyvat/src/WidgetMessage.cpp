#include "Widget.h"

// 初始化场景背景和对象列表，传入的对象所有权交给 Widget。
void Widget::init_widget(int Width, int height, std::vector<Object*> objects, IMAGE* page)
{
	(VOID)Width;
	(VOID)height;
	m_objects = objects;
	this->page = page;
}

// 拉取 EasyX 消息队列，并把每条消息分发给所有场景对象。
void Widget::run_widget()
{
	while (peekmessage(&m_msg))
	{
		for (size_t i = 0; i < m_objects.size(); i++)
		{
			m_objects[i]->load_object(m_msg);
		}
	}
}
